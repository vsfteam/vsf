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

#ifndef __VSF_TEMPLATE_USART_H__
#define __VSF_TEMPLATE_USART_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_USART_CFG_MULTI_CLASS
#   define VSF_USART_CFG_MULTI_CLASS                ENABLED
#endif

#if defined(VSF_HW_USART_COUNT) && !defined(VSF_HW_USART_MASK)
#   define VSF_HW_USART_MASK                        VSF_HAL_COUNT_TO_MASK(VSF_HW_USART_COUNT)
#endif

#if defined(VSF_HW_USART_MASK) && !defined(VSF_HW_USART_COUNT)
#   define VSF_HW_USART_COUNT                       VSF_HAL_MASK_TO_COUNT(VSF_HW_USART_MASK)
#endif

/*!
In the application code, we can redefine this macro to call the actual API directly without
a runtime cost. For example:
#define VSF_USART_CFG_MULTI_CLASS vsf_hw
vsf_usart_init(&vsf_hw_usart0, &cfg);
 It will be expanded at compile time to vsf_hw_usart_init(&vsf_hw_usart0, &cfg).

Dependency: VSF_USART_CFG_FUNCTION_RENAME enable
*/
#ifndef VSF_USART_CFG_PREFIX
#   if VSF_USART_CFG_MULTI_CLASS == ENABLED
#       define VSF_USART_CFG_PREFIX                 vsf
#   elif defined(VSF_HW_USART_COUNT) && (VSF_HW_USART_COUNT != 0)
#       define VSF_USART_CFG_PREFIX                 vsf_hw
#   else
#       define VSF_USART_CFG_PREFIX                 vsf
#   endif
#endif

//! If VSF_USART_CFG_FUNCTION_RENAME is enabled, the actual API is called
//! based on the value of VSF_USART_CFG_PREFIX
#ifndef VSF_USART_CFG_FUNCTION_RENAME
#   define VSF_USART_CFG_FUNCTION_RENAME            ENABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_USART_CFG_REIMPLEMENT_TYPE_MODE to redefine VSF_USART_mode_t as needed.
#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE      DISABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK to redefine vsf_usart_irq_mask_t as needed.
#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK  DISABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS to redefine vsf_usart_status_t as needed.
#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS    DISABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_USART_CFG_REIMPLEMENT_TYPE_CMD to redefine vsf_usart_cmd_t as needed.
#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_CMD
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_CMD       DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_MODE_TO_DATA_BITS
#   define VSF_USART_CFG_REIMPLEMENT_MODE_TO_DATA_BITS      \
                                                    DISABLED
#endif

#ifndef VSF_USART_CFG_INHERT_HAL_CAPABILITY
#   define VSF_USART_CFG_INHERT_HAL_CAPABILITY      ENABLED
#endif

#ifndef VSF_USART_CFG_REQUEST_API
#   define VSF_USART_CFG_REQUEST_API                ENABLED
#endif

#ifndef VSF_USART_CFG_FIFO_API
#   define VSF_USART_CFG_FIFO_API                   ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_USART_BASE_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              usart, init,                  VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   usart, fini,                  VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               usart, enable,                VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               usart, disable,               VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_usart_capability_t, usart, capability,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   usart, irq_enable,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   usart, irq_disable,           VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_usart_status_t,     usart, status,                VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)

#if VSF_USART_CFG_FIFO_API == ENABLED
#   define __VSF_USART_FIFO_APIS(__prefix_name) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast16_t,      usart, rxfifo_get_data_count, VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast16_t,      usart, rxfifo_read,           VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast16_t count) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast16_t,      usart, txfifo_get_free_count, VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast16_t,      usart, txfifo_write,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast16_t count)
#else
#   define __VSF_USART_FIFO_APIS(__prefix_name)
#endif

#if VSF_USART_CFG_REQUEST_API == ENABLED
#   define __VSF_USART_REQUEST_APIS(__prefix_name) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, request_rx,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast32_t count) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, request_tx,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast32_t count) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, cancel_rx,             VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, cancel_tx,             VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, int_fast32_t,       usart, get_rx_count,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, int_fast32_t,       usart, get_tx_count,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)
#else
#   define __VSF_USART_REQUEST_APIS(__prefix_name)
#endif

#define __VSF_USART_REQUEST_EXTRA_APIS(__prefix_name)                           \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, cmd,                   VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_cmd_t cmd, void* param)

