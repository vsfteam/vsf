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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED

#include "./vsf_dwcotg_common.h"

#if     defined(__VSF_DWCOTG_DCD_CLASS_IMPLEMENT)
#   undef __VSF_DWCOTG_DCD_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_usb_dc_from_dwcotg_ip(__n, __obj, __drv_name)                       \
            __USB_DC_FROM_IP(__n, (__obj), __drv_name, vk_dwcotg_dcd)

/*============================ TYPES =========================================*/

typedef struct vk_dwcotg_dcd_param_t {
    const i_usb_dc_ip_t *op;
    implement(vk_dwcotg_param_t)
} vk_dwcotg_dcd_param_t;

typedef struct vk_dwcotg_dc_ip_info_t {
    implement(usb_dc_ip_info_t)
    implement(vk_dwcotg_hw_info_t)
} vk_dwcotg_dc_ip_info_t;

dcl_simple_class(vk_dwcotg_dcd_trans_t)
def_simple_class(vk_dwcotg_dcd_trans_t) {
    private_member(
        uint8_t *buffer;
        uint32_t remain;
        uint32_t size;
        uint32_t zlp    : 1;
        uint32_t use_dma: 1;
    )
};

typedef enum ctrl_transfer_state_t {
    DWCOTG_SETUP_STAGE,
    DWCOTG_DATA_STAGE,
    DWCOTG_STATUS_STAGE,
} ctrl_transfer_state_t;

dcl_simple_class(vk_dwcotg_dcd_t)
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

extern vsf_err_t vk_dwcotg_dcd_init(vk_dwcotg_dcd_t *dwcotg_dcd, usb_dc_cfg_t *cfg);
extern void vk_dwcotg_dcd_fini(vk_dwcotg_dcd_t *dwcotg_dcd);
extern void vk_dwcotg_dcd_reset(vk_dwcotg_dcd_t *dwcotg_dcd, usb_dc_cfg_t *cfg);

extern void vk_dwcotg_dcd_connect(vk_dwcotg_dcd_t *dwcotg_dcd);
extern void vk_dwcotg_dcd_disconnect(vk_dwcotg_dcd_t *dwcotg_dcd);
extern void vk_dwcotg_dcd_wakeup(vk_dwcotg_dcd_t *dwcotg_dcd);

extern void vk_dwcotg_dcd_set_address(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t addr);
extern uint_fast8_t vk_dwcotg_dcd_get_address(vk_dwcotg_dcd_t *dwcotg_dcd);

extern uint_fast16_t vk_dwcotg_dcd_get_frame_number(vk_dwcotg_dcd_t *dwcotg_dcd);
extern uint_fast8_t vk_dwcotg_dcd_get_mframe_number(vk_dwcotg_dcd_t *dwcotg_dcd);

extern void vk_dwcotg_dcd_get_setup(vk_dwcotg_dcd_t *dwcotg_dcd, uint8_t *buffer);
extern void vk_dwcotg_dcd_status_stage(vk_dwcotg_dcd_t *dwcotg_dcd, bool is_in);

extern uint_fast8_t vk_dwcotg_dcd_ep_get_feature(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t vk_dwcotg_dcd_ep_add(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vk_dwcotg_dcd_ep_get_size(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep);

extern vsf_err_t vk_dwcotg_dcd_ep_set_stall(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep);
extern bool vk_dwcotg_dcd_ep_is_stalled(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep);
extern vsf_err_t vk_dwcotg_dcd_ep_clear_stall(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep);

extern uint_fast32_t vk_dwcotg_dcd_ep_get_data_size(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep);

extern vsf_err_t vk_dwcotg_dcd_ep_transaction_read_buffer(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vk_dwcotg_dcd_ep_transaction_enable_out(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep);
extern vsf_err_t vk_dwcotg_dcd_ep_transaction_set_data_size(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vk_dwcotg_dcd_ep_transaction_write_buffer(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vk_dwcotg_dcd_ep_transfer_recv(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vk_dwcotg_dcd_ep_transfer_send(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void vk_dwcotg_dcd_irq(vk_dwcotg_dcd_t *dwcotg_dcd);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
