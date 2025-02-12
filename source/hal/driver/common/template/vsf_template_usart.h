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

/**
 * \~english
 * @brief Enable multi-class support by default for maximum availability.
 * \~chinese
 * @brief 默认启用多类支持以获得最大可用性。
 */
#ifndef VSF_USART_CFG_MULTI_CLASS
#   define VSF_USART_CFG_MULTI_CLASS                      ENABLED
#endif

/**
 * \~english
 * @brief Define USART hardware mask if count is defined.
 * If VSF_HW_USART_COUNT is defined, this macro will generate the corresponding mask.
 * \~chinese
 * @brief 如果定义了 USART 硬件数量，则定义对应的掩码。
 * 如果定义了 VSF_HW_USART_COUNT，此宏将生成对应的掩码。
 */
#if defined(VSF_HW_USART_COUNT) && !defined(VSF_HW_USART_MASK)
#   define VSF_HW_USART_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_USART_COUNT)
#endif

/**
 * \~english
 * @brief Define USART hardware count if mask is defined.
 * If VSF_HW_USART_MASK is defined, this macro will calculate the corresponding count.
 * \~chinese
 * @brief 如果定义了 USART 硬件掩码，则定义对应的数量。
 * 如果定义了 VSF_HW_USART_MASK，此宏将计算对应的数量。
 */
#if defined(VSF_HW_USART_MASK) && !defined(VSF_HW_USART_COUNT)
#   define VSF_HW_USART_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_USART_MASK)
#endif

/**
 * \~english
 * @brief Macro VSF_USART_CFG_PREFIX can be redefined in application code to specify
 * a prefix for directly calling a specific driver. This allows applications to
 * access different USART implementations through unique function names.
 * For example:
 *    #define VSF_USART_CFG_PREFIX vsf_hw
 *    vsf_usart_init(&vsf_hw_usart0, &cfg);
 * It will be expanded at compile time to vsf_hw_usart_init(&vsf_hw_usart0, &cfg).
 * \~chinese
 * @brief 可以在应用代码中重新定义宏 VSF_USART_CFG_PREFIX，以指定直接调用特定驱动时使用的前缀。
 * 这使得应用程序可以通过唯一的函数名访问不同的 USART 实现。
 * 例如：
 *    #define VSF_USART_CFG_PREFIX vsf_hw
 *    vsf_usart_init(&vsf_hw_usart0, &cfg);
 * 编译器会将其展开为 vsf_hw_usart_init(&vsf_hw_usart0, &cfg)。
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

/**
 * \~english
 * @brief Disable VSF_USART_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_usart_init()) without prefix.
 * \~chinese
 * @brief 禁用 VSF_USART_CFG_FUNCTION_RENAME 以使用不带前缀的原始函数名
 * (例如 vsf_usart_init())。
 */
#ifndef VSF_USART_CFG_FUNCTION_RENAME
#   define VSF_USART_CFG_FUNCTION_RENAME            ENABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_USART_CFG_REIMPLEMENT_TYPE_MODE to redefine vsf_usart_mode_t as needed.
 * \~chinese
 * @brief 在特定的硬件驱动中，可以启用宏
 * VSF_USART_CFG_REIMPLEMENT_TYPE_MODE 来根据需要重新定义 vsf_usart_mode_t。
 */
#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE      DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK to redefine vsf_usart_irq_mask_t as needed.
 * \~chinese
 * @brief 在特定的硬件驱动中，可以启用宏
 * VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK 来根据需要重新定义 vsf_usart_irq_mask_t。
 */
#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK  DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS to redefine struct vsf_usart_status_t.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS
 * 来重新定义结构体 vsf_usart_status_t。
 */
#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS    DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL to redefine vsf_usart_ctrl_t as needed.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL
 * 来重新定义 vsf_usart_ctrl_t。
 */
#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL       DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_USART_CFG_REIMPLEMENT_MODE_TO_DATA_BITS to re-implement the function
 * vsf_usart_mode_to_data_bits().
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_USART_CFG_REIMPLEMENT_MODE_TO_DATA_BITS
 * 来重新实现函数 vsf_usart_mode_to_data_bits()。
 */
#ifndef VSF_USART_CFG_REIMPLEMENT_MODE_TO_DATA_BITS
#   define VSF_USART_CFG_REIMPLEMENT_MODE_TO_DATA_BITS      \
                                                    DISABLED
#endif

/**
 * \~english
 * @brief Redefine struct vsf_usart_cfg_t. The vsf_usart_isr_handler_t type also needs to be redefined.
 * For compatibility, members should not be deleted when struct @ref vsf_usart_cfg_t redefining.
 *
 * \~chinese
 * @brief 重新定义结构体 vsf_usart_cfg_t。vsf_usart_isr_handler_t 类型也需要重新定义。
 * 为了兼容性，在重新定义结构体 @ref vsf_usart_cfg_t 时不应删除成员。
 */
#if VSF_USART_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_USART_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english
 * @brief Redefine struct vsf_usart_capability_t.
 * For compatibility, members should not be deleted when struct @ref
 * vsf_usart_capability_t redefining.
 *
 * \~chinese
 * @brief 重新定义结构体 vsf_usart_capability_t。
 * 为了兼容性，在重新定义结构体 @ref vsf_usart_capability_t 时不应删除成员。
 */
#if VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS to redefine struct vsf_usart_status_t.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS
 * 来重新定义结构体 vsf_usart_status_t。
 */
#if VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
#    define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_USART_CFG_INHERIT_HAL_CAPABILITY to inherit capability from hal.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_USART_CFG_INHERIT_HAL_CAPABILITY
 * 来从 hal 继承能力。
 */
#ifndef VSF_USART_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_USART_CFG_INHERIT_HAL_CAPABILITY      ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief USART API template, used to generate USART type, specific prefix function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating USART functions.
 * \~chinese
 * @brief USART API 模板，用于生成 USART 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 USART 函数的前缀。
 */
