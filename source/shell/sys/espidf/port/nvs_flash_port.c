/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_ESPIDF_CFG_USE_NVS == ENABLED

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "hal/vsf_hal.h"
#include "service/heap/vsf_heap.h"

// Include the public ESP-IDF shim headers.
#include "../include/esp_err.h"
#include "../include/esp_partition.h"
#include "../include/nvs.h"
#include "../include/nvs_flash.h"

/*============================ MACROS ========================================*/

// Maximum total serialised size per store (arbitrary safety limit).
#ifndef __NVS_MAX_SERIAL_SIZE
#   define __NVS_MAX_SERIAL_SIZE    (64u * 1024u)
#endif

// Magic word / version for our custom on-disk format.
#define __NVS_MAGIC                 0x3153564Eu     // "NVS1" little-endian
#define __NVS_VERSION               1u

/*============================ MACROFIED FUNCTIONS ===========================*/

// Simple CRC-32 (ISO 3309 / zlib polynomial). We use a byte-at-a-time
// table-less implementation to avoid a 1 KiB lookup table in .rodata.
static uint32_t __nvs_crc32(const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (-(crc & 1u)));
        }
    }
    return crc ^ 0xFFFFFFFFu;
}

/*============================ TYPES =========================================*/

// ---------------------------------------------------------------------------
//  On-disk layout (all little-endian, naturally aligned on the host)
//
//  Offset  Size   Field
//  0       4      magic      __NVS_MAGIC
//  4       4      version    __NVS_VERSION
//  8       4      entry_count
//  12      4      total_size (byte length of (header + entries + payloads))
//  16      4      crc32      computed over bytes [0 .. total_size-4)
//                              i.e. crc32 field itself is excluded from the
//                              calculation; its offset = total_size - 4.
//  20      ...    entry table  (entry_count × sizeof(__nvs_disk_entry_t))
//  ...     ...    payload area (variable-length str/blob data, packed)
//  last 4  4      crc32 value (duplicated at fixed tail for easy verify)
//
//  NOTE: The CRC32 is stored at the very end of the serialised image so that
//  it covers all preceding bytes (header + entries + payloads).
// ---------------------------------------------------------------------------

#define __NVS_HEADER_SIZE           20u

typedef struct __nvs_disk_header_t {
    uint32_t    magic;
    uint32_t    version;
    uint32_t    entry_count;
    uint32_t    total_size;
    uint32_t    header_crc;         // reserved / unused in current version
} __nvs_disk_header_t;

// Fixed-size on-disk entry descriptor.
typedef struct __nvs_disk_entry_t {
    char        ns[NVS_NS_NAME_MAX_SIZE];   // 16
    char        key[NVS_KEY_NAME_MAX_SIZE];  // 16
    uint8_t     type;                        // nvs_type_t
    uint8_t     pad[3];
    uint32_t    data_size;                   // for inline: sizeof(type)
                                             // for str: strlen+1
                                             // for blob: raw length
    // Inline storage for types <= 8 bytes; str/blob data resides in the
    // payload area and inline_value is unused.
    uint8_t     inline_value[8];
} __nvs_disk_entry_t;

// ---------------------------------------------------------------------------
//  RAM model
// ---------------------------------------------------------------------------

// A single key-value pair kept in a linked list while the store is open.
typedef struct __nvs_kv_t {
    struct __nvs_kv_t  *next;
    char                ns[NVS_NS_NAME_MAX_SIZE];
    char                key[NVS_KEY_NAME_MAX_SIZE];
    nvs_type_t          type;
    uint32_t            data_size;
    // For integer types the value is stored directly in inline_value.
    // For str/blob, heap-allocated buffer is at ext_data.
    union {
        uint8_t         inline_value[8];
        void           *ext_data;
    };
} __nvs_kv_t;

// Per-partition store.
typedef struct __nvs_store_t {
    struct __nvs_store_t   *next;
    bool                    dirty;          // RAM differs from flash
    char                    part_label[NVS_PART_NAME_MAX_SIZE];
    const esp_partition_t  *partition;
    __nvs_kv_t             *kv_head;        // linked list of all KV pairs
} __nvs_store_t;

// Per-handle context.
typedef struct __nvs_handle_ctx_t {
    struct __nvs_handle_ctx_t *next;
    nvs_open_mode_t     mode;
    __nvs_store_t      *store;
    char                ns[NVS_NS_NAME_MAX_SIZE];
} __nvs_handle_ctx_t;

