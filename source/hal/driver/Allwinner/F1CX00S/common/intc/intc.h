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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_INTC_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_INTC_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"
#include "utilities/vsf_utilities.h"
#include "./i_reg_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/


/**
  \brief   Enable Interrupt
  \details Enables a device specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void intc_enable_irq(IRQn_Type IRQn)
{
    if (IRQn >= 64) {
        return ;
    }

    __COMPILER_BARRIER();
    F1CX00S_INTC.DISABLE[(((uint32_t)IRQn) >> 5UL)] &= ~(uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __COMPILER_BARRIER();
}


/**
  \brief   Get Interrupt Enable status
  \details Returns a device specific interrupt enable status from the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt is not enabled.
  \return             1  Interrupt is enabled.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE bool intc_is_irq_enabled(IRQn_Type IRQn)
{
    if (IRQn >= 64) {
        return false;
    }

    return !(   (   F1CX00S_INTC.DISABLE[(((uint32_t)IRQn) >> 5UL)] 
                &   (1UL << (((uint32_t)IRQn) & 0x1FUL)))
            != 0UL);
}


/**
  \brief   Disable Interrupt
  \details Disables a device specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__arm 
extern void intc_disable_irq(IRQn_Type IRQn);


/**
  \brief   Get Pending Interrupt
  \details Reads the NVIC pending register and returns the pending bit for the specified device specific interrupt.
  \param [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt status is not pending.
  \return             1  Interrupt status is pending.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE bool intc_is_irq_pending(IRQn_Type IRQn)
{
    if (IRQn >= 64) {
        return false;
    }

    return (    (   F1CX00S_INTC.PENDING[(((uint32_t)IRQn) >> 5UL)] 
                &   (1UL << (((uint32_t)IRQn) & 0x1FUL))) 
            != 0UL);
}


/**
  \brief   Set Pending Interrupt
  \details Sets the pending bit of a device specific interrupt in the NVIC pending register.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void intc_pending_set(IRQn_Type IRQn)
{
    if (IRQn >= 64) {
        return ;
    }

    F1CX00S_INTC.STIR[(((uint32_t)IRQn) >> 5UL)] 
        |= (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));

}


/**
  \brief   Clear Pending Interrupt
  \details Clears the pending bit of a device specific interrupt in the NVIC pending register.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void intc_pending_clear(IRQn_Type IRQn)
{
    if (IRQn >= 64) {
        return ;
    }


    F1CX00S_INTC.PENDING[(((uint32_t)IRQn) >> 5UL)] 
        = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    F1CX00S_INTC.STIR[(((uint32_t)IRQn) >> 5UL)] 
        &= ~(uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));

}


__STATIC_INLINE uint32_t intc_get_active_irq(void)
{
    return (F1CX00S_INTC.VECTOR - F1CX00S_INTC.VTOR) >> 2;
}


/**
  \brief   Set Interrupt Priority
  \details Sets the priority of a device specific interrupt or a processor exception.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
  \param [in]      IRQn  Interrupt number.
  \param [in]  priority  Priority to set.
  \note    The priority cannot be set for every processor exception.
 */
__STATIC_INLINE void intc_priority_set(IRQn_Type IRQn, uint32_t priority)
{
    if (IRQn >= 64) {
        return ;
    }

    F1CX00S_INTC.PRIO[IRQn>>4] &= ~(0x03 << (IRQn & 0x0F));
    F1CX00S_INTC.PRIO[IRQn>>4] |= ((priority & 0x03) << (IRQn & 0x0F));
}


/**
  \brief   Get Interrupt Priority
  \details Reads the priority of a device specific interrupt or a processor exception.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
  \param [in]   IRQn  Interrupt number.
  \return             Interrupt Priority.
                      Value is aligned automatically to the implemented priority bits of the microcontroller.
 */
__STATIC_INLINE uint32_t intc_priority_get(IRQn_Type IRQn)
{
    if (IRQn >= 64) {
        return 0;
    }
    
    return F1CX00S_INTC.PRIO[IRQn>>4] & (0x03 << (IRQn & 0x0F));
}

extern void intc_init(void);


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
