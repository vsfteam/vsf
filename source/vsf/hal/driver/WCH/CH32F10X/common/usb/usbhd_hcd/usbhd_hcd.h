/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#ifndef __OSA_HAL_DRIVER_WCH_HC32F10X_USBHD_HCD_H__
#define __OSA_HAL_DRIVER_WCH_HC32F10X_USBHD_HCD_H__

/*============================ INCLUDES ======================================*/

#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_USE_USB_HOST == ENABLED

#include "component/usb/host/vsf_usbh.h"
#include "kernel/vsf_kernel.h"
#include "hal/vsf_hal.h"

//! include the infrastructure
//#include "../../io/io.h"
//#include "../../pm/pm.h"

#include "hal/interface/vsf_interface_usb.h"

#if     defined(CH32F10X_USBHD_HCD_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef CH32F10X_USBHD_HCD_CLASS_IMPLEMENT
#elif   defined(CH32F10X_USBHD_HCD_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#   undef CH32F10X_USBHD_HCD_CLASS_INHERIT
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#undef USB_HDC_HAS_CONST
#if !defined(USB_HDC_IRQN) || !defined(USB_DC_REG_BASE)
#   define USB_HDC_HAS_CONST
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(hc32f10x_usbhd_hcd_t)

#ifdef USB_HDC_HAS_CONST
struct hc32f10x_usbhd_const_t {
    // information from device.h
#ifndef USB_HDC_IRQN
    IRQn_Type irq;
#endif
#ifndef USB_HDC_REG_BASE
    void *reg;
#endif
};
typedef struct hc32f10x_usbhd_const_t hc32f10x_usbhd_const_t;
#endif

enum hc32f10x_usbhd_hcd_state_t {
    HC32F10X_HCD_STATE_WAIT_CONNECT,
    HC32F10X_HCD_STATE_WAIT_RESET,
    HC32F10X_HCD_STATE_WAIT_RESET_CLEAR,
    HC32F10X_HCD_STATE_CONNECTED,
};
typedef enum hc32f10x_usbhd_hcd_state_t hc32f10x_usbhd_hcd_state_t;

def_simple_class(hc32f10x_usbhd_hcd_t) {
    public_member(
#ifdef USB_HDC_HAS_CONST
        const hc32f10x_usbhd_const_t *param;
#endif
        vsf_arch_prio_t priority;
    )

    private_member(
        vsf_dlist_t urb_list;
        void *urb_cur;
        vsf_teda_t teda;
        vk_usbh_hcd_t *hcd;
        vk_usbh_dev_t *dev;
        hc32f10x_usbhd_hcd_state_t state;
        uint8_t cur_size;
        bool is_transacting;
        void *cur_buffer;
        uint8_t ep_buffer[64] ALIGN(4);
    )
};
typedef struct hc32f10x_usbhd_hcd_t hc32f10x_usbhd_hcd_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_hcd_drv_t hc32f10x_usbhd_hcd_drv;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_HOST
#endif      // __OSA_HAL_DRIVER_WCH_HC32F10X_USBHD_HCD_H__
/* EOF */
