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

#ifndef __VSF_DISTBUS_DCD_H__
#define __VSF_DISTBUS_DCD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     VSF_USE_USB_DEVICE == ENABLED                                           \
    &&  VSF_HAL_USE_DISTBUS == ENABLED && VSF_HAL_USE_DISTBUS_USBD == ENABLED

#include "hal/driver/common/template/vsf_template_usb.h"

#if     defined(__VSF_DISTBUS_DCD_CLASS_IMPLEMENT)
#   undef __VSF_DISTBUS_DCD_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_usb_dc_from_distbus_ip(__n, __obj, __drv_name)                      \
        __USB_DC_FROM_IP(__n, (__obj), __drv_name, vsf_hal_distbus_usbd)

/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_hal_distbus_t)

typedef struct vsf_hal_distbus_usbd_ep_t {
    uint16_t                                size;
    uint8_t                                 type : 2;
    uint8_t                                 is_stalled : 1;
    uint8_t                                 zlp : 1;
    uint8_t                                 pending : 1;
    uint8_t                                 ep;

    vsf_distbus_msg_t                       *msg;
} vsf_hal_distbus_usbd_ep_t;

vsf_class(vsf_hal_distbus_usbd_t) {
    private_member(
        union {
            struct {
                vsf_hal_distbus_usbd_ep_t   ep_out[16];
                vsf_hal_distbus_usbd_ep_t   ep_in[16];
            };
            vsf_hal_distbus_usbd_ep_t       ep[32];
        };
        uint8_t                             setup[8];
        uint16_t                            frame_number;
        uint16_t                            mframe_number;
        uint8_t                             address;
        uint8_t                             ep_feature;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_hal_distbus_usbd_init(vsf_hal_distbus_t *hal_distbus, usb_dc_cfg_t *cfg);
extern void vsf_hal_distbus_usbd_fini(vsf_hal_distbus_t *hal_distbus);
extern void vsf_hal_distbus_usbd_reset(vsf_hal_distbus_t *hal_distbus, usb_dc_cfg_t *cfg);

extern void vsf_hal_distbus_usbd_connect(vsf_hal_distbus_t *hal_distbus);
extern void vsf_hal_distbus_usbd_disconnect(vsf_hal_distbus_t *hal_distbus);
extern void vsf_hal_distbus_usbd_wakeup(vsf_hal_distbus_t *hal_distbus);

extern void vsf_hal_distbus_usbd_set_address(vsf_hal_distbus_t *hal_distbus, uint_fast8_t addr);
extern uint_fast8_t vsf_hal_distbus_usbd_get_address(vsf_hal_distbus_t *hal_distbus);

extern uint_fast16_t vsf_hal_distbus_usbd_get_frame_number(vsf_hal_distbus_t *hal_distbus);
extern uint_fast8_t vsf_hal_distbus_usbd_get_mframe_number(vsf_hal_distbus_t *hal_distbus);

extern void vsf_hal_distbus_usbd_get_setup(vsf_hal_distbus_t *hal_distbus, uint8_t *buffer);
extern void vsf_hal_distbus_usbd_status_stage(vsf_hal_distbus_t *hal_distbus, bool is_in);

extern uint_fast8_t vsf_hal_distbus_usbd_ep_get_feature(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t vsf_hal_distbus_usbd_ep_add(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vsf_hal_distbus_usbd_ep_get_size(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep);

extern vsf_err_t vsf_hal_distbus_usbd_ep_set_stall(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep);
extern bool vsf_hal_distbus_usbd_ep_is_stalled(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep);
extern vsf_err_t vsf_hal_distbus_usbd_ep_clear_stall(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep);

extern uint_fast32_t vsf_hal_distbus_usbd_ep_get_data_size(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep);

extern vsf_err_t vsf_hal_distbus_usbd_ep_transaction_read_buffer(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_hal_distbus_usbd_ep_transaction_enable_out(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep);
extern vsf_err_t vsf_hal_distbus_usbd_ep_transaction_set_data_size(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vsf_hal_distbus_usbd_ep_transaction_write_buffer(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vsf_hal_distbus_usbd_ep_transfer_recv(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vsf_hal_distbus_usbd_ep_transfer_send(vsf_hal_distbus_t *hal_distbus, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void vsf_hal_distbus_usbd_irq(vsf_hal_distbus_t *hal_distbus);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
