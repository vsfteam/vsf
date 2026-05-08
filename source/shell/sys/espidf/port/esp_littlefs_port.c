/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*
 * esp_littlefs.h -> component/fs/driver/littlefs bridge.
 *
 * Each call to esp_vfs_littlefs_register() claims one slot in __lfs_slots,
 * looks up the partition identified by partition_label, allocates a
 * vk_lfs_info_t whose lfs_config is bound to the partition's vk_mal_t
 * (via vsf_lfs_bind_mal), and issues mount(base_path, &vk_lfs_op, info).
 *
 * VSF's lfs driver auto-formats a fresh volume on a failed lfs_mount
 * (see __vk_lfs_thread in vsf_littlefs.c), so callers that set
 * format_if_mount_failed get that behaviour implicitly. Callers that did
 * NOT set it and still expect mount to fail on a blank partition cannot
 * be served by the current lfs driver and see ESP_OK anyway -- this is a
 * documented limitation, not a bug.
 *
 * esp_littlefs_format() temporarily constructs a vk_lfs_info_t, performs
 * an explicit lfs_format via the underlying LittleFS API, and tears the
 * temporary down without leaving a mount in place.
 *
 * Threading: all APIs MUST be invoked from a vsf_thread_t context so
 * that the peda sub-calls inside mount()/umount() complete synchronously.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_LITTLEFS == ENABLED

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define __VSF_LITTLEFS_CLASS_IMPLEMENT
#include "component/fs/vsf_fs.h"
#include "component/fs/driver/littlefs/vsf_littlefs.h"
#include "component/mal/vsf_mal.h"
#include "service/heap/vsf_heap.h"

#include "lfs.h"
#include "lfs_port.h"

#include <sys/mount.h>

#include "../vsf_espidf.h"
#include "esp_partition.h"
#include "esp_littlefs.h"

#if !defined(VSF_USE_LINUX) || VSF_USE_LINUX != ENABLED
#   error "esp_littlefs port requires VSF_USE_LINUX == ENABLED"
#endif
#if !defined(VSF_USE_FS) || VSF_USE_FS != ENABLED \
    || !defined(VSF_FS_USE_LITTLEFS) || VSF_FS_USE_LITTLEFS != ENABLED
#   error "esp_littlefs port requires VSF_FS_USE_LITTLEFS == ENABLED"
#endif
#if !defined(VSF_USE_LITTLEFS) || VSF_USE_LITTLEFS != ENABLED
#   error "esp_littlefs port requires VSF_USE_LITTLEFS == ENABLED"
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_ESPIDF_CFG_LITTLEFS_MAX_INSTANCES
#   define VSF_ESPIDF_CFG_LITTLEFS_MAX_INSTANCES    4
#endif

/*============================ TYPES =========================================*/

typedef struct __lfs_slot_t {
    bool             in_use;
    char             base_path[32];
    char             partition_label[16 + 1];
    uint32_t         partition_size;
    vk_lfs_info_t   *fsinfo;
} __lfs_slot_t;

/*============================ LOCAL VARIABLES ===============================*/

static __lfs_slot_t __lfs_slots[VSF_ESPIDF_CFG_LITTLEFS_MAX_INSTANCES];

/*============================ PROTOTYPES ====================================*/

static __lfs_slot_t * __lfs_slot_find(const char *partition_label);
static __lfs_slot_t * __lfs_slot_alloc(void);
static void           __lfs_slot_free(__lfs_slot_t *slot);

/*============================ IMPLEMENTATION ================================*/

static __lfs_slot_t * __lfs_slot_find(const char *partition_label)
{
    if (partition_label == NULL) {
        return NULL;
    }
    for (uint16_t i = 0; i < VSF_ESPIDF_CFG_LITTLEFS_MAX_INSTANCES; i++) {
        if (    __lfs_slots[i].in_use
            &&  (strncmp(__lfs_slots[i].partition_label, partition_label,
                         sizeof(__lfs_slots[i].partition_label)) == 0)) {
            return &__lfs_slots[i];
        }
    }
    return NULL;
}

static __lfs_slot_t * __lfs_slot_alloc(void)
{
    for (uint16_t i = 0; i < VSF_ESPIDF_CFG_LITTLEFS_MAX_INSTANCES; i++) {
        if (!__lfs_slots[i].in_use) {
            return &__lfs_slots[i];
        }
    }
    return NULL;
}

static void __lfs_slot_free(__lfs_slot_t *slot)
{
    if (slot->fsinfo != NULL) {
        vsf_heap_free(slot->fsinfo);
        slot->fsinfo = NULL;
    }
    memset(slot, 0, sizeof(*slot));
}