#define VSF_USART_INLINE_APIS(__prefix_name)                                    \
    static inline vsf_err_t VSF_MCONNECT(__prefix_name, _usart_send_break)      \
        (VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)                      \
    {                                                                           \
        return VSF_MCONNECT(__prefix_name, _usart_cmd)(usart_ptr,               \
            VSF_USART_CMD_SEND_BREAK, NULL);                                    \
    }                                                                           \
    static inline vsf_err_t VSF_MCONNECT(__prefix_name, _usart_set_break)       \
        (VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)                      \
    {                                                                           \
        vsf_usart_capability_t cap =                                            \
            VSF_MCONNECT(__prefix_name, _usart_capability)(usart_ptr);          \
        VSF_HAL_ASSERT(cap.support_set_and_clear_break);                        \
        return VSF_MCONNECT(__prefix_name, _usart_cmd)(usart_ptr,               \
            VSF_USART_CMD_SET_BREAK, NULL);                                     \
    }                                                                           \
    static inline vsf_err_t VSF_MCONNECT(__prefix_name, _usart_clear_break)     \
        (VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)                      \
    {                                                                           \
        vsf_usart_capability_t cap =                                            \
            VSF_MCONNECT(__prefix_name, _usart_capability)(usart_ptr);          \
        VSF_HAL_ASSERT(cap.support_set_and_clear_break);                        \
        return VSF_MCONNECT(__prefix_name, _usart_cmd)(usart_ptr,               \
            VSF_USART_CMD_CLEAR_BREAK, NULL);                                   \
    }

#define VSF_USART_APIS(__prefix_name)                                           \
    __VSF_USART_BASE_APIS(__prefix_name)                                        \
    __VSF_USART_FIFO_APIS(__prefix_name)                                        \
    __VSF_USART_REQUEST_APIS(__prefix_name)                                     \
    __VSF_USART_REQUEST_EXTRA_APIS(__prefix_name)

/*============================ TYPES =========================================*/

#if VSF_USART_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF USART modes that can be reimplemented in specific hal drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF USART 模式，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, the following modes must be kept，
 * If the hardware supports more modes, e.g. more parity modes, more databits, more stopbits,
 * more FIFO threshold size, we can implement it in the hardware driver:
 * \~chinese
 * 即使硬件不支持这些功能，但是以下的模式必须保留，如果硬件支持更多模式，例如更多的奇偶校验模式、
 * 数据位大小、停止位大小、FIFO 门限值，我们可以在硬件驱动里实现它：
 *
 * - VSF_USART_NO_PARITY
 * - VSF_USART_EVEN_PARITY
 * - VSF_USART_ODD_PARITY
 * - VSF_USART_FORCE_0_PARITY
 * - VSF_USART_FORCE_1_PARITY
 * - VSF_USART_1_STOPBIT
 * - VSF_USART_1_5_STOPBIT
 * - VSF_USART_0_5_STOPBIT
 * - VSF_USART_2_STOPBIT
 * - VSF_USART_5_BIT_LENGTH
 * - VSF_USART_6_BIT_LENGTH
 * - VSF_USART_7_BIT_LENGTH
 * - VSF_USART_8_BIT_LENGTH
 * - VSF_USART_9_BIT_LENGTH
 * - VSF_USART_10_BIT_LENGTH
 * - VSF_USART_NO_HWCONTROL
 * - VSF_USART_RTS_HWCONTROL
 * - VSF_USART_CTS_HWCONTROL
 * - VSF_USART_RTS_CTS_HWCONTROL
 * - VSF_USART_TX_ENABLE
 * - VSF_USART_TX_DISABLE
 * - VSF_USART_RX_ENABLE
 * - VSF_USART_RX_DISABLE
 * - VSF_USART_HALF_DUPLEX_DISABLE
 * - VSF_USART_HALF_DUPLEX_ENABLE
 * - VSF_USART_TX_FIFO_THRESH_ONE
 * - VSF_USART_TX_FIFO_THRESH_HALF_FULL
 * - VSF_USART_TX_FIFO_THRESH_FULL
 * - VSF_USART_RX_FIFO_THRESH_ONE
 * - VSF_USART_RX_FIFO_THRESH_HALF_FULL
 * - VSF_USART_RX_FIFO_THRESH_FULL
 *
 * \~english
 * If more new modes are added to the driver, then the corresponding MASK macros need to
 * be defined to include the values of the new modes. For example, Adding the new TX FIFO
 * Threshold option requires that the macro VSF_USART_TX_FIFO_THRESH_MASK be defined.
 * Adding the new parity check option requires the macro VSF_USART_PARITY_MASK be defined.
 * Adding the new stop bit option requires the macro of VSF_USART_BIT_LENGTH_MASK be defined.
 * \~chinese
 * 驱动里如果添加更多的新模式的时候，那也需要定义对应的 MASK 宏 来包含新的模式的值。例如添加了新的
 * TX FIFO 门限值选项，就需要定义宏 VSF_USART_TX_FIFO_THRESH_MASK; 添加了新的奇偶检验选项，就需要
 * 定义宏 VSF_USART_PARITY_MASK; 添加了新的停止位选项，就需要定义宏 VSF_USART_BIT_LENGTH_MASK。
 *
 * \~english
 *  Optional features require one or more enumeration options and a macro with the same
 *  name to determine if they are supported at runtime. If the feature supports more than
 *  one option, it is recommended to provide the corresponding MASK option, so that the
 *  user can switch to different modes at runtime.
 * \~chinese
 * 可选特性需要提供一个或者多个枚举选项，还需要提供同名的宏，方便用户在运行时判断是否支持。
 * 如果它特性支持多个选项，建议提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 *
 */