#define VSF_USART_APIS(__prefix_name)                                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              usart, init,                  VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   usart, fini,                  VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               usart, enable,                VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               usart, disable,               VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_usart_capability_t, usart, capability,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   usart, irq_enable,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   usart, irq_disable,           VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_usart_status_t,     usart, status,                VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,          usart, rxfifo_get_data_count, VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,          usart, rxfifo_read,           VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,          usart, txfifo_get_free_count, VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,          usart, txfifo_write,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              usart, request_rx,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              usart, request_tx,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              usart, cancel_rx,             VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              usart, cancel_tx,             VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, int_fast32_t,           usart, get_rx_count,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, int_fast32_t,           usart, get_tx_count,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              usart, ctrl,                  VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_ctrl_t ctrl, void* param)

#define VSF_USART_ADDITIONAL_APIS(__prefix_name)                                \
    static inline vsf_err_t VSF_MCONNECT(__prefix_name, _usart_send_break)      \
        (VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)                      \
    {                                                                           \
        return VSF_MCONNECT(__prefix_name, _usart_ctrl)(usart_ptr,              \
            VSF_USART_CTRL_SEND_BREAK, NULL);                                   \
    }                                                                           \
    static inline vsf_err_t VSF_MCONNECT(__prefix_name, _usart_set_break)       \
        (VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)                      \
    {                                                                           \
        vsf_usart_capability_t cap =                                            \
            VSF_MCONNECT(__prefix_name, _usart_capability)(usart_ptr);          \
        VSF_HAL_ASSERT(cap.support_set_and_clear_break);                        \
        return VSF_MCONNECT(__prefix_name, _usart_ctrl)(usart_ptr,              \
            VSF_USART_CTRL_SET_BREAK, NULL);                                    \
    }                                                                           \
    static inline vsf_err_t VSF_MCONNECT(__prefix_name, _usart_clear_break)     \
        (VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)                      \
    {                                                                           \
        vsf_usart_capability_t cap =                                            \
            VSF_MCONNECT(__prefix_name, _usart_capability)(usart_ptr);          \
        VSF_HAL_ASSERT(cap.support_set_and_clear_break);                        \
        return VSF_MCONNECT(__prefix_name, _usart_ctrl)(usart_ptr,              \
            VSF_USART_CTRL_CLEAR_BREAK, NULL);                                  \
    }

/*============================ TYPES =========================================*/

#if VSF_USART_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF USART modes that can be reimplemented in specific HAL drivers.
 * Even if the hardware doesn't support these modes, the following modes must be kept.
 * If the hardware supports more modes, e.g. more parity modes, more databits, more stopbits,
 * more FIFO threshold size, we can implement it in the hardware driver.
 *
 * If more new modes are added to the driver, then the corresponding MASK macros need to
 * be defined to include the values of the new modes. For example:
 * - Adding new TX FIFO Threshold options requires VSF_USART_TX_FIFO_THRESHOLD_MASK
 * - Adding new parity check options requires VSF_USART_PARITY_MASK
 * - Adding new stop bit options requires VSF_USART_STOPBIT_MASK
 *
 * \~chinese
 * @brief 预定义的 VSF USART 模式，可以在特定的 HAL 驱动中重新实现。
 * 即使硬件不支持这些功能，但以下的模式必须保留。如果硬件支持更多模式，例如更多的奇偶校验模式、
 * 数据位大小、停止位大小、FIFO 阈值，可以在硬件驱动中实现它。
 *
 * 如果在驱动中添加更多的新模式，则需要定义对应的 MASK 宏来包含新模式的值。例如：
 * - 添加新的 TX FIFO 阈值选项需要定义 VSF_USART_TX_FIFO_THRESHOLD_MASK
 * - 添加新的奇偶校验选项需要定义 VSF_USART_PARITY_MASK
 * - 添加新的停止位选项需要定义 VSF_USART_STOPBIT_MASK
 */
