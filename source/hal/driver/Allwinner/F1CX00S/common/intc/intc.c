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

/*============================ INCLUDES ======================================*/

#include "./intc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef void (*__isr_t)(void);

/*============================ GLOBAL VARIABLES ==============================*/
extern 
const __isr_t __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

ROOT 
#if __IS_COMPILER_IAR__
__irq __nested __arm 
#endif
void IRQ_Handler(void)
{
    __isr_t *isr_handler = NULL;
    int32_t irq_idx = 0;
    
    do {
        //! fetch the vector from INTC
        isr_handler = (__isr_t *)F1CX00S_INTC.VECTOR;
        if (F1CX00S_INTC.BASE_ADDR == (uintptr_t)isr_handler) {                 //!< take NMI as no IRQ
            break;
        }

        //! calculate irq_idx
        irq_idx = ((uintptr_t)isr_handler - (uintptr_t)F1CX00S_INTC.VTOR) >> 2;

        //! clear corresponding pending bit
        F1CX00S_INTC.PENDING[irq_idx >> 5] = 1 << (irq_idx & 0x1F);             //!< clear pending bit
        F1CX00S_INTC.STIR[irq_idx >> 5] &= ~(1 << (irq_idx & 0x1F));            //!< clear STIR bit
        F1CX00S_INTC.RESP[irq_idx >> 5] = (1 << (irq_idx & 0x1F));              //!< disable same level interrupt 
        isb();
        vsf_enable_interrupt();                                                 //!< enable preemption

        //! run the ISR
        (*isr_handler)();

        vsf_disable_interrupt();                                                //!< disable preemption
        F1CX00S_INTC.RESP[irq_idx >> 5] &= ~(1 << (irq_idx & 0x1F));            //!< enable same level interrupt

        isb();
    } while(true);              //!< try low priority ISR
}

void intc_init(void)
{
    __vsf_interrupt_safe(

        //! set vector table
        F1CX00S_INTC.VTOR = (uintptr_t)__VECTOR_TABLE;

        //! enable all interrupt source
        F1CX00S_INTC.SRC_MASK[0] = 0xFFFFFFFF;
        F1CX00S_INTC.SRC_MASK[1] = 0xFFFFFFFF;

        F1CX00S_INTC.NMI_INT_CTRL.NMI_SRC_TYPE = 0x01;

        //! disable all interrupt
        F1CX00S_INTC.DISABLE[0] = 0xFFFFFFFF;
        F1CX00S_INTC.DISABLE[1] = 0xFFFFFFFF;

        //! clear all pending bit
        F1CX00S_INTC.PENDING[0] = 0xFFFFFFFF;
        F1CX00S_INTC.PENDING[1] = 0xFFFFFFFF;

        //! Mask interrupts with lower or the same priority
        F1CX00S_INTC.RESP[0] = 0;
        F1CX00S_INTC.RESP[1] = 0;
   )
}


/**
  \brief   Disable Interrupt
  \details Disables a device specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__arm void intc_disable_irq(IRQn_Type IRQn)
{

    if (IRQn >= 64) {
        return ;
    }

    F1CX00S_INTC.DISABLE[(((uint32_t)IRQn) >> 5UL)] |= (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));

    dsb();
    isb();
}


#ifdef __cplusplus
}
#endif