typedef enum vsf_usart_mode_t {
    //! USART Parity
    VSF_USART_NO_PARITY                 = (0x0ul << 0),
    VSF_USART_EVEN_PARITY               = (0x1ul << 0),
    VSF_USART_ODD_PARITY                = (0x2ul << 0),
    VSF_USART_FORCE_0_PARITY            = (0x3ul << 0),
    VSF_USART_FORCE_1_PARITY            = (0x4ul << 0),

    //! USART Stopbit
    VSF_USART_1_STOPBIT                 = (0x0ul << 3),     //!< stopbit: 1   bit
    VSF_USART_1_5_STOPBIT               = (0x1ul << 3),     //!< stopbit: 1.5 bit
    VSF_USART_0_5_STOPBIT               = (0x2ul << 3),     //!< stopbit: 0.5 bit
    VSF_USART_2_STOPBIT                 = (0x3ul << 3),     //!< stopbit: 2   bit

    //! USART Databit Lenght
    VSF_USART_5_BIT_LENGTH              = (0x0ul << 5),     //!< data bits : 5,
    VSF_USART_6_BIT_LENGTH              = (0x1ul << 5),     //!< data bits : 6,
    VSF_USART_7_BIT_LENGTH              = (0x2ul << 5),     //!< data bits : 7,
    VSF_USART_8_BIT_LENGTH              = (0x3ul << 5),     //!< data bits : 8,
    VSF_USART_9_BIT_LENGTH              = (0x4ul << 5),     //!< data bits : 9,
    VSF_USART_10_BIT_LENGTH             = (0x5ul << 5),     //!< data bits : 10,

    //! USART Hardware Control
    VSF_USART_NO_HWCONTROL              = (0x0ul << 8),
    VSF_USART_RTS_HWCONTROL             = (0x1ul << 8),
    VSF_USART_CTS_HWCONTROL             = (0x2ul << 8),
    VSF_USART_RTS_CTS_HWCONTROL         = (0x3ul << 8),

    VSF_USART_TX_ENABLE                 = (0x0ul << 9),
    VSF_USART_TX_DISABLE                = (0x1ul << 9),

    VSF_USART_RX_ENABLE                 = (0x0ul << 10),
    VSF_USART_RX_DISABLE                = (0x1ul << 10),

    VSF_USART_HALF_DUPLEX_DISABLE       = (0x0ul << 10),
    VSF_USART_HALF_DUPLEX_ENABLE        = (0x1ul << 10),

    //! As generic options, only three modes are defined here.
    //! More options for thresholds can be defined in the specific driver.

    VSF_USART_TX_FIFO_THRESH_ONE        = (0x0ul << 12),    //!< one data for txfifo
    VSF_USART_TX_FIFO_THRESH_HALF_FULL  = (0x1ul << 12),    //!< Half of the threshold for txfifo
    VSF_USART_TX_FIFO_THRESH_FULL       = (0x2ul << 12),    //!< Full of the threshold for txfifo

    VSF_USART_RX_FIFO_THRESH_ONE        = (0x0ul << 14),    //!< one data for txfifo
    VSF_USART_RX_FIFO_THRESH_HALF_FULL  = (0x1ul << 14),    //!< Half of the threshold for txfifo
    VSF_USART_RX_FIFO_THRESH_FULL       = (0x2ul << 14),    //!< Full of the threshold for txfifo
} vsf_usart_mode_t;
#endif