typedef enum vsf_usart_mode_t {
    /**
     * \~english
     * @brief USART Parity configuration options
     * \~chinese
     * @brief USART 奇偶校验配置选项
     */
    VSF_USART_NO_PARITY         = (0x0ul << 0),    //!< \~english No parity \~chinese 无奇偶校验
    VSF_USART_EVEN_PARITY       = (0x1ul << 0),    //!< \~english Even parity \~chinese 偶校验
    VSF_USART_ODD_PARITY        = (0x2ul << 0),    //!< \~english Odd parity \~chinese 奇校验
    VSF_USART_FORCE_0_PARITY    = (0x3ul << 0),    //!< \~english Force 0 parity \~chinese 强制 0 校验
    VSF_USART_FORCE_1_PARITY    = (0x4ul << 0),    //!< \~english Force 1 parity \~chinese 强制 1 校验

    /**
     * \~english
     * @brief USART Stop bit configuration options
     * \~chinese
     * @brief USART 停止位配置选项
     */
    VSF_USART_1_STOPBIT         = (0x0ul << 3),    //!< \~english 1 stop bit \~chinese 1 个停止位
    VSF_USART_1_5_STOPBIT       = (0x1ul << 3),    //!< \~english 1.5 stop bits \~chinese 1.5 个停止位
    VSF_USART_0_5_STOPBIT       = (0x2ul << 3),    //!< \~english 0.5 stop bit \~chinese 0.5 个停止位
    VSF_USART_2_STOPBIT         = (0x3ul << 3),    //!< \~english 2 stop bits \~chinese 2 个停止位

    /**
     * \~english
     * @brief USART Data bit length configuration options
     * \~chinese
     * @brief USART 数据位长度配置选项
     */
    VSF_USART_5_BIT_LENGTH      = (0x0ul << 5),    //!< \~english 5-bit data length \~chinese 5 位数据长度
    VSF_USART_6_BIT_LENGTH      = (0x1ul << 5),    //!< \~english 6-bit data length \~chinese 6 位数据长度
    VSF_USART_7_BIT_LENGTH      = (0x2ul << 5),    //!< \~english 7-bit data length \~chinese 7 位数据长度
    VSF_USART_8_BIT_LENGTH      = (0x3ul << 5),    //!< \~english 8-bit data length \~chinese 8 位数据长度
    VSF_USART_9_BIT_LENGTH      = (0x4ul << 5),    //!< \~english 9-bit data length \~chinese 9 位数据长度
    VSF_USART_10_BIT_LENGTH     = (0x5ul << 5),    //!< \~english 10-bit data length \~chinese 10 位数据长度

    /**
     * \~english
     * @brief USART Hardware flow control configuration options
     * \~chinese
     * @brief USART 硬件流控配置选项
     */
    VSF_USART_NO_HWCONTROL      = (0x0ul << 8),    //!< \~english No hardware flow control \~chinese 无硬件流控
    VSF_USART_RTS_HWCONTROL     = (0x1ul << 8),    //!< \~english RTS hardware flow control \~chinese RTS 硬件流控
    VSF_USART_CTS_HWCONTROL     = (0x2ul << 8),    //!< \~english CTS hardware flow control \~chinese CTS 硬件流控
    VSF_USART_RTS_CTS_HWCONTROL = (0x3ul << 8),    //!< \~english RTS/CTS hardware flow control \~chinese RTS/CTS 硬件流控

    /**
     * \~english
     * @brief USART TX/RX enable/disable options
     * \~chinese
     * @brief USART 发送/接收使能/禁用选项
     */
    VSF_USART_TX_ENABLE         = (0x0ul << 9),    //!< \~english TX enabled \~chinese 发送使能
    VSF_USART_TX_DISABLE        = (0x1ul << 9),    //!< \~english TX disabled \~chinese 发送禁用
    VSF_USART_RX_ENABLE         = (0x0ul << 10),   //!< \~english RX enabled \~chinese 接收使能
    VSF_USART_RX_DISABLE        = (0x1ul << 10),   //!< \~english RX disabled \~chinese 接收禁用

    /**
     * \~english
     * @brief USART Synchronous clock configuration options
     * \~chinese
     * @brief USART 同步时钟配置选项
     */
    VSF_USART_SYNC_CLOCK_ENABLE  = (0x0ul << 11),  //!< \~english Sync clock enabled \~chinese 同步时钟使能
    VSF_USART_SYNC_CLOCK_DISABLE = (0x1ul << 11),  //!< \~english Sync clock disabled \~chinese 同步时钟禁用

    /**
     * \~english
     * @brief USART Half-duplex configuration options
     * \~chinese
     * @brief USART 半双工配置选项
     */
    VSF_USART_HALF_DUPLEX_DISABLE = (0x0ul << 14), //!< \~english Half-duplex disabled \~chinese 半双工禁用
    VSF_USART_HALF_DUPLEX_ENABLE  = (0x1ul << 14), //!< \~english Half-duplex enabled \~chinese 半双工使能

    /**
     * \~english
     * @brief USART TX FIFO threshold configuration options
     * Note: Some devices may support additional threshold levels
     * \~chinese
     * @brief USART 发送 FIFO 阈值配置选项
     * 注意：某些设备可能支持额外的阈值级别
     */
    VSF_USART_TX_FIFO_THRESHOLD_EMPTY      = (0x0ul << 15), //!< \~english TX FIFO empty \~chinese 发送 FIFO 空
    VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY = (0x1ul << 15), //!< \~english TX FIFO half empty \~chinese 发送 FIFO 半空
    VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL   = (0x2ul << 15), //!< \~english TX FIFO not full \~chinese 发送 FIFO 未满

    /**
     * \~english
     * @brief USART RX FIFO threshold configuration options
     * Note: Some devices may support additional threshold levels
     * \~chinese
     * @brief USART 接收 FIFO 阈值配置选项
     * 注意：某些设备可能支持额外的阈值级别
     */
    VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY  = (0x0ul << 17), //!< \~english RX FIFO not empty \~chinese 接收 FIFO 非空
    VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL  = (0x1ul << 17), //!< \~english RX FIFO half full \~chinese 接收 FIFO 半满
    VSF_USART_RX_FIFO_THRESHOLD_FULL       = (0x2ul << 17), //!< \~english RX FIFO full \~chinese 接收 FIFO 满
} vsf_usart_mode_t;
#endif

enum {
#ifndef VSF_USART_PARITY_MASK
    VSF_USART_PARITY_MASK               = VSF_USART_NO_PARITY
                                        | VSF_USART_EVEN_PARITY
                                        | VSF_USART_ODD_PARITY
                                        | VSF_USART_FORCE_0_PARITY
                                        | VSF_USART_FORCE_1_PARITY,
#endif

#ifndef VSF_USART_STOPBIT_MASK
    VSF_USART_STOPBIT_MASK              = VSF_USART_1_STOPBIT
                                        | VSF_USART_1_5_STOPBIT
                                        | VSF_USART_0_5_STOPBIT
                                        | VSF_USART_2_STOPBIT,
#endif

#ifndef VSF_USART_BIT_LENGTH_MASK
    VSF_USART_BIT_LENGTH_MASK           = VSF_USART_5_BIT_LENGTH
                                        | VSF_USART_6_BIT_LENGTH
                                        | VSF_USART_7_BIT_LENGTH
                                        | VSF_USART_8_BIT_LENGTH
                                        | VSF_USART_9_BIT_LENGTH
                                        | VSF_USART_10_BIT_LENGTH,
#endif

#ifndef VSF_USART_TX_FIFO_THRESHOLD_MASK
    VSF_USART_TX_FIFO_THRESHOLD_MASK    = VSF_USART_TX_FIFO_THRESHOLD_EMPTY
                                        | VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY
                                        | VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL,
#endif

#ifndef VSF_USART_RX_FIFO_THRESHOLD_MASK
    VSF_USART_RX_FIFO_THRESHOLD_MASK    = VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY
                                        | VSF_USART_RX_FIFO_THRESHOLD_FULL
                                        | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
#endif

#ifndef VSF_USART_HWCONTROL_MASK
    VSF_USART_HWCONTROL_MASK            = VSF_USART_NO_HWCONTROL
                                        | VSF_USART_RTS_HWCONTROL
                                        | VSF_USART_CTS_HWCONTROL
                                        | VSF_USART_RTS_CTS_HWCONTROL,
#endif

    VSF_USART_TX_MASK                   = VSF_USART_TX_ENABLE
                                        | VSF_USART_TX_DISABLE,

