#include "vsf.h"
#include <assert.h>
#include "esp_log.h"
#include "FreeRTOS.h"

void vsf_assert(int expression)
{
    if (VSF_ARCH_FREERTOS_CFG_IS_IN_ISR()) {
        if (!expression) {
            ESP_EARLY_LOGE(__FUNCTION__, "assert failed in isr !!!");
            while(1);
        }
        return;
    }
    assert(expression);
}

void app_main(void)
{
    vsf_freertos_start();
}