enum {
#ifndef VSF_USART_PARITY_MASK
    VSF_USART_PARITY_MASK           = VSF_USART_NO_PARITY |
                                      VSF_USART_EVEN_PARITY |
                                      VSF_USART_ODD_PARITY |
                                      VSF_USART_FORCE_0_PARITY |
                                      VSF_USART_FORCE_1_PARITY,
#endif

#ifndef VSF_USART_STOPBIT_MASK
    VSF_USART_STOPBIT_MASK          = VSF_USART_1_STOPBIT |
                                      VSF_USART_1_5_STOPBIT |
                                      VSF_USART_0_5_STOPBIT |
                                      VSF_USART_2_STOPBIT,
#endif

#ifndef VSF_USART_BIT_LENGTH_MASK
    VSF_USART_BIT_LENGTH_MASK       = VSF_USART_5_BIT_LENGTH |
                                      VSF_USART_6_BIT_LENGTH |
                                      VSF_USART_7_BIT_LENGTH |
                                      VSF_USART_8_BIT_LENGTH |
                                      VSF_USART_9_BIT_LENGTH |
                                      VSF_USART_10_BIT_LENGTH,
#endif

#ifndef VSF_USART_TX_FIFO_THRESH_MASK
    VSF_USART_TX_FIFO_THRESH_MASK   = VSF_USART_TX_FIFO_THRESH_ONE |
                                      VSF_USART_TX_FIFO_THRESH_HALF_FULL |
                                      VSF_USART_TX_FIFO_THRESH_FULL,
#endif

#ifndef VSF_USART_RX_FIFO_THRESH_MASK
    VSF_USART_RX_FIFO_THRESH_MASK   = VSF_USART_RX_FIFO_THRESH_ONE |
                                      VSF_USART_RX_FIFO_THRESH_HALF_FULL |
                                      VSF_USART_RX_FIFO_THRESH_FULL,
#endif

#ifndef VSF_USART_HWCONTROL_MASK
    VSF_USART_HWCONTROL_MASK        = VSF_USART_NO_HWCONTROL |
                                      VSF_USART_RTS_HWCONTROL |
                                      VSF_USART_CTS_HWCONTROL |
                                      VSF_USART_RTS_CTS_HWCONTROL,
#endif

    VSF_USART_TX_MASK               = VSF_USART_TX_ENABLE |
                                      VSF_USART_TX_DISABLE,

    VSF_USART_RX_MASK               = VSF_USART_RX_ENABLE |
                                      VSF_USART_RX_DISABLE,

    VSF_USART_HALF_DUPLEX_MASK      = VSF_USART_HALF_DUPLEX_DISABLE |
                                      VSF_USART_HALF_DUPLEX_ENABLE,

    VSF_USART_MODE_ALL_BITS_MASK    = VSF_USART_PARITY_MASK |
                                      VSF_USART_STOPBIT_MASK |
                                      VSF_USART_BIT_LENGTH_MASK |
                                      VSF_USART_HWCONTROL_MASK |
                                      VSF_USART_TX_FIFO_THRESH_MASK |
                                      VSF_USART_RX_FIFO_THRESH_MASK |
                                      VSF_USART_TX_MASK |
                                      VSF_USART_RX_MASK |
                                      VSF_USART_HALF_DUPLEX_MASK,
};

#if VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/**
 * \~english
 * @brief Predefined VSF USART interrupt that can be reimplemented in specific hal drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF USART 中断，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these interrupt must be kept.
 * \~chinese
 * 即使硬件不支持这些中断，但是这些中断是必须实现的：
 *
 * - VSF_USART_IRQ_MASK_TX_CPL
 * - VSF_USART_IRQ_MASK_RX_CPL
 * - VSF_USART_IRQ_MASK_TX
 * - VSF_USART_IRQ_MASK_RX
 * - VSF_USART_IRQ_MASK_RX_TIMEOUT
 * - VSF_USART_IRQ_MASK_FRAME_ERR
 * - VSF_USART_IRQ_MASK_PARITY_ERR
 * - VSF_USART_IRQ_MASK_BREAK_ERR
 * - VSF_USART_IRQ_MASK_OVERFLOW_ERR
 */
