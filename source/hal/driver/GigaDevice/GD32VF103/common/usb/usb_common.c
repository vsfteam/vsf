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

/*============================ INCLUDES ======================================*/

//#include "../common.h"
#include "./usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t gd32vf103_usb_init(gd32vf103_usb_t *usb, vsf_arch_prio_t priority)
{
    struct dwcotg_core_global_regs_t *global_regs;
    const gd32vf103_usb_const_t *param = usb->param;
    uint_fast32_t usbfs_prescaler;
    // TODO: get real ckpll_clock
    uint_fast32_t ckpll_clock = 96000000;

    if (ckpll_clock == 48000000) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1;
    } else if (ckpll_clock == 72000000) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1_5;
    } else if (ckpll_clock == 96000000) {
          usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2;
    } else if (ckpll_clock == 120000000) {
          usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2_5;
    } else {
        VSF_HAL_ASSERT(false);
    }

    rcu_usb_clock_config(usbfs_prescaler);
    rcu_periph_clock_enable(RCU_USBFS);

    global_regs = param->reg;
    global_regs->gahbcfg &= ~USB_OTG_GAHBCFG_GINT;

    if (priority != vsf_arch_prio_invalid) {
        eclic_set_irq_lvl(param->irq, priority << (8 - VSF_ARCH_PRI_BIT));
        eclic_enable_interrupt(param->irq);
    } else {
        eclic_disable_interrupt(param->irq);
    }
    return VSF_ERR_NONE;
}

void gd32vf103_usb_irq(gd32vf103_usb_t *usb)
{
    if (usb->callback.irq_handler != NULL) {
        usb->callback.irq_handler(usb->callback.param);
    }
}
