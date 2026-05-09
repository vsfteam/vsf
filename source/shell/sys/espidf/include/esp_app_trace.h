#ifndef __ESP_APP_TRACE_H__
#define __ESP_APP_TRACE_H__

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "esp_app_trace_config.h"
#include "esp_app_trace_util.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_apptrace_init(const esp_apptrace_config_t *config);
esp_err_t esp_apptrace_down_buffer_config(uint8_t *buf, uint32_t size);
uint8_t *esp_apptrace_buffer_get(uint32_t size, uint32_t tmo);
esp_err_t esp_apptrace_buffer_put(uint8_t *ptr, uint32_t tmo);
esp_err_t esp_apptrace_write(const void *data, uint32_t size, uint32_t tmo);
int esp_apptrace_vprintf_to(uint32_t tmo, const char *fmt, va_list ap);
int esp_apptrace_vprintf(const char *fmt, va_list ap);
esp_err_t esp_apptrace_flush(uint32_t tmo);
esp_err_t esp_apptrace_flush_nolock(uint32_t min_sz, uint32_t tmo);
esp_err_t esp_apptrace_read(void *data, uint32_t *size, uint32_t tmo);
uint8_t *esp_apptrace_down_buffer_get(uint32_t *size, uint32_t tmo);
esp_err_t esp_apptrace_down_buffer_put(uint8_t *ptr, uint32_t tmo);
bool esp_apptrace_host_is_connected(void);
esp_apptrace_dest_t esp_apptrace_get_destination(void);
esp_err_t esp_apptrace_set_header_size(esp_apptrace_header_size_t header_size);

#ifdef __cplusplus
}
#endif

#endif
