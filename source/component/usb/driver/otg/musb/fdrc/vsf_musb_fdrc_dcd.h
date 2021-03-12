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

#ifndef __VSF_MUSB_FDRC_DCD_H__
#define __VSF_MUSB_FDRC_DCD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED

#include "hal/vsf_hal.h"

#if     defined(__VSF_MUSB_FDRC_DCD_CLASS_IMPLEMENT)
#   undef __VSF_MUSB_FDRC_DCD_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_usb_dc_from_musbfdrc_ip(__n, __obj, __drv_name)                     \
        __USB_DC_FROM_IP(__n, (__obj), __drv_name, vk_musb_fdrc_usbd)

/*============================ TYPES =========================================*/

typedef struct vk_musb_fdrc_dcd_param_t {
    const i_usb_dc_ip_t *op;
} vk_musb_fdrc_dcd_param_t;

dcl_simple_class(vk_musb_fdrc_dcd_t)

typedef enum vk_musb_fdrc_dcd_ep0state_t {
    MUSB_FDRC_USBD_EP0_WAIT_SETUP,
    MUSB_FDRC_USBD_EP0_DATA_IN,
    MUSB_FDRC_USBD_EP0_DATA_OUT,
    MUSB_FDRC_USBD_EP0_STATUS,
} vk_musb_fdrc_dcd_ep0state_t;

def_simple_class(vk_musb_fdrc_dcd_t) {

    public_member(
        vk_musb_fdrc_dcd_param_t const * const param;
    )

    private_member(
        void *reg;
        struct {
            usb_dc_evt_handler_t evt_handler;
            void *param;
        } callback;
        uint16_t ep_buf_ptr;
        uint16_t out_mask;
        vk_musb_fdrc_dcd_ep0state_t ep0_state;
        bool has_data_stage;
        uint8_t ep_num;
        bool is_dma;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_musb_fdrc_usbd_init(vk_musb_fdrc_dcd_t *usbd, usb_dc_cfg_t *cfg);
extern void vk_musb_fdrc_usbd_fini(vk_musb_fdrc_dcd_t *usbd);
extern void vk_musb_fdrc_usbd_reset(vk_musb_fdrc_dcd_t *usbd, usb_dc_cfg_t *cfg);

extern void vk_musb_fdrc_usbd_connect(vk_musb_fdrc_dcd_t *usbd);
extern void vk_musb_fdrc_usbd_disconnect(vk_musb_fdrc_dcd_t *usbd);
extern void vk_musb_fdrc_usbd_wakeup(vk_musb_fdrc_dcd_t *usbd);

extern void vk_musb_fdrc_usbd_set_address(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vk_musb_fdrc_usbd_get_address(vk_musb_fdrc_dcd_t *usbd);

extern uint_fast16_t vk_musb_fdrc_usbd_get_frame_number(vk_musb_fdrc_dcd_t *usbd);
extern uint_fast8_t vk_musb_fdrc_usbd_get_mframe_number(vk_musb_fdrc_dcd_t *usbd);

extern void vk_musb_fdrc_usbd_get_setup(vk_musb_fdrc_dcd_t *usbd, uint8_t *buffer);
extern void vk_musb_fdrc_usbd_status_stage(vk_musb_fdrc_dcd_t *usbd, bool is_in);

extern uint_fast8_t vk_musb_fdrc_usbd_ep_get_feature(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t vk_musb_fdrc_usbd_ep_add(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vk_musb_fdrc_usbd_ep_get_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vk_musb_fdrc_usbd_ep_set_stall(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);
extern bool vk_musb_fdrc_usbd_ep_is_stalled(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_musb_fdrc_usbd_ep_clear_stall(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t vk_musb_fdrc_usbd_ep_get_data_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vk_musb_fdrc_usbd_ep_transaction_read_buffer(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vk_musb_fdrc_usbd_ep_transaction_enable_out(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_musb_fdrc_usbd_ep_transaction_set_data_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vk_musb_fdrc_usbd_ep_transaction_write_buffer(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vk_musb_fdrc_usbd_ep_transfer_recv(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vk_musb_fdrc_usbd_ep_transfer_send(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void vk_musb_fdrc_usbd_irq(vk_musb_fdrc_dcd_t *usbd);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
