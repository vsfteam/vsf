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

/*============================ INCLUDES ======================================*/

#include "./vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED

#include "./vsf_espidf.h"
#include "service/trace/vsf_trace.h"
#if (VSF_ESPIDF_CFG_USE_TIMER == ENABLED) || (VSF_ESPIDF_CFG_USE_EVENT == ENABLED)
#   include "esp_err.h"
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

// Internal singleton state. The ESP-IDF compatibility shim has no
// per-instance notion (esp_* APIs are global functions), so ownership
// of the state belongs to the shim itself -- users only hand in a cfg.
static struct {
    bool        is_inited;
#if VSF_HAL_USE_RNG == ENABLED
    vsf_rng_t  *rng;
#endif
#if VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED
    vk_usbh_t  *usbh;
#endif
#if VSF_ESPIDF_CFG_USE_USB_DEVICE == ENABLED
    vk_usbd_dev_t *usbd;
#endif
} __vsf_espidf = { 0 };

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_espidf_init(const vsf_espidf_cfg_t *cfg)
{
    // Idempotent: a second init is a no-op. cfg == NULL is treated as a
    // zero-initialised cfg so callers with no peripherals to inject can
    // simply call vsf_espidf_init(NULL).
    if (__vsf_espidf.is_inited) {
        return;
    }
#if VSF_HAL_USE_RNG == ENABLED
    __vsf_espidf.rng = (cfg != NULL) ? cfg->rng : NULL;
#endif
    __vsf_espidf.is_inited = true;

#if VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED
    __vsf_espidf.usbh = (cfg != NULL) ? cfg->usb_host.usbh : NULL;
#endif
#if VSF_ESPIDF_CFG_USE_USB_DEVICE == ENABLED
    __vsf_espidf.usbd = (cfg != NULL) ? cfg->usb_device.usbd : NULL;
#endif

    // Per-module init hooks. Only modules with visible init state are
    // chained here; esp_err/esp_log/esp_system/esp_ringbuf are stateless
    // from the sub-system perspective.
#if VSF_ESPIDF_CFG_USE_TIMER == ENABLED
    extern esp_err_t esp_timer_init(void);
    vsf_trace_info("vsf_espidf_init: before esp_timer_init"VSF_TRACE_CFG_LINEEND);
    (void)esp_timer_init();
    vsf_trace_info("vsf_espidf_init: after esp_timer_init"VSF_TRACE_CFG_LINEEND);
#endif
#if VSF_ESPIDF_CFG_USE_EVENT == ENABLED
    extern esp_err_t esp_event_loop_create_default(void);
    vsf_trace_info("vsf_espidf_init: before esp_event_loop_create_default"VSF_TRACE_CFG_LINEEND);
    (void)esp_event_loop_create_default();
    vsf_trace_info("vsf_espidf_init: after esp_event_loop_create_default"VSF_TRACE_CFG_LINEEND);
#endif
#if VSF_ESPIDF_CFG_USE_PARTITION == ENABLED
    extern void vsf_espidf_partition_init(const vsf_espidf_partition_cfg_t *cfg);
    vsf_trace_info("vsf_espidf_init: before partition_init"VSF_TRACE_CFG_LINEEND);
    vsf_espidf_partition_init((cfg != NULL) ? &cfg->partition : NULL);
    vsf_trace_info("vsf_espidf_init: after partition_init"VSF_TRACE_CFG_LINEEND);
#endif
#if VSF_ESPIDF_CFG_USE_ESP_FLASH == ENABLED
    extern void vsf_espidf_esp_flash_init(vk_mal_t *root_mal);
    vsf_trace_info("vsf_espidf_init: before esp_flash_init"VSF_TRACE_CFG_LINEEND);
    vsf_espidf_esp_flash_init(
#   if VSF_ESPIDF_CFG_USE_PARTITION == ENABLED
            (cfg != NULL) ? cfg->partition.root_mal : NULL
#   else
            NULL
#   endif
    );
    vsf_trace_info("vsf_espidf_init: after esp_flash_init"VSF_TRACE_CFG_LINEEND);
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER == ENABLED
    extern void vsf_espidf_gptimer_init(const vsf_espidf_gptimer_cfg_t *cfg);
    vsf_espidf_gptimer_init((cfg != NULL) ? &cfg->gptimer : NULL);
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_UART == ENABLED
    extern void vsf_espidf_uart_init(const vsf_espidf_uart_cfg_t *cfg);
    vsf_espidf_uart_init((cfg != NULL) ? &cfg->uart : NULL);
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_I2C == ENABLED
    extern void vsf_espidf_i2c_init(const vsf_espidf_i2c_cfg_t *cfg);
    vsf_espidf_i2c_init((cfg != NULL) ? &cfg->i2c : NULL);
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_SPI_MASTER == ENABLED
    extern void vsf_espidf_spi_init(const vsf_espidf_spi_cfg_t *cfg);
    vsf_espidf_spi_init((cfg != NULL) ? &cfg->spi : NULL);
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_LEDC == ENABLED
    extern void vsf_espidf_ledc_init(const vsf_espidf_ledc_cfg_t *cfg);
    vsf_espidf_ledc_init((cfg != NULL) ? &cfg->ledc : NULL);
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_ADC == ENABLED
    extern void vsf_espidf_adc_init(const vsf_espidf_adc_cfg_t *cfg);
    vsf_espidf_adc_init((cfg != NULL) ? &cfg->adc : NULL);
#endif
#if VSF_ESPIDF_CFG_USE_NETIF == ENABLED
    extern void vsf_espidf_netif_init(void);
    vsf_trace_info("vsf_espidf_init: before netif_init"VSF_TRACE_CFG_LINEEND);
    vsf_trace_info("vsf_espidf_init: VSF_OS_CFG_PRIORITY_NUM=%d VSF_ARCH_SWI_NUM=%d VSF_SWI_NUM=%d"VSF_TRACE_CFG_LINEEND,
                   (int)VSF_OS_CFG_PRIORITY_NUM, (int)VSF_ARCH_SWI_NUM, (int)VSF_SWI_NUM);
    vsf_espidf_netif_init();
    vsf_trace_info("vsf_espidf_init: after netif_init"VSF_TRACE_CFG_LINEEND);
#endif
#if VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED
    // USB Host is initialized lazily by the application via usb_host_install().
    // The caller-supplied vk_usbh_t (with user-selected HCD driver already set)
    // is stored during vsf_espidf_init() and consumed by usb_host_install().
#endif
#if VSF_ESPIDF_CFG_USE_USB_DEVICE == ENABLED
    // USB Device is initialized lazily by the application via usb_enable().
    // The caller-supplied vk_usbd_dev_t (with user-selected DCD driver already
    // set) is stored during vsf_espidf_init() and consumed by usb_enable().
#endif
    // TODO:
    //   vsf_espidf_nvs_init();
}

#if VSF_HAL_USE_RNG == ENABLED
vsf_rng_t * vsf_espidf_get_rng(void)
{
    return __vsf_espidf.rng;
}
#endif

#if VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED
vk_usbh_t * vsf_espidf_get_usbh(void)
{
    return __vsf_espidf.usbh;
}
#endif

#if VSF_ESPIDF_CFG_USE_USB_DEVICE == ENABLED
vk_usbd_dev_t * vsf_espidf_get_usbd(void)
{
    return __vsf_espidf.usbd;
}
#endif

#endif      // VSF_USE_ESPIDF
