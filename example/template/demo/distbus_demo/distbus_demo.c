/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#define __VSF_DISTBUS_CLASS_INHERIT__
#include "vsf.h"

#include "../common/usrapp_common.h"

#if     VSF_USE_USB_DEVICE == ENABLED && VSF_USE_DISTBUS == ENABLED             \
    &&  VSF_USBD_USE_DCD_DISTBUS == ENABLED && APP_USE_DISTBUS_DEMO == ENABLED

/*============================ MACROS ========================================*/

#ifndef APP_DISTBUS_DEMO_CFG_MTU
#   define APP_DISTBUS_DEMO_CFG_MTU         1024
#endif
#ifndef APP_DISTBUS_DEMO_CFG_POOL_NUM
#   define APP_DISTBUS_DEMO_CFG_POOL_NUM    16
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __user_distbus_msg_t {
    implement(vsf_distbus_msg_t)
    uint8_t buffer[APP_DISTBUS_DEMO_CFG_MTU];
} __user_distbus_msg_t;

dcl_vsf_pool(__user_distbus_msg_pool)
def_vsf_pool(__user_distbus_msg_pool, __user_distbus_msg_t)

typedef struct __user_t {
    vsf_distbus_t                           distbus;
    vsf_distbus_service_t                   service;

    vsf_pool(__user_distbus_msg_pool)       msg_pool;

    vsf_distbus_msg_t                       *msg[32];
} __user_t;

/*============================ PROTOTYPES ====================================*/

static void __user_distbus_on_error(vsf_distbus_t *distbus);
static void * __user_distbus_alloc_msg(uint_fast32_t size);
static void __user_distbus_free_msg(void *msg);
static bool __user_distbus_msghandler(vsf_distbus_t *bus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);

extern void __user_distbus_init(void);
extern bool __user_distbus_send(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_sent)(void *p));
extern bool __user_distbus_recv(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_recv)(void *p));

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_distbus_service_info_t __user_distbus_usbd_service_info = {
    .mtu            = APP_DISTBUS_DEMO_CFG_MTU,
    .addr_range     = VSF_DISTBUS_DCD_ADDR_RANGE,
    .handler        = __user_distbus_msghandler,
};

static __user_t __user = {
    .distbus                    = {
        .op                     = {
            .mem                = {
                .alloc_msg      = __user_distbus_alloc_msg,
                .free_msg       = __user_distbus_free_msg,
            },
            .bus                = {
                .init           = __user_distbus_init,
                .send           = __user_distbus_send,
                .recv           = __user_distbus_recv,
            },
            .on_error           = __user_distbus_on_error,
        },
    },
    .service                    = {
        .addr_start             = 0,
        .info                   = &__user_distbus_usbd_service_info,
    },
};

/*============================ IMPLEMENTATION ================================*/

imp_vsf_pool(__user_distbus_msg_pool, __user_distbus_msg_t)

static void __user_distbus_on_error(vsf_distbus_t *distbus)
{
    VSF_ASSERT(false);
}

static void * __user_distbus_alloc_msg(uint_fast32_t size)
{
    return VSF_POOL_ALLOC(__user_distbus_msg_pool, &__user.msg_pool);
}

static void __user_distbus_free_msg(void *msg)
{
    VSF_POOL_FREE(__user_distbus_msg_pool, &__user.msg_pool, msg);
}

static void __user_usbd_evthandler(void *param, usb_evt_t evt, uint_fast8_t value)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg;

    if (USB_ON_OUT == evt) {
        msg = __user.msg[value & 0x0F];
        if (msg != NULL) {
            data = (uint8_t *)&msg->header + sizeof(msg->header);

            msg->header.datalen = 2 + VSF_USB_DC0.Ep.GetDataSize(value);
            msg->header.addr = VSF_DISTBUS_DCD_CMD_ON_EVT;
            data[0] = evt;
            data[1] = value;
            vsf_distbus_send_msg(&__user.distbus, &__user.service, msg);
            return;
        }
    } else if (USB_ON_IN == evt) {
        msg = __user.msg[16 + (value & 0x0F)];
        if (msg != NULL) {
            vsf_distbus_free_msg(&__user.distbus, msg);
            __user.msg[16 + (value & 0x0F)] = NULL;
        }
    }

    msg = vsf_distbus_alloc_msg(&__user.distbus, 2, &data);
    if (NULL == msg) {
        VSF_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_ON_EVT;
    data[0] = evt;
    if (USB_ON_RESET == evt) {
        data[1] = VSF_USB_DC0.Ep.GetFeature(0, USB_DC_FEATURE_TRANSFER);
    } else {
        data[1] = value;
    }
    vsf_distbus_send_msg(&__user.distbus, &__user.service, msg);
}