// Iterator internals.
typedef struct nvs_opaque_iterator_t {
    __nvs_store_t  *store;
    __nvs_kv_t     *cur;
    char            ns_filter[NVS_NS_NAME_MAX_SIZE];
    nvs_type_t      type_filter;
} nvs_opaque_iterator_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static __nvs_store_t      *__nvs_store_head;
static __nvs_handle_ctx_t *__nvs_handle_head;

/*============================ IMPLEMENTATION ================================*/

// ---------------------------------------------------------------------------
//  Helpers
// ---------------------------------------------------------------------------

static inline bool __nvs_is_inline(nvs_type_t t)
{
    return (t != NVS_TYPE_STR) && (t != NVS_TYPE_BLOB);
}

static inline size_t __nvs_type_size(nvs_type_t t)
{
    switch (t) {
    case NVS_TYPE_I8:  case NVS_TYPE_U8:  return 1;
    case NVS_TYPE_I16: case NVS_TYPE_U16: return 2;
    case NVS_TYPE_I32: case NVS_TYPE_U32: return 4;
    case NVS_TYPE_I64: case NVS_TYPE_U64: return 8;
    default: return 0;
    }
}

static bool __nvs_validate_key(const char *key)
{
    if (key == NULL) { return false; }
    size_t len = strlen(key);
    return (len > 0) && (len < NVS_KEY_NAME_MAX_SIZE);
}

static bool __nvs_validate_ns(const char *ns)
{
    if (ns == NULL) { return false; }
    size_t len = strlen(ns);
    return (len > 0) && (len < NVS_NS_NAME_MAX_SIZE);
}

// Find a store by partition label.
static __nvs_store_t * __nvs_find_store(const char *label)
{
    for (__nvs_store_t *s = __nvs_store_head; s != NULL; s = s->next) {
        if (strcmp(s->part_label, label) == 0) {
            return s;
        }
    }
    return NULL;
}

// Allocate a new store node and prepend to the list.
static __nvs_store_t * __nvs_alloc_store(void)
{
    __nvs_store_t *store;
    store = (__nvs_store_t *)vsf_heap_malloc(sizeof(*store));
    if (store == NULL) { return NULL; }
    memset(store, 0, sizeof(*store));
    store->next = __nvs_store_head;
    __nvs_store_head = store;
    return store;
}

// Remove and free a store from the linked list.
static void __nvs_free_store(__nvs_store_t *store)
{
    __nvs_store_t **pp = &__nvs_store_head;
    while (*pp != NULL) {
        if (*pp == store) {
            *pp = store->next;
            vsf_heap_free(store);
            return;
        }
        pp = &(*pp)->next;
    }
}

// Find a KV entry by namespace + key in a store.
static __nvs_kv_t * __nvs_find_kv(__nvs_store_t *store,
                                   const char *ns, const char *key)
{
    for (__nvs_kv_t *kv = store->kv_head; kv != NULL; kv = kv->next) {
        if (strcmp(kv->ns, ns) == 0 && strcmp(kv->key, key) == 0) {
            return kv;
        }
    }
    return NULL;
}

// Free a single KV node (and its external data if any).
static void __nvs_free_kv(__nvs_kv_t *kv)
{
    if (!__nvs_is_inline(kv->type) && kv->ext_data != NULL) {
        vsf_heap_free(kv->ext_data);
    }
    vsf_heap_free(kv);
}

// Free all KV entries in a store.
static void __nvs_free_all_kv(__nvs_store_t *store)
{
    __nvs_kv_t *kv = store->kv_head;
    while (kv != NULL) {
        __nvs_kv_t *next = kv->next;
        __nvs_free_kv(kv);
        kv = next;
    }
    store->kv_head = NULL;
}

// Remove a KV from the linked list and free it.
static void __nvs_remove_kv(__nvs_store_t *store, __nvs_kv_t *target)
{
    __nvs_kv_t **pp = &store->kv_head;
    while (*pp != NULL) {
        if (*pp == target) {
            *pp = target->next;
            __nvs_free_kv(target);
            return;
        }
        pp = &(*pp)->next;
    }
}

// Count KV entries (optionally filtered by namespace).
static size_t __nvs_count_kv(__nvs_store_t *store, const char *ns)
{
    size_t n = 0;
    for (__nvs_kv_t *kv = store->kv_head; kv != NULL; kv = kv->next) {
        if (ns == NULL || strcmp(kv->ns, ns) == 0) { n++; }
    }
    return n;
}

// Count distinct namespaces.
static size_t __nvs_count_ns(__nvs_store_t *store)
{
    // Small O(n²) scan – fine for a test shim with few entries.
    size_t count = 0;
    for (__nvs_kv_t *kv = store->kv_head; kv != NULL; kv = kv->next) {
        bool dup = false;
        for (__nvs_kv_t *prev = store->kv_head; prev != kv; prev = prev->next) {
            if (strcmp(prev->ns, kv->ns) == 0) { dup = true; break; }
        }
        if (!dup) { count++; }
    }
    return count;
}