    VSF_USART_RX_MASK                   = VSF_USART_RX_ENABLE
                                        | VSF_USART_RX_DISABLE,

    VSF_USART_SYNC_CLOCK_MASK           = VSF_USART_SYNC_CLOCK_ENABLE
                                        | VSF_USART_SYNC_CLOCK_DISABLE,

    VSF_USART_HALF_DUPLEX_MASK          = VSF_USART_HALF_DUPLEX_DISABLE
                                        | VSF_USART_HALF_DUPLEX_ENABLE,

    VSF_USART_SYNC_CLOCK_POLARITY_MASK  = VSF_USART_SYNC_CLOCK_POLARITY_LOW
                                        | VSF_USART_SYNC_CLOCK_POLARITY_HIGH,

    VSF_USART_SYNC_CLOCK_PHASE_MASK     = VSF_USART_SYNC_CLOCK_PHASE_1_EDGE
                                        | VSF_USART_SYNC_CLOCK_PHASE_2_EDGE,



    VSF_USART_MODE_ALL_BITS_MASK        = VSF_USART_PARITY_MASK
                                        | VSF_USART_STOPBIT_MASK
                                        | VSF_USART_BIT_LENGTH_MASK
                                        | VSF_USART_HWCONTROL_MASK
                                        | VSF_USART_TX_FIFO_THRESHOLD_MASK
                                        | VSF_USART_RX_FIFO_THRESHOLD_MASK
                                        | VSF_USART_TX_MASK
                                        | VSF_USART_RX_MASK
                                        | VSF_USART_SYNC_CLOCK_MASK
                                        | VSF_USART_HALF_DUPLEX_MASK
                                        | VSF_USART_SYNC_CLOCK_POLARITY_MASK
                                        | VSF_USART_SYNC_CLOCK_PHASE_MASK
#ifdef VSF_USART_SYNC_CLOCK_LAST_BIT_MASK
                                        | VSF_USART_SYNC_CLOCK_LAST_BIT_MASK
#endif

#ifdef VSF_USART_IRDA_PRESCALER_MASK
                                        | VSF_USART_IRDA_PRESCALER_MASK
#endif
#ifdef VSF_USART_IRDA_MASK
                                        | VSF_USART_IRDA_MASK
#endif

#ifdef VSF_USART_SMARTCARD_MASK
                                        | VSF_USART_SMARTCARD_MASK
#endif
};

#if VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/**
 * \~english
 * @brief USART interrupt mask definitions
 * These interrupts provide status and event notifications for USART operations
 * \~chinese
 * @brief USART 中断掩码定义
 * 这些中断提供 USART 操作的状态和事件通知
 */
typedef enum vsf_usart_irq_mask_t {
    VSF_USART_IRQ_MASK_TX_CPL         = (0x1ul << 0),  //!< \~english TX complete interrupt \~chinese 发送完成中断
    VSF_USART_IRQ_MASK_RX_CPL         = (0x1ul << 1),  //!< \~english RX complete interrupt \~chinese 接收完成中断
    VSF_USART_IRQ_MASK_TX             = (0x1ul << 2),  //!< \~english TX FIFO threshold interrupt \~chinese 发送 FIFO 阈值中断
    VSF_USART_IRQ_MASK_RX             = (0x1ul << 3),  //!< \~english RX FIFO threshold interrupt \~chinese 接收 FIFO 阈值中断
    VSF_USART_IRQ_MASK_RX_TIMEOUT     = (0x1ul << 4),  //!< \~english RX timeout interrupt \~chinese 接收超时中断
    VSF_USART_IRQ_MASK_CTS            = (0x1ul << 5),  //!< \~english CTS change interrupt \~chinese CTS 变化中断
    VSF_USART_IRQ_MASK_FRAME_ERR       = (0x1ul << 6),  //!< \~english Frame error interrupt \~chinese 帧错误中断
    VSF_USART_IRQ_MASK_PARITY_ERR      = (0x1ul << 7),  //!< \~english Parity error interrupt \~chinese 奇偶校验错误中断
    VSF_USART_IRQ_MASK_BREAK_ERR       = (0x1ul << 8),  //!< \~english Break error interrupt \~chinese BREAK 信号错误中断
    VSF_USART_IRQ_MASK_OVERFLOW_ERR     = (0x1ul << 9),  //!< \~english Overflow error interrupt \~chinese 溢出错误中断
} vsf_usart_irq_mask_t;
#endif

/**
 * \~english
 * @brief USART interrupts and control flags
 * \~chinese
 * @brief USART 中断和控制标志
 */
enum {
    VSF_USART_IRQ_MASK_RX_IDLE              = VSF_USART_IRQ_MASK_RX_TIMEOUT,     //!< \~english RX idle timeout interrupt mask \~chinese RX 空闲超时中断掩码
    VSF_USART_IRQ_MASK_TX_FIFO_THRESHOLD    = VSF_USART_IRQ_MASK_TX,            //!< \~english TX FIFO threshold interrupt mask \~chinese TX FIFO 阈值中断掩码
    VSF_USART_IRQ_MASK_RX_FIFO_THRESHOLD    = VSF_USART_IRQ_MASK_RX,            //!< \~english RX FIFO threshold interrupt mask \~chinese RX FIFO 阈值中断掩码

#ifndef VSF_USART_IRQ_MASK_ERR
    VSF_USART_IRQ_MASK_ERR                  = VSF_USART_IRQ_MASK_FRAME_ERR       //!< \~english Combined error interrupt mask \~chinese 组合错误中断掩码
                                            | VSF_USART_IRQ_MASK_PARITY_ERR
                                            | VSF_USART_IRQ_MASK_BREAK_ERR
                                            | VSF_USART_IRQ_MASK_OVERFLOW_ERR,
#endif

#ifndef VSF_USART_IRQ_ALL_BITS_MASK
    VSF_USART_IRQ_ALL_BITS_MASK             = VSF_USART_IRQ_MASK_TX            //!< \~english All supported interrupt mask bits \~chinese 全部支持的中断掩码位
                                            | VSF_USART_IRQ_MASK_RX
                                            | VSF_USART_IRQ_MASK_RX_TIMEOUT
                                            | VSF_USART_IRQ_MASK_CTS
                                            | VSF_USART_IRQ_MASK_TX_CPL
                                            | VSF_USART_IRQ_MASK_RX_CPL
                                            | VSF_USART_IRQ_MASK_ERR,
#    ifdef VSF_USART_IRQ_MASK_TX_HALF_CPL
                                            | VSF_USART_IRQ_MASK_TX_HALF_CPL
#    endif
#    ifdef VSF_USART_IRQ_MASK_RX_HALF_CPL
                                            | VSF_USART_IRQ_MASK_RX_HALF_CPL
#    endif
#    ifdef VSF_USART_IRQ_MASK_CANCEL_TX_CPL
                                            | VSF_USART_IRQ_MASK_CANCEL_TX_CPL
#    endif
#    ifdef VSF_USART_IRQ_MASK_CANCEL_RX_CPL
                                            | VSF_USART_IRQ_MASK_CANCEL_RX_CPL
#    endif
#endif
};

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
/**
 * \~english
 * @brief USART instance structure forward declaration
 * \~chinese
 * @brief USART 实例结构体前置声明
 */
