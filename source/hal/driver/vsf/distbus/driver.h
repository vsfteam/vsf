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

#include "./io/vsf_hal_distbus_io.h"
#include "./gpio/vsf_hal_distbus_gpio.h"
#include "./i2c/vsf_hal_distbus_i2c.h"
#include "./spi/vsf_hal_distbus_spi.h"
#include "./usart/vsf_hal_distbus_usart.h"
#include "./mmc/vsf_hal_distbus_mmc.h"
#include "./adc/vsf_hal_distbus_adc.h"
#include "./dac/vsf_hal_distbus_dac.h"
#include "./pwm/vsf_hal_distbus_pwm.h"
#include "./i2s/vsf_hal_distbus_i2s.h"
#include "./usbd/vsf_hal_distbus_usbd.h"
#include "./usbh/vsf_hal_distbus_usbh.h"

#if     defined(__VSF_HAL_DISTBUS_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_DISTBUS_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
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
    VSF_HAL_DISTBUS_MMC,
    VSF_HAL_DISTBUS_ADC,
    VSF_HAL_DISTBUS_DAC,
    VSF_HAL_DISTBUS_PWM,
    VSF_HAL_DISTBUS_I2S,
    VSF_HAL_DISTBUS_USBD,
    VSF_HAL_DISTBUS_USBH,
} vsf_hal_distbus_type_t;

#if defined(__VSF_HAL_DISTBUS_CLASS_INHERIT__) || defined(__VSF_HAL_DISTBUS_CLASS_IMPLEMENT)
typedef enum vsf_hal_distbus_cmd_t {
    VSF_HAL_DISTBUS_CMD_CONNECT = 0,
    VSF_HAL_DISTBUS_CMD_DECLARE,

    VSF_HAL_DISTBUS_CMD_ADDR_RANGE,
} vsf_hal_distbus_cmd_t;
#endif

vsf_class(vsf_hal_distbus_t) {
    protected_member(
        vsf_distbus_service_t       service;
        vsf_distbus_t               *distbus;

#if VSF_HAL_USE_IO == ENABLED && VSF_HAL_DISTBUS_USE_IO == ENABLED
        struct {
            vsf_hal_distbus_io_t    *dev;
        } io;
#endif
#if VSF_HAL_USE_GPIO == ENABLED && VSF_HAL_DISTBUS_USE_GPIO == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_gpio_t  *dev;
        } gpio;
#endif
#if VSF_HAL_USE_I2C == ENABLED && VSF_HAL_DISTBUS_USE_I2C == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_i2c_t   *dev;
        } i2c;
#endif
#if VSF_HAL_USE_SPI == ENABLED && VSF_HAL_DISTBUS_USE_SPI == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_spi_t   *dev;
        } spi;
#endif
#if VSF_HAL_USE_USART == ENABLED && VSF_HAL_DISTBUS_USE_USART == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_usart_t *dev;
        } usart;
#endif
#if VSF_HAL_USE_MMC == ENABLED && VSF_HAL_DISTBUS_USE_MMC == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_mmc_t   *dev;
        } mmc;
#endif
#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_DISTBUS_USE_ADC == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_adc_t   *dev;
        } adc;
#endif
#if VSF_HAL_USE_DAC == ENABLED && VSF_HAL_DISTBUS_USE_DAC == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_dac_t   *dev;
        } dac;
#endif
#if VSF_HAL_USE_PWM == ENABLED && VSF_HAL_DISTBUS_USE_PWM == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_pwm_t   *dev;
        } pwm;
#endif
#if VSF_HAL_USE_I2S == ENABLED && VSF_HAL_DISTBUS_USE_I2S == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_i2s_t   *dev;
        } i2s;
#endif
#if VSF_USE_USB_DEVICE == ENABLED && VSF_HAL_USE_USBD == ENABLED && VSF_HAL_DISTBUS_USE_USBD == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_usbd_t  *dev;
        } usbd;
#endif
#if VSF_USE_USB_HOST == ENABLED && VSF_HAL_USE_USBH == ENABLED && VSF_HAL_DISTBUS_USE_USBH == ENABLED
        struct {
            uint8_t                 dev_num;
            vsf_hal_distbus_usbh_t  *dev;
        } usbh;
#endif
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
