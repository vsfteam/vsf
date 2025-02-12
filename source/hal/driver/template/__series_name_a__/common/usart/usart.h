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

// HW/IPCore
/**
 * \note When vsf_peripheral_status_t is inherited, vsf_template_hal_driver.h needs to be included
 */
#include "hal/driver/common/template/vsf_template_hal_driver.h"
// HW/IPCore end

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

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
/*\note hw USART driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_MODE for vsf_usart_mode_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS for vsf_usart_status_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_usart_irq_mask_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL for vsf_usart_ctrl_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_CFG for vsf_usart_cfg_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_usart_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED
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

/*\note You can add more member in vsf_${usart_ip}_usart_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${usart_ip}_iwc_reg_t  *reg;
        vsf_usart_isr_t             isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
#if VSF_USART_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_usart_mode_t {
    VSF_USART_NO_PARITY                 = (0x0ul << 0),
    VSF_USART_EVEN_PARITY               = (0x1ul << 0),
    VSF_USART_ODD_PARITY                = (0x2ul << 0),
    VSF_USART_FORCE_0_PARITY            = (0x3ul << 0),
    VSF_USART_FORCE_1_PARITY            = (0x4ul << 0),

    VSF_USART_1_STOPBIT                 = (0x0ul << 3),
    VSF_USART_1_5_STOPBIT               = (0x1ul << 3),
    VSF_USART_0_5_STOPBIT               = (0x2ul << 3),
    VSF_USART_2_STOPBIT                 = (0x3ul << 3),

    VSF_USART_5_BIT_LENGTH              = (0x0ul << 5),
    VSF_USART_6_BIT_LENGTH              = (0x1ul << 5),
    VSF_USART_7_BIT_LENGTH              = (0x2ul << 5),
    VSF_USART_8_BIT_LENGTH              = (0x3ul << 5),
    VSF_USART_9_BIT_LENGTH              = (0x4ul << 5),
    VSF_USART_10_BIT_LENGTH             = (0x5ul << 5),

    VSF_USART_NO_HWCONTROL              = (0x0ul << 8),
    VSF_USART_RTS_HWCONTROL             = (0x1ul << 8),
    VSF_USART_CTS_HWCONTROL             = (0x2ul << 8),
    VSF_USART_RTS_CTS_HWCONTROL         = (0x3ul << 8),

    VSF_USART_TX_ENABLE                 = (0x0ul << 9),
    VSF_USART_TX_DISABLE                = (0x1ul << 9),

    VSF_USART_RX_ENABLE                 = (0x0ul << 10),
    VSF_USART_RX_DISABLE                = (0x1ul << 10),

    VSF_USART_SYNC_CLOCK_ENABLE         = (0x0ul << 11),
    VSF_USART_SYNC_CLOCK_DISABLE        = (0x1ul << 11),

    VSF_USART_HALF_DUPLEX_DISABLE       = (0x0ul << 12),
    VSF_USART_HALF_DUPLEX_ENABLE        = (0x1ul << 12),

    VSF_USART_TX_FIFO_THRESHOLD_EMPTY   = (0x0ul << 13),
    VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY
                                        = (0x1ul << 13),
    VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL
                                        = (0x2ul << 15),

    VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY
                                        = (0x0ul << 15),
    VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL
                                        = (0x1ul << 15),
    VSF_USART_RX_FIFO_THRESHOLD_FULL    = (0x2ul << 15),

    VSF_USART_SYNC_CLOCK_POLARITY_LOW   = (0x0ul << 16),
    VSF_USART_SYNC_CLOCK_POLARITY_HIGH  = (0x1ul << 16),
    VSF_USART_SYNC_CLOCK_PHASE_1_EDGE   = (0x0ul << 17),
    VSF_USART_SYNC_CLOCK_PHASE_2_EDGE   = (0x1ul << 17),

    // more vendor specified modes can be added here
} vsf_usart_mode_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_usart_irq_mask_t {
    // request_rx/request_tx complete
    VSF_USART_IRQ_MASK_TX_CPL           = (0x1ul << 0),
    VSF_USART_IRQ_MASK_RX_CPL           = (0x1ul << 1),

    // TX/RX reach fifo threshold, threshold on some devices is bound to 1
    VSF_USART_IRQ_MASK_TX               = (0x1ul << 2),
    VSF_USART_IRQ_MASK_RX               = (0x1ul << 3),
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (0x1ul << 4),

    // clear to send interrupt
    VSF_USART_IRQ_MASK_CTS              = (0x1ul << 5),

    // Error interrupt
    VSF_USART_IRQ_MASK_FRAME_ERR        = (0x1ul << 6),
    VSF_USART_IRQ_MASK_PARITY_ERR       = (0x1ul << 7),
    VSF_USART_IRQ_MASK_BREAK_ERR        = (0x1ul << 8),
    VSF_USART_IRQ_MASK_OVERFLOW_ERR     = (0x1ul << 9),

    // more vendor specified irq_masks can be added here
} vsf_usart_irq_mask_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL == ENABLED
typedef enum vsf_usart_ctrl_t {
    // usart default command
    VSF_USART_CTRL_SEND_BREAK    = (0x01ul << 0),
    VSF_USART_CTRL_SET_BREAK     = (0x01ul << 1),
    VSF_USART_CTRL_CLEAR_BREAK   = (0x01ul << 2),

    // more vendor specified commnad can be added here
} vsf_usart_ctrl_t;
#endif

/** \note It's not obligated to inherit from vsf_peripheral_status_t.
 *      If not, there MUST be a is_busy bit in vsf_usart_status_t.
 */

#if VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_usart_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
        struct {
            uint32_t is_busy         : 1;
            uint32_t is_tx_busy      : 1;
            uint32_t is_rx_busy      : 1;
            uint32_t tx_fifo_thresh  : 8;
            uint32_t rx_fifo_thresh  : 8;
        };
    };
} vsf_usart_status_t;
#endif

/** \note Redefining vsf_usart_cfg_t usually requires declaring vsf_usart_t
 *        and vsf_usart_isr_handler_t types and define vsf_usart_isr_t.
 */

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_usart_t vsf_usart_t;
typedef void vsf_usart_isr_handler_t(void *target_ptr,
                                     vsf_usart_t *usart_ptr,
                                     vsf_usart_irq_mask_t irq_mask);
typedef struct vsf_usart_isr_t {
    vsf_usart_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_usart_isr_t;
typedef struct vsf_usart_cfg_t {
    uint32_t                mode;
    uint32_t                baudrate;
    uint32_t                rx_timeout;
    vsf_usart_isr_t         isr;

    // more vendor specified cfg can be added here
} vsf_usart_cfg_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_usart_capability_t {
    vsf_usart_irq_mask_t irq_mask;

    uint32_t max_baudrate;
    uint32_t min_baudrate;

    uint8_t txfifo_depth;
    uint8_t rxfifo_depth;

    uint8_t max_data_bits;
    uint8_t min_data_bits;

    uint8_t support_rx_timeout          : 1;
    uint8_t support_send_break          : 1;
    uint8_t support_set_and_clear_break : 1;

    // more vendor specified capability can be added here
} vsf_usart_capability_t;
#endif
// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

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
#undef __VSF_HAL_${USART_IP}_USART_CLASS_IMPLEMENT
#undef __VSF_HAL_${USART_IP}_USART_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_USART
#endif      // __HAL_DRIVER_${SERIES/USART_IP}_USART_H__
/* EOF */