typedef struct vsf_usart_t vsf_usart_t;

/**
 * \~english
 * @brief USART interrupt handler function type declaration
 * @param[in,out] target_ptr User defined target pointer
 * @param[in,out] usart_ptr USART instance structure pointer
 * @param[in] irq_mask Interrupt mask indicating which events occurred
 *
 * @note This handler is called when configured interrupts occur
 *
 * Example:
 * \code{.c}
 * void usart_irq_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
 * {
 *     if (irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
 *         // Handle TX complete
 *     }
 *     if (irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
 *         // Handle RX complete
 *     }
 * }
 * \endcode
 *
 * \~chinese
 * @brief USART 中断处理函数类型声明
 * @param[in,out] target_ptr 用户自定义目标指针
 * @param[in,out] usart_ptr USART 实例结构体指针
 * @param[in] irq_mask 指示发生哪些事件的中断掩码
 *
 * @note 当配置的中断发生时会调用此处理函数
 *
 * 示例:
 * \code{.c}
 * void usart_irq_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
 * {
 *     if (irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
 *         // 处理发送完成
 *     }
 *     if (irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
 *         // 处理接收完成
 *     }
 * }
 * \endcode
 */
typedef void vsf_usart_isr_handler_t(void *target_ptr,
                                     vsf_usart_t *usart_ptr,
                                     vsf_usart_irq_mask_t irq_mask);

/**
 * \~english
 * @brief USART interrupt configuration structure
 * Contains all necessary information for configuring USART interrupts
 * \~chinese
 * @brief USART 中断配置结构体
 * 包含配置 USART 中断所需的所有信息
 */
typedef struct vsf_usart_isr_t {
    vsf_usart_isr_handler_t *handler_fn;    //!< \~english Interrupt handler function pointer \~chinese 中断处理函数指针
    void                    *target_ptr;     //!< \~english User target pointer \~chinese 用户目标指针
    vsf_arch_prio_t         prio;            //!< \~english Interrupt priority \~chinese 中断优先级
} vsf_usart_isr_t;

/**
 * \~english
 * @brief USART configuration structure
 * Contains all parameters needed to initialize and configure a USART instance
 * \~chinese
 * @brief USART 配置结构体
 * 包含初始化和配置 USART 实例所需的所有参数
 */
typedef struct vsf_usart_cfg_t {
    uint32_t                mode;           //!< \~english USART working mode \~chinese USART 工作模式
    uint32_t                baudrate;       //!< \~english Baudrate in Hz \~chinese 波特率(Hz)
    uint32_t                rx_timeout;     //!< \~english RX timeout in microseconds \~chinese 接收超时时间(微秒)
    vsf_usart_isr_t         isr;            //!< \~english Interrupt configuration \~chinese 中断配置
} vsf_usart_cfg_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL == DISABLED
/**
 * \~english
 * @brief USART control commands
 * Defines the available control operations for USART instances
 * \~chinese
 * @brief USART 控制命令
 * 定义 USART 实例可用的控制操作
 */
typedef enum vsf_usart_ctrl_t {
    /**
     * \~english
     * @brief Send BREAK signal
     * The BREAK signal will be automatically cleared after a number of bit times
     * \~chinese
     * @brief 发送 BREAK 信号
     * BREAK 信号将在若干位时间后自动清除
     */
    VSF_USART_CTRL_SEND_BREAK    = (0x01ul << 0),

    /**
     * \~english
     * @brief Set BREAK signal
     * The BREAK signal will remain until explicitly cleared
     * \~chinese
     * @brief 设置 BREAK 信号
     * BREAK 信号将保持直到显式清除
     */
    VSF_USART_CTRL_SET_BREAK     = (0x01ul << 1),

    /**
     * \~english
     * @brief Clear BREAK signal
     * Clears a previously set BREAK signal
     * \~chinese
     * @brief 清除 BREAK 信号
     * 清除之前设置的 BREAK 信号
     */
    VSF_USART_CTRL_CLEAR_BREAK   = (0x01ul << 2),

    /*
    VSF_USART_CTRL_REQUEST_RX_RESUME                     = (0x1ul << 3),
    #define VSF_USART_CTRL_REQUEST_RX_RESUME VSF_USART_CTRL_REQUEST_RX_RESUME
    VSF_USART_CTRL_REQUEST_TX_RESUME                     = (0x1ul << 4),
    #define VSF_USART_CTRL_REQUEST_TX_RESUME VSF_USART_CTRL_REQUEST_TX_RESUME

    VSF_USART_CTRL_REQUEST_RX_PAUSE                      = (0x1ul << 5),
    #define VSF_USART_CTRL_REQUEST_RX_PAUSE VSF_USART_CTRL_REQUEST_RX_PAUSE
    VSF_USART_CTRL_REQUEST_TX_PAUSE                      = (0x1ul << 6),
    #define VSF_USART_CTRL_REQUEST_TX_PAUSE VSF_USART_CTRL_REQUEST_TX_PAUSE

    VSF_USART_CTRL_HALF_DUPLEX_ENABLE_TRANSMITTER        = (0x1ul << 7),
    #define VSF_USART_CTRL_HALF_DUPLEX_ENABLE_TRANSMITTER VSF_USART_CTRL_HALF_DUPLEX_ENABLE_TRANSMITTER

    VSF_USART_CTRL_HALF_DUPLEX_ENABLE_RECEIVER           = (0x1ul << 8),
    #define VSF_USART_CTRL_HALF_DUPLEX_ENABLE_RECEIVER VSF_USART_CTRL_HALF_DUPLEX_ENABLE_RECEIVER
    */
} vsf_usart_ctrl_t;
#endif