typedef enum vsf_usart_irq_mask_t {
    // request_rx/request_tx complete
    VSF_USART_IRQ_MASK_TX_CPL           = (0x1ul << 23),
    VSF_USART_IRQ_MASK_RX_CPL           = (0x1ul << 24),

    // TX/RX reach fifo threshold, threshold on some devices is bound to 1
    VSF_USART_IRQ_MASK_TX               = (0x1ul << 25),
    VSF_USART_IRQ_MASK_RX               = (0x1ul << 26),
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (0x1ul << 27),

    // Error interrupt
    VSF_USART_IRQ_MASK_FRAME_ERR        = (0x1ul << 28),
    VSF_USART_IRQ_MASK_PARITY_ERR       = (0x1ul << 29),
    VSF_USART_IRQ_MASK_BREAK_ERR        = (0x1ul << 30),
    VSF_USART_IRQ_MASK_OVERFLOW_ERR     = (0x1ul << 31),
} vsf_usart_irq_mask_t;
#endif

enum {
    VSF_USART_IRQ_MASK_RX_IDLE          = VSF_USART_IRQ_MASK_RX_TIMEOUT,

#ifndef VSF_USART_IRQ_MASK_ERR
    VSF_USART_IRQ_MASK_ERR              = VSF_USART_IRQ_MASK_FRAME_ERR |
                                          VSF_USART_IRQ_MASK_PARITY_ERR |
                                          VSF_USART_IRQ_MASK_BREAK_ERR |
                                          VSF_USART_IRQ_MASK_OVERFLOW_ERR,
#endif

#ifndef VSF_USART_IRQ_ALL_BITS_MASK
    VSF_USART_IRQ_ALL_BITS_MASK         = VSF_USART_IRQ_MASK_TX |
                                          VSF_USART_IRQ_MASK_RX |
                                          VSF_USART_IRQ_MASK_RX_TIMEOUT |
                                          VSF_USART_IRQ_MASK_TX_CPL |
                                          VSF_USART_IRQ_MASK_RX_CPL |
                                          VSF_USART_IRQ_MASK_ERR,
#endif
};

typedef struct vsf_usart_t vsf_usart_t;

typedef void vsf_usart_isr_handler_t(void *target_ptr,
                                     vsf_usart_t *usart_ptr,
                                     vsf_usart_irq_mask_t irq_mask);

typedef struct vsf_usart_isr_t {
    vsf_usart_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t          prio;
} vsf_usart_isr_t;

typedef struct vsf_usart_cfg_t {
    uint32_t mode;
    uint32_t baudrate;
    uint32_t rx_timeout;
    vsf_usart_isr_t isr;
} vsf_usart_cfg_t;

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CMD == DISABLED
/**
 * \~english
 * @brief Predefined VSF USART command that can be reimplemented in specific hal drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF USART 命令，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these commands must be kept.
 * If the hardware supports more commands, but the following commands must be kept:
 * \~chinese
 * 即使硬件不支持这些功能，但是这些命令是必须保留：
 *
 * - VSF_USART_CMD_SEND_BREAK
 * - VSF_USART_CMD_SET_BREAK
 * - VSF_USART_CMD_CLEAR_BREAK
 */
typedef enum vsf_usart_cmd_t {
    //! usart send break
    //! After a number of bits, the hardware will automatically clear the break condition
    //! automatically.
    VSF_USART_CMD_SEND_BREAK    = (0x01ul << 0),

    //! usart set break condition
    VSF_USART_CMD_SET_BREAK     = (0x01ul << 1),
    //! usart clean break condition
    VSF_USART_CMD_CLEAR_BREAK   = (0x01ul << 2),
} vsf_usart_cmd_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_usart_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy         : 1;
            uint32_t is_tx_busy      : 1;
            uint32_t is_rx_busy      : 1;
            // tx fifo threshold in bytes
            uint32_t tx_fifo_thresh  : 8;
            // rx fifo threshold in bytes
            uint32_t rx_fifo_thresh  : 8;
        };
    };
} vsf_usart_status_t;
#endif

typedef struct vsf_usart_capability_t {
#if VSF_USART_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_usart_irq_mask_t irq_mask;

    // baudrate range
    uint32_t max_baudrate;
    uint32_t min_baudrate;

    // tx fifo depth, in data count (frame)
    uint8_t txfifo_depth;
    // rx fifo depth, in data count (frame)
    uint8_t rxfifo_depth;

    uint8_t max_data_bits;
    uint8_t min_data_bits;

    uint8_t support_rx_timeout          : 1;
    uint8_t support_send_break          : 1;
    uint8_t support_set_and_clear_break : 1;
} vsf_usart_capability_t;

