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

#ifndef __VSF_HAL_DISTBUS_USBD_H__
#define __VSF_HAL_DISTBUS_USBD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"
#include "hal/vsf_hal_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_HAL_USE_USBD == ENABLED &&VSF_HAL_DISTBUS_USE_USBD == ENABLED

#include "hal/driver/common/template/vsf_template_usb.h"

#if     defined(__VSF_HAL_DISTBUS_USBD_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_DISTBUS_USBD_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_DISTBUS_USBD_CFG_MULTI_CLASS
#   define VSF_HAL_DISTBUS_USBD_CFG_MULTI_CLASS     VSF_USBD_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_usb_dc_from_distbus_ip(__n, __obj, __drv_name)                      \
        __USB_DC_FROM_IP(__n, (__obj), __drv_name, vsf_hal_distbus_usbd)

/*============================ TYPES =========================================*/

#if defined(__VSF_HAL_DISTBUS_USBD_CLASS_IMPLEMENT) || defined(__VSF_HAL_DISTBUS_USBD_CLASS_INHERIT__)
enum {
    VSF_HAL_DISTBUS_USBD_CMD_INIT = 0,
    VSF_HAL_DISTBUS_USBD_CMD_FINI,
    VSF_HAL_DISTBUS_USBD_CMD_RESET,
    VSF_HAL_DISTBUS_USBD_CMD_CONNECT,
    VSF_HAL_DISTBUS_USBD_CMD_DISCONNECT,
    VSF_HAL_DISTBUS_USBD_CMD_WAKEUP,
    VSF_HAL_DISTBUS_USBD_CMD_SET_ADDRESS,
    VSF_HAL_DISTBUS_USBD_CMD_STATUS_STAGE,
    VSF_HAL_DISTBUS_USBD_CMD_EP_ADD,
    VSF_HAL_DISTBUS_USBD_CMD_EP_SET_STALL,
    VSF_HAL_DISTBUS_USBD_CMD_EP_CLEAR_STALL,
    VSF_HAL_DISTBUS_USBD_CMD_EP_ENABLE_OUT,
    VSF_HAL_DISTBUS_USBD_CMD_EP_SET_DATA_SIZE,
    VSF_HAL_DISTBUS_USBD_CMD_EP_WRITE_BUFFER,
    VSF_HAL_DISTBUS_USBD_CMD_TRANSFER_SEND,
    VSF_HAL_DISTBUS_USBD_CMD_TRANSFER_RECV,
    VSF_HAL_DISTBUS_USBD_CMD_ON_EVT,

    VSF_HAL_DISTBUS_USBD_CMD_ADDR_RANGE,
};
#endif

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
    protected_member(
        vsf_distbus_service_t               service;
    )
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

        struct {
            usb_dc_evthandler_t             evthandler;
            void                            *param;
        } callback;

        vsf_distbus_t                       *distbus;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern uint32_t vsf_hal_distbus_usbd_register_service(vsf_distbus_t *distbus, vsf_hal_distbus_usbd_t *usbd, void *info, uint32_t infolen);

extern vsf_err_t vsf_hal_distbus_usbd_init(vsf_hal_distbus_usbd_t *usbd, usb_dc_cfg_t *cfg);
extern void vsf_hal_distbus_usbd_fini(vsf_hal_distbus_usbd_t *usbd);
extern void vsf_hal_distbus_usbd_reset(vsf_hal_distbus_usbd_t *usbd, usb_dc_cfg_t *cfg);

extern void vsf_hal_distbus_usbd_connect(vsf_hal_distbus_usbd_t *usbd);
extern void vsf_hal_distbus_usbd_disconnect(vsf_hal_distbus_usbd_t *usbd);
extern void vsf_hal_distbus_usbd_wakeup(vsf_hal_distbus_usbd_t *usbd);

extern void vsf_hal_distbus_usbd_set_address(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vsf_hal_distbus_usbd_get_address(vsf_hal_distbus_usbd_t *usbd);

extern uint_fast16_t vsf_hal_distbus_usbd_get_frame_number(vsf_hal_distbus_usbd_t *usbd);
extern uint_fast8_t vsf_hal_distbus_usbd_get_mframe_number(vsf_hal_distbus_usbd_t *usbd);

extern void vsf_hal_distbus_usbd_get_setup(vsf_hal_distbus_usbd_t *usbd, uint8_t *buffer);
extern void vsf_hal_distbus_usbd_status_stage(vsf_hal_distbus_usbd_t *usbd, bool is_in);

extern uint_fast8_t vsf_hal_distbus_usbd_ep_get_feature(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t vsf_hal_distbus_usbd_ep_add(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vsf_hal_distbus_usbd_ep_get_size(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_hal_distbus_usbd_ep_set_stall(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep);
extern bool vsf_hal_distbus_usbd_ep_is_stalled(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_hal_distbus_usbd_ep_clear_stall(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t vsf_hal_distbus_usbd_ep_get_data_size(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_hal_distbus_usbd_ep_transaction_read_buffer(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_hal_distbus_usbd_ep_transaction_enable_out(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_hal_distbus_usbd_ep_transaction_set_data_size(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vsf_hal_distbus_usbd_ep_transaction_write_buffer(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vsf_hal_distbus_usbd_ep_transfer_recv(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vsf_hal_distbus_usbd_ep_transfer_send(vsf_hal_distbus_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void vsf_hal_distbus_usbd_irq(vsf_hal_distbus_usbd_t *usbd);

#ifdef __cplusplus
}
#endif

#undef __VSF_HAL_DISTBUS_USBD_CLASS_IMPLEMENT
#undef __VSF_HAL_DISTBUS_USBD_CLASS_INHERIT__

#endif
#endif
/* EOF */