/**
 * \~english
 * @brief USART status information structure
 * Contains the current status of USART operations and FIFO thresholds
 * \~chinese
 * @brief USART 状态信息结构体
 * 包含 USART 操作的当前状态和 FIFO 阈值
 */
#if VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_usart_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
        struct {
            uint32_t is_busy         : 1;    //!< \~english USART is busy with any operation \~chinese USART 正在进行任何操作
            uint32_t is_tx_busy      : 1;    //!< \~english TX is busy transmitting data \~chinese 发送正在进行
            uint32_t is_rx_busy      : 1;    //!< \~english RX is busy receiving data \~chinese 接收正在进行
            uint32_t tx_fifo_thresh  : 8;    //!< \~english TX FIFO threshold level (0-255) \~chinese 发送 FIFO 阈值水平(0-255)
            uint32_t rx_fifo_thresh  : 8;    //!< \~english RX FIFO threshold level (0-255) \~chinese 接收 FIFO 阈值水平(0-255)
        };
    };
} vsf_usart_status_t;
#endif

/**
 * \~english
 * @brief USART capability structure
 * Defines the hardware capabilities and limitations of the USART interface
 * \~chinese
 * @brief USART 能力结构体
 * 定义 USART 接口的硬件能力和限制
 */
#if VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_usart_capability_t {
#if VSF_USART_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_usart_irq_mask_t irq_mask;          //!< \~english Supported interrupt mask bits \~chinese 支持的中断掩码位

    uint32_t max_baudrate;                  //!< \~english Maximum supported baudrate (bps) \~chinese 支持的最大波特率(bps)
    uint32_t min_baudrate;                  //!< \~english Minimum supported baudrate (bps) \~chinese 支持的最小波特率(bps)

    uint8_t txfifo_depth;                   //!< \~english TX FIFO depth in data frames (0 means no FIFO) \~chinese TX FIFO 深度(数据帧数，0表示无FIFO)
    uint8_t rxfifo_depth;                   //!< \~english RX FIFO depth in data frames (0 means no FIFO) \~chinese RX FIFO 深度(数据帧数，0表示无FIFO)

    uint8_t max_data_bits;                  //!< \~english Maximum data bits per frame \~chinese 每帧最大数据位数
    uint8_t min_data_bits;                  //!< \~english Minimum data bits per frame \~chinese 每帧最小数据位数

    uint8_t support_rx_timeout          : 1;//!< \~english Support receive timeout \~chinese 支持接收超时
    uint8_t support_send_break          : 1;//!< \~english Support send break \~chinese 支持发送 break 信号
    uint8_t support_set_and_clear_break : 1;//!< \~english Support set and clear break \~chinese 支持设置和清除 break 信号
} vsf_usart_capability_t;
#endif
/**
 * \~english
 * @brief USART operation interface structure
 * Contains function pointers for all USART operations
 * \~chinese
 * @brief USART 操作接口结构体
 * 包含所有 USART 操作的函数指针
 */
typedef struct vsf_usart_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_USART_APIS(vsf)
} vsf_usart_op_t;

#if VSF_USART_CFG_MULTI_CLASS == ENABLED
/**
 * \~english
 * @brief USART instance structure
 * Used for USART Multi Class support
 * \~chinese
 * @brief USART 实例结构体
 * 用于 USART 多类支持
 */
struct vsf_usart_t {
    const vsf_usart_op_t * op;              //!< \~english Operation functions \~chinese 操作函数
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a USART instance
 * @param[in] usart_ptr Pointer to USART instance
 * @param[in] cfg_ptr Pointer to USART configuration
 * @return vsf_err_t VSF_ERR_NONE if successful, otherwise an error code
 *
 * @note This function must be called before using any other USART functions
 *
 * \~chinese
 * @brief 初始化 USART 实例
 * @param[in] usart_ptr USART 实例指针
 * @param[in] cfg_ptr USART 配置指针
 * @return vsf_err_t 成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 必须在使用其他 USART 函数之前调用此函数
 */
extern vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr);

/**
 * \~english
 * @brief Finalize a USART instance
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return none
 *
 * \~chinese
 * @brief 终止一个 USART 实例
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return 无
 */
extern void vsf_usart_fini(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Enable a USART instance
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return fsm_rt_t: fsm_rt_cpl if USART was enabled, fsm_rt_on_going if USART is still enabling
 *
 * \~chinese
 * @brief 启用一个 USART 实例
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return fsm_rt_t: 如果 USART 实例已启用返回 fsm_rt_cpl，如果 USART 实例正在启用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Disable a USART instance
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return fsm_rt_t: fsm_rt_cpl if USART was disabled, fsm_rt_on_going if USART is still disabling
 *
 * \~chinese
 * @brief 禁用一个 USART 实例
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return fsm_rt_t: 如果 USART 实例已禁用返回 fsm_rt_cpl，如果 USART 实例正在禁用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Enable interrupt masks of USART instance
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @param[in] irq_mask: one or more values of enum @ref vsf_usart_irq_mask_t
 * @return none
 * @note All pending interrupts should be cleared before interrupts are enabled
 *
 * \~chinese
 * @brief 启用 USART 实例的中断
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @param[in] irq_mask: 一个或多个枚举 vsf_usart_irq_mask_t 值的按位或，参考 @ref vsf_usart_irq_mask_t
 * @return 无
 * @note 在启用中断之前，应该清除所有挂起的中断
 */
extern void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask);

