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
#define __VSF_DISTBUS_DCD_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "./vsf_distbus_dcd.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
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

static bool __vk_distbus_usbd_is_short(vk_distbus_dcd_ep_t *dcd_ep, uint_fast32_t transfer_size)
{
    uint_fast32_t short_size = transfer_size % dcd_ep->size;
    return !transfer_size || ((short_size > 0) && (short_size < dcd_ep->size));
}

vsf_err_t vk_distbus_usbd_init(vk_distbus_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    VSF_USB_ASSERT((usbd != NULL) && (cfg != NULL));

    usbd->callback.param = cfg->param;
    usbd->callback.evthandler = cfg->evthandler;
    return VSF_ERR_NONE;
}

void vk_distbus_usbd_fini(vk_distbus_dcd_t *usbd)
{
    // TODO:
}

void vk_distbus_usbd_reset(vk_distbus_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    memset(usbd->ep, 0, sizeof(usbd->ep));
    usbd->address = 0;
}

void vk_distbus_usbd_connect(vk_distbus_dcd_t *usbd)
{
    
}

void vk_distbus_usbd_disconnect(vk_distbus_dcd_t *usbd)
{
    
}

void vk_distbus_usbd_wakeup(vk_distbus_dcd_t *usbd)
{
}

void vk_distbus_usbd_set_address(vk_distbus_dcd_t *usbd, uint_fast8_t addr)
{
    usbd->address = addr;
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
    vk_distbus_dcd_ep_t *dcd_ep = is_in ? &usbd->ep_out[0] : &usbd->ep_in[0];
    
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
    return VSF_ERR_NONE;
}

uint_fast32_t vk_distbus_usbd_ep_get_data_size(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    vk_distbus_dcd_ep_t *dcd_ep = __vk_distbus_usbd_get_ep(usbd, ep);
    return dcd_ep->msg->header.datalen - dcd_ep->transfer_size;
}

vsf_err_t vk_distbus_usbd_ep_transaction_read_buffer(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transaction_enable_out(vk_distbus_dcd_t *usbd, uint_fast8_t ep)
{
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transaction_set_data_size(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transaction_write_buffer(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transfer_recv(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    return VSF_ERR_NONE;
}

vsf_err_t vk_distbus_usbd_ep_transfer_send(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    return VSF_ERR_NONE;
}

void vk_distbus_usbd_irq(vk_distbus_dcd_t *usbd)
{

}

#endif