// Handle validation.
static __nvs_handle_ctx_t * __nvs_get_handle(nvs_handle_t h)
{
    // nvs_handle_t stores the pointer to __nvs_handle_ctx_t.
    __nvs_handle_ctx_t *ctx = (__nvs_handle_ctx_t *)(uintptr_t)h;
    for (__nvs_handle_ctx_t *c = __nvs_handle_head; c != NULL; c = c->next) {
        if (c == ctx) { return ctx; }
    }
    return NULL;
}

// ---------------------------------------------------------------------------
//  Serialisation  (RAM → flash)
// ---------------------------------------------------------------------------

static esp_err_t __nvs_flush(__nvs_store_t *store)
{
    if (!store->dirty) { return ESP_OK; }

    // 1. Count entries & compute total serialised size.
    uint32_t entry_count = 0;
    size_t payload_size = 0;
    for (__nvs_kv_t *kv = store->kv_head; kv != NULL; kv = kv->next) {
        entry_count++;
        if (!__nvs_is_inline(kv->type)) {
            payload_size += kv->data_size;
        }
    }

    size_t total = __NVS_HEADER_SIZE
                 + entry_count * sizeof(__nvs_disk_entry_t)
                 + payload_size
                 + 4u;  // trailing CRC32
    if (total > __NVS_MAX_SERIAL_SIZE || total > store->partition->size) {
        return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
    }

    // 2. Allocate a contiguous buffer and fill it.
    uint8_t *buf = (uint8_t *)vsf_heap_malloc(total);
    if (buf == NULL) { return ESP_ERR_NO_MEM; }
    memset(buf, 0, total);

    __nvs_disk_header_t *hdr = (__nvs_disk_header_t *)buf;
    hdr->magic       = __NVS_MAGIC;
    hdr->version     = __NVS_VERSION;
    hdr->entry_count = entry_count;
    hdr->total_size  = (uint32_t)total;
    hdr->header_crc  = 0;  // unused in v1

    __nvs_disk_entry_t *de = (__nvs_disk_entry_t *)(buf + __NVS_HEADER_SIZE);
    uint8_t *payload_ptr = (uint8_t *)(de + entry_count);

    for (__nvs_kv_t *kv = store->kv_head; kv != NULL; kv = kv->next, de++) {
        strncpy(de->ns,  kv->ns,  NVS_NS_NAME_MAX_SIZE);
        strncpy(de->key, kv->key, NVS_KEY_NAME_MAX_SIZE);
        de->type      = (uint8_t)kv->type;
        de->data_size = kv->data_size;
        if (__nvs_is_inline(kv->type)) {
            memcpy(de->inline_value, kv->inline_value, kv->data_size);
        } else {
            memcpy(payload_ptr, kv->ext_data, kv->data_size);
            payload_ptr += kv->data_size;
        }
    }

    // 3. CRC32 over everything except the last 4 bytes (the crc field).
    uint32_t crc = __nvs_crc32(buf, total - 4u);
    memcpy(buf + total - 4u, &crc, 4u);

    // 4. Write to the partition in one shot.
    esp_err_t rc = esp_partition_write(store->partition, 0, buf, total);
    vsf_heap_free(buf);
    if (rc == ESP_OK) { store->dirty = false; }
    return rc;
}

// ---------------------------------------------------------------------------
//  Deserialisation  (flash → RAM)
// ---------------------------------------------------------------------------