esp_err_t esp_vfs_littlefs_register(const esp_vfs_littlefs_conf_t *conf)
{
    if (    (conf == NULL)
        ||  (conf->base_path == NULL)
        ||  (conf->base_path[0] != '/')
        ||  (conf->partition_label == NULL)
        ||  (conf->partition_label[0] == '\0')) {
        return ESP_ERR_INVALID_ARG;
    }
    if (__lfs_slot_find(conf->partition_label) != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    const esp_partition_t *part = esp_partition_find_first(
            ESP_PARTITION_TYPE_ANY,
            ESP_PARTITION_SUBTYPE_ANY,
            conf->partition_label);
    if (part == NULL) {
        return ESP_ERR_NOT_FOUND;
    }
    if ((part->mal == NULL) || (part->size == 0)) {
        return ESP_ERR_INVALID_STATE;
    }

    __lfs_slot_t *slot = __lfs_slot_alloc();
    if (slot == NULL) {
        return ESP_ERR_NO_MEM;
    }

    vk_lfs_info_t *fsinfo = vsf_heap_calloc(1, sizeof(vk_lfs_info_t));
    if (fsinfo == NULL) {
        return ESP_ERR_NO_MEM;
    }
    if (vsf_lfs_bind_mal(&fsinfo->config, (vk_mal_t *)part->mal) != 0) {
        vsf_heap_free(fsinfo);
        return ESP_FAIL;
    }

    memset(slot, 0, sizeof(*slot));
    slot->in_use = true;
    strncpy(slot->base_path, conf->base_path, sizeof(slot->base_path) - 1);
    strncpy(slot->partition_label, conf->partition_label,
            sizeof(slot->partition_label) - 1);
    slot->partition_size = part->size;
    slot->fsinfo         = fsinfo;

    // The "dont_mount" flag leaves the slot reserved with a prepared
    // fsinfo but no POSIX mount point, matching joltwallet's semantics.
    if (conf->dont_mount) {
        return ESP_OK;
    }

    (void)mkdir(slot->base_path, 0777);
    if (mount(NULL, slot->base_path, &vk_lfs_op, 0, fsinfo) != 0) {
        (void)rmdir(slot->base_path);
        __lfs_slot_free(slot);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_vfs_littlefs_unregister(const char *partition_label)
{
    __lfs_slot_t *slot = __lfs_slot_find(partition_label);
    if (slot == NULL) {
        return ESP_ERR_NOT_FOUND;
    }
    // umount is a best-effort: if it fails (e.g. dont_mount instance),
    // we still clean up the slot so the caller can re-register.
    (void)umount(slot->base_path);
    (void)rmdir(slot->base_path);
    __lfs_slot_free(slot);
    return ESP_OK;
}

esp_err_t esp_littlefs_format(const char *partition_label)
{
    if ((partition_label == NULL) || (partition_label[0] == '\0')) {
        return ESP_ERR_INVALID_ARG;
    }
    if (__lfs_slot_find(partition_label) != NULL) {
        // ESP-IDF requires the volume to be unregistered first.
        return ESP_ERR_INVALID_STATE;
    }

    const esp_partition_t *part = esp_partition_find_first(
            ESP_PARTITION_TYPE_ANY,
            ESP_PARTITION_SUBTYPE_ANY,
            partition_label);
    if (part == NULL) {
        return ESP_ERR_NOT_FOUND;
    }
    if ((part->mal == NULL) || (part->size == 0)) {
        return ESP_ERR_INVALID_STATE;
    }

    vk_lfs_info_t *fsinfo = vsf_heap_calloc(1, sizeof(vk_lfs_info_t));
    if (fsinfo == NULL) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t result = ESP_OK;
    if (vsf_lfs_bind_mal(&fsinfo->config, (vk_mal_t *)part->mal) != 0) {
        result = ESP_FAIL;
    } else {
#ifndef LFS_READONLY
        int rc = lfs_format(&fsinfo->lfs, &fsinfo->config);
        if (rc != LFS_ERR_OK) {
            result = ESP_FAIL;
        }
#else
        result = ESP_ERR_NOT_SUPPORTED;
#endif
    }
    vsf_heap_free(fsinfo);
    return result;
}

esp_err_t esp_littlefs_info(const char *partition_label,
                            size_t *total_bytes,
                            size_t *used_bytes)
{
    __lfs_slot_t *slot = __lfs_slot_find(partition_label);
    if (slot == NULL) {
        return ESP_ERR_NOT_FOUND;
    }
    if (total_bytes != NULL) {
        *total_bytes = slot->partition_size;
    }
    if (used_bytes != NULL) {
        // VSF's lfs port does not expose a cheap used-block counter.
        // Reported as 0 until a scanning helper lands; see header.
        *used_bytes = 0;
    }
    return ESP_OK;
}

bool esp_littlefs_mounted(const char *partition_label)
{
    return __lfs_slot_find(partition_label) != NULL;
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_LITTLEFS
