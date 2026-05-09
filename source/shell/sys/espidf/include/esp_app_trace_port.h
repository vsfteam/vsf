#ifndef ESP_APP_TRACE_PORT_H_
#define ESP_APP_TRACE_PORT_H_

#include "esp_app_trace_util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    esp_err_t (*init)(void *hw_data, const esp_apptrace_config_t *config);
    uint8_t *(*get_up_buffer)(void *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo);
    esp_err_t (*put_up_buffer)(void *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
    esp_err_t (*flush_up_buffer_nolock)(void *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo);
    esp_err_t (*flush_up_buffer)(void *hw_data, esp_apptrace_tmo_t *tmo);
    void (*down_buffer_config)(void *hw_data, uint8_t *buf, uint32_t size);
    uint8_t *(*get_down_buffer)(void *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo);
    esp_err_t (*put_down_buffer)(void *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
    bool (*host_is_connected)(void *hw_data);
    void (*set_header_size)(void *hw_data, esp_apptrace_header_size_t header_size);
} esp_apptrace_hw_t;

esp_apptrace_hw_t *esp_apptrace_virt_hw_get(void **data);

#ifdef __cplusplus
}
#endif

#endif
