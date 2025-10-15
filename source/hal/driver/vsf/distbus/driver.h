/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#ifndef __HAL_DRIVER_VSF_DISTBUS_H__
#define __HAL_DRIVER_VSF_DISTBUS_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_USE_DISTBUS == ENABLED && VSF_HAL_USE_DISTBUS == ENABLED

// for distbus
#include "service/vsf_service.h"

#ifdef VSF_HAL_DISTBUS_AS_REAL_DRIVER
#   define VSF_HAL_DISTBUS_ENUM(__ELEMENT)          __ELEMENT
#   define VSF_HAL_DISTBUS_PREFIX                   vsf_
#else
#   define VSF_HAL_DISTBUS_ENUM(__ELEMENT)          VSF_HAL_DISTBUS_ ## __ELEMENT
#   define VSF_HAL_DISTBUS_PREFIX                   vsf_hal_distbus_
#endif

#if VSF_HAL_DISTBUS_USE_GPIO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
#   include "./gpio/vsf_hal_distbus_gpio.h"
#endif
#if VSF_HAL_DISTBUS_USE_I2C == ENABLED && VSF_HAL_USE_I2C == ENABLED
#   include "./i2c/vsf_hal_distbus_i2c.h"
#endif
#if VSF_HAL_DISTBUS_USE_SPI == ENABLED && VSF_HAL_USE_SPI == ENABLED
#   include "./spi/vsf_hal_distbus_spi.h"
#endif
#if VSF_HAL_DISTBUS_USE_USART == ENABLED && VSF_HAL_USE_USART == ENABLED
#   include "./usart/vsf_hal_distbus_usart.h"
#endif
#if VSF_HAL_DISTBUS_USE_SDIO == ENABLED && VSF_HAL_USE_SDIO == ENABLED
#   include "./sdio/vsf_hal_distbus_sdio.h"
#endif
#if VSF_HAL_DISTBUS_USE_ADC == ENABLED && VSF_HAL_USE_ADC == ENABLED
#   include "./adc/vsf_hal_distbus_adc.h"
#endif
#if VSF_HAL_DISTBUS_USE_DAC == ENABLED && VSF_HAL_USE_DAC == ENABLED
#   include "./dac/vsf_hal_distbus_dac.h"
#endif
#if VSF_HAL_DISTBUS_USE_PWM == ENABLED && VSF_HAL_USE_PWM == ENABLED
#   include "./pwm/vsf_hal_distbus_pwm.h"
#endif
#if VSF_HAL_DISTBUS_USE_I2S == ENABLED && VSF_HAL_USE_I2S == ENABLED
#   include "./i2s/vsf_hal_distbus_i2s.h"
#endif
#if VSF_HAL_DISTBUS_USE_USBD == ENABLED && VSF_HAL_USE_USBD == ENABLED
#   include "./usbd/vsf_hal_distbus_usbd.h"
#endif
#if VSF_HAL_DISTBUS_USE_USBH == ENABLED && VSF_HAL_USE_USBH == ENABLED
#   include "./usbh/vsf_hal_distbus_usbh.h"
#endif

#undef PUBLIC_CONST
#if     defined(__VSF_HAL_DISTBUS_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#   define PUBLIC_CONST
#elif   defined(__VSF_HAL_DISTBUS_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#   define PUBLIC_CONST
#else
#   define PUBLIC_CONST                     const
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_DISTBUS_CFG_MTU
#   define VSF_HAL_DISTBUS_CFG_MTU          512
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_hal_distbus_type_t {
    VSF_HAL_DISTBUS_GPIO = 0,
    VSF_HAL_DISTBUS_I2C,
    VSF_HAL_DISTBUS_SPI,
    VSF_HAL_DISTBUS_USART,
    VSF_HAL_DISTBUS_SDIO,
    VSF_HAL_DISTBUS_ADC,
    VSF_HAL_DISTBUS_DAC,
    VSF_HAL_DISTBUS_PWM,
    VSF_HAL_DISTBUS_I2S,
    VSF_HAL_DISTBUS_USBD,
    VSF_HAL_DISTBUS_USBH,

    // for MACRO usage only
    VSF_HAL_DISTBUS_gpio    = VSF_HAL_DISTBUS_GPIO,
    VSF_HAL_DISTBUS_i2c     = VSF_HAL_DISTBUS_I2C,
    VSF_HAL_DISTBUS_spi     = VSF_HAL_DISTBUS_SPI,
    VSF_HAL_DISTBUS_usart   = VSF_HAL_DISTBUS_USART,
    VSF_HAL_DISTBUS_sdio    = VSF_HAL_DISTBUS_SDIO,
    VSF_HAL_DISTBUS_adc     = VSF_HAL_DISTBUS_ADC,
    VSF_HAL_DISTBUS_dac     = VSF_HAL_DISTBUS_DAC,
    VSF_HAL_DISTBUS_pwm     = VSF_HAL_DISTBUS_PWM,
    VSF_HAL_DISTBUS_i2s     = VSF_HAL_DISTBUS_I2S,
    VSF_HAL_DISTBUS_usbd    = VSF_HAL_DISTBUS_USBD,
    VSF_HAL_DISTBUS_usbh    = VSF_HAL_DISTBUS_USBH,
} vsf_hal_distbus_type_t;