static bool __user_distbus_msghandler(vsf_distbus_t *bus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    uint8_t *data = (uint8_t *)&msg->header + sizeof(msg->header);
    bool retain_msg = false;

    switch (msg->header.addr) {
    case VSF_DISTBUS_DCD_CMD_INIT: {
            usb_dc_cfg_t cfg = {
                .speed          = USB_DC_SPEED_HIGH,
                .priority       = VSF_USBD_CFG_HW_PRIORITY,
                .evthandler     = __user_usbd_evthandler,
            };
            VSF_USB_DC0.Init(&cfg);
        }
        break;
    case VSF_DISTBUS_DCD_CMD_FINI:
        VSF_USB_DC0.Fini();
        break;
    case VSF_DISTBUS_DCD_CMD_RESET: {
            usb_dc_cfg_t cfg = {
                .speed          = USB_DC_SPEED_HIGH,
                .priority       = VSF_USBD_CFG_HW_PRIORITY,
                .evthandler     = __user_usbd_evthandler,
            };
            VSF_USB_DC0.Reset(&cfg);
        }
        break;
    case VSF_DISTBUS_DCD_CMD_CONNECT:
        VSF_USB_DC0.Connect();
        break;
    case VSF_DISTBUS_DCD_CMD_DISCONNECT:
        VSF_USB_DC0.Disconnect();
        break;
    case VSF_DISTBUS_DCD_CMD_WAKEUP:
        VSF_USB_DC0.Wakeup();
        break;
    case VSF_DISTBUS_DCD_CMD_SET_ADDRESS:
        VSF_USB_DC0.SetAddress(data[0]);
        break;
    case VSF_DISTBUS_DCD_CMD_STATUS_STAGE:
        VSF_USB_DC0.StatusStage(data[0]);
        break;
    case VSF_DISTBUS_DCD_CMD_EP_ADD:
        VSF_USB_DC0.Ep.Add(data[0], data[1], get_unaligned_le16(&data[2]));
        break;
    case VSF_DISTBUS_DCD_CMD_EP_SET_STALL:
        VSF_USB_DC0.Ep.SetStall(data[0]);
        break;
    case VSF_DISTBUS_DCD_CMD_EP_CLEAR_STALL:
        VSF_USB_DC0.Ep.ClearStall(data[0]);
        break;
    case VSF_DISTBUS_DCD_CMD_EP_ENABLE_OUT:
        VSF_USB_DC0.Ep.Transaction.EnableOut(data[0]);
        break;
    case VSF_DISTBUS_DCD_CMD_EP_SET_DATA_SIZE:
        VSF_USB_DC0.Ep.Transaction.SetDataSize(data[0], get_unaligned_le16(&data[1]));
        break;
    case VSF_DISTBUS_DCD_CMD_EP_WRITE_BUFFER:
        VSF_USB_DC0.Ep.Transaction.WriteBuffer(data[0], &data[3], get_unaligned_le16(&data[1]));
        break;
    case VSF_DISTBUS_DCD_CMD_TRANSFER_SEND:
        retain_msg = true;
        __user.msg[16 + (data[0] & 0x0F)] = msg;
        VSF_USB_DC0.Ep.Transfer.Send(data[0], &data[4], get_unaligned_le16(&data[1]), data[2]);
        break;
    case VSF_DISTBUS_DCD_CMD_TRANSFER_RECV:
        retain_msg = true;
        __user.msg[data[0] & 0x0F] = msg;
        VSF_USB_DC0.Ep.Transfer.Recv(data[0], &data[2], get_unaligned_le16(&data[1]));
        break;
    }
    return retain_msg;
}

#if APP_USE_LINUX_DEMO == ENABLED
int distbus_dcd_server_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    VSF_POOL_INIT(__user_distbus_msg_pool, &__user.msg_pool, APP_DISTBUS_DEMO_CFG_POOL_NUM);
    vsf_distbus_init(&__user.distbus);
    vsf_distbus_register_service(&__user.distbus, &__user.service);

#if APP_USE_LINUX_DEMO != ENABLED
    while (1) {
        vsf_arch_sleep(0);
    }
#endif

    return 0;
}

#endif