static esp_err_t __nvs_load(__nvs_store_t *store)
{
    __nvs_free_all_kv(store);

    // 1. Read the header.
    __nvs_disk_header_t hdr;
    esp_err_t rc = esp_partition_read(store->partition, 0, &hdr, sizeof(hdr));
    if (rc != ESP_OK) { return rc; }

    // Virgin / erased partition → treat as empty (success).
    if (hdr.magic != __NVS_MAGIC) {
        store->dirty = false;
        return ESP_OK;
    }
    if (hdr.version != __NVS_VERSION) {
        return ESP_ERR_NVS_NEW_VERSION_FOUND;
    }
    if (hdr.total_size < __NVS_HEADER_SIZE + 4u
            || hdr.total_size > store->partition->size
            || hdr.total_size > __NVS_MAX_SERIAL_SIZE) {
        return ESP_ERR_NVS_INVALID_STATE;
    }

    // 2. Read entire serialised image.
    uint8_t *buf = (uint8_t *)vsf_heap_malloc(hdr.total_size);
    if (buf == NULL) { return ESP_ERR_NO_MEM; }
    rc = esp_partition_read(store->partition, 0, buf, hdr.total_size);
    if (rc != ESP_OK) { vsf_heap_free(buf); return rc; }

    // 3. Verify CRC32.
    uint32_t stored_crc;
    memcpy(&stored_crc, buf + hdr.total_size - 4u, 4u);
    uint32_t calc_crc = __nvs_crc32(buf, hdr.total_size - 4u);
    if (stored_crc != calc_crc) {
        vsf_heap_free(buf);
        return ESP_ERR_NVS_INVALID_STATE;
    }

    // 4. Rebuild the KV linked list.
    __nvs_disk_entry_t *de = (__nvs_disk_entry_t *)(buf + __NVS_HEADER_SIZE);
    uint8_t *payload_ptr = (uint8_t *)(de + hdr.entry_count);

    for (uint32_t i = 0; i < hdr.entry_count; i++, de++) {
        __nvs_kv_t *kv = (__nvs_kv_t *)vsf_heap_malloc(sizeof(*kv));
        if (kv == NULL) { vsf_heap_free(buf); return ESP_ERR_NO_MEM; }
        memset(kv, 0, sizeof(*kv));

        memcpy(kv->ns,  de->ns,  NVS_NS_NAME_MAX_SIZE);
        memcpy(kv->key, de->key, NVS_KEY_NAME_MAX_SIZE);
        kv->type      = (nvs_type_t)de->type;
        kv->data_size = de->data_size;

        if (__nvs_is_inline(kv->type)) {
            memcpy(kv->inline_value, de->inline_value, kv->data_size);
        } else {
            kv->ext_data = vsf_heap_malloc(kv->data_size);
            if (kv->ext_data == NULL) {
                vsf_heap_free(kv);
                vsf_heap_free(buf);
                return ESP_ERR_NO_MEM;
            }
            memcpy(kv->ext_data, payload_ptr, kv->data_size);
            payload_ptr += kv->data_size;
        }

        // prepend
        kv->next = store->kv_head;
        store->kv_head = kv;
    }

    vsf_heap_free(buf);
    store->dirty = false;
    return ESP_OK;
}

// ---------------------------------------------------------------------------
//  nvs_flash_*  (init / deinit / erase)
// ---------------------------------------------------------------------------

static esp_err_t __nvs_init_partition(const char *label,
                                      const esp_partition_t *part)
{
    if (part == NULL) {
        if (label == NULL) { return ESP_ERR_INVALID_ARG; }
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                        ESP_PARTITION_SUBTYPE_DATA_NVS,
                                        label);
        if (part == NULL) { return ESP_ERR_NVS_PART_NOT_FOUND; }
    }

    const char *use_label = (label != NULL) ? label : part->label;
    if (__nvs_find_store(use_label) != NULL) {
        // Already initialised – treat as success (matches ESP-IDF behaviour).
        return ESP_OK;
    }

    __nvs_store_t *store = __nvs_alloc_store();
    if (store == NULL) { return ESP_ERR_NO_MEM; }

    store->partition = part;
    strncpy(store->part_label, use_label, NVS_PART_NAME_MAX_SIZE - 1);

    return __nvs_load(store);
}

esp_err_t nvs_flash_init(void)
{
    return __nvs_init_partition(NVS_DEFAULT_PART_NAME, NULL);
}

esp_err_t nvs_flash_init_partition(const char *partition_label)
{
    return __nvs_init_partition(partition_label, NULL);
}

esp_err_t nvs_flash_init_partition_ptr(const esp_partition_t *partition)
{
    if (partition == NULL) { return ESP_ERR_INVALID_ARG; }
    return __nvs_init_partition(partition->label, partition);
}

static esp_err_t __nvs_deinit_partition(const char *label)
{
    if (label == NULL) { return ESP_ERR_INVALID_ARG; }
    __nvs_store_t *store = __nvs_find_store(label);
    if (store == NULL) { return ESP_ERR_NVS_NOT_INITIALIZED; }

    // Close all handles that reference this store.
    __nvs_handle_ctx_t *h = __nvs_handle_head;
    while (h != NULL) {
        __nvs_handle_ctx_t *next = h->next;
        if (h->store == store) {
            // Remove from list.
            __nvs_handle_ctx_t **pp = &__nvs_handle_head;
            while (*pp != NULL) {
                if (*pp == h) { *pp = h->next; break; }
                pp = &(*pp)->next;
            }
            vsf_heap_free(h);
        }
        h = next;
    }
    __nvs_free_all_kv(store);
    __nvs_free_store(store);
    return ESP_OK;
}