#if defined(__VSF_HAL_DISTBUS_CLASS_INHERIT__) || defined(__VSF_HAL_DISTBUS_CLASS_IMPLEMENT)
typedef enum vsf_hal_distbus_cmd_t {
    VSF_HAL_DISTBUS_CMD_CONNECT = 0,
    VSF_HAL_DISTBUS_CMD_DECLARE,

    VSF_HAL_DISTBUS_CMD_ADDR_RANGE,
} vsf_hal_distbus_cmd_t;
#endif

typedef struct __vsf_hal_distbus_enum_t {
#define VSF_HAL_DISTBUS_DEFINE(__TYPE)                                          \
        struct {                                                                \
            uint8_t                 dev_num;                                    \
            VSF_MCONNECT(vsf_hal_distbus_, __TYPE, _t) *dev;                    \
        } __TYPE;

#define __VSF_HAL_DISTBUS_ENUM      VSF_HAL_DISTBUS_DEFINE
#include "vsf_hal_distbus_enum.inc"
#undef VSF_HAL_DISTBUS_DEFINE
} __vsf_hal_distbus_enum_t;

vsf_class(vsf_hal_distbus_t) {
    public_member(
        implement(__vsf_hal_distbus_enum_t)
        void (*on_remote_connected)(vsf_hal_distbus_t *hal_distbus);
        PUBLIC_CONST bool           remote_connected;
        PUBLIC_CONST bool           remote_declared;
    )
    protected_member(
        vsf_distbus_service_t       service;
        vsf_distbus_t               *distbus;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_hal_distbus_register(vsf_distbus_t *distbus, vsf_hal_distbus_t *hal_distbus);
extern void vsf_hal_distbus_start(vsf_hal_distbus_t *hal_distbus);

extern bool vsf_hal_distbus_on_irq(void *devs, uint16_t irqn);

/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_GPIO == ENABLED
#   include "hal/driver/common/template/vsf_template_gpio.h"

#   define VSF_GPIO_CFG_DEC_PREFIX                          vsf_hal_distbus
#   define VSF_GPIO_CFG_DEC_UPCASE_PREFIX                   VSF_HAL_DISTBUS
#   include "hal/driver/common/gpio/gpio_template.h"
#endif

#if VSF_HAL_USE_ADC == ENABLED
#   include "hal/driver/common/template/vsf_template_adc.h"

#   define VSF_ADC_CFG_DEC_PREFIX                           vsf_hal_distbus
#   define VSF_ADC_CFG_DEC_UPCASE_PREFIX                    VSF_HAL_DISTBUS
#   include "hal/driver/common/adc/adc_template.h"
#endif

#if VSF_HAL_USE_FLASH == ENABLED
#   include "hal/driver/common/template/vsf_template_flash.h"

#   define VSF_FLASH_CFG_DEC_PREFIX                         vsf_hal_distbus
#   define VSF_FLASH_CFG_DEC_UPCASE_PREFIX                  VSF_HAL_DISTBUS
#   include "hal/driver/common/flash/flash_template.h"
#endif

#if VSF_HAL_USE_I2C == ENABLED
#   include "hal/driver/common/template/vsf_template_i2c.h"

#   define VSF_I2C_CFG_DEC_PREFIX                           vsf_hal_distbus
#   define VSF_I2C_CFG_DEC_UPCASE_PREFIX                    VSF_HAL_DISTBUS
#   include "hal/driver/common/i2c/i2c_template.h"
#endif

#if VSF_HAL_USE_PWM == ENABLED
#   include "hal/driver/common/template/vsf_template_pwm.h"

#   define VSF_PWM_CFG_DEC_PREFIX                           vsf_hal_distbus
#   define VSF_PWM_CFG_DEC_UPCASE_PREFIX                    VSF_HAL_DISTBUS
#   include "hal/driver/common/pwm/pwm_template.h"
#endif

#if VSF_HAL_USE_RTC == ENABLED
#   include "hal/driver/common/template/vsf_template_rtc.h"

#   define VSF_RTC_CFG_DEC_PREFIX                           vsf_hal_distbus
#   define VSF_RTC_CFG_DEC_UPCASE_PREFIX                    VSF_HAL_DISTBUS
#   include "hal/driver/common/rtc/rtc_template.h"
#endif

#if VSF_HAL_USE_SPI == ENABLED
#   include "hal/driver/common/template/vsf_template_spi.h"

#   define VSF_SPI_CFG_DEC_PREFIX                           vsf_hal_distbus
#   define VSF_SPI_CFG_DEC_UPCASE_PREFIX                    VSF_HAL_DISTBUS
#   include "hal/driver/common/spi/spi_template.h"

// Optional: Add QSPI
//#   define VSF_SPI_CFG_DEC_DEVICE_PREFIX                  vsf_hal_distbus_qspi
//#   define VSF_SPI_CFG_DEC_DEVICE_UPCASE_PREFIX           VSF_HAL_DISTBUS_QSPI
//#   define VSF_SPI_CFG_DEC_RENAME_DEVICE_PREFIX           ENABLED
//#   include "hal/driver/common/spi/spi_template.h"
#endif

#if VSF_HAL_USE_SDIO == ENABLED
#   include "hal/driver/common/template/vsf_template_sdio.h"

#   define VSF_SDIO_CFG_DEC_PREFIX                          vsf_hal_distbus
#   define VSF_SDIO_CFG_DEC_UPCASE_PREFIX                   VSF_HAL_DISTBUS
#   include "hal/driver/common/sdio/sdio_template.h"
#endif

#if VSF_HAL_USE_TIMER == ENABLED
#   include "hal/driver/common/template/vsf_template_timer.h"

#   define VSF_TIMER_CFG_DEC_PREFIX                         vsf_hal_distbus
#   define VSF_TIMER_CFG_DEC_UPCASE_PREFIX                  VSF_HAL_DISTBUS
#   include "hal/driver/common/timer/timer_template.h"
#endif

#if VSF_HAL_USE_RNG == ENABLED
#   include "hal/driver/common/template/vsf_template_rng.h"

#   define VSF_RNG_CFG_DEC_PREFIX                           vsf_hal_distbus
#   define VSF_RNG_CFG_DEC_UPCASE_PREFIX                    VSF_HAL_DISTBUS
#   include "hal/driver/common/rng/rng_template.h"
#endif

#if VSF_HAL_USE_USART == ENABLED
#   include "hal/driver/common/template/vsf_template_usart.h"

#   define VSF_USART_CFG_DEC_PREFIX                         vsf_hal_distbus
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX                  VSF_HAL_DISTBUS
#   include "hal/driver/common/usart/usart_template.h"
#endif

#if VSF_HAL_USE_WDT == ENABLED
#   include "hal/driver/common/template/vsf_template_wdt.h"

#   define VSF_WDT_CFG_DEC_PREFIX                           vsf_hal_distbus
#   define VSF_WDT_CFG_DEC_UPCASE_PREFIX                    VSF_HAL_DISTBUS
#   include "hal/driver/common/wdt/wdt_template.h"
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_HAL_DISTBUS_CLASS_IMPLEMENT
#undef __VSF_HAL_DISTBUS_CLASS_INHERIT__

#endif
#endif
/* EOF */
