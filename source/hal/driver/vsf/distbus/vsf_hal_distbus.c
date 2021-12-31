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
    .mtu                = 8 + VSF_HAL_DISTBUS_CFG_MTU,
    .type               = 0,
    .addr_range         = VSF_HAL_DISTBUS_ADDR_RANGE,
    .handler            = __vsf_hal_distbus_msghandler,
};

/*============================ IMPLEMENTATION ================================*/

static bool __vsf_hal_distbus_msghandler(vsf_distbus_t *bus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    uint8_t addr = msg->header.addr;
    bool retain_msg = false;

#if VSF_HAL_USE_DISTBUS_USBD == ENABLED
    if ((addr >= VSF_HAL_DISTBUS_USBD_CMD_BEGIN) && (addr <= VSF_HAL_DISTBUS_USBD_CMD_END)) {
        extern bool __vsf_hal_distbus_usbd_msghandler(vsf_distbus_t *bus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);
        retain_msg = __vsf_hal_distbus_usbd_msghandler(bus, service, msg);
    }
#endif

    return retain_msg;
}

void vsf_hal_distbus_register_service(vsf_hal_distbus_t *hal_distbus)
{
    hal_distbus->service.info = &__vsf_hal_distbus_info;
    vsf_distbus_register_service(hal_distbus->distbus, &hal_distbus->service);
}

#endif
/* EOF */