esp_err_t nvs_flash_deinit(void)
{
    return __nvs_deinit_partition(NVS_DEFAULT_PART_NAME);
}

esp_err_t nvs_flash_deinit_partition(const char *partition_label)
{
    return __nvs_deinit_partition(partition_label);
}

static esp_err_t __nvs_erase_partition_impl(const char *label,
                                            const esp_partition_t *part)
{
    if (label == NULL && part == NULL) { return ESP_ERR_INVALID_ARG; }
    const char *use_label = label ? label : part->label;

    // Deinit if currently initialised.
    __nvs_store_t *store = __nvs_find_store(use_label);
    if (store != NULL) { __nvs_deinit_partition(use_label); }

    // Lookup partition if not given.
    if (part == NULL) {
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                        ESP_PARTITION_SUBTYPE_DATA_NVS,
                                        use_label);
        if (part == NULL) { return ESP_ERR_NVS_PART_NOT_FOUND; }
    }

    // Zero-fill the partition to wipe the NVS image.
    // We avoid esp_partition_erase_range because mem_mal / file_mal may not
    // support erase; a zero-fill write is always safe and destroys the magic.
    uint8_t zeros[256];
    memset(zeros, 0, sizeof(zeros));
    for (uint32_t off = 0; off < part->size; off += sizeof(zeros)) {
        uint32_t chunk = part->size - off;
        if (chunk > sizeof(zeros)) { chunk = sizeof(zeros); }
        esp_err_t rc = esp_partition_write(part, off, zeros, chunk);
        if (rc != ESP_OK) { return rc; }
    }
    return ESP_OK;
}

esp_err_t nvs_flash_erase(void)
{
    return __nvs_erase_partition_impl(NVS_DEFAULT_PART_NAME, NULL);
}

esp_err_t nvs_flash_erase_partition(const char *part_name)
{
    return __nvs_erase_partition_impl(part_name, NULL);
}

esp_err_t nvs_flash_erase_partition_ptr(const esp_partition_t *partition)
{
    if (partition == NULL) { return ESP_ERR_INVALID_ARG; }
    return __nvs_erase_partition_impl(partition->label, partition);
}

// ---------------------------------------------------------------------------
//  nvs_open / close / commit
// ---------------------------------------------------------------------------

esp_err_t nvs_open(const char *namespace_name, nvs_open_mode_t open_mode,
                   nvs_handle_t *out_handle)
{
    return nvs_open_from_partition(NVS_DEFAULT_PART_NAME, namespace_name,
                                   open_mode, out_handle);
}

