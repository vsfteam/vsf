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
            __USB_DC_FROM_IP(__N, (__OBJ), __DRV_NAME, vsf_dwcotg_usbd)

/*============================ TYPES =========================================*/

struct vsf_dwcotg_dcd_param_t {
    const i_usb_dc_ip_t *op;
    implement(vsf_dwcotg_param_t)
};
typedef struct vsf_dwcotg_dcd_param_t vsf_dwcotg_dcd_param_t;

struct vsf_dwcotg_dc_ip_info_t {
    implement(usb_dc_ip_info_t)
    implement(vsf_dwcotg_hw_info_t)
};
typedef struct vsf_dwcotg_dc_ip_info_t vsf_dwcotg_dc_ip_info_t;

declare_simple_class(vsf_dwcotg_dcd_trans_t)
def_simple_class(vsf_dwcotg_dcd_trans_t) {
    private_member(
        uint8_t *buffer;
        uint32_t size   : 19;
        uint32_t use_dma: 1;
    )
};

declare_simple_class(vsf_dwcotg_dcd_t)
def_simple_class(vsf_dwcotg_dcd_t) {

    public_member(
        vsf_dwcotg_dcd_param_t const * const param;
    )

    private_member(
        implement(vsf_dwcotg_t)
        struct {
            usb_dc_evt_handler_t evt_handler;
            void *param;
        } callback;
        uint8_t setup[8];
        bool dma_en;
        bool status_phase;
        vsf_dwcotg_dcd_trans_t trans[2 * 16];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_dwcotg_usbd_init(vsf_dwcotg_dcd_t *usbd, usb_dc_cfg_t *cfg);
extern void vsf_dwcotg_usbd_fini(vsf_dwcotg_dcd_t *usbd);
extern void vsf_dwcotg_usbd_reset(vsf_dwcotg_dcd_t *usbd, usb_dc_cfg_t *cfg);

extern void vsf_dwcotg_usbd_connect(vsf_dwcotg_dcd_t *usbd);
extern void vsf_dwcotg_usbd_disconnect(vsf_dwcotg_dcd_t *usbd);
extern void vsf_dwcotg_usbd_wakeup(vsf_dwcotg_dcd_t *usbd);

extern void vsf_dwcotg_usbd_set_address(vsf_dwcotg_dcd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vsf_dwcotg_usbd_get_address(vsf_dwcotg_dcd_t *usbd);

extern uint_fast16_t vsf_dwcotg_usbd_get_frame_number(vsf_dwcotg_dcd_t *usbd);
extern uint_fast8_t vsf_dwcotg_usbd_get_mframe_number(vsf_dwcotg_dcd_t *usbd);

extern void vsf_dwcotg_usbd_get_setup(vsf_dwcotg_dcd_t *usbd, uint8_t *buffer);
extern void vsf_dwcotg_usbd_status_stage(vsf_dwcotg_dcd_t *usbd, bool is_in);

extern bool vsf_dwcotg_usbd_ep_is_dma(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_dwcotg_usbd_ep_add(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vsf_dwcotg_usbd_ep_get_size(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_dwcotg_usbd_ep_set_stall(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep);
extern bool vsf_dwcotg_usbd_ep_is_stalled(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_dwcotg_usbd_ep_clear_stall(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t vsf_dwcotg_usbd_ep_get_data_size(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_dwcotg_usbd_ep_read_buffer(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_dwcotg_usbd_ep_enable_OUT(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_dwcotg_usbd_ep_set_data_size(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vsf_dwcotg_usbd_ep_write_buffer(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vsf_dwcotg_usbd_ep_recv_dma(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_dwcotg_usbd_ep_send_dma(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size, bool zlp);

extern void vsf_dwcotg_usbd_irq(vsf_dwcotg_dcd_t *usbd);

#endif
#endif
/* EOF */
