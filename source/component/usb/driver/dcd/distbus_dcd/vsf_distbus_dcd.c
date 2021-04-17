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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DISTBUS == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_DISTBUS_DCD_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "./vsf_distbus_dcd.h"

/*============================ MACROS ========================================*/

#ifndef VSF_DISTBUS_DCD_CFG_MTU
#   define VSF_DISTBUS_DCD_CFG_MTU          512
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static bool __vk_distbus_usbd_msghandler(vsf_distbus_t *bus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_distbus_service_info_t __vk_distbus_usbd_info = {
    .mtu                = 8 + VSF_DISTBUS_DCD_CFG_MTU,
    .type               = 0,
    .addr_range         = VSF_DISTBUS_DCD_ADDR_RANGE,
    .handler            = __vk_distbus_usbd_msghandler,
};

/*============================ IMPLEMENTATION ================================*/

static void __vk_distbus_usbd_notify(vk_distbus_dcd_t *usbd, usb_evt_t evt, uint_fast8_t value)
{
    if (usbd->callback.evthandler != NULL) {
        usbd->callback.evthandler(usbd->callback.param, evt, value);
    }
}

static vk_distbus_dcd_ep_t * __vk_distbus_usbd_get_ep(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    vk_distbus_dcd_ep_t *dcd_ep;
    if ((ep & USB_DIR_MASK) == USB_DIR_IN) {
        ep &= ~USB_DIR_MASK;
        dcd_ep = usbd->ep_in;
    } else {
        dcd_ep = usbd->ep_out;
    }
    VSF_USB_ASSERT(ep < 16);
    return &dcd_ep[ep];
}

static bool __vk_distbus_usbd_msghandler(vsf_distbus_t *bus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    vk_distbus_dcd_t *usbd = container_of(service, vk_distbus_dcd_t, service);
    uint8_t *data = (uint8_t *)&msg->header + sizeof(msg->header);
    bool retain_msg = false;

    switch (msg->header.addr) {
    case VSF_DISTBUS_DCD_CMD_ON_EVT: {
            usb_evt_t evt = data[0];
            uint_fast8_t value = data[1];

            // for USB_ON_RESET, value returned is ep_feature
            switch (evt) {
            case USB_ON_RESET:
                usbd->ep_feature = value;
                value = 0;
                break;
            case USB_ON_SETUP:
                memcpy(&usbd->setup, &data[2], 8);
                break;
            case USB_ON_OUT: {
                    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, value);
                    VSF_USB_ASSERT(NULL == dcd_ep->msg);
                    dcd_ep->msg = msg;
                    retain_msg = true;
                }
                break;
            }
            __vk_distbus_usbd_notify(usbd, evt, value);
        }
        break;
    default:
        VSF_USB_ASSERT(false);
        break;
    }
    return retain_msg;
}

void vk_distbus_usbd_register_service(vk_distbus_dcd_t *usbd)
{
    usbd->service.info = &__vk_distbus_usbd_info;
    vsf_distbus_register_service(usbd->distbus, &usbd->service);
}

vsf_err_t vk_distbus_usbd_init(vk_distbus_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    VSF_USB_ASSERT((usbd != NULL) && (cfg != NULL));

    usbd->callback.param = cfg->param;
    usbd->callback.evthandler = cfg->evthandler;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 1, &data);
    if (NULL == msg) {
        return VSF_ERR_FAIL;
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_INIT;
    data[0] = cfg->speed;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

void vk_distbus_usbd_fini(vk_distbus_dcd_t *usbd)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_FINI;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
}

void vk_distbus_usbd_reset(vk_distbus_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    memset(usbd->ep, 0, sizeof(usbd->ep));
    usbd->address = 0;

    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_RESET;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
}

void vk_distbus_usbd_connect(vk_distbus_dcd_t *usbd)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_CONNECT;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
}

void vk_distbus_usbd_disconnect(vk_distbus_dcd_t *usbd)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_DISCONNECT;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
}