esp_err_t nvs_open_from_partition(const char *part_name,
                                  const char *namespace_name,
                                  nvs_open_mode_t open_mode,
                                  nvs_handle_t *out_handle)
{
    if (out_handle == NULL || !__nvs_validate_ns(namespace_name)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (part_name == NULL) { part_name = NVS_DEFAULT_PART_NAME; }

    __nvs_store_t *store = __nvs_find_store(part_name);
    if (store == NULL) { return ESP_ERR_NVS_NOT_INITIALIZED; }

    // Allocate a handle dynamically.
    __nvs_handle_ctx_t *ctx;
    ctx = (__nvs_handle_ctx_t *)vsf_heap_malloc(sizeof(*ctx));
    if (ctx == NULL) { return ESP_ERR_NO_MEM; }
    memset(ctx, 0, sizeof(*ctx));
    ctx->mode   = open_mode;
    ctx->store  = store;
    strncpy(ctx->ns, namespace_name, NVS_NS_NAME_MAX_SIZE - 1);
    ctx->ns[NVS_NS_NAME_MAX_SIZE - 1] = '\0';

    ctx->next = __nvs_handle_head;
    __nvs_handle_head = ctx;

    *out_handle = (nvs_handle_t)(uintptr_t)ctx;
    return ESP_OK;
}

void nvs_close(nvs_handle_t handle)
{
    __nvs_handle_ctx_t *ctx = __nvs_get_handle(handle);
    if (ctx == NULL) { return; }
    __nvs_handle_ctx_t **pp = &__nvs_handle_head;
    while (*pp != NULL) {
        if (*pp == ctx) { *pp = ctx->next; break; }
        pp = &(*pp)->next;
    }
    vsf_heap_free(ctx);
}

esp_err_t nvs_commit(nvs_handle_t handle)
{
    __nvs_handle_ctx_t *ctx = __nvs_get_handle(handle);
    if (ctx == NULL) { return ESP_ERR_NVS_INVALID_HANDLE; }
    return __nvs_flush(ctx->store);
}

// ---------------------------------------------------------------------------
//  Setters
// ---------------------------------------------------------------------------

static esp_err_t __nvs_set_typed(nvs_handle_t handle, const char *key,
                                 nvs_type_t type,
                                 const void *value, size_t size)
{
    __nvs_handle_ctx_t *ctx = __nvs_get_handle(handle);
    if (ctx == NULL) { return ESP_ERR_NVS_INVALID_HANDLE; }
    if (ctx->mode == NVS_READONLY) { return ESP_ERR_NVS_READ_ONLY; }
    if (!__nvs_validate_key(key)) { return ESP_ERR_NVS_KEY_TOO_LONG; }

    __nvs_kv_t *kv = __nvs_find_kv(ctx->store, ctx->ns, key);
    if (kv != NULL) {
        // Update existing – free old ext_data if type changed to inline.
        if (!__nvs_is_inline(kv->type) && kv->ext_data != NULL) {
            vsf_heap_free(kv->ext_data);
            kv->ext_data = NULL;
        }
    } else {
        // Create new node.
        kv = (__nvs_kv_t *)vsf_heap_malloc(sizeof(*kv));
        if (kv == NULL) { return ESP_ERR_NO_MEM; }
        memset(kv, 0, sizeof(*kv));
        strncpy(kv->ns,  ctx->ns, NVS_NS_NAME_MAX_SIZE - 1);
        strncpy(kv->key, key,     NVS_KEY_NAME_MAX_SIZE - 1);
        kv->next = ctx->store->kv_head;
        ctx->store->kv_head = kv;
    }

    kv->type      = type;
    kv->data_size = (uint32_t)size;
    if (__nvs_is_inline(type)) {
        memcpy(kv->inline_value, value, size);
    } else {
        kv->ext_data = vsf_heap_malloc(size);
        if (kv->ext_data == NULL) {
            __nvs_remove_kv(ctx->store, kv);
            return ESP_ERR_NO_MEM;
        }
        memcpy(kv->ext_data, value, size);
    }
    ctx->store->dirty = true;
    return ESP_OK;
}

#define __NVS_DEF_SET_INT(__suffix, __ctype, __nvs_type)                    \
esp_err_t nvs_set_##__suffix(nvs_handle_t handle, const char *key,          \
                             __ctype value)                                  \
{                                                                            \
    return __nvs_set_typed(handle, key, __nvs_type, &value, sizeof(value)); \
}

__NVS_DEF_SET_INT(i8,  int8_t,   NVS_TYPE_I8)
__NVS_DEF_SET_INT(u8,  uint8_t,  NVS_TYPE_U8)
__NVS_DEF_SET_INT(i16, int16_t,  NVS_TYPE_I16)
__NVS_DEF_SET_INT(u16, uint16_t, NVS_TYPE_U16)
__NVS_DEF_SET_INT(i32, int32_t,  NVS_TYPE_I32)
__NVS_DEF_SET_INT(u32, uint32_t, NVS_TYPE_U32)
__NVS_DEF_SET_INT(i64, int64_t,  NVS_TYPE_I64)
__NVS_DEF_SET_INT(u64, uint64_t, NVS_TYPE_U64)

esp_err_t nvs_set_str(nvs_handle_t handle, const char *key, const char *value)
{
    if (value == NULL) { return ESP_ERR_INVALID_ARG; }
    return __nvs_set_typed(handle, key, NVS_TYPE_STR,
                           value, strlen(value) + 1);
}

esp_err_t nvs_set_blob(nvs_handle_t handle, const char *key,
                       const void *value, size_t length)
{
    if (value == NULL && length > 0) { return ESP_ERR_INVALID_ARG; }
    return __nvs_set_typed(handle, key, NVS_TYPE_BLOB, value, length);
}

// ---------------------------------------------------------------------------
//  Getters
// ---------------------------------------------------------------------------

