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

#if VSF_HAL_USE_DISTBUS == ENABLED

// for distbus
#include "service/vsf_service.h"

#if VSF_HAL_USE_DISTBUS_USBD == ENABLED
#   include "./usbd/vsf_hal_distbus_usbd.h"
#endif

#if     defined(__VSF_HAL_DISTBUS_CLASS_IMPLEMENT)
#   undef __VSF_HAL_DISTBUS_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_DISTBUS_CLASS_INHERIT__)
#   undef __VSF_HAL_DISTBUS_CLASS_INHERIT__
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

enum {
#if VSF_HAL_USE_DISTBUS_USBD == ENABLED
    VSF_HAL_DISTBUS_USBD_CMD_BEGIN,
    VSF_HAL_DISTBUS_USBD_CMD_INIT = VSF_HAL_DISTBUS_USBD_CMD_BEGIN,
    VSF_HAL_DISTBUS_USBD_CMD_FINI,
    VSF_HAL_DISTBUS_USBD_CMD_RESET,
    VSF_HAL_DISTBUS_USBD_CMD_CONNECT,
    VSF_HAL_DISTBUS_USBD_CMD_DISCONNECT,
    VSF_HAL_DISTBUS_USBD_CMD_WAKEUP,
    VSF_HAL_DISTBUS_USBD_CMD_SET_ADDRESS,
    VSF_HAL_DISTBUS_USBD_CMD_STATUS_STAGE,
    VSF_HAL_DISTBUS_USBD_CMD_EP_ADD,
    VSF_HAL_DISTBUS_USBD_CMD_EP_SET_STALL,
    VSF_HAL_DISTBUS_USBD_CMD_EP_CLEAR_STALL,
    VSF_HAL_DISTBUS_USBD_CMD_EP_ENABLE_OUT,
    VSF_HAL_DISTBUS_USBD_CMD_EP_SET_DATA_SIZE,
    VSF_HAL_DISTBUS_USBD_CMD_EP_WRITE_BUFFER,
    VSF_HAL_DISTBUS_USBD_CMD_TRANSFER_SEND,
    VSF_HAL_DISTBUS_USBD_CMD_TRANSFER_RECV,
    VSF_HAL_DISTBUS_USBD_CMD_ON_EVT,
    VSF_HAL_DISTBUS_USBD_CMD_END = VSF_HAL_DISTBUS_USBD_CMD_ON_EVT,
#endif

    VSF_HAL_DISTBUS_ADDR_RANGE,
};

vsf_class(vsf_hal_distbus_t) {
    public_member(
        vsf_distbus_t               *distbus;
    )

    protected_member(
        vsf_distbus_service_t       service;

        struct {
            usb_dc_evthandler_t     evthandler;
            void                    *param;
        } callback;

#if VSF_HAL_USE_DISTBUS_USBD == ENABLED
        vsf_hal_distbus_usbd_t      usbd;
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
