#ifndef __ESP_SLEEP_H__
#define __ESP_SLEEP_H__

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_sleep_enable_timer_wakeup(uint64_t time_in_us);
esp_err_t esp_light_sleep_start(void);
esp_err_t esp_deep_sleep_start(void);
esp_err_t esp_sleep_pd_config(int domain, int option);

#ifdef __cplusplus
}
#endif

#endif
