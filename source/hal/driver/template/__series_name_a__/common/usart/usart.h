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

#ifndef __HAL_DRIVER_${SERIES/USART_IP}_USART_H__
#define __HAL_DRIVER_${SERIES/USART_IP}_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripherial drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Incudes CAN ONLY be put here. */
/*\note If current header is for a peripherial driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

// IPCore
#if     defined(__VSF_HAL_${USART_IP}_USART_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${USART_IP}_USART_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${USART_IP}_USART_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_USART_CFG_MULTI_CLASS should be in usart.c.
 */

// IPCore
#ifndef VSF_${USART_IP}_USART_CFG_MULTI_CLASS
#   define VSF_${USART_IP}_USART_CFG_MULTI_CLASS    VSF_USART_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw USART driver can reimplement vsf_usart_mode_t/vsf_usart_irq_mask_t/vsf_usart_status_t.
 *      To enable reimplementation, please enable macro below:
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_MODE for vsf_usart_mode_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS for vsf_usart_status_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_usart_irq_mask_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
// HW end

// TODO: add comments about fifo2req

/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${usart_ip}_usart_t) {
#if VSF_${USART_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_usart_t                 vsf_usart;
    )
#endif

/*\note You can add more memmber in vsf_${usart_ip}_usart_t instance.
 *      For members accessable from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${usart_ip}_iwc_reg_t  *reg;
        vsf_usart_isr_t             isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
typedef enum vsf_usart_mode_t {
    VSF_USART_9_BIT_LENGTH              = (0x4ul << 0),
    VSF_USART_8_BIT_LENGTH              = (0x3ul << 0),
    VSF_USART_7_BIT_LENGTH              = (0x2ul << 0),
    VSF_USART_6_BIT_LENGTH              = (0x1ul << 0),
    VSF_USART_5_BIT_LENGTH              = (0x0ul << 0),

    VSF_USART_1_STOPBIT                 = (0x0ul << 3),
    VSF_USART_2_STOPBIT                 = (0x1ul << 3),
    VSF_USART_1_5_STOPBIT               = (0x2ul << 3),

    VSF_USART_NO_PARITY                 = (0x0ul << 5),
    VSF_USART_ODD_PARITY                = (0x1ul << 5),
    VSF_USART_EVEN_PARITY               = (0x2ul << 5),
    VSF_USART_FORCE_0_PARITY            = (0x3ul << 5),
    VSF_USART_FORCE_1_PARITY            = (0x4ul << 5),

    VSF_USART_NO_HWCONTROL              = (0x0ul << 8),
    VSF_USART_RTS_HWCONTROL             = (0x1ul << 8),
    VSF_USART_CTS_HWCONTROL             = (0x2ul << 8),
    VSF_USART_RTS_CTS_HWCONTROL         = (0x3ul << 8),

    VSF_USART_TX_ENABLE                 = (0x1ul << 10),
    VSF_USART_TX_DISABLE                = (0x0ul << 10),
    VSF_USART_RX_ENABLE                 = (0x1ul << 11),
    VSF_USART_RX_DISABLE                = (0x0ul << 11),

    // more vendor specified modes can be added here
} vsf_usart_mode_t;

typedef enum vsf_usart_irq_mask_t {
    // usart fifo interrupt
    VSF_USART_IRQ_MASK_RX               = (0x1ul << 0),
    VSF_USART_IRQ_MASK_TX               = (0x1ul << 1),
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (0x1ul << 2),

    // usart request interrupt
    VSF_USART_IRQ_MASK_RX_CPL           = (0x1ul << 3),
    VSF_USART_IRQ_MASK_TX_CPL           = (0x1ul << 4),

    // usart error interrupt
    VSF_USART_IRQ_MASK_FRAME_ERR        = (0x1ul << 5),
    VSF_USART_IRQ_MASK_PARITY_ERR       = (0x1ul << 6),
    VSF_USART_IRQ_MASK_BREAK_ERR        = (0x1ul << 7),
    VSF_USART_IRQ_MASK_OVERFLOW_ERR     = (0x1ul << 8),

    // more vendor specified irq_masks can be added here
} vsf_usart_irq_mask_t;

/*\note It's not obligated to inherit from vsf_peripheral_status_t.
 *      If not, there MUST be a is_busy bit in vsf_usart_status_t.
 */

typedef struct vsf_usart_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
    };
} vsf_usart_status_t;
// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

// IPCore
/*\note Extern APIs for ip core diriver.
 *      There is no requirement about how APIs of IPCore drivers should be implemented.
 *      Just consider the simplicity for actual peripherial drivers.
 */
// IPCore end

#ifdef __cplusplus
}
#endif

// IPCore
#undef __VSF_HAL_${USART_IP}_USART_CLASS_IMPLEMENT
#undef __VSF_HAL_${USART_IP}_USART_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_USART
#endif      // __HAL_DRIVER_${SERIES/USART_IP}_USART_H__
/* EOF */
