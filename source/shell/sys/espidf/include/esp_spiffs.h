#ifndef __ESP_SPIFFS_H__
#define __ESP_SPIFFS_H__

#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *base_path;
    const char *partition_label;
    size_t      max_files;
    int         format_if_mount_failed;
} esp_vfs_spiffs_conf_t;

static inline esp_err_t esp_vfs_spiffs_register(const esp_vfs_spiffs_conf_t *conf)
{
    (void)conf;
    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t esp_vfs_spiffs_unregister(const char *partition_label)
{
    (void)partition_label;
    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t esp_spiffs_format(const char *partition_label)
{
    (void)partition_label;
    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t esp_spiffs_info(const char *partition_label, size_t *total_bytes, size_t *used_bytes)
{
    (void)partition_label;
    if (total_bytes) *total_bytes = 0;
    if (used_bytes)  *used_bytes  = 0;
    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t esp_spiffs_check(const char *partition_label)
{
    (void)partition_label;
    return ESP_ERR_NOT_SUPPORTED;
}

#ifdef __cplusplus
}
#endif

#endif
