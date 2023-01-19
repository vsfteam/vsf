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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_DISTBUS == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_HAL_DISTBUS_CLASS_IMPLEMENT
#include "./driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static bool __vsf_hal_distbus_msghandler(vsf_distbus_t *bus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_distbus_service_info_t __vsf_hal_distbus_info = {
    .mtu                = VSF_HAL_DISTBUS_CFG_MTU,
    .type               = 0,
    .addr_range         = VSF_HAL_DISTBUS_CMD_ADDR_RANGE,
    .handler            = __vsf_hal_distbus_msghandler,
};

/*============================ IMPLEMENTATION ================================*/

WEAK(vsf_hal_distbus_on_new)
void vsf_hal_distbus_on_new(vsf_hal_distbus_t *hal_distbus, vsf_hal_distbus_type_t type, uint8_t num, void *devs)
{
}

static bool __vsf_hal_distbus_msghandler(vsf_distbus_t *distbus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    vsf_hal_distbus_t *hal_distbus = container_of(service, vsf_hal_distbus_t, service);
    uint8_t *data = (uint8_t *)&msg->header + sizeof(msg->header);
    uint32_t datalen = msg->header.datalen;
    union {
        void *ptr;
    } u_arg;
    union {
        void *ptr;
#if VSF_HAL_USE_IO == ENABLED && VSF_HAL_DISTBUS_USE_IO == ENABLED
        vsf_hal_distbus_io_t *io;
#endif
#if VSF_HAL_USE_GPIO == ENABLED && VSF_HAL_DISTBUS_USE_GPIO == ENABLED
        vsf_hal_distbus_gpio_t *gpio;
#endif
#if VSF_HAL_USE_I2C == ENABLED && VSF_HAL_DISTBUS_USE_I2C == ENABLED
        vsf_hal_distbus_i2c_t *i2c;
#endif
#if VSF_HAL_USE_SPI == ENABLED && VSF_HAL_DISTBUS_USE_SPI == ENABLED
        vsf_hal_distbus_spi_t *spi;
#endif
#if VSF_HAL_USE_USART == ENABLED && VSF_HAL_DISTBUS_USE_USART == ENABLED
        vsf_hal_distbus_usart_t *usart;
#endif
#if VSF_HAL_USE_MMC == ENABLED && VSF_HAL_DISTBUS_USE_MMC == ENABLED
        vsf_hal_distbus_mmc_t *mmc;
#endif
#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_DISTBUS_USE_ADC == ENABLED
        vsf_hal_distbus_adc_t *adc;
#endif
#if VSF_HAL_USE_DAC == ENABLED && VSF_HAL_DISTBUS_USE_DAC == ENABLED
        vsf_hal_distbus_dac_t *dac;
#endif
#if VSF_HAL_USE_PWM == ENABLED && VSF_HAL_DISTBUS_USE_PWM == ENABLED
        vsf_hal_distbus_pwm_t *pwm;
#endif
#if VSF_HAL_USE_I2S == ENABLED && VSF_HAL_DISTBUS_USE_I2S == ENABLED
        vsf_hal_distbus_i2s_t *i2s;
#endif
#if VSF_USE_USB_DEVICE == ENABLED && VSF_HAL_USE_USBD == ENABLED && VSF_HAL_DISTBUS_USE_USBD == ENABLED
        vsf_hal_distbus_usbd_t *usbd;
#endif
#if VSF_USE_USB_HOST == ENABLED && VSF_HAL_USE_USBH == ENABLED && VSF_HAL_DISTBUS_USE_USBH == ENABLED
        vsf_hal_distbus_usbh_t *usbh;
#endif
    } u_devs;
    uint8_t dev_type;
    uint8_t dev_num;

    switch (msg->header.addr) {
    case VSF_HAL_DISTBUS_CMD_CONNECT:
        // nothing to declare
        break;
    case VSF_HAL_DISTBUS_CMD_DECLARE:
        while (datalen > 0) {
            VSF_HAL_ASSERT(datalen >= 2);
            dev_type = *data++;
            dev_num = *data++;
            VSF_HAL_ASSERT(dev_num > 0);
            datalen -= 2;

            u_arg.ptr = data;
            switch (dev_type) {
            case VSF_HAL_DISTBUS_IO:
#if VSF_HAL_USE_IO == ENABLED && VSF_HAL_DISTBUS_USE_IO == ENABLED
                VSF_HAL_ASSERT(1 == dev_num);
                u_devs.io = vsf_heap_malloc(sizeof(vsf_hal_distbus_io_t));
                VSF_HAL_ASSERT(u_devs.io != NULL);
                hal_distbus->io.dev = u_devs.io;

                vsf_hal_distbus_io_register_service(hal_distbus->distbus, u_devs.io);
#else
                VSF_HAL_ASSERT(false);
#endif
            case VSF_HAL_DISTBUS_GPIO:
#if VSF_HAL_USE_GPIO == ENABLED && VSF_HAL_DISTBUS_USE_GPIO == ENABLED
                u_devs.gpio = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_gpio_t));
                VSF_HAL_ASSERT(u_devs.gpio != NULL);
                hal_distbus->gpio.dev_num = dev_num;
                hal_distbus->gpio.dev = u_devs.gpio;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_gpio_register_service(hal_distbus->distbus, &u_devs.gpio[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_I2C:
#if VSF_HAL_USE_I2C == ENABLED && VSF_HAL_DISTBUS_USE_I2C == ENABLED
                u_devs.i2c = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_i2c_t));
                VSF_HAL_ASSERT(u_devs.i2c != NULL);
                hal_distbus->i2c.dev_num = dev_num;
                hal_distbus->i2c.dev = u_devs.i2c;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_i2c_register_service(hal_distbus->distbus, &u_devs.i2c[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_SPI:
#if VSF_HAL_USE_SPI == ENABLED && VSF_HAL_DISTBUS_USE_SPI == ENABLED
                u_devs.spi = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_spi_t));
                VSF_HAL_ASSERT(u_devs.spi != NULL);
                hal_distbus->spi.dev_num = dev_num;
                hal_distbus->spi.dev = u_devs.spi;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_spi_register_service(hal_distbus->distbus, &u_devs.spi[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_USART:
#if VSF_HAL_USE_USART == ENABLED && VSF_HAL_DISTBUS_USE_USART == ENABLED
                u_devs.usart = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_usart_t));
                VSF_HAL_ASSERT(u_devs.usart != NULL);
                hal_distbus->usart.dev_num = dev_num;
                hal_distbus->usart.dev = u_devs.usart;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_usart_register_service(hal_distbus->distbus, &u_devs.usart[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_MMC:
#if VSF_HAL_USE_MMC == ENABLED && VSF_HAL_DISTBUS_USE_MMC == ENABLED
                u_devs.mmc = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_mmc_t));
                VSF_HAL_ASSERT(u_devs.mmc != NULL);
                hal_distbus->mmc.dev_num = dev_num;
                hal_distbus->mmc.dev = u_devs.mmc;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_mmc_register_service(hal_distbus->distbus, &u_devs.mmc[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_ADC:
#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_DISTBUS_USE_ADC == ENABLED
                u_devs.adc = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_adc_t));
                VSF_HAL_ASSERT(u_devs.adc != NULL);
                hal_distbus->adc.dev_num = dev_num;
                hal_distbus->adc.dev = u_devs.adc;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_adc_register_service(hal_distbus->distbus, &u_devs.adc[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_DAC:
#if VSF_HAL_USE_DAC == ENABLED && VSF_HAL_USE_DISTBUS_DAC == ENABLED
                u_devs.dac = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_dac_t));
                VSF_HAL_ASSERT(u_devs.dac != NULL);
                hal_distbus->dac.dev_num = dev_num;
                hal_distbus->dac.dev = u_devs.dac;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_dac_register_service(hal_distbus->distbus, &u_devs.dac[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_PWM:
#if VSF_HAL_USE_PWM == ENABLED && VSF_HAL_DISTBUS_USE_PWM == ENABLED
                u_devs.pwm = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_pwm_t));
                VSF_HAL_ASSERT(u_devs.pwm != NULL);
                hal_distbus->pwm.dev_num = dev_num;
                hal_distbus->pwm.dev = u_devs.pwm;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_pwm_register_service(hal_distbus->distbus, &u_devs.pwm[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_I2S:
#if VSF_HAL_USE_I2S == ENABLED && VSF_HAL_DISTBUS_USE_I2S == ENABLED
                u_devs.i2s = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_i2s_t));
                VSF_HAL_ASSERT(u_devs.i2s != NULL);
                hal_distbus->i2s.dev_num = dev_num;
                hal_distbus->i2s.dev = u_devs.i2s;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_i2s_register_service(hal_distbus->distbus, &u_devs.i2s[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_USBD:
#if VSF_USE_USB_DEVICE == ENABLED && VSF_HAL_USE_USBD == ENABLED && VSF_HAL_DISTBUS_USE_USBD == ENABLED
                u_devs.usbd = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_usbd_t));
                VSF_HAL_ASSERT(u_devs.usbd != NULL);
                hal_distbus->usbd.dev_num = dev_num;
                hal_distbus->usbd.dev = u_devs.usbd;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_usbd_register_service(hal_distbus->distbus, &u_devs.usbd[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_USBH:
#if VSF_USE_USB_HOST == ENABLED && VSF_HAL_USE_USBH == ENABLED && VSF_HAL_DISTBUS_USE_USBH == ENABLED
                u_devs.usbh = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_usbh_t));
                VSF_HAL_ASSERT(u_devs.usbh != NULL);
                hal_distbus->usbh.dev_num = dev_num;
                hal_distbus->usbh.dev = u_devs.usbh;

                for (uint8_t i = 0; i < dev_num; i++) {
                    vsf_hal_distbus_usbh_register_service(hal_distbus->distbus, &u_devs.usbh[i]);
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            default:
                VSF_HAL_ASSERT(false);
                break;
            }

            vsf_hal_distbus_on_new(hal_distbus, dev_type, dev_num, u_devs.ptr);
        }
        break;
    }
    return false;
}

void vsf_hal_distbus_register(vsf_distbus_t *distbus, vsf_hal_distbus_t *hal_distbus)
{
    hal_distbus->distbus = distbus;
    hal_distbus->service.info = &__vsf_hal_distbus_info;
    vsf_distbus_register_service(distbus, &hal_distbus->service);
}

void vsf_hal_distbus_start(vsf_hal_distbus_t *hal_distbus)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 0, NULL);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_CMD_CONNECT;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
}

#endif
/* EOF */