/**
 * \~english
 * @brief Disable interrupt masks of USART instance
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @param[in] irq_mask: one or more values of enum @ref vsf_usart_irq_mask_t
 * @return none
 *
 * \~chinese
 * @brief 禁用 USART 实例的中断
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @param[in] irq_mask: 一个或多个枚举 vsf_usart_irq_mask_t 值的按位或，参考 @ref vsf_usart_irq_mask_t
 * @return 无
 */
extern void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask);

/**
 * \~english
 * @brief Get the status of USART instance
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return vsf_usart_status_t: return all status of current USART
 *
 * \~chinese
 * @brief 获取 USART 实例的状态
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return vsf_usart_status_t: 返回当前 USART 的所有状态
 */
extern vsf_usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Get the capability of USART instance
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return vsf_usart_capability_t: return all capabilities of current USART @ref vsf_usart_capability_t
 *
 * \~chinese
 * @brief 获取 USART 实例的能力
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return vsf_usart_capability_t: 返回当前 USART 的所有能力 @ref vsf_usart_capability_t
 */
extern vsf_usart_capability_t vsf_usart_capability(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Get the number of data currently received in the USART receive FIFO
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return uint_fast32_t: the number of data currently in the USART receive queue
 *
 * \~chinese
 * @brief 获取当前在 USART 接收 FIFO 中收到的数据数量
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return uint_fast32_t: 当前在 USART 接收队列中的数据数量
 */
extern uint_fast32_t vsf_usart_rxfifo_get_data_count(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Try to read the maximum length of data from the USART receive FIFO
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @param[in] buffer_ptr: data buffer
 * @param[in] count: maximum number of reads
 * @return uint_fast32_t: return the actual number of bytes read from the current USART receive FIFO
 *
 * \~chinese
 * @brief 尝试从 USART 接收队列中读取指定最大长度的数据
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @param[in] buffer_ptr: 数据缓冲区
 * @param[in] count: 最大读取数量
 * @return uint_fast32_t: 返回从当前 USART 接收队列中实际读取的字节数
 */
extern uint_fast32_t vsf_usart_rxfifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);

/**
 * \~english
 * @brief Get the maximum number of data that can be sent through the USART send FIFO
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return uint_fast32_t: return the maximum number of bytes that can be sent through the current USART send FIFO
 *
 * \~chinese
 * @brief 获取 USART 发送队列的最大可发送数据数量
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return uint_fast32_t: 返回当前 USART 发送队列的最大可发送字节数
 */
extern uint_fast32_t vsf_usart_txfifo_get_free_count(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Try to write the maximum length of data to the USART send FIFO
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @param[in] buffer_ptr: data buffer
 * @param[in] count: maximum number of writes
 * @return uint_fast32_t: return the actual number of bytes written to the current USART send queue
 *
 * \~chinese
 * @brief 尝试向 USART 发送队列中写入指定最大长度的数据
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @param[in] buffer_ptr: 数据缓冲区
 * @param[in] count: 最大写入数量
 * @return uint_fast32_t: 返回实际写入当前 USART 发送队列的字节数
 */
extern uint_fast32_t vsf_usart_txfifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);

/**
 * \~english
 * @brief Request a DMA receive operation through the USART
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @param[in] buffer_ptr: Pointer to the receive buffer
 * @param[in] count: Number of bytes to receive
 * @return vsf_err_t: VSF_ERR_NONE if the receive request was successfully started, otherwise returns error code
 * @note This is an asynchronous operation
 *
 * \~chinese
 * @brief 请求通过 USART 进行 DMA 接收操作
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @param[in] buffer_ptr: 接收缓冲区指针
 * @param[in] count: 要接收的字节数
 * @return vsf_err_t: 如果接收请求成功启动则返回 VSF_ERR_NONE，否则返回错误码
 * @note 这是一个异步操作
 */
extern vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);

/**
 * \~english
 * @brief Request a DMA transmit operation through the USART
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @param[in] buffer_ptr: Pointer to the transmit buffer
 * @param[in] count: Number of bytes to transmit
 * @return vsf_err_t: VSF_ERR_NONE if the transmit request was successfully started, otherwise returns error code
 * @note This is an asynchronous operation
 *
 * \~chinese
 * @brief 请求通过 USART 进行 DMA 发送操作
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @param[in] buffer_ptr: 发送缓冲区指针
 * @param[in] count: 要发送的字节数
 * @return vsf_err_t: 如果发送请求成功启动则返回 VSF_ERR_NONE，否则返回错误码
 * @note 这是一个异步操作
 */
extern vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);

/**
 * \~english
 * @brief Cancel an ongoing USART receive operation
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return vsf_err_t: VSF_ERR_NONE if the receive operation was successfully cancelled, otherwise returns error code
 *
 * \~chinese
 * @brief 取消正在进行的 USART 接收操作
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return vsf_err_t: 如果成功取消接收操作则返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Cancel an ongoing USART transmit operation
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return vsf_err_t: VSF_ERR_NONE if the transmit operation was successfully cancelled, otherwise returns error code
 *
 * \~chinese
 * @brief 取消正在进行的 USART 发送操作
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return vsf_err_t: 如果成功取消发送操作则返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Get the number of bytes received in the current or last USART operation
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return int_fast32_t: Number of received bytes, negative value for error
 *
 * \~chinese
 * @brief 获取当前或最后一次 USART 操作中接收的字节数
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return int_fast32_t: 已接收的字节数，负值表示错误
 */
