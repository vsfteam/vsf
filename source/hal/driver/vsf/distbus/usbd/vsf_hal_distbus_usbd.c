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

#include "component/usb/vsf_usb_cfg.h"

#if     VSF_USE_USB_DEVICE == ENABLED                                           \
    &&  VSF_HAL_USE_DISTBUS == ENABLED && VSF_HAL_USE_DISTBUS_USBD == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_HAL_DISTBUS_CLASS_INHERIT__
#define __VSF_DISTBUS_DCD_CLASS_IMPLEMENT

#include "component/usb/common/usb_common.h"
#include "../driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_hal_distbus_usbd_notify(vsf_hal_distbus_t *hal_distbus, usb_evt_t evt, uint_fast8_t value)
{
    if (hal_distbus->callback.evthandler != NULL) {
        hal_distbus->callback.evthandler(hal_distbus->callback.param, evt, value);
    }
}

static vsf_hal_distbus_usbd_ep_t * __vsf_hal_distbus_usbd_get_ep(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep)
{
    vsf_hal_distbus_usbd_ep_t *dcd_ep;
    if ((ep & USB_DIR_MASK) == USB_DIR_IN) {
        ep &= ~USB_DIR_MASK;
        dcd_ep = hal_distbus->usbd.ep_in;
    } else {
        dcd_ep = hal_distbus->usbd.ep_out;
    }
    VSF_USB_ASSERT(ep < 16);
    return &dcd_ep[ep];
}

bool __vsf_hal_distbus_usbd_msghandler(vsf_distbus_t *bus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    vsf_hal_distbus_t *hal_distbus = container_of(service, vsf_hal_distbus_t, service);
    uint8_t *data = (uint8_t *)&msg->header + sizeof(msg->header);
    bool retain_msg = false;

    switch (msg->header.addr) {
    case VSF_HAL_DISTBUS_USBD_CMD_ON_EVT: {
            usb_evt_t evt = data[0];
            uint_fast8_t value = data[1];

            // for USB_ON_RESET, value returned is ep_feature
            switch (evt) {
            case USB_ON_RESET:
                hal_distbus->usbd.ep_feature = value;
                value = 0;
                break;
            case USB_ON_SETUP:
                memcpy(&hal_distbus->usbd.setup, &data[2], 8);
                break;
            case USB_ON_OUT: {
                    vsf_hal_distbus_usbd_ep_t *dcd_ep = __vsf_hal_distbus_usbd_get_ep(hal_distbus, value);
                    VSF_USB_ASSERT(NULL == dcd_ep->msg);
                    dcd_ep->msg = msg;
                    retain_msg = true;
                }
                break;
            }
            __vsf_hal_distbus_usbd_notify(hal_distbus, evt, value);
        }
        break;
    default:
        VSF_USB_ASSERT(false);
        break;
    }
    return retain_msg;
}

vsf_err_t vsf_hal_distbus_usbd_init(vsf_hal_distbus_t *hal_distbus, usb_dc_cfg_t *cfg)
{
    VSF_USB_ASSERT((hal_distbus != NULL) && (cfg != NULL));

    hal_distbus->callback.param = cfg->param;
    hal_distbus->callback.evthandler = cfg->evthandler;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 1, &data);
    if (NULL == msg) {
        return VSF_ERR_FAIL;
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_INIT;
    data[0] = cfg->speed;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

void vsf_hal_distbus_usbd_fini(vsf_hal_distbus_t *hal_distbus)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_FINI;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
}

void vsf_hal_distbus_usbd_reset(vsf_hal_distbus_t *hal_distbus, usb_dc_cfg_t *cfg)
{
    memset(hal_distbus->usbd.ep, 0, sizeof(hal_distbus->usbd.ep));
    hal_distbus->usbd.address = 0;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_RESET;
    data[0] = cfg->speed;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
}

void vsf_hal_distbus_usbd_connect(vsf_hal_distbus_t *hal_distbus)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_CONNECT;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
}

void vsf_hal_distbus_usbd_disconnect(vsf_hal_distbus_t *hal_distbus)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_DISCONNECT;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
}

void vsf_hal_distbus_usbd_wakeup(vsf_hal_distbus_t *hal_distbus)
{
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 0, NULL);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_WAKEUP;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
}

void vsf_hal_distbus_usbd_set_address(vsf_hal_distbus_t *hal_distbus, uint_fast8_t addr)
{
    hal_distbus->usbd.address = addr;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_SET_ADDRESS;
    data[0] = addr;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
}

uint_fast8_t vsf_hal_distbus_usbd_get_address(vsf_hal_distbus_t *hal_distbus)
{
    return hal_distbus->usbd.address;
}

uint_fast16_t vsf_hal_distbus_usbd_get_frame_number(vsf_hal_distbus_t *hal_distbus)
{
    return hal_distbus->usbd.frame_number;
}

extern uint_fast8_t vsf_hal_distbus_usbd_get_mframe_number(vsf_hal_distbus_t *hal_distbus)
{
    return hal_distbus->usbd.mframe_number;
}