typedef struct vsf_usart_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_USART_APIS(vsf)
} vsf_usart_op_t;

#if VSF_USART_CFG_MULTI_CLASS == ENABLED
struct vsf_usart_t  {
    const vsf_usart_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a usart instance.
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_usart_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if usart was initialized, or a negative error code

 @note It is not necessary to call vsf_usart_fini() to deinitialization.
       vsf_usart_init() should be called before any other usart API except vsf_usart_capability().

 \~chinese
 @brief 初始化一个 usart 实例
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @param[in] cfg_ptr: 结构体 vsf_usart_cfg_t 的指针，参考 @ref vsf_usart_cfg_t
 @return vsf_err_t: 如果 usart 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_usart_fini() 反初始化。
       vsf_usart_init() 应该在除 vsf_usart_capability() 之外的其他 usart API 之前调用。
 */
extern vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a usart instance.
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return none

 \~chinese
 @brief 终止一个 usart 实例
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return 无。
 */
extern void vsf_usart_fini(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief enable interrupt masks of usart instance.
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return none.

 \~chinese
 @brief 使能 usart 实例的中断
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return 无。
 */
extern fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief disable interrupt masks of usart instance.
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return none.

 \~chinese
 @brief 禁能 usart 实例的中断
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return 无。
 */
extern fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief enable interrupt masks of usart instance.
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @param[in] irq_mask: one or more value of enum @ref vsf_usart_irq_mask_t
 @return none.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 使能 usart 实例的中断
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_usart_irq_mask_t 的值的按位或，@ref vsf_usart_irq_mask_t
 @return 无。
 @note 在中断使能之前，应该清除所有悬挂的中断。
 */
extern void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of usart instance.
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @param[in] irq_mask: one or more value of enum vsf_usart_irq_mask_t, @ref vsf_usart_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 usart 实例的中断
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_usart_irq_mask_t 的值的按位或，@ref vsf_usart_irq_mask_t
 @return 无。
 */
extern void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask);

/**
 \~english
 @brief get the status of usart instance.
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return vsf_usart_status_t: return all status of current usart

 \~chinese
 @brief 获取 usart 实例的状态
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return vsf_usart_status_t: 返回当前 usart 的所有状态
 */
extern vsf_usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief get the capability of usart instance.
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return vsf_usart_capability_t: return all capability of current usart @ref vsf_usart_capability_t

