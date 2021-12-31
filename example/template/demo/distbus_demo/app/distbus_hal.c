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

#include "vsf_cfg.h"

#if     VSF_USE_USB_DEVICE == ENABLED && VSF_USE_DISTBUS == ENABLED             \
    &&  APP_USE_DISTBUS_DEMO == ENABLED && APP_USE_DISTBUS_HAL_DEMO == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
// for distbus_dcd constants
#define VSF_HAL_USE_DISTBUS                         ENABLED
#   define VSF_HAL_USE_DISTBUS_USBD                 ENABLED
#include "vsf.h"

#include "../../common/usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __user_distbus_hal_service_t {
    vsf_distbus_service_t   service;

#if APP_USE_DISTBUS_HAL_USBD_DEMO == ENABLED
    struct {
        vsf_distbus_msg_t   *msg[32];
        bool                is_set_address;
        uint8_t             address;
    } usbd;
#endif
} __user_distbus_hal_service_t;

/*============================ PROTOTYPES ====================================*/

static bool __user_distbus_hal_service_msghandler(vsf_distbus_t *bus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_distbus_service_info_t __user_distbus_hal_service_info = {
    .mtu                    = VSF_HAL_DISTBUS_CFG_MTU,
    .addr_range             = VSF_HAL_DISTBUS_ADDR_RANGE,
    .handler                = __user_distbus_hal_service_msghandler,
};

static __user_distbus_hal_service_t __user_distbus_hal_service = {
    .service                = {
        .info               = &__user_distbus_hal_service_info,
    },
};

/*============================ IMPLEMENTATION ================================*/

#if APP_USE_DISTBUS_HAL_USBD_DEMO == ENABLED
static void __user_usbd_evthandler(void *param, usb_evt_t evt, uint_fast8_t value)
{
    vsf_distbus_t *distbus = (vsf_distbus_t *)param;
    vsf_distbus_msg_t *msg;
    uint8_t *data;

    if (USB_ON_OUT == evt) {
        msg = __user_distbus_hal_service.usbd.msg[value & 0x0F];
        if (msg != NULL) {
            data = (uint8_t *)&msg->header + sizeof(msg->header);

            msg->header.datalen = 2 + VSF_USB_DC0.Ep.GetDataSize(value);
            msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_ON_EVT;
            data[0] = evt;
            data[1] = value;
            vsf_distbus_send_msg(distbus, &__user_distbus_hal_service.service, msg);
            return;
        }
    } else if (USB_ON_IN == evt) {
        msg = __user_distbus_hal_service.usbd.msg[16 + (value & 0x0F)];
        if (msg != NULL) {
            vsf_distbus_free_msg(distbus, msg);
            __user_distbus_hal_service.usbd.msg[16 + (value & 0x0F)] = NULL;
        }
    } else if (USB_ON_RESET == evt) {
        // do reset on USB_ON_RESET, this can not wait
        usb_dc_cfg_t cfg = {
            .speed          = data[0],
            .priority       = VSF_USBD_CFG_HW_PRIORITY,
            .evthandler     = __user_usbd_evthandler,
            .param          = distbus,
        };
        __user_distbus_hal_service.usbd.is_set_address = false;
        VSF_USB_DC0.Reset(&cfg);
    }

    if (USB_ON_SETUP == evt) {
        msg = vsf_distbus_alloc_msg(distbus, 2 + 8, &data);
        if (msg != NULL) {
            struct usb_ctrlrequest_t *request = (struct usb_ctrlrequest_t *)&data[2];
            VSF_USB_DC0.GetSetup(&data[2]);
            if ((0x00 == request->bRequestType) && (USB_REQ_SET_ADDRESS == request->bRequest)) {
                // set address can not delay
                __user_distbus_hal_service.usbd.is_set_address = true;
                __user_distbus_hal_service.usbd.address = (uint8_t)request->wValue;
            }
        }
    } else {
        msg = vsf_distbus_alloc_msg(distbus, 2, &data);
    }
    if (NULL == msg) {
        VSF_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_ON_EVT;
    data[0] = evt;
    if (USB_ON_RESET == evt) {
        data[1] = VSF_USB_DC0.Ep.GetFeature(0, USB_DC_FEATURE_TRANSFER);
    } else {
        data[1] = value;
    }
    vsf_distbus_send_msg(distbus, &__user_distbus_hal_service.service, msg);
}

static bool __user_distbus_usbd_service_msghandler(vsf_distbus_t *distbus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    uint8_t *data = (uint8_t *)&msg->header + sizeof(msg->header);
    uint_fast16_t size;
    bool retain_msg = false;

    switch (msg->header.addr) {
    case VSF_HAL_DISTBUS_USBD_CMD_INIT: {
            usb_dc_cfg_t cfg = {
                .speed          = data[0],
                .priority       = VSF_USBD_CFG_HW_PRIORITY,
                .evthandler     = __user_usbd_evthandler,
                .param          = distbus,
            };
            __user_distbus_hal_service.usbd.is_set_address = false;
            VSF_USB_DC0.Init(&cfg);
        }
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_FINI:
        VSF_USB_DC0.Fini();
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_RESET:
        // reset can not wait
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_CONNECT:
        VSF_USB_DC0.Connect();
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_DISCONNECT:
        VSF_USB_DC0.Disconnect();
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_WAKEUP:
        VSF_USB_DC0.Wakeup();
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_SET_ADDRESS:
        // can not wait till now to set address
//        VSF_USB_DC0.SetAddress(data[0]);
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_STATUS_STAGE:
        VSF_USB_DC0.StatusStage(data[0]);
        if (__user_distbus_hal_service.usbd.is_set_address) {
            __user_distbus_hal_service.usbd.is_set_address = false;
            VSF_USB_DC0.SetAddress(__user_distbus_hal_service.usbd.address);
        }
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_EP_ADD:
        size = get_unaligned_le16(&data[2]);
        VSF_USB_DC0.Ep.Add(data[0], data[1], size);
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_EP_SET_STALL:
        VSF_USB_DC0.Ep.SetStall(data[0]);
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_EP_CLEAR_STALL:
        VSF_USB_DC0.Ep.ClearStall(data[0]);
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_EP_ENABLE_OUT:
        VSF_USB_DC0.Ep.Transaction.EnableOut(data[0]);
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_EP_SET_DATA_SIZE:
        size = get_unaligned_le16(&data[1]);
        VSF_USB_DC0.Ep.Transaction.SetDataSize(data[0], size);
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_EP_WRITE_BUFFER:
        size = get_unaligned_le16(&data[1]);
        VSF_USB_DC0.Ep.Transaction.WriteBuffer(data[0], &data[3], size);
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_TRANSFER_SEND:
        retain_msg = true;
        __user_distbus_hal_service.usbd.msg[16 + (data[0] & 0x0F)] = msg;
        size = get_unaligned_le16(&data[1]);
        VSF_USB_DC0.Ep.Transfer.Send(data[0], &data[4], size, data[3]);
        break;
    case VSF_HAL_DISTBUS_USBD_CMD_TRANSFER_RECV:
        retain_msg = true;
        __user_distbus_hal_service.usbd.msg[data[0] & 0x0F] = msg;
        size = get_unaligned_le16(&data[1]);
        VSF_USB_DC0.Ep.Transfer.Recv(data[0], &data[2], size);
        break;
    }
    return retain_msg;
}
#endif

static bool __user_distbus_hal_service_msghandler(vsf_distbus_t *distbus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    uint8_t addr = msg->header.addr;
    bool retain_msg = false;

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe186
#endif

#if APP_USE_DISTBUS_HAL_USBD_DEMO == ENABLED
    if ((addr >= VSF_HAL_DISTBUS_USBD_CMD_BEGIN) && (addr <= VSF_HAL_DISTBUS_USBD_CMD_END)) {
        retain_msg = __user_distbus_usbd_service_msghandler(distbus, service, msg);
    }
#endif

    return retain_msg;
}

void __user_distbus_hal_service_init(vsf_distbus_t *distbus)
{
    vsf_distbus_register_service(distbus, &__user_distbus_hal_service.service);
}

#endif