void vk_distbus_usbd_wakeup(vk_distbus_dcd_t *usbd)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_WAKEUP;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
}

void vk_distbus_usbd_set_address(vk_distbus_dcd_t *usbd, uint_fast8_t addr)
{
    usbd->address = addr;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_SET_ADDRESS;
    data[0] = addr;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
}

uint_fast8_t vk_distbus_usbd_get_address(vk_distbus_dcd_t *usbd)
{
    return usbd->address;
}

uint_fast16_t vk_distbus_usbd_get_frame_number(vk_distbus_dcd_t *usbd)
{
    return usbd->frame_number;
}

extern uint_fast8_t vk_distbus_usbd_get_mframe_number(vk_distbus_dcd_t *usbd)
{
    return usbd->mframe_number;
}

void vk_distbus_usbd_get_setup(vk_distbus_dcd_t *usbd, uint8_t *buffer)
{
    memcpy(buffer, (uint8_t *)&usbd->setup, sizeof(usbd->setup));
}

void vk_distbus_usbd_status_stage(vk_distbus_dcd_t *usbd, bool is_in)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_STATUS_STAGE;
    data[0] = is_in;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
}

uint_fast8_t vk_distbus_usbd_ep_get_feature(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint_fast8_t feature)
{
    return usbd->ep_feature;
}

vsf_err_t vk_distbus_usbd_ep_add(vk_distbus_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, ep);
    dcd_ep->ep = ep;
    dcd_ep->type = type;
    dcd_ep->size = size;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 4, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_EP_ADD;
    data[0] = ep;
    data[1] = type;
    put_unaligned_le16(size, &data[2]);
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

uint_fast16_t vk_distbus_usbd_ep_get_size(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, ep);
    return dcd_ep->size;
}

vsf_err_t vk_distbus_usbd_ep_set_stall(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, ep);
    dcd_ep->is_stalled = true;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_EP_SET_STALL;
    data[0] = ep;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

bool vk_distbus_usbd_ep_is_stalled(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, ep);
    return dcd_ep->is_stalled;
}

vsf_err_t vk_distbus_usbd_ep_clear_stall(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, ep);
    dcd_ep->is_stalled = false;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_EP_CLEAR_STALL;
    data[0] = ep;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

uint_fast32_t vk_distbus_usbd_ep_get_data_size(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, ep);
    vsf_distbus_msg_t *msg = dcd_ep->msg;
    VSF_USB_ASSERT(msg != NULL);
    return msg->header.datalen - 2;
}

vsf_err_t vk_distbus_usbd_ep_transaction_read_buffer(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, ep);
    vsf_distbus_msg_t *msg = dcd_ep->msg;
    VSF_USB_ASSERT((msg != NULL) && (msg->header.datalen == (size + 2)));
    memcpy(buffer, &msg->header + sizeof(msg->header) + 2, size);

    vsf_distbus_free_msg(usbd->distbus, dcd_ep->msg);
    dcd_ep->msg = NULL;
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transaction_enable_out(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_EP_ENABLE_OUT;
    data[0] = ep;
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transaction_set_data_size(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 3, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_EP_SET_DATA_SIZE;
    data[0] = ep;
    put_unaligned_le16(size, &data[1]);
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transaction_write_buffer(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 3 + size, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_EP_WRITE_BUFFER;
    data[0] = ep;
    put_unaligned_le16(size, &data[1]);
    memcpy(&data[3], buffer, size);
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transfer_recv(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 3, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_TRANSFER_RECV;
    data[0] = ep;
    put_unaligned_le16(size, &data[1]);
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transfer_send(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usbd->distbus, 4 + size, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_DISTBUS_DCD_CMD_TRANSFER_SEND;
    data[0] = ep;
    put_unaligned_le16(size, &data[1]);
    data[3] = zlp;
    memcpy(&data[4], buffer, size);
    vsf_distbus_send_msg(usbd->distbus, &usbd->service, msg);
    return VSF_ERR_NONE;
}

void vk_distbus_usbd_irq(vk_distbus_dcd_t *usbd)
{

}

#endif