extern int_fast32_t vsf_usart_get_rx_count(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Get the number of bytes transmitted in the current or last USART operation
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return int_fast32_t: Number of transmitted bytes, negative value for error
 *
 * \~chinese
 * @brief 获取当前或最后一次 USART 操作中发送的字节数
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return int_fast32_t: 已发送的字节数，负值表示错误
 */
extern int_fast32_t vsf_usart_get_tx_count(vsf_usart_t *usart_ptr);

/**
 * \~english
 * @brief Execute a control command on the USART instance
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @param[in] ctrl: Control command from @ref vsf_usart_ctrl_t enumeration
 * @param[in,out] param: Command-specific parameter (can be NULL depending on command)
 * @return vsf_err_t: VSF_ERR_NONE if control command executed successfully, otherwise returns error code
 * @note Available commands and their parameters depend on hardware support
 *
 * \~chinese
 * @brief 执行 USART 实例的控制命令
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @param[in] ctrl: 来自 @ref vsf_usart_ctrl_t 枚举的控制命令
 * @param[in,out] param: 命令特定的参数（根据命令可以为 NULL）
 * @return vsf_err_t: 如果成功执行控制命令则返回 VSF_ERR_NONE，否则返回错误码
 * @note 可用的命令及其参数取决于硬件支持
 */
extern vsf_err_t vsf_usart_ctrl(vsf_usart_t *usart_ptr, vsf_usart_ctrl_t ctrl, void *param);

/*============================ INLINE FUNCTIONS ==============================*/

#if VSF_USART_CFG_REIMPLEMENT_MODE_TO_DATA_BITS == DISABLED
/**
 * \~english
 * @brief Convert mode value to data bits
 * @param[in] mode: Mode value to convert
 * @return Number of data bits
 * \~chinese
 * @brief 从模式值转换到数据位数
 * @param[in] mode: 要转换的模式值
 * @return 数据位数
 */
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
 * \~english
 * @brief The USART instance sends a BREAK signal, hardware will automatically
 *        clear the BREAK signal after a number of data bits of time
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return vsf_err_t: returns the result of the USART send BREAK signal, success returns VSF_ERR_NONE
 *
 * \~chinese
 * @brief USART 实例发送 BREAK 信号，硬件在若干个数据位的时间后会自动清除 BREAK 信号
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return vsf_err_t: 返回 USART 发送 BREAK 信号的结果，成功返回 VSF_ERR_NONE
 */
static inline vsf_err_t vsf_usart_send_break(vsf_usart_t *usart_ptr)
{
    vsf_usart_capability_t cap = vsf_usart_capability(usart_ptr);
    VSF_HAL_ASSERT(cap.support_send_break);

    return vsf_usart_ctrl(usart_ptr, VSF_USART_CTRL_SEND_BREAK, NULL);
}

/**
 * \~english
 * @brief The USART instance sets BREAK signal, hardware will not automatically clear BREAK signal
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return vsf_err_t: returns the result of the USART set BREAK signal, success returns VSF_ERR_NONE
 *
 * \~chinese
 * @brief USART 实例设置 BREAK 信号，硬件不会自动清除 BREAK 信号
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return vsf_err_t: 返回 USART 设置 BREAK 信号的结果，成功返回 VSF_ERR_NONE
 */
static inline vsf_err_t vsf_usart_set_break(vsf_usart_t *usart_ptr)
{
    vsf_usart_capability_t cap = vsf_usart_capability(usart_ptr);
    VSF_HAL_ASSERT(cap.support_set_and_clear_break);

    return vsf_usart_ctrl(usart_ptr, VSF_USART_CTRL_SET_BREAK, NULL);
}

/**
 * \~english
 * @brief The USART instance clears BREAK signal
 * @param[in] usart_ptr: a pointer to structure @ref vsf_usart_t
 * @return vsf_err_t: returns the result of the USART clear BREAK signal, success returns VSF_ERR_NONE
 *
 * \~chinese
 * @brief USART 实例清除 BREAK 信号
 * @param[in] usart_ptr: 指向结构体 @ref vsf_usart_t 的指针
 * @return vsf_err_t: 返回 USART 清除 BREAK 信号的结果，成功返回 VSF_ERR_NONE
 */
static inline vsf_err_t vsf_usart_clear_break(vsf_usart_t *usart_ptr)
{
    vsf_usart_capability_t cap = vsf_usart_capability(usart_ptr);
    VSF_HAL_ASSERT(cap.support_set_and_clear_break);

    return vsf_usart_ctrl(usart_ptr, VSF_USART_CTRL_CLEAR_BREAK, NULL);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_USART_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_usart_t                                 VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_t)
#   define vsf_usart_init(__USART, ...)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_init)                  ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_fini(__USART)                       VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_fini)                  ((__vsf_usart_t *)(__USART))
#   define vsf_usart_capability(__USART)                 VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_capability)            ((__vsf_usart_t *)(__USART))
#   define vsf_usart_enable(__USART)                     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_enable)                ((__vsf_usart_t *)(__USART))
#   define vsf_usart_disable(__USART)                    VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_disable)               ((__vsf_usart_t *)(__USART))
#   define vsf_usart_irq_enable(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_irq_enable)            ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_irq_disable(__USART, ...)           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_irq_disable)           ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_status(__USART)                     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_status)                ((__vsf_usart_t *)(__USART))
#   define vsf_usart_rxfifo_get_data_count(__USART, ...) VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_rxfifo_get_data_count) ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_rxfifo_read(__USART, ...)           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_rxfifo_read)           ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_txfifo_get_free_count(__USART, ...) VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_txfifo_get_free_count) ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_txfifo_write(__USART, ...)          VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_txfifo_write)          ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_request_rx(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_rx)            ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_request_tx(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_tx)            ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#   define vsf_usart_cancel_rx(__USART)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_rx)             ((__vsf_usart_t *)(__USART))
#   define vsf_usart_cancel_tx(__USART)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_tx)             ((__vsf_usart_t *)(__USART))
#   define vsf_usart_get_rx_count(__USART)               VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_rx_count)          ((__vsf_usart_t *)(__USART))
#   define vsf_usart_get_tx_count(__USART)               VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_tx_count)          ((__vsf_usart_t *)(__USART))
#   define vsf_usart_send_break(__USART)                VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_send_break)            ((__vsf_usart_t *)(__USART))
#   define vsf_usart_set_break(__USART)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_set_break)             ((__vsf_usart_t *)(__USART))
#   define vsf_usart_clear_break(__USART)                VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_clear_break)           ((__vsf_usart_t *)(__USART))
#   define vsf_usart_ctrl(__USART, ...)                   VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_ctrl)                 ((__vsf_usart_t *)(__USART), ##__VA_ARGS__)
#endif
/// @endcond

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#endif
