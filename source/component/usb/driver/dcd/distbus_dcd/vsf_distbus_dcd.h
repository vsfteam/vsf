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

#ifndef __VSF_DISTBUS_DCD_H__
#define __VSF_DISTBUS_DCD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DISTBUS == ENABLED

#include "hal/vsf_hal.h"
// for distbus
#include "service/vsf_service.h"
#include "component/usb/common/usb_common.h"

#if     defined(__VSF_DISTBUS_DCD_CLASS_IMPLEMENT)
#   undef __VSF_DISTBUS_DCD_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_usb_dc_from_distbus_ip(__n, __obj, __drv_name)                      \
        __USB_DC_FROM_IP(__n, (__obj), __drv_name, vk_distbus_usbd)

/*============================ TYPES =========================================*/

enum {
    VSF_DISTBUS_DCD_CMD_INIT,
    VSF_DISTBUS_DCD_CMD_FINI,
    VSF_DISTBUS_DCD_CMD_RESET,
    VSF_DISTBUS_DCD_CMD_CONNECT,
    VSF_DISTBUS_DCD_CMD_DISCONNECT,
    VSF_DISTBUS_DCD_CMD_WAKEUP,
    VSF_DISTBUS_DCD_CMD_SET_ADDRESS,
    VSF_DISTBUS_DCD_CMD_STATUS_STAGE,
    VSF_DISTBUS_DCD_CMD_EP_ADD,
    VSF_DISTBUS_DCD_CMD_EP_SET_STALL,
    VSF_DISTBUS_DCD_CMD_EP_CLEAR_STALL,
    VSF_DISTBUS_DCD_CMD_ENABLE_OUT,
    VSF_DISTBUS_DCD_CMD_SET_DATA_SIZE,
    VSF_DISTBUS_DCD_CMD_WRITE_BUFFER,
    VSF_DISTBUS_DCD_CMD_ON_EVT,

    VSF_DISTBUS_DCD_ADDR_RANGE,
};

dcl_simple_class(vk_distbus_dcd_t)

typedef struct vk_distbus_dcd_ep_t {
    uint16_t                        size;
    uint8_t                         type : 2;
    uint8_t                         is_stalled : 1;
    uint8_t                         zlp : 1;
    uint8_t                         pending : 1;
    uint8_t                         ep;
    uint32_t                        transfer_size;

    vsf_distbus_msg_t               *msg;
} vk_distbus_dcd_ep_t;

def_simple_class(vk_distbus_dcd_t) {
    public_member(
        vsf_distbus_t               *distbus;
    )

    private_member(
        struct {
            usb_dc_evthandler_t     evthandler;
            void                    *param;
        } callback;

        union {
            struct {
                vk_distbus_dcd_ep_t ep_out[16];
                vk_distbus_dcd_ep_t ep_in[16];
            };
            vk_distbus_dcd_ep_t     ep[32];
        };
        struct usb_ctrlrequest_t    setup;
        uint16_t                    frame_number;
        uint16_t                    mframe_number;
        uint8_t                     address;
        uint8_t                     ep_feature;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_distbus_usbd_init(vk_distbus_dcd_t *usbd, usb_dc_cfg_t *cfg);
extern void vk_distbus_usbd_fini(vk_distbus_dcd_t *usbd);
extern void vk_distbus_usbd_reset(vk_distbus_dcd_t *usbd, usb_dc_cfg_t *cfg);

extern void vk_distbus_usbd_connect(vk_distbus_dcd_t *usbd);
extern void vk_distbus_usbd_disconnect(vk_distbus_dcd_t *usbd);
extern void vk_distbus_usbd_wakeup(vk_distbus_dcd_t *usbd);

extern void vk_distbus_usbd_set_address(vk_distbus_dcd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vk_distbus_usbd_get_address(vk_distbus_dcd_t *usbd);

extern uint_fast16_t vk_distbus_usbd_get_frame_number(vk_distbus_dcd_t *usbd);
extern uint_fast8_t vk_distbus_usbd_get_mframe_number(vk_distbus_dcd_t *usbd);

extern void vk_distbus_usbd_get_setup(vk_distbus_dcd_t *usbd, uint8_t *buffer);
extern void vk_distbus_usbd_status_stage(vk_distbus_dcd_t *usbd, bool is_in);

extern uint_fast8_t vk_distbus_usbd_ep_get_feature(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t vk_distbus_usbd_ep_add(vk_distbus_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vk_distbus_usbd_ep_get_size(vk_distbus_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vk_distbus_usbd_ep_set_stall(vk_distbus_dcd_t *usbd, uint_fast8_t ep);
extern bool vk_distbus_usbd_ep_is_stalled(vk_distbus_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_distbus_usbd_ep_clear_stall(vk_distbus_dcd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t vk_distbus_usbd_ep_get_data_size(vk_distbus_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vk_distbus_usbd_ep_transaction_read_buffer(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vk_distbus_usbd_ep_transaction_enable_out(vk_distbus_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_distbus_usbd_ep_transaction_set_data_size(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vk_distbus_usbd_ep_transaction_write_buffer(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vk_distbus_usbd_ep_transfer_recv(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vk_distbus_usbd_ep_transfer_send(vk_distbus_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void vk_distbus_usbd_irq(vk_distbus_dcd_t *usbd);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
