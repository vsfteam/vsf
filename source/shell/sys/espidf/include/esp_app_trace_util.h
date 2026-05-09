#ifndef ESP_APP_TRACE_UTIL_H_
#define ESP_APP_TRACE_UTIL_H_

#include "esp_err.h"
#include "esp_app_trace_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_APPTRACE_TMO_INFINITE               ((uint32_t)-1)

void esp_apptrace_tmo_init(esp_apptrace_tmo_t *tmo, uint32_t user_tmo);
esp_err_t esp_apptrace_tmo_check(esp_apptrace_tmo_t *tmo);
uint32_t esp_apptrace_tmo_remaining_us(esp_apptrace_tmo_t *tmo);

void esp_apptrace_rb_init(esp_apptrace_rb_t *rb, uint8_t *data, uint32_t size);
uint8_t *esp_apptrace_rb_produce(esp_apptrace_rb_t *rb, uint32_t size);
uint8_t *esp_apptrace_rb_consume(esp_apptrace_rb_t *rb, uint32_t size);
uint32_t esp_apptrace_rb_read_size_get(esp_apptrace_rb_t *rb);
uint32_t esp_apptrace_rb_write_size_get(esp_apptrace_rb_t *rb);

#ifdef __cplusplus
}
#endif

#endif