void vsf_hal_distbus_usbd_get_setup(vsf_hal_distbus_t *hal_distbus, uint8_t *buffer)
{
    memcpy(buffer, (uint8_t *)&hal_distbus->usbd.setup, sizeof(hal_distbus->usbd.setup));
}

void vsf_hal_distbus_usbd_status_stage(vsf_hal_distbus_t *hal_distbus, bool is_in)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_STATUS_STAGE;
    data[0] = is_in;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
}

uint_fast8_t vsf_hal_distbus_usbd_ep_get_feature(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint_fast8_t feature)
{
    return hal_distbus->usbd.ep_feature;
}

vsf_err_t vsf_hal_distbus_usbd_ep_add(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    vsf_hal_distbus_usbd_ep_t *dcd_ep = __vsf_hal_distbus_usbd_get_ep(hal_distbus, ep);
    dcd_ep->ep = ep;
    dcd_ep->type = type;
    dcd_ep->size = size;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 4, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_EP_ADD;
    data[0] = ep;
    data[1] = type;
    put_unaligned_le16(size, &data[2]);
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

uint_fast16_t vsf_hal_distbus_usbd_ep_get_size(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep)
{
    vsf_hal_distbus_usbd_ep_t *dcd_ep = __vsf_hal_distbus_usbd_get_ep(hal_distbus, ep);
    return dcd_ep->size;
}

vsf_err_t vsf_hal_distbus_usbd_ep_set_stall(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep)
{
    vsf_hal_distbus_usbd_ep_t *dcd_ep = __vsf_hal_distbus_usbd_get_ep(hal_distbus, ep);
    dcd_ep->is_stalled = true;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_EP_SET_STALL;
    data[0] = ep;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

bool vsf_hal_distbus_usbd_ep_is_stalled(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep)
{
    vsf_hal_distbus_usbd_ep_t *dcd_ep = __vsf_hal_distbus_usbd_get_ep(hal_distbus, ep);
    return dcd_ep->is_stalled;
}

vsf_err_t vsf_hal_distbus_usbd_ep_clear_stall(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep)
{
    vsf_hal_distbus_usbd_ep_t *dcd_ep = __vsf_hal_distbus_usbd_get_ep(hal_distbus, ep);
    dcd_ep->is_stalled = false;

    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_EP_CLEAR_STALL;
    data[0] = ep;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

uint_fast32_t vsf_hal_distbus_usbd_ep_get_data_size(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep)
{
    vsf_hal_distbus_usbd_ep_t *dcd_ep = __vsf_hal_distbus_usbd_get_ep(hal_distbus, ep);
    vsf_distbus_msg_t *msg = dcd_ep->msg;
    VSF_USB_ASSERT(msg != NULL);
    return msg->header.datalen - 2;
}

vsf_err_t vsf_hal_distbus_usbd_ep_transaction_read_buffer(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    vsf_hal_distbus_usbd_ep_t *dcd_ep = __vsf_hal_distbus_usbd_get_ep(hal_distbus, ep);
    vsf_distbus_msg_t *msg = dcd_ep->msg;
    VSF_USB_ASSERT((msg != NULL) && (msg->header.datalen == (size + 2)));
    memcpy(buffer, &msg->header + sizeof(msg->header) + 2, size);

    vsf_distbus_free_msg(hal_distbus->distbus, dcd_ep->msg);
    dcd_ep->msg = NULL;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hal_distbus_usbd_ep_transaction_enable_out(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 1, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_EP_ENABLE_OUT;
    data[0] = ep;
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hal_distbus_usbd_ep_transaction_set_data_size(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint_fast16_t size)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 3, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_EP_SET_DATA_SIZE;
    data[0] = ep;
    put_unaligned_le16(size, &data[1]);
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hal_distbus_usbd_ep_transaction_write_buffer(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 3 + size, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_EP_WRITE_BUFFER;
    data[0] = ep;
    put_unaligned_le16(size, &data[1]);
    memcpy(&data[3], buffer, size);
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hal_distbus_usbd_ep_transfer_recv(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 3, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_TRANSFER_RECV;
    data[0] = ep;
    put_unaligned_le16(size, &data[1]);
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hal_distbus_usbd_ep_transfer_send(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    uint8_t *data;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(hal_distbus->distbus, 4 + size, &data);
    if (NULL == msg) {
        VSF_USB_ASSERT(false);
    }

    msg->header.addr = VSF_HAL_DISTBUS_USBD_CMD_TRANSFER_SEND;
    data[0] = ep;
    put_unaligned_le16(size, &data[1]);
    data[3] = zlp;
    memcpy(&data[4], buffer, size);
    vsf_distbus_send_msg(hal_distbus->distbus, &hal_distbus->service, msg);
    return VSF_ERR_NONE;
}

void vsf_hal_distbus_usbd_irq(vsf_hal_distbus_t *hal_distbus)
{

}

#endif
