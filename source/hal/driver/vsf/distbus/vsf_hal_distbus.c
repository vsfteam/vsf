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
#include "hal/vsf_hal.h"

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

VSF_CAL_WEAK(vsf_hal_distbus_on_new)
void vsf_hal_distbus_on_new(vsf_hal_distbus_t *hal_distbus, vsf_hal_distbus_type_t type, uint8_t num, void *devs)
{
}

static bool __vsf_hal_distbus_msghandler(vsf_distbus_t *distbus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    vsf_hal_distbus_t *hal_distbus = vsf_container_of(service, vsf_hal_distbus_t, service);
    uint8_t *data = (uint8_t *)&msg->header + sizeof(msg->header);
    uint32_t datalen = msg->header.datalen, tmplen;
    union {
        void *ptr;
#define VSF_HAL_DISTBUS_DEFINE_PARAM(__TYPE)                                    \
        VSF_MCONNECT(vsf_hal_distbus_, __TYPE, _t) *__TYPE;

#define __VSF_HAL_DISTBUS_ENUM      VSF_HAL_DISTBUS_DEFINE_PARAM
#include "vsf_hal_distbus_enum.inc"
    } u_devs;
    uint8_t dev_type;
    uint8_t dev_num;

    switch (msg->header.addr) {
    case VSF_HAL_DISTBUS_CMD_CONNECT:
        msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 0, NULL);
        VSF_HAL_ASSERT(msg != NULL);

        msg->header.addr = VSF_HAL_DISTBUS_CMD_DECLARE;
        vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);

        if (!hal_distbus->remote_connected) {
            hal_distbus->remote_connected = true;
            if (hal_distbus->on_remote_connected != NULL) {
                hal_distbus->on_remote_connected(hal_distbus);
            }
        }
        break;
    case VSF_HAL_DISTBUS_CMD_DECLARE:
        if (hal_distbus->remote_declared) {
            break;
        }

        hal_distbus->remote_declared = true;
        while (datalen > 0) {
            VSF_HAL_ASSERT(datalen >= 2);
            dev_type = *data++;
            dev_num = *data++;
            VSF_HAL_ASSERT(dev_num > 0);
            datalen -= 2;

            switch (dev_type) {

#define VSF_HAL_DISTBUS_PROCESS_DECLARE_CMD(__TYPE)                             \
            case VSF_MCONNECT(VSF_HAL_DISTBUS_, __TYPE):                        \
                u_devs.__TYPE = vsf_heap_malloc(dev_num * sizeof(VSF_MCONNECT(vsf_hal_distbus_, __TYPE, _t)));\
                VSF_HAL_ASSERT(u_devs.__TYPE != NULL);                          \
                hal_distbus->__TYPE.dev = u_devs.__TYPE;                        \
                                                                                \
                for (uint8_t i = 0; i < dev_num; i++) {                         \
                    tmplen = VSF_MCONNECT(vsf_hal_distbus_, __TYPE, _register_service)(hal_distbus->distbus, &u_devs.__TYPE[i], data, datalen);\
                    VSF_HAL_ASSERT(datalen >= tmplen);                          \
                    data += tmplen;                                             \
                    datalen -= tmplen;                                          \
                }                                                               \
                break;

#define __VSF_HAL_DISTBUS_ENUM      VSF_HAL_DISTBUS_PROCESS_DECLARE_CMD
#include "vsf_hal_distbus_enum.inc"

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
    hal_distbus->remote_connected = false;
    hal_distbus->remote_declared = false;
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