 \~chinese
 @brief 获取 usart 实例的能力
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return vsf_usart_capability_t: 返回当前 usart 的所有能力 @ref vsf_usart_capability_t
 */
extern vsf_usart_capability_t vsf_usart_capability(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief get the number of data currently received in the usart receive fifo
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return uint_fast16_t: return the number of data currently in the usart receive queue.

 \~chinese
 @brief 获取 usart 接收队列里当前收到的数据的个数
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return uint_fast16_t: 返回当前 usart 接收队列的数据的个数
 */
extern uint_fast16_t vsf_usart_rxfifo_get_data_count(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief try to read the maximum length of data from the usart receive fifo
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @param[in] buffer_ptr: data buffer
 @param[in] count: maximum number of reads
 @return uint_fast16_t: return the actual number of reads from the current usart receive fifo

 \~chinese
 @brief 尝试从 usart 接收队列里读取指定最大长度的数据
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @param[in] buffer_ptr: 数据缓冲区
 @param[in] count: 最大读取数量
 @return uint_fast16_t: 返回当前 usart 接收队列的实际读到的数量
 */
extern uint_fast16_t vsf_usart_rxfifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count);

/**
 \~english
 @brief try to get the maximum number of data that can be sent from the usart send fifo
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return uint_fast16_t: return the maximum number of data that can be sent from the current usart send fifo
 @note Some usart has no fifo, or has fifo but can't get the number of fifo, can return 1~2<br>
        1. The data register is empty, but the shift register is not empty<br>
        2. Both data register and shift register are empty

 \~chinese
 @brief 获取 usart 发送队列的最大可发送数据的个数
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return uint_fast16_t: 返回当前 usart 发送队列的最大可发送数据的个数
 @note 部分usart没有fifo，或者有fifo但是无法获取fifo的数量，可以返回1~2<br>
        1. 数据寄存器为空，但是移位寄存器不为空<br>
        2. 数据寄存器和移位寄存器都为空
 */
extern uint_fast16_t vsf_usart_txfifo_get_free_count(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief try to write the maximum length of data from the usart send fifo
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @param[in] buffer_ptr: data buffer
 @param[in] count: maximum number of writes
 @return uint_fast16_t: return the actual number of writes to the current usart send queue

 \~chinese
 @brief 尝试从 usart 发送队列里写入指定最大长度的数据
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @param[in] buffer_ptr: 数据缓冲区
 @param[in] count: 最大读取数量
 @return uint_fast16_t: 返回当前 usart 接收队列的实际读到的数量
 */
extern uint_fast16_t vsf_usart_txfifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count);

/**
 \~english
 @brief usart request to receive data of specified length
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @param[in] buffer_ptr: data buffer
 @param[in] count: number of requested data
 @return vsf_err_t: VSF_ERR_NONE if the usart request was successfully, or a negative error code

 \~chinese
 @brief usart 请求接收指定长度的数据
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @param[in] buffer_ptr: 数据缓冲区
 @param[in] count: 请求数据的数量
 @return vsf_err_t: 如果 usart 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);

/**
 \~english
 @brief usart request to send data of specified length
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @param[in] buffer_ptr: data buffer
 @param[in] count: number of requested data
 @return vsf_err_t: VSF_ERR_NONE if the usart request was successfully, or a negative error code
 @note: The request interface is usually implemented via DMA.

 \~chinese
 @brief usart 请求发送指定长度的数据
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @param[in] buffer_ptr: 数据缓冲区
 @param[in] count: 请求数据的数量
 @return vsf_err_t: 如果 usart 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 @note: 请求接口通常是通过 DMA 来实现。
 */
extern vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);

/**
 \~english
 @brief cancel current current receive request
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return vsf_err_t: VSF_ERR_NONE if the usart request was successfully, or a negative error code
 @note: The request interface is usually implemented via DMA.

 \~chinese
 @brief 取消当前接收请求
 @return vsf_err_t: 如果 usart 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief cancel current send request
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return vsf_err_t: VSF_ERR_NONE if the usart request was successfully, or a negative error code

 \~chinese
 @brief 取消当前发送请求
 @return vsf_err_t: 如果 usart 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief get the number of current receive requests that have been received
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return int_fast32_t: number already received
 @note: only valid until the current receive request is completed.

 \~chinese
 @brief 获取接收请求已经接收到的数量
 @return int_fast32_t: 已经接收到的数量
 @note: 只在当前接收请求没完成之前是有效的。
 */
extern int_fast32_t vsf_usart_get_rx_count(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief get the number of current send requests that have been sended
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return int_fast32_t: number already send
 @note: only valid until the current send request is completed.

 \~chinese
 @brief 获取发送请求已经发送到的数量
 @return int_fast32_t: 已经发送到的数量
 @note: 只在当前发送请求没完成之前是有效的。
 */
extern int_fast32_t vsf_usart_get_tx_count(vsf_usart_t *usart_ptr);

/**
 \~english
 @brief Calls the specified usart command
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @param[in] cmd: usart command @ref vsf_usart_cmd_t.
 @param[in] param: the parameter of the command, its use is determined by the command
 @return vsf_err_t: returns the result of the usart command when it is invoked,
        success returns VSF_ERR_NONE

 \~chinese
 @brief 获取 usart 实例的状态
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @param[in] duration: usart 传输一位的时间的倍数，如果 duration 等于0，表示使用硬件默认的时间
 @return vsf_err_t: 返回当调用 uart 命令的结果，成功返回 VSF_ERR_NONE
 */
extern vsf_err_t vsf_usart_cmd(vsf_usart_t *usart_ptr, vsf_usart_cmd_t cmd, void * param);

/*============================ INLINE FUNCTIONS ==============================*/

#if VSF_USART_CFG_REIMPLEMENT_MODE_TO_DATA_BITS == DISABLED
static inline uint8_t vsf_usart_mode_to_data_bits(vsf_usart_mode_t mode)
{
    switch (mode & VSF_USART_BIT_LENGTH_MASK) {
    case VSF_USART_5_BIT_LENGTH:
        return 5;
    case VSF_USART_6_BIT_LENGTH:
        return 6;
    case VSF_USART_7_BIT_LENGTH:
        return 7;
    case VSF_USART_8_BIT_LENGTH:
        return 8;
    case VSF_USART_9_BIT_LENGTH:
        return 9;
    case VSF_USART_10_BIT_LENGTH:
        return 10;
    default:
        return 0;
    }
}
#endif

/**
 \~english
 @brief The usart instance sends a break and the hardware automatically
        clears the break after a number of data bits of time
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return vsf_err_t: returns the result of the usart send break, success returns VSF_ERR_NONE

 \~chinese
 @brief usart 实例发送 break，硬件在若干个数据位的时间后会自动清除 break
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return vsf_err_t: 返回当前 usart 发送 break 的结果，成功返回 VSF_ERR_NONE
 */
static inline vsf_err_t vsf_usart_send_break(vsf_usart_t *usart_ptr)
{
    vsf_usart_capability_t cap = vsf_usart_capability(usart_ptr);
    VSF_HAL_ASSERT(cap.support_send_break);

    return vsf_usart_cmd(usart_ptr, VSF_USART_CMD_SEND_BREAK, NULL);
}

/**
 \~english
 @brief The usart instance sets break, hardware does not clear break automatically
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return vsf_err_t: returns the result of the usart set break, success returns VSF_ERR_NONE

 \~chinese
 @brief usart 实例设置 break，硬件不会自动清除 break
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return vsf_err_t: 返回当前 usart 发送 break 的结果，成功返回 VSF_ERR_NONE
 */
static inline vsf_err_t vsf_usart_set_break(vsf_usart_t *usart_ptr)
{
    vsf_usart_capability_t cap = vsf_usart_capability(usart_ptr);
    VSF_HAL_ASSERT(cap.support_set_and_clear_break);

    return vsf_usart_cmd(usart_ptr, VSF_USART_CMD_SET_BREAK, NULL);
}

/**
 \~english
 @brief The usart instance clear break
 @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 @return vsf_err_t: returns the result of the usart clean break, success returns VSF_ERR_NONE

 \~chinese
 @brief usart 实例清除 break
 @param[in] usart_ptr: 结构体 vsf_usart_t 的指针，参考 @ref vsf_usart_t
 @return vsf_err_t: 返回当前 usart 发送 break 的结果，成功返回 VSF_ERR_NONE
 */
static inline vsf_err_t vsf_usart_clear_break(vsf_usart_t *usart_ptr)
{
    vsf_usart_capability_t cap = vsf_usart_capability(usart_ptr);
    VSF_HAL_ASSERT(cap.support_set_and_clear_break);

    return vsf_usart_cmd(usart_ptr, VSF_USART_CMD_CLEAR_BREAK, NULL);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_USART_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_usart_t                                 VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_t)
#   define vsf_usart_init(__USART, ...)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_init)                  ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_fini(__USART)                       VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_fini)                  ((__vsf_usart_t *)__USART)
#   define vsf_usart_capability(__USART)                 VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_capability)            ((__vsf_usart_t *)__USART)
#   define vsf_usart_enable(__USART)                     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_enable)                ((__vsf_usart_t *)__USART)
#   define vsf_usart_disable(__USART)                    VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_disable)               ((__vsf_usart_t *)__USART)
#   define vsf_usart_irq_enable(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_irq_enable)            ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_irq_disable(__USART, ...)           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_irq_disable)           ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_status(__USART)                     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_status)                ((__vsf_usart_t *)__USART)
#   define vsf_usart_rxfifo_get_data_count(__USART, ...) VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_rxfifo_get_data_count) ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_rxfifo_read(__USART, ...)           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_rxfifo_read)           ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_txfifo_get_free_count(__USART, ...) VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_txfifo_get_free_count) ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_txfifo_write(__USART, ...)          VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_txfifo_write)          ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_request_rx(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_rx)            ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_request_tx(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_tx)            ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_cancel_rx(__USART)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_rx)             ((__vsf_usart_t *)__USART)
#   define vsf_usart_cancel_tx(__USART)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_tx)             ((__vsf_usart_t *)__USART)
#   define vsf_usart_get_rx_count(__USART)               VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_rx_count)          ((__vsf_usart_t *)__USART)
#   define vsf_usart_get_tx_count(__USART)               VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_tx_count)          ((__vsf_usart_t *)__USART)
#   define vsf_usart_send_break(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_send_break)            ((__vsf_usart_t *)__USART)
#   define vsf_usart_cmd(__USART, ...)                   VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cmd)                   ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#endif

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#endif
