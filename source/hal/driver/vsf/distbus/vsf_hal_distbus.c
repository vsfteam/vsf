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

enum {
    VSF_HAL_DISTBUS_CMD_CONNECT = 0,
    VSF_HAL_DISTBUS_CMD_DECLARE,

    VSF_HAL_DISTBUS_CMD_ADDR_RANGE,
};

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
    } u;
    uint8_t dev_type;
    uint8_t dev_num;
    void *devs;

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

            u.ptr = data;
            switch (dev_type) {
            case VSF_HAL_DISTBUS_GPIO:
                VSF_HAL_ASSERT(false);
                break;
            case VSF_HAL_DISTBUS_I2C:
                VSF_HAL_ASSERT(false);
                break;
            case VSF_HAL_DISTBUS_SPI:
                VSF_HAL_ASSERT(false);
                break;
            case VSF_HAL_DISTBUS_USART:
                VSF_HAL_ASSERT(false);
                break;
            case VSF_HAL_DISTBUS_MMC:
                VSF_HAL_ASSERT(false);
                break;
            case VSF_HAL_DISTBUS_ADC:
                VSF_HAL_ASSERT(false);
                break;
            case VSF_HAL_DISTBUS_PWM:
                VSF_HAL_ASSERT(false);
                break;
            case VSF_HAL_DISTBUS_USBD:
#if VSF_USE_USB_DEVICE == ENABLED && VSF_HAL_USE_DISTBUS_USBD == ENABLED
                {
                    vsf_hal_distbus_usbd_t *usbd = vsf_heap_malloc(dev_num * sizeof(vsf_hal_distbus_usbd_t));
                    VSF_HAL_ASSERT(usbd != NULL);

                    hal_distbus->usbd.dev_num = dev_num;
                    hal_distbus->usbd.dev = usbd;

                    for (uint8_t i = 0; i < dev_num; i++) {
                        vsf_hal_distbus_usbd_register_service(hal_distbus->distbus, &hal_distbus->usbd.dev[i]);
                    }
                    devs = usbd;
                }
#else
                VSF_HAL_ASSERT(false);
#endif
                break;
            case VSF_HAL_DISTBUS_USBH:
                VSF_HAL_ASSERT(false);
                break;
            }

            vsf_hal_distbus_on_new(hal_distbus, dev_type, dev_num, devs);
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

    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(distbus, 0, NULL);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_CMD_CONNECT;
    vsf_distbus_send_msg(distbus, &hal_distbus->service, msg);
}

#endif
/* EOF */
