/*============================ INCLUDES ======================================*/

#ifndef __VSF_ESPIDF_NVS_H__
#define __VSF_ESPIDF_NVS_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// ---------------------------------------------------------------------------
//  NVS-specific error codes  (ESP_ERR_NVS_BASE = 0x1100)
// ---------------------------------------------------------------------------

#define ESP_ERR_NVS_BASE                    0x1100
#define ESP_ERR_NVS_NOT_INITIALIZED         (ESP_ERR_NVS_BASE + 0x01)
#define ESP_ERR_NVS_NOT_FOUND               (ESP_ERR_NVS_BASE + 0x02)
#define ESP_ERR_NVS_TYPE_MISMATCH           (ESP_ERR_NVS_BASE + 0x03)
#define ESP_ERR_NVS_READ_ONLY               (ESP_ERR_NVS_BASE + 0x04)
#define ESP_ERR_NVS_NOT_ENOUGH_SPACE        (ESP_ERR_NVS_BASE + 0x05)
#define ESP_ERR_NVS_INVALID_NAME            (ESP_ERR_NVS_BASE + 0x06)
#define ESP_ERR_NVS_INVALID_HANDLE          (ESP_ERR_NVS_BASE + 0x07)
#define ESP_ERR_NVS_REMOVE_FAILED           (ESP_ERR_NVS_BASE + 0x08)
#define ESP_ERR_NVS_KEY_TOO_LONG            (ESP_ERR_NVS_BASE + 0x09)
#define ESP_ERR_NVS_PAGE_FULL               (ESP_ERR_NVS_BASE + 0x0a)
#define ESP_ERR_NVS_INVALID_STATE           (ESP_ERR_NVS_BASE + 0x0b)
#define ESP_ERR_NVS_INVALID_LENGTH          (ESP_ERR_NVS_BASE + 0x0c)
#define ESP_ERR_NVS_NO_FREE_PAGES           (ESP_ERR_NVS_BASE + 0x0d)
#define ESP_ERR_NVS_VALUE_TOO_LONG          (ESP_ERR_NVS_BASE + 0x0e)
#define ESP_ERR_NVS_PART_NOT_FOUND          (ESP_ERR_NVS_BASE + 0x0f)
#define ESP_ERR_NVS_NEW_VERSION_FOUND       (ESP_ERR_NVS_BASE + 0x10)
#define ESP_ERR_NVS_XTS_ENCR_FAILED         (ESP_ERR_NVS_BASE + 0x11)
#define ESP_ERR_NVS_XTS_DECR_FAILED         (ESP_ERR_NVS_BASE + 0x12)
#define ESP_ERR_NVS_XTS_CFG_FAILED          (ESP_ERR_NVS_BASE + 0x13)
#define ESP_ERR_NVS_XTS_CFG_NOT_FOUND       (ESP_ERR_NVS_BASE + 0x14)
#define ESP_ERR_NVS_ENCR_NOT_SUPPORTED      (ESP_ERR_NVS_BASE + 0x15)
#define ESP_ERR_NVS_KEYS_NOT_INITIALIZED    (ESP_ERR_NVS_BASE + 0x16)
#define ESP_ERR_NVS_CORRUPT_KEY_PART        (ESP_ERR_NVS_BASE + 0x17)
#define ESP_ERR_NVS_CONTENT_DIFFERS         (ESP_ERR_NVS_BASE + 0x18)
#define ESP_ERR_NVS_WRONG_ENCRYPTION        (ESP_ERR_NVS_BASE + 0x19)

// ---------------------------------------------------------------------------
//  Constants
// ---------------------------------------------------------------------------

#define NVS_DEFAULT_PART_NAME               "nvs"
#define NVS_PART_NAME_MAX_SIZE              16
#define NVS_KEY_NAME_MAX_SIZE               16
#define NVS_NS_NAME_MAX_SIZE                NVS_KEY_NAME_MAX_SIZE

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// ---------------------------------------------------------------------------
//  Handle
// ---------------------------------------------------------------------------

typedef uint32_t nvs_handle_t;

// ---------------------------------------------------------------------------
//  Open mode
// ---------------------------------------------------------------------------

typedef enum {
    NVS_READONLY            = 0,
    NVS_READWRITE           = 1,
} nvs_open_mode_t;

// ---------------------------------------------------------------------------
//  Value types (encoding matches ESP-IDF v5.x)
// ---------------------------------------------------------------------------

typedef enum {
    NVS_TYPE_U8             = 0x01,
    NVS_TYPE_I8             = 0x11,
    NVS_TYPE_U16            = 0x02,
    NVS_TYPE_I16            = 0x12,
    NVS_TYPE_U32            = 0x04,
    NVS_TYPE_I32            = 0x14,
    NVS_TYPE_U64            = 0x08,
    NVS_TYPE_I64            = 0x18,
    NVS_TYPE_STR            = 0x21,
    NVS_TYPE_BLOB           = 0x42,
    NVS_TYPE_ANY            = 0xFF,
} nvs_type_t;

