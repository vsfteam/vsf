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

/*============================ INCLUDES ======================================*/

#include "../__common.h"
#include "./usb.h"

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED

#include "component/usb/driver/otg/musb/fdrc/vsf_musb_fdrc_hw.h"

#ifdef FALSE
#   undef FALSE
#endif
#ifdef TRUE
#   undef TRUE
#endif
#include "air105.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __AIR105_MUSB_ISR_DEF(__N, __VALUE)                                     \
            USB_OTG##__N##_IRQHandler,
#define _AIR105_MUSB_ISR_DEF(__N, __VALUE)      __AIR105_MUSB_ISR_DEF(__N, __VALUE)
#define AIR105_MUSB_ISR_DEF(__N, __VALUE)       _AIR105_MUSB_ISR_DEF(__N, __VALUE)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __vsf_hw_usb_irq(vsf_hw_usb_t *usb)
{
    if (usb->callback.irqhandler != NULL) {
        usb->callback.irqhandler(usb->callback.param);
    }
}

vsf_err_t __vsf_hw_usb_init_interrupt(vsf_hw_usb_t *usb, vsf_arch_prio_t priority)
{
    {
        vk_musb_fdrc_common_reg_t *common_reg = (vk_musb_fdrc_common_reg_t *)usb->param->reg;
        common_reg->Power = 0;
        common_reg->IntrUSBE = 0;
        common_reg->IntrTx1E = 0;
        common_reg->IntrTx2E = 0;
        common_reg->IntrRx1E = 0;
        common_reg->IntrRx2E = 0;
    }

    if (priority >= 0) {
        IRQn_Type irq = usb->param->irqn;
        NVIC_SetPriority(irq, (uint32_t)priority);
        NVIC_EnableIRQ(irq);
    }
    return VSF_ERR_NONE;
}

void __vsf_hw_usb_fifo_reset(vsf_hw_usb_t *usb)
{
}

#endif      // VSF_HAL_USE_USBD && VSF_HAL_USE_USBH
