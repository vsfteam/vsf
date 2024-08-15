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

#if VSF_HAL_DISTBUS_USE_IO == ENABLED && VSF_HAL_USE_IO == ENABLED
#   include "./io/vsf_hal_distbus_io.h"
#endif
#if VSF_HAL_DISTBUS_USE_GPIO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
#   include "./gpio/vsf_hal_distbus_gpio.h"
#endif
#if VSF_HAL_DISTBUS_USE_I2C == ENABLED && VSF_HAL_USE_I2C == ENABLED
#   include "./i2c/vsf_hal_distbus_i2c.h"
#endif
#if VSF_HAL_DISTBUS_USE_SPI == ENABLED && VSF_HAL_USE_SPI == ENABLED
#include "./spi/vsf_hal_distbus_spi.h"
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
#include "./usbh/vsf_hal_distbus_usbh.h"
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
    VSF_HAL_DISTBUS_IO = 0,
    VSF_HAL_DISTBUS_GPIO,
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
    VSF_HAL_DISTBUS_io      = VSF_HAL_DISTBUS_IO,
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

#ifdef __cplusplus
}
#endif

#undef __VSF_HAL_DISTBUS_CLASS_IMPLEMENT
#undef __VSF_HAL_DISTBUS_CLASS_INHERIT__

#endif
#endif
/* EOF */