// ---------------------------------------------------------------------------
//  Iterator entry info
// ---------------------------------------------------------------------------

typedef struct {
    char        namespace_name[NVS_NS_NAME_MAX_SIZE];
    char        key[NVS_KEY_NAME_MAX_SIZE];
    nvs_type_t  type;
} nvs_entry_info_t;

// ---------------------------------------------------------------------------
//  Opaque iterator handle
// ---------------------------------------------------------------------------

typedef struct nvs_opaque_iterator_t *nvs_iterator_t;

// ---------------------------------------------------------------------------
//  NVS statistics
// ---------------------------------------------------------------------------

typedef struct {
    size_t  used_entries;
    size_t  free_entries;
    size_t  available_entries;
    size_t  total_entries;
    size_t  namespace_count;
} nvs_stats_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

// ---------------------------------------------------------------------------
//  Open / close / commit
// ---------------------------------------------------------------------------

esp_err_t nvs_open(const char *namespace_name, nvs_open_mode_t open_mode,
                   nvs_handle_t *out_handle);
esp_err_t nvs_open_from_partition(const char *part_name,
                                  const char *namespace_name,
                                  nvs_open_mode_t open_mode,
                                  nvs_handle_t *out_handle);
void      nvs_close(nvs_handle_t handle);
esp_err_t nvs_commit(nvs_handle_t handle);

// ---------------------------------------------------------------------------
//  Setters  (integer / string / blob)
// ---------------------------------------------------------------------------

esp_err_t nvs_set_i8 (nvs_handle_t handle, const char *key, int8_t   value);
esp_err_t nvs_set_u8 (nvs_handle_t handle, const char *key, uint8_t  value);
esp_err_t nvs_set_i16(nvs_handle_t handle, const char *key, int16_t  value);
esp_err_t nvs_set_u16(nvs_handle_t handle, const char *key, uint16_t value);
esp_err_t nvs_set_i32(nvs_handle_t handle, const char *key, int32_t  value);
esp_err_t nvs_set_u32(nvs_handle_t handle, const char *key, uint32_t value);
esp_err_t nvs_set_i64(nvs_handle_t handle, const char *key, int64_t  value);
esp_err_t nvs_set_u64(nvs_handle_t handle, const char *key, uint64_t value);
esp_err_t nvs_set_str (nvs_handle_t handle, const char *key,
                       const char *value);
esp_err_t nvs_set_blob(nvs_handle_t handle, const char *key,
                       const void *value, size_t length);

// ---------------------------------------------------------------------------
//  Getters  (integer / string / blob)
//  For str/blob: if *length is 0, the actual required length is written back
//  to *length and the function returns ESP_OK with out_value untouched.
// ---------------------------------------------------------------------------

esp_err_t nvs_get_i8 (nvs_handle_t handle, const char *key, int8_t   *out_value);
esp_err_t nvs_get_u8 (nvs_handle_t handle, const char *key, uint8_t  *out_value);
esp_err_t nvs_get_i16(nvs_handle_t handle, const char *key, int16_t  *out_value);
esp_err_t nvs_get_u16(nvs_handle_t handle, const char *key, uint16_t *out_value);
esp_err_t nvs_get_i32(nvs_handle_t handle, const char *key, int32_t  *out_value);
esp_err_t nvs_get_u32(nvs_handle_t handle, const char *key, uint32_t *out_value);
esp_err_t nvs_get_i64(nvs_handle_t handle, const char *key, int64_t  *out_value);
esp_err_t nvs_get_u64(nvs_handle_t handle, const char *key, uint64_t *out_value);
esp_err_t nvs_get_str (nvs_handle_t handle, const char *key,
                       char *out_value, size_t *length);
esp_err_t nvs_get_blob(nvs_handle_t handle, const char *key,
                       void *out_value, size_t *length);

// ---------------------------------------------------------------------------
//  Erase
// ---------------------------------------------------------------------------

esp_err_t nvs_erase_key(nvs_handle_t handle, const char *key);
esp_err_t nvs_erase_all(nvs_handle_t handle);

// ---------------------------------------------------------------------------
//  Stats / used-entry-count
// ---------------------------------------------------------------------------

esp_err_t nvs_get_stats(const char *part_name, nvs_stats_t *nvs_stats);
esp_err_t nvs_get_used_entry_count(nvs_handle_t handle, size_t *used_entries);

// ---------------------------------------------------------------------------
//  Iterator
// ---------------------------------------------------------------------------

esp_err_t        nvs_entry_find(const char *part_name, const char *namespace_name,
                                nvs_type_t type, nvs_iterator_t *output_iterator);
esp_err_t        nvs_entry_next(nvs_iterator_t *iterator);
esp_err_t        nvs_entry_info(const nvs_iterator_t iterator,
                                nvs_entry_info_t *out_info);
void             nvs_release_iterator(nvs_iterator_t iterator);

#ifdef __cplusplus
}
#endif

#endif      /* __VSF_ESPIDF_NVS_H__ */