static esp_err_t __nvs_get_typed(nvs_handle_t handle, const char *key,
                                 nvs_type_t expected_type,
                                 void *out, size_t *length)
{
    __nvs_handle_ctx_t *ctx = __nvs_get_handle(handle);
    if (ctx == NULL) { return ESP_ERR_NVS_INVALID_HANDLE; }
    if (!__nvs_validate_key(key)) { return ESP_ERR_NVS_KEY_TOO_LONG; }

    __nvs_kv_t *kv = __nvs_find_kv(ctx->store, ctx->ns, key);
    if (kv == NULL) { return ESP_ERR_NVS_NOT_FOUND; }
    if (kv->type != expected_type) { return ESP_ERR_NVS_TYPE_MISMATCH; }

    if (__nvs_is_inline(expected_type)) {
        if (out == NULL) { return ESP_ERR_INVALID_ARG; }
        memcpy(out, kv->inline_value, kv->data_size);
        return ESP_OK;
    }

    // str / blob: length query when out is NULL or *length is 0.
    if (length == NULL) { return ESP_ERR_INVALID_ARG; }
    if (out == NULL || *length == 0) {
        *length = kv->data_size;
        return ESP_OK;
    }
    if (*length < kv->data_size) {
        *length = kv->data_size;
        return ESP_ERR_NVS_INVALID_LENGTH;
    }
    memcpy(out, kv->ext_data, kv->data_size);
    *length = kv->data_size;
    return ESP_OK;
}

#define __NVS_DEF_GET_INT(__suffix, __ctype, __nvs_type)                    \
esp_err_t nvs_get_##__suffix(nvs_handle_t handle, const char *key,          \
                             __ctype *out_value)                             \
{                                                                            \
    return __nvs_get_typed(handle, key, __nvs_type, out_value, NULL);       \
}

__NVS_DEF_GET_INT(i8,  int8_t,   NVS_TYPE_I8)
__NVS_DEF_GET_INT(u8,  uint8_t,  NVS_TYPE_U8)
__NVS_DEF_GET_INT(i16, int16_t,  NVS_TYPE_I16)
__NVS_DEF_GET_INT(u16, uint16_t, NVS_TYPE_U16)
__NVS_DEF_GET_INT(i32, int32_t,  NVS_TYPE_I32)
__NVS_DEF_GET_INT(u32, uint32_t, NVS_TYPE_U32)
__NVS_DEF_GET_INT(i64, int64_t,  NVS_TYPE_I64)
__NVS_DEF_GET_INT(u64, uint64_t, NVS_TYPE_U64)

esp_err_t nvs_get_str(nvs_handle_t handle, const char *key,
                      char *out_value, size_t *length)
{
    return __nvs_get_typed(handle, key, NVS_TYPE_STR, out_value, length);
}

esp_err_t nvs_get_blob(nvs_handle_t handle, const char *key,
                       void *out_value, size_t *length)
{
    return __nvs_get_typed(handle, key, NVS_TYPE_BLOB, out_value, length);
}

// ---------------------------------------------------------------------------
//  Erase key / all
// ---------------------------------------------------------------------------

esp_err_t nvs_erase_key(nvs_handle_t handle, const char *key)
{
    __nvs_handle_ctx_t *ctx = __nvs_get_handle(handle);
    if (ctx == NULL) { return ESP_ERR_NVS_INVALID_HANDLE; }
    if (ctx->mode == NVS_READONLY) { return ESP_ERR_NVS_READ_ONLY; }
    if (!__nvs_validate_key(key)) { return ESP_ERR_NVS_KEY_TOO_LONG; }

    __nvs_kv_t *kv = __nvs_find_kv(ctx->store, ctx->ns, key);
    if (kv == NULL) { return ESP_ERR_NVS_NOT_FOUND; }

    __nvs_remove_kv(ctx->store, kv);
    ctx->store->dirty = true;
    return ESP_OK;
}

esp_err_t nvs_erase_all(nvs_handle_t handle)
{
    __nvs_handle_ctx_t *ctx = __nvs_get_handle(handle);
    if (ctx == NULL) { return ESP_ERR_NVS_INVALID_HANDLE; }
    if (ctx->mode == NVS_READONLY) { return ESP_ERR_NVS_READ_ONLY; }

    __nvs_kv_t **pp = &ctx->store->kv_head;
    while (*pp != NULL) {
        if (strcmp((*pp)->ns, ctx->ns) == 0) {
            __nvs_kv_t *del = *pp;
            *pp = del->next;
            __nvs_free_kv(del);
        } else {
            pp = &(*pp)->next;
        }
    }
    ctx->store->dirty = true;
    return ESP_OK;
}

// ---------------------------------------------------------------------------
//  Stats
// ---------------------------------------------------------------------------

