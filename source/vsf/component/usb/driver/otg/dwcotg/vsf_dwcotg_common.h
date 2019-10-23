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

#ifndef __VSF_DWCOTG_COMMON_H__
#define __VSF_DWCOTG_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     (VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_DWCOTG == ENABLED)\
    ||  (VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_DWCOTG == ENABLED)

#include "hal/vsf_hal.h"
#include "hal/interface/vsf_interface_usb.h"

#include "./vsf_dwcotg_hw.h"

/*============================ MACROS ========================================*/

#ifndef VSF_DWCOTG_DCD_CFG_EP_NUM
#   ifdef USB_DWCOTG_MAX_EP_NUM
#       define VSF_DWCOTG_DCD_CFG_EP_NUM        USB_DWCOTG_MAX_EP_NUM
#   else
#       define VSF_DWCOTG_DCD_CFG_EP_NUM        16
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsf_dwcotg_param_t {
    union {
        struct {
            uint8_t speed : 4;
            uint8_t ulpi_en : 1;
            uint8_t utmi_en : 1;
            uint8_t vbus_en : 1;
        };
        uint8_t feature;
    };
};
typedef struct vsf_dwcotg_param_t vsf_dwcotg_param_t;

struct vsf_dwcotg_reg_t {
    // Core Global registers starting at offset 000h
	struct dwcotg_core_global_regs_t *global_regs;
    union {
        struct {
            // Host Global Registers starting at offset 400h.
            struct dwcotg_host_global_regs_t *global_regs;
            // Host Port 0 Control and Status Register at offset 440h.
            volatile uint32_t *hprt0;
            // Host Channel Specific Registers at offsets 500h-5FCh.
            struct dwcotg_hc_regs_t *hc_regs;
        } host;
        struct {
            // Device Global Registers starting at offset 800h
            struct dwcotg_dev_global_regs_t *global_regs;
            struct {
                // Device Logical IN Endpoint-Specific Registers 900h-AFCh
                struct dwcotg_dev_in_ep_regs_t *in_regs;
                // Device Logical OUT Endpoint-Specific Registers B00h-CFCh
                struct dwcotg_dev_out_ep_regs_t *out_regs;
            } ep;
        } dev;
    };
    uint32_t * dfifo[VSF_DWCOTG_DCD_CFG_EP_NUM];
};
typedef struct vsf_dwcotg_reg_t vsf_dwcotg_reg_t;

struct vsf_dwcotg_t {
    vsf_dwcotg_reg_t reg;
    uint8_t ep_num;
};
typedef struct vsf_dwcotg_t vsf_dwcotg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

void vsf_dwcotg_phy_init(vsf_dwcotg_t *dwcotg,
                        const vsf_dwcotg_param_t *param,
                        vsf_dwcotg_hw_info_t *hw_info);

#endif
#endif
/* EOF */
