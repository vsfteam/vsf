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

#ifndef __HAL_DRIVER_${SERIES/TIMER_IP}_TIMER_H__
#define __HAL_DRIVER_${SERIES/TIMER_IP}_TIMER_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_TIMER == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

// IPCore
#if     defined(__VSF_HAL_${TIMER_IP}_TIMER_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${TIMER_IP}_TIMER_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${TIMER_IP}_TIMER_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_TIMER_CFG_MULTI_CLASS should be in timer.c.
 */

// IPCore
#ifndef VSF_${TIMER_IP}_TIMER_CFG_MULTI_CLASS
#   define VSF_${TIMER_IP}_TIMER_CFG_MULTI_CLASS    VSF_TIMER_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw TIMER driver can reimplement vsf_usart_mode_t/vsf_usart_irq_mask_t/vsf_usart_status_t.
 *      To enable reimplementation, please enable macro below:
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE for vsf_timer_mode_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_timer_irq_mask_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE     ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK         ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${timer_ip}_timer_t) {
#if VSF_${TIMER_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_timer_t               vsf_timer;
    )
#endif

/*\note You can add more member in vsf_${timer_ip}_timer_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${timer_ip}_timer_reg_t *reg;
        vsf_timer_isr_t           isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
typedef enum vsf_timer_mode_t {
    VSF_TIMER_MODE_ONESHOT      = (0x00 << 0),
    VSF_TIMER_MODE_CONTINUES    = (0x01 << 0),

    VSF_TIMER_MODE_PWM          = (0x01 << 1),
    VSF_TIMER_MODE_NO_PWM       = (0x00 << 1),

    // more vendor specified modes can be added here
} vsf_timer_mode_t;

typedef enum vsf_timer_irq_mask_t {
    VSF_TIMER_IRQ_MASK_OVERFLOW = (0x01 << 0),

    // more vendor specified irq_masks can be added here
} vsf_timer_irq_mask_t;
// HW/IPCore end

/*============================ INCLUDES ======================================*/

// IPCore
/*\note Extern APIs for ip core diriver.
 *      There is no requirement about how APIs of IPCore drivers should be implemented.
 *      Just consider the simplicity for actual peripheral drivers.
 */
// IPCore end

#ifdef __cplusplus
}
#endif

// IPCore
#undef __VSF_HAL_${TIMER_IP}_TIMER_CLASS_IMPLEMENT
#undef __VSF_HAL_${TIMER_IP}_TIMER_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_TIMER
#endif      // __HAL_DRIVER_${SERIES/TIMER_IP}_TIMER_H__
/* EOF */
