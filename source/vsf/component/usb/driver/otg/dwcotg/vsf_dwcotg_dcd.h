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

#ifndef __VSF_DWCOTG_DCD_H__
#define __VSF_DWCOTG_DCD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_DWCOTG == ENABLED

#include "./vsf_dwcotg_common.h"

#if     defined(VSF_DWCOTG_DCD_IMPLEMENT)
#   undef VSF_DWCOTG_DCD_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_DWCOTG_DCD_INHERIT)
#   undef VSF_DWCOTG_DCD_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_USB_DC_FROM_DWCOTG_IP(__N, __OBJ, __DRV_NAME)                       \
            __USB_DC_FROM_IP(__N, (__OBJ), __DRV_NAME, vk_dwcotg_usbd)

/*============================ TYPES =========================================*/

struct vk_dwcotg_dcd_param_t {
    const i_usb_dc_ip_t *op;
    implement(vk_dwcotg_param_t)
};
typedef struct vk_dwcotg_dcd_param_t vk_dwcotg_dcd_param_t;

struct vk_dwcotg_dc_ip_info_t {
    implement(usb_dc_ip_info_t)
    implement(vk_dwcotg_hw_info_t)
};
typedef struct vk_dwcotg_dc_ip_info_t vk_dwcotg_dc_ip_info_t;

declare_simple_class(vk_dwcotg_dcd_trans_t)
def_simple_class(vk_dwcotg_dcd_trans_t) {
    private_member(
        uint8_t *buffer;
        uint32_t remain;
        uint32_t size;
        uint32_t zlp    : 1;
        uint32_t use_dma: 1;
    )
};

enum ctrl_transfer_state_t{
    DWCOTG_SETUP_STAGE,
    DWCOTG_DATA_STAGE,
    DWCOTG_STATUS_STAGE,
};
typedef enum ctrl_transfer_state_t ctrl_transfer_state_t;

declare_simple_class(vk_dwcotg_dcd_t)
def_simple_class(vk_dwcotg_dcd_t) {

    public_member(
        vk_dwcotg_dcd_param_t const * const param;
    )

    private_member(
        implement(vk_dwcotg_t)
        struct {
            usb_dc_evt_handler_t evt_handler;
            void *param;
        } callback;
        uint8_t setup[8];
        uint16_t buffer_word_pos;
        bool dma_en;
        ctrl_transfer_state_t ctrl_transfer_state;
        vk_dwcotg_dcd_trans_t trans[2 * VSF_DWCOTG_DCD_CFG_EP_NUM];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_dwcotg_usbd_init(vk_dwcotg_dcd_t *usbd, usb_dc_cfg_t *cfg);
extern void vk_dwcotg_usbd_fini(vk_dwcotg_dcd_t *usbd);
extern void vk_dwcotg_usbd_reset(vk_dwcotg_dcd_t *usbd, usb_dc_cfg_t *cfg);

extern void vk_dwcotg_usbd_connect(vk_dwcotg_dcd_t *usbd);
extern void vk_dwcotg_usbd_disconnect(vk_dwcotg_dcd_t *usbd);
extern void vk_dwcotg_usbd_wakeup(vk_dwcotg_dcd_t *usbd);

extern void vk_dwcotg_usbd_set_address(vk_dwcotg_dcd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vk_dwcotg_usbd_get_address(vk_dwcotg_dcd_t *usbd);

extern uint_fast16_t vk_dwcotg_usbd_get_frame_number(vk_dwcotg_dcd_t *usbd);
extern uint_fast8_t vk_dwcotg_usbd_get_mframe_number(vk_dwcotg_dcd_t *usbd);

extern void vk_dwcotg_usbd_get_setup(vk_dwcotg_dcd_t *usbd, uint8_t *buffer);
extern void vk_dwcotg_usbd_status_stage(vk_dwcotg_dcd_t *usbd, bool is_in);

extern uint_fast8_t vk_dwcotg_usbd_ep_get_feature(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_dwcotg_usbd_ep_add(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vk_dwcotg_usbd_ep_get_size(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vk_dwcotg_usbd_ep_set_stall(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep);
extern bool vk_dwcotg_usbd_ep_is_stalled(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_dwcotg_usbd_ep_clear_stall(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t vk_dwcotg_usbd_ep_get_data_size(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vk_dwcotg_usbd_ep_transaction_read_buffer(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vk_dwcotg_usbd_ep_transaction_enable_out(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_dwcotg_usbd_ep_transaction_set_data_size(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vk_dwcotg_usbd_ep_transaction_write_buffer(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vk_dwcotg_usbd_ep_transfer_recv(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vk_dwcotg_usbd_ep_transfer_send(vk_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void vk_dwcotg_usbd_irq(vk_dwcotg_dcd_t *usbd);

#endif
#endif
/* EOF */