esp_err_t nvs_get_stats(const char *part_name, nvs_stats_t *nvs_stats)
{
    if (nvs_stats == NULL) { return ESP_ERR_INVALID_ARG; }
    if (part_name == NULL) { part_name = NVS_DEFAULT_PART_NAME; }
    __nvs_store_t *store = __nvs_find_store(part_name);
    if (store == NULL) { return ESP_ERR_NVS_NOT_INITIALIZED; }

    nvs_stats->used_entries    = __nvs_count_kv(store, NULL);
    nvs_stats->namespace_count = __nvs_count_ns(store);
    // free_entries / total_entries are synthetic: we report how many more
    // entries would fit assuming each uses ~48 bytes of partition space.
    size_t used_est = __NVS_HEADER_SIZE
                    + nvs_stats->used_entries * sizeof(__nvs_disk_entry_t)
                    + 4u;
    nvs_stats->total_entries = store->partition->size / sizeof(__nvs_disk_entry_t);
    nvs_stats->free_entries  = (used_est < store->partition->size)
                              ? (store->partition->size - used_est) / sizeof(__nvs_disk_entry_t)
                              : 0;
    nvs_stats->available_entries = nvs_stats->free_entries;
    return ESP_OK;
}

esp_err_t nvs_get_used_entry_count(nvs_handle_t handle, size_t *used_entries)
{
    if (used_entries == NULL) { return ESP_ERR_INVALID_ARG; }
    __nvs_handle_ctx_t *ctx = __nvs_get_handle(handle);
    if (ctx == NULL) { return ESP_ERR_NVS_INVALID_HANDLE; }
    *used_entries = __nvs_count_kv(ctx->store, ctx->ns);
    return ESP_OK;
}

// ---------------------------------------------------------------------------
//  Iterator
// ---------------------------------------------------------------------------

static bool __nvs_iter_matches(nvs_opaque_iterator_t *it, __nvs_kv_t *kv)
{
    if (it->ns_filter[0] != '\0' && strcmp(kv->ns, it->ns_filter) != 0) {
        return false;
    }
    if (it->type_filter != NVS_TYPE_ANY && kv->type != it->type_filter) {
        return false;
    }
    return true;
}

static __nvs_kv_t * __nvs_iter_advance(nvs_opaque_iterator_t *it,
                                        __nvs_kv_t *start)
{
    for (__nvs_kv_t *kv = start; kv != NULL; kv = kv->next) {
        if (__nvs_iter_matches(it, kv)) { return kv; }
    }
    return NULL;
}

esp_err_t nvs_entry_find(const char *part_name, const char *namespace_name,
                         nvs_type_t type, nvs_iterator_t *output_iterator)
{
    if (output_iterator == NULL) { return ESP_ERR_INVALID_ARG; }
    *output_iterator = NULL;
    if (part_name == NULL) { part_name = NVS_DEFAULT_PART_NAME; }

    __nvs_store_t *store = __nvs_find_store(part_name);
    if (store == NULL) { return ESP_ERR_NVS_NOT_INITIALIZED; }

    nvs_opaque_iterator_t *it =
        (nvs_opaque_iterator_t *)vsf_heap_malloc(sizeof(*it));
    if (it == NULL) { return ESP_ERR_NO_MEM; }
    memset(it, 0, sizeof(*it));
    it->store       = store;
    it->type_filter = type;
    if (namespace_name != NULL) {
        strncpy(it->ns_filter, namespace_name, NVS_NS_NAME_MAX_SIZE - 1);
    }

    it->cur = __nvs_iter_advance(it, store->kv_head);
    if (it->cur == NULL) {
        vsf_heap_free(it);
        return ESP_ERR_NVS_NOT_FOUND;
    }
    *output_iterator = it;
    return ESP_OK;
}

esp_err_t nvs_entry_next(nvs_iterator_t *iterator)
{
    if (iterator == NULL || *iterator == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    nvs_opaque_iterator_t *it = *iterator;
    if (it->cur == NULL) {
        nvs_release_iterator(it);
        *iterator = NULL;
        return ESP_ERR_NVS_NOT_FOUND;
    }
    it->cur = __nvs_iter_advance(it, it->cur->next);
    if (it->cur == NULL) {
        nvs_release_iterator(it);
        *iterator = NULL;
        return ESP_ERR_NVS_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t nvs_entry_info(const nvs_iterator_t iterator,
                         nvs_entry_info_t *out_info)
{
    if (iterator == NULL || out_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (iterator->cur == NULL) { return ESP_ERR_INVALID_ARG; }

    memset(out_info, 0, sizeof(*out_info));
    strncpy(out_info->namespace_name, iterator->cur->ns,
            NVS_NS_NAME_MAX_SIZE - 1);
    strncpy(out_info->key, iterator->cur->key, NVS_KEY_NAME_MAX_SIZE - 1);
    out_info->type = iterator->cur->type;
    return ESP_OK;
}

void nvs_release_iterator(nvs_iterator_t iterator)
{
    if (iterator != NULL) { vsf_heap_free(iterator); }
}

#endif      /* VSF_ESPIDF_CFG_USE_NVS */
