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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_MUSB_FDRC == ENABLED

#if     defined(VSF_MUSB_FDRC_DCD_IMPLEMENT)
#   undef VSF_MUSB_FDRC_DCD_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_MUSB_FDRC_DCD_INHERIT)
#   undef VSF_MUSB_FDRC_DCD_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_USB_DC_FROM_MUSB_FDRC_IP(__N, __OBJ, __DRV_NAME)                    \
        __USB_DC_FROM_IP(__N, (__OBJ), __DRV_NAME, vsf_musb_fdrc_usbd)

/*============================ TYPES =========================================*/

struct vsf_musb_fdrc_dcd_param_t {
    const i_usb_dc_ip_t *op;
};
typedef struct vsf_musb_fdrc_dcd_param_t vsf_musb_fdrc_dcd_param_t;

declare_simple_class(vsf_musb_fdrc_dcd_t)

enum vsf_musb_fdrc_dcd_ep0state_t {
    MUSB_FDRC_USBD_EP0_IDLE,
    MUSB_FDRC_USBD_EP0_DATA_IN,
    MUSB_FDRC_USBD_EP0_DATA_OUT,
};
typedef enum vsf_musb_fdrc_dcd_ep0state_t vsf_musb_fdrc_dcd_ep0state_t;

def_simple_class(vsf_musb_fdrc_dcd_t) {

    public_member(
        vsf_musb_fdrc_dcd_param_t const * const param;
    )

    private_member(
        void *reg;
        struct {
            usb_dc_evt_handler_t evt_handler;
            void *param;
        } callback;
        uint16_t ep_buf_ptr;
        vsf_musb_fdrc_dcd_ep0state_t ep0_state;
        bool has_data_stage;
        uint8_t ep_num;
        bool is_dma;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_musb_fdrc_usbd_init(vsf_musb_fdrc_dcd_t *usbd, usb_dc_cfg_t *cfg);
extern void vsf_musb_fdrc_usbd_fini(vsf_musb_fdrc_dcd_t *usbd);
extern void vsf_musb_fdrc_usbd_reset(vsf_musb_fdrc_dcd_t *usbd, usb_dc_cfg_t *cfg);

extern void vsf_musb_fdrc_usbd_connect(vsf_musb_fdrc_dcd_t *usbd);
extern void vsf_musb_fdrc_usbd_disconnect(vsf_musb_fdrc_dcd_t *usbd);
extern void vsf_musb_fdrc_usbd_wakeup(vsf_musb_fdrc_dcd_t *usbd);

extern void vsf_musb_fdrc_usbd_set_address(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vsf_musb_fdrc_usbd_get_address(vsf_musb_fdrc_dcd_t *usbd);

extern uint_fast16_t vsf_musb_fdrc_usbd_get_frame_number(vsf_musb_fdrc_dcd_t *usbd);
extern uint_fast8_t vsf_musb_fdrc_usbd_get_mframe_number(vsf_musb_fdrc_dcd_t *usbd);

extern void vsf_musb_fdrc_usbd_get_setup(vsf_musb_fdrc_dcd_t *usbd, uint8_t *buffer);
extern void vsf_musb_fdrc_usbd_status_stage(vsf_musb_fdrc_dcd_t *usbd, bool is_in);

extern bool vsf_musb_fdrc_usbd_ep_is_dma(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_add(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vsf_musb_fdrc_usbd_ep_get_size(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_musb_fdrc_usbd_ep_set_stall(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);
extern bool vsf_musb_fdrc_usbd_ep_is_stalled(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_clear_stall(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t vsf_musb_fdrc_usbd_ep_get_data_size(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_read_buffer(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_enable_OUT(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_musb_fdrc_usbd_ep_set_data_size(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_write_buffer(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vsf_musb_fdrc_usbd_ep_recv_dma(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_send_dma(vsf_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size, bool zlp);

extern void vsf_musb_fdrc_usbd_irq(vsf_musb_fdrc_dcd_t *usbd);

#endif
#endif
/* EOF */
