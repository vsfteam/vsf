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

#include "../common.h"
#include "./usb.h"

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t __mt071_usb_init_interrupt(mt071_usb_t *usb, vsf_arch_prio_t priority)
{
    {
        USB_Common_Typedef *reg = usb->param->reg;
        reg->Power = 0;
        reg->IntrUSBE = 0;
        reg->IntrRxE = 0;
        reg->IntrTxE = 0;
        reg->IntrUSB = 0;
        reg->IntrRx = 0;
        reg->IntrTx = 0;
    }

    if (priority >= 0) {
        IRQn_Type irq = usb->param->irq;
        NVIC_SetPriority(irq, (uint32_t)priority);
        NVIC_EnableIRQ(irq);
    }
    return VSF_ERR_NONE;
}

void __mt071_usb_irq(mt071_usb_t *usb)
{
    if (usb->callback.irq_handler != NULL) {
        usb->callback.irq_handler(usb->callback.param);
    }
}

#endif      // VSF_HAL_USE_USBD && VSF_HAL_USE_USBH
