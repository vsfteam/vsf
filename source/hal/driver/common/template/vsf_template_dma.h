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

#ifndef __VSF_TEMPLATE_DMA_H__
#define __VSF_TEMPLATE_DMA_H__

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
 *
 * \~chinese
 * @brief 默认启用多类支持，以获得最大可用性。
 */
#ifndef VSF_DMA_CFG_MULTI_CLASS
#   define VSF_DMA_CFG_MULTI_CLASS                        ENABLED
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_DMA_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 *
 * \~chinese
 * @brief 可重新定义宏 VSF_DMA_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动。
 */
#ifndef VSF_DMA_CFG_PREFIX
#   if VSF_DMA_CFG_MULTI_CLASS == ENABLED
#       define VSF_DMA_CFG_PREFIX                         vsf
#   elif defined(VSF_HW_DMA_COUNT) && (VSF_HW_DMA_COUNT != 0)
#       define VSF_DMA_CFG_PREFIX                         vsf_hw
#   else
#       define VSF_DMA_CFG_PREFIX                         vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_DMA_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_dma_init()).
 *
 * \~chinese
 * @brief 禁用 VSF_DMA_CFG_FUNCTION_RENAME 以使用原始函数名
 * (例如 vsf_dma_init())。
 */
#ifndef VSF_DMA_CFG_FUNCTION_RENAME
#   define VSF_DMA_CFG_FUNCTION_RENAME                  ENABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement channel mode type in specific hardware drivers
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现通道模式类型的选项
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE    DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement interrupt mask type in specific hardware drivers
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现中断掩码类型的选项
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement DMA address type in specific hardware drivers
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现 DMA 地址类型的选项
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_ADDR
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_ADDR            DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement Channal Hint type configuration in specific hardware drivers
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现 Channal Hint 类型配置的选项
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT    DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement channel configuration type.
 * For compatibility, do not delete members when redefining vsf_dma_channel_cfg_t
 *
 * \~chinese
 * @brief 启用重新实现通道配置类型的选项。
 * 为保证兼容性，重新定义vsf_dma_channel_cfg_t时不要删除成员
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG     DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement DMA channel scatter-gather configuration type.
 *
 * \~chinese
 * @brief 启用重新实现DMA通道scatter-gather配置类型的选项。
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG  \
                                                        DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement DMA configuration type.
 * For compatibility, do not delete members when redefining vsf_dma_cfg_t
 * The vsf_dma_isr_handler_t type also needs to be redefined
 *
 * \~chinese
 * @brief 启用重新实现DMA配置类型的选项。
 * 为保证兼容性，重新定义vsf_dma_cfg_t时不要删除成员
 * 同时也需要重新定义vsf_dma_isr_handler_t类型
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG             DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement DMA channel status type.
 * \~chinese
 * @brief 启用重新实现DMA通道状态类型的选项。
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_STATUS
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_STATUS          DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement capability type.
 * For compatibility, do not delete members when redefining vsf_dma_capability_t
 *
 * \~chinese
 * @brief 启用重新实现能力类型的选项。
 * 为保证兼容性，重新定义vsf_dma_capability_t时不要删除成员
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY     DISABLED
#endif

/**
 * \~english
 * @brief Enable inheriting HAL capability to reuse common capability definitions
 *
 * \~chinese
 * @brief 启用继承HAL能力以重用通用能力定义
 */
#ifndef VSF_DMA_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_DMA_CFG_INHERIT_HAL_CAPABILITY            ENABLED
#endif

/**
 \~english
 @brief DMA scatter-gather configuration macro
 @note Use macro VSF_DMA_CHANNEL_SG_ARRAY to initialize the vsf_dma_channel_sg_desc_t
       structure array for better compatibility.
 @note In specific hardware drivers, the actual array size may be redefined based on
       the passed __COUNT, e.g., actual array size may be __COUNT+1 (for list termination).
 @warning Do NOT use sizeof(array)/sizeof(array[0]) to calculate sg_count.
          Always use the __COUNT value that was passed to this macro, as the actual
          array size may differ from __COUNT in driver-specific implementations.

 \~chinese
 @brief DMA scatter-gather 配置宏
 @note 使用宏 VSF_DMA_CHANNEL_SG_ARRAY 来初始化 vsf_dma_channel_sg_desc_t 结构体数组以保证兼容性。
 @note 在特定的硬件驱动中，可以根据传入的 __COUNT 重新指定结构体数组的大小，
       例如实际数组大小为 __COUNT+1（用于链表终止）。
 @warning 不要使用 sizeof(array)/sizeof(array[0]) 来计算 sg_count。
          始终使用传递给此宏的 __COUNT 值，因为在特定驱动实现中实际数组大小可能与 __COUNT 不同。
 */
#ifndef VSF_DMA_CHANNEL_SG_ARRAY
#   define VSF_DMA_CHANNEL_SG_ARRAY(__NAME, __COUNT, ...)                       \
    vsf_dma_channel_sg_desc_t __NAME[__COUNT] = {                               \
        __VA_ARGS__                                                             \
    };
#endif

/**
 \~english
 @brief DMA scatter-gather item configuration macro
 @note Use macro VSF_DMA_CHANNEL_SG_ITEM to initialize the @ref vsf_dma_channel_sg_desc_t
       structure for better compatibility.
 @note The 'next' field is intentionally NOT initialized by this macro - it is reserved
       for driver use and will be set up by the driver in vsf_dma_channel_sg_config_desc().

 \~chinese
 @brief DMA scatter-gather 项配置宏
 @note 使用宏 VSF_DMA_CHANNEL_SG_ITEM 来初始化 @ref vsf_dma_channel_sg_desc_t 对应的结构体以保证兼容性。
 @note 此宏故意不初始化 'next' 字段 - 该字段保留给驱动使用，
       将由驱动在 vsf_dma_channel_sg_config_desc() 中设置。
 */
#ifndef VSF_DMA_CHANNEL_SG_ITEM
#   define VSF_DMA_CHANNEL_SG_ITEM(__MODE, __SRC_ADDR, __DST_ADDR, __CNT, ...)  \
    {                                                                           \
        .mode = __MODE,                                                         \
        .src_address = __SRC_ADDR,                                              \
        .dst_address = __DST_ADDR,                                              \
        .count = __CNT,                                                         \
        __VA_ARGS__                                                             \
    }
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief DMA API template, used to generate DMA type, specific prefix function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating DMA functions.
 *
 * \~chinese
 * @brief DMA API 模板，用于生成 DMA 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 DMA 函数的前缀。
 */
#define VSF_DMA_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, init,                               VSF_MCONNECT(__prefix_name, _t) *dma_ptr, vsf_dma_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                     dma, fini,                               VSF_MCONNECT(__prefix_name, _t) *dma_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, get_configuration,                  VSF_MCONNECT(__prefix_name, _t) *dma_ptr, vsf_dma_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dma_capability_t,     dma, capability,                         VSF_MCONNECT(__prefix_name, _t) *dma_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_request,                    VSF_MCONNECT(__prefix_name, _t) *dma_ptr, vsf_dma_channel_hint_t *channel_hint_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                     dma, channel_release,                    VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_config,                     VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_get_configuration,          VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_start,                      VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel, vsf_dma_addr_t src_address, vsf_dma_addr_t dst_address, uint32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_cancel,                     VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_sg_config_desc,             VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel, vsf_dma_isr_t isr, vsf_dma_channel_sg_desc_t *scatter_gather_cfg, uint32_t sg_count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_sg_start,                   VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,                 dma, channel_get_transferred_count,      VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dma_channel_status_t, dma, channel_status,                     VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel)

/*============================ TYPES =========================================*/

/**
 * \~english
 * @brief DMA address type, used for source address, destination address and scatter-gather descriptor address.
 * @note The default type is uintptr_t.
 *
 * \~chinese
 * @brief DMA 地址类型，用于源地址、目标地址和 scatter-gather 描述符地址。
 * @note 默认类型为 uintptr_t。
 */
#if VSF_DMA_CFG_REIMPLEMENT_TYPE_ADDR == DISABLED
typedef uintptr_t vsf_dma_addr_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == DISABLED
typedef enum vsf_dma_channel_mode_t {
    VSF_DMA_MEMORY_TO_MEMORY        = (0x00 << 0),
    VSF_DMA_MEMORY_TO_PERIPHERAL    = (0x01 << 0),
    VSF_DMA_PERIPHERAL_TO_MEMORY    = (0x02 << 0),
    VSF_DMA_PERIPHERAL_TO_PERIPHERAL= (0x03 << 0),

    VSF_DMA_SRC_ADDR_INCREMENT      = (0x00 << 2),
    VSF_DMA_SRC_ADDR_DECREMENT      = (0x01 << 2),
    VSF_DMA_SRC_ADDR_NO_CHANGE      = (0x02 << 2),

    VSF_DMA_DST_ADDR_INCREMENT      = (0x00 << 4),
    VSF_DMA_DST_ADDR_DECREMENT      = (0x01 << 4),
    VSF_DMA_DST_ADDR_NO_CHANGE      = (0x02 << 4),

    VSF_DMA_SRC_WIDTH_BYTE_1        = (0x00 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_2       = (0x01 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_4       = (0x02 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_8       = (0x03 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_16      = (0x04 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_32      = (0x05 << 6),

    VSF_DMA_DST_WIDTH_BYTE_1        = (0x00 << 10),
    VSF_DMA_DST_WIDTH_BYTES_2       = (0x01 << 10),
    VSF_DMA_DST_WIDTH_BYTES_4       = (0x02 << 10),
    VSF_DMA_DST_WIDTH_BYTES_8       = (0x03 << 10),
    VSF_DMA_DST_WIDTH_BYTES_16      = (0x04 << 10),
    VSF_DMA_DST_WIDTH_BYTES_32      = (0x05 << 10),

    VSF_DMA_SRC_BURST_LENGTH_1       = (0x00 << 14),
    VSF_DMA_SRC_BURST_LENGTH_2       = (0x01 << 14),
    VSF_DMA_SRC_BURST_LENGTH_4       = (0x02 << 14),
    VSF_DMA_SRC_BURST_LENGTH_8       = (0x03 << 14),
    VSF_DMA_SRC_BURST_LENGTH_16      = (0x04 << 14),
    VSF_DMA_SRC_BURST_LENGTH_32      = (0x05 << 14),
    VSF_DMA_SRC_BURST_LENGTH_64      = (0x06 << 14),
    VSF_DMA_SRC_BURST_LENGTH_128     = (0x07 << 14),

    VSF_DMA_DST_BURST_LENGTH_1       = (0x00 << 18),
    VSF_DMA_DST_BURST_LENGTH_2       = (0x01 << 18),
    VSF_DMA_DST_BURST_LENGTH_4       = (0x02 << 18),
    VSF_DMA_DST_BURST_LENGTH_8       = (0x03 << 18),
    VSF_DMA_DST_BURST_LENGTH_16      = (0x04 << 18),
    VSF_DMA_DST_BURST_LENGTH_32      = (0x05 << 18),
    VSF_DMA_DST_BURST_LENGTH_64      = (0x06 << 18),
    VSF_DMA_DST_BURST_LENGTH_128     = (0x07 << 18),

    //prio
    VSF_DMA_PRIORITY_LOW             = (0x00 << 22),
    VSF_DMA_PRIORITY_MEDIUM          = (0x01 << 22),
    VSF_DMA_PRIORITY_HIGH            = (0x02 << 22),
    VSF_DMA_PRIORITY_VERY_HIGH       = (0x03 << 22),

} vsf_dma_channel_mode_t;
#endif

enum {
    VSF_DMA_DIRECTION_COUNT           = 4,
    VSF_DMA_DIRECTION_MASK            = VSF_DMA_MEMORY_TO_MEMORY        |
                                        VSF_DMA_MEMORY_TO_PERIPHERAL    |
                                        VSF_DMA_PERIPHERAL_TO_MEMORY    |
                                        VSF_DMA_PERIPHERAL_TO_PERIPHERAL,

    VSF_DMA_SRC_ADDR_COUNT            = 3,
    VSF_DMA_SRC_ADDR_MASK             = VSF_DMA_SRC_ADDR_INCREMENT |
                                        VSF_DMA_SRC_ADDR_DECREMENT |
                                        VSF_DMA_SRC_ADDR_NO_CHANGE,

    VSF_DMA_DST_ADDR_COUNT            = 3,
    VSF_DMA_DST_ADDR_MASK             = VSF_DMA_DST_ADDR_INCREMENT |
                                        VSF_DMA_DST_ADDR_DECREMENT |
                                        VSF_DMA_DST_ADDR_NO_CHANGE,

    VSF_DMA_SRC_WIDTH_COUNT           = 6,
    VSF_DMA_SRC_WIDTH_MASK            = VSF_DMA_SRC_WIDTH_BYTE_1   |
                                        VSF_DMA_SRC_WIDTH_BYTES_2  |
                                        VSF_DMA_SRC_WIDTH_BYTES_4  |
                                        VSF_DMA_SRC_WIDTH_BYTES_8  |
                                        VSF_DMA_SRC_WIDTH_BYTES_16 |
                                        VSF_DMA_SRC_WIDTH_BYTES_32,

    VSF_DMA_DST_WIDTH_COUNT           = 6,
    VSF_DMA_DST_WIDTH_MASK            = VSF_DMA_DST_WIDTH_BYTE_1   |
                                        VSF_DMA_DST_WIDTH_BYTES_2  |
                                        VSF_DMA_DST_WIDTH_BYTES_4  |
                                        VSF_DMA_DST_WIDTH_BYTES_8  |
                                        VSF_DMA_DST_WIDTH_BYTES_16 |
                                        VSF_DMA_DST_WIDTH_BYTES_32,

    VSF_DMA_SRC_BURST_COUNT           = 8,
    VSF_DMA_SRC_BURST_MASK            = VSF_DMA_SRC_BURST_LENGTH_1   |
                                        VSF_DMA_SRC_BURST_LENGTH_2   |
                                        VSF_DMA_SRC_BURST_LENGTH_4   |
                                        VSF_DMA_SRC_BURST_LENGTH_8   |
                                        VSF_DMA_SRC_BURST_LENGTH_16  |
                                        VSF_DMA_SRC_BURST_LENGTH_32  |
                                        VSF_DMA_SRC_BURST_LENGTH_64  |
                                        VSF_DMA_SRC_BURST_LENGTH_128,

    VSF_DMA_DST_BURST_COUNT           = 8,
    VSF_DMA_DST_BURST_MASK            = VSF_DMA_DST_BURST_LENGTH_1   |
                                        VSF_DMA_DST_BURST_LENGTH_2   |
                                        VSF_DMA_DST_BURST_LENGTH_4   |
                                        VSF_DMA_DST_BURST_LENGTH_8   |
                                        VSF_DMA_DST_BURST_LENGTH_16  |
                                        VSF_DMA_DST_BURST_LENGTH_32  |
                                        VSF_DMA_DST_BURST_LENGTH_64  |
                                        VSF_DMA_DST_BURST_LENGTH_128,

    VSF_DMA_PRIORITY_COUNT            = 4,
    VSF_DMA_PRIORITY_MASK             = VSF_DMA_PRIORITY_LOW       |
                                        VSF_DMA_PRIORITY_MEDIUM    |
                                        VSF_DMA_PRIORITY_HIGH      |
                                        VSF_DMA_PRIORITY_VERY_HIGH,

    VSF_DMA_MODE_MASK_COUNT           = 8,
    VSF_DMA_MODE_ALL_BITS_MASK        = VSF_DMA_DIRECTION_MASK  |
                                        VSF_DMA_SRC_ADDR_MASK   |
                                        VSF_DMA_DST_ADDR_MASK   |
                                        VSF_DMA_SRC_WIDTH_MASK  |
                                        VSF_DMA_DST_WIDTH_MASK  |
                                        VSF_DMA_SRC_BURST_MASK  |
                                        VSF_DMA_DST_BURST_MASK  |
                                        VSF_DMA_PRIORITY_MASK,

};

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_dma_irq_mask_t {
    VSF_DMA_IRQ_MASK_CPL        = (0x1ul << 0),
    VSF_DMA_IRQ_MASK_HALF_CPL   = (0x1ul << 1),
    VSF_DMA_IRQ_MASK_ERROR      = (0x1ul << 2),
} vsf_dma_irq_mask_t;
#endif

enum {
    VSF_DMA_IRQ_COUNT               = 3,
    VSF_DMA_IRQ_ALL_BITS_MASK       = VSF_DMA_IRQ_MASK_CPL      |
                                      VSF_DMA_IRQ_MASK_HALF_CPL |
                                      VSF_DMA_IRQ_MASK_ERROR,
};


#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
/**
 * \~english
 * @brief Configuration structure for DMA.
 *
 * \~chinese
 * @brief DMA 的配置结构体。
 */
typedef struct vsf_dma_cfg_t {
    vsf_arch_prio_t         prio;
} vsf_dma_cfg_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT == DISABLED
/**
 * \~english
 * @brief DMA channel hint structure for channel allocation
 * @note This structure is used only during channel request phase to provide hints
 *       for channel allocation. It contains only information needed for channel selection.
 *
 * \~chinese
 * @brief DMA 通道分配提示结构体
 * @note 此结构体仅在通道请求阶段使用，用于提供通道分配提示。
 *       它仅包含通道选择所需的信息。
 */
typedef struct vsf_dma_channel_hint_t {
    //! \~english Specify the DMA channel number. Use negative value (e.g., -1) for automatic allocation.
    //!           (Some chips have fixed functionality configurations for DMA channels)
    //! \~chinese 指定具体的DMA通道号。使用负值（如 -1）表示自动分配。
    //!           （部分芯片中DMA通道具有固定的功能配置）
    int8_t channel;
    //! \~english Peripheral request line number, specifying the peripheral request source using DMA service.
    //! \~english (Some chips provide a mapping table between peripherals and request lines, refer to chip manual to select the correct request line number).
    //! \~chinese 外设请求线编号，指定使用DMA服务的外设请求源。
    //! \~chinese （部分芯片会提供外设和请求线的对应表，需根据芯片手册选择正确的请求线编号）。
    uint8_t request_line;
} vsf_dma_channel_hint_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == DISABLED
typedef struct vsf_dma_t vsf_dma_t;

typedef void vsf_dma_isr_handler_t(void *target_ptr, vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_irq_mask_t irq_mask);

typedef struct vsf_dma_isr_t {
    vsf_dma_isr_handler_t *handler_fn;
    void                  *target_ptr;
} vsf_dma_isr_t;

//! dma configuration
/**
 * \~english
 * @brief DMA channel configuration structure for transfer setup
 * @note This structure contains all configuration needed for DMA transfer operations,
 *       including transfer mode, interrupt settings, and peripheral indices.
 *
 * \~chinese
 * @brief DMA 通道传输配置结构体
 * @note 此结构体包含 DMA 传输操作所需的所有配置，
 *       包括传输模式、中断设置和外设索引。
 */
typedef struct vsf_dma_channel_cfg_t {
    //! \~english DMA channel mode, including transfer direction, address increment mode, data width, burst length, priority, etc.
    //! \~english Use VSF_DMA_XXX macros to configure (e.g. VSF_DMA_MEMORY_TO_PERIPHERAL | VSF_DMA_SRC_ADDR_INCREMENT | VSF_DMA_DST_WIDTH_BYTES_4)
    //! \~chinese DMA 通道模式，包括传输方向、地址增量模式、数据宽度、突发长度、优先级等
    //! \~chinese 使用 VSF_DMA_XXX 宏进行配置（例如 VSF_DMA_MEMORY_TO_PERIPHERAL | VSF_DMA_SRC_ADDR_INCREMENT | VSF_DMA_DST_WIDTH_BYTES_4）
    vsf_dma_channel_mode_t  mode;
    //! \~english Interrupt service routine configuration, including handler function and target pointer
    //! \~chinese 中断服务例程配置，包括处理函数和目标指针
    vsf_dma_isr_t           isr;
    //! \~english Interrupt mask, indicating which interrupts to enable (e.g. VSF_DMA_IRQ_MASK_CPL | VSF_DMA_IRQ_MASK_ERROR)
    //! \~english Multiple interrupt types can be combined using bitwise OR operation
    //! \~chinese 中断掩码，指定需要启用的中断（例如 VSF_DMA_IRQ_MASK_CPL | VSF_DMA_IRQ_MASK_ERROR）
    //! \~chinese 可以使用按位或操作组合多个中断类型
    vsf_dma_irq_mask_t      irq_mask;
    //! \~english Interrupt priority for this channel. Use vsf_arch_prio_invalid to use default priority from vsf_dma_cfg_t.
    //!           Only effective when hardware supports per-channel or per-group interrupt priority configuration.
    //! \~chinese 此通道的中断优先级。使用 vsf_arch_prio_invalid 表示使用 vsf_dma_cfg_t 中的默认优先级。
    //!           仅在硬件支持每通道或每组中断独立优先级配置时生效。
    vsf_arch_prio_t         prio;
    //! \~english DMA peripheral request signal index for source side
    //! \~english This is the hardware-specific request signal ID used for DMA handshaking
    //! \~english Only effective when source is a peripheral (ignored for memory source)
    //! \~chinese DMA 源端外设请求信号索引
    //! \~chinese 这是用于 DMA 硬件握手的特定请求信号 ID
    //! \~chinese 仅当源端为外设时有效（内存源时忽略）
    uint8_t                 src_request_idx;
    //! \~english DMA peripheral request signal index for destination side
    //! \~english This is the hardware-specific request signal ID used for DMA handshaking
    //! \~english Only effective when destination is a peripheral (ignored for memory destination)
    //! \~chinese DMA 目标端外设请求信号索引
    //! \~chinese 这是用于 DMA 硬件握手的特定请求信号 ID
    //! \~chinese 仅当目标端为外设时有效（内存目标时忽略）
    uint8_t                 dst_request_idx;
} vsf_dma_channel_cfg_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG == DISABLED
/**
 * \~english
 * @brief DMA scatter-gather descriptor structure
 * @note: The vsf_dma_channel_sg_desc_t structure can be redefined
 * according to the descriptor structure required by DMA Scatter-Gather in
 * specific hardware drivers.
 *
 * \~chinese
 * @brief DMA scatter-gather 描述符结构体
 * @note: vsf_dma_channel_sg_desc_t
 * 结构体，在特定的硬件驱动中可以根据 DMA Scatter-Gather
 * 需要的描述符结构体重新定义。
 */
typedef struct vsf_dma_channel_sg_desc_t {
    vsf_dma_channel_mode_t mode;    //!< DMA channel mode
    vsf_dma_addr_t src_address;     //!< Source address
    vsf_dma_addr_t dst_address;     //!< Destination address
    uint32_t count;                 //!< Number of data items to be transferred
    //! \~english Next descriptor address. Reserved for driver use only - users should NOT
    //!           initialize this field. The driver will set up the linked list chain internally
    //!           in vsf_dma_channel_sg_config_desc().
    //! \~chinese 下一个描述符地址。仅供驱动内部使用 - 用户不应初始化此字段。
    //!           驱动会在 vsf_dma_channel_sg_config_desc() 内部设置链表链接。
    vsf_dma_addr_t next;
} vsf_dma_channel_sg_desc_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_STATUS == DISABLED
typedef struct vsf_dma_channel_status_t {
    union {
        inherit(vsf_peripheral_status_t)
            struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_dma_channel_status_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
/**
 * \~english
 * @brief DMA capability structure that can be reimplemented in specific HAL drivers
 * @note This structure defines the features supported by the DMA hardware.
 *       When reimplementing, all existing members must be preserved for compatibility.
 *
 * \~chinese
 * @brief 可在具体 HAL 驱动中重新实现的 DMA 能力结构体
 * @note 此结构体定义了 DMA 硬件支持的特性。
 *       为保证兼容性，重新实现时需保留所有现有成员。
 */
/**
 * \~english
 * @brief DMA capability structure that can be reimplemented in specific HAL drivers
 * @note This structure defines the features supported by the DMA hardware.
 *       When reimplementing, all existing members must be preserved for compatibility.
 *
 * \~chinese
 * @brief 可在具体 HAL 驱动中重新实现的 DMA 能力结构体
 * @note 此结构体定义了 DMA 硬件支持的特性。
 *       为保证兼容性，重新实现时需保留所有现有成员。
 */
typedef struct vsf_dma_capability_t {
#if VSF_DMA_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif

    vsf_dma_irq_mask_t irq_mask;               //!< \~english Supported interrupt mask bits \~chinese 支持的中断掩码位

    uint32_t max_request_count;                //!< \~english Maximum count per single transfer request \~chinese 单次传输请求的最大计数
    uint8_t channel_count;                     //!< \~english Number of DMA channels \~chinese DMA 通道数量

    /**
     * \~english
     * Number of independent interrupt vectors for DMA.
     * If irq_count < channel_count, some channels share interrupts.
     * \~chinese
     * DMA 的独立中断向量数量。
     * 如果 irq_count < channel_count，说明部分通道共享中断。
     */
    uint8_t irq_count;

    /**
     * \~english
     * Supported channel modes, use VSF_DMA_XXX_MASK to extract specific capabilities:
     *    - VSF_DMA_DIRECTION_MASK: transfer directions
     *    - VSF_DMA_SRC_ADDR_MASK / VSF_DMA_DST_ADDR_MASK: address modes
     *    - VSF_DMA_SRC_WIDTH_MASK / VSF_DMA_DST_WIDTH_MASK: data widths
     *    - VSF_DMA_SRC_BURST_MASK / VSF_DMA_DST_BURST_MASK: burst lengths
     *    - VSF_DMA_PRIORITY_MASK: priority levels
     * \~chinese
     * 支持的通道模式，使用 VSF_DMA_XXX_MASK 提取特定能力：
     *    - VSF_DMA_DIRECTION_MASK: 传输方向
     *    - VSF_DMA_SRC_ADDR_MASK / VSF_DMA_DST_ADDR_MASK: 地址模式
     *    - VSF_DMA_SRC_WIDTH_MASK / VSF_DMA_DST_WIDTH_MASK: 数据宽度
     *    - VSF_DMA_SRC_BURST_MASK / VSF_DMA_DST_BURST_MASK: 突发长度
     *    - VSF_DMA_PRIORITY_MASK: 优先级
     */
    vsf_dma_channel_mode_t supported_modes;

    uint32_t max_transfer_size;                //!< \~english Maximum number of data items per transfer (0 means no limit) \~chinese 每次传输的最大数据项数量（0 表示无限制）
    uint8_t addr_alignment;                    //!< \~english Address alignment requirement in bytes (1 means no alignment required) \~chinese 地址对齐要求（字节），1 表示无对齐要求

    uint8_t support_scatter_gather : 1;        //!< \~english Support scatter-gather transfer \~chinese 支持 Scatter-Gather 传输
} vsf_dma_capability_t;
#endif

typedef struct vsf_dma_op_t {
/// @cond
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_DMA_APIS(vsf_dma)
} vsf_dma_op_t;

#if VSF_DMA_CFG_MULTI_CLASS == ENABLED
struct vsf_dma_t  {
    const vsf_dma_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief Initialize a DMA instance
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_dma_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise returns error code

 @note It is not necessary to call vsf_dma_fini() to deinitialization.
       vsf_dma_init() should be called before any other DMA API except vsf_dma_capability().

 \~chinese
 @brief 初始化一个 DMA 实例
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] cfg_ptr: 指向结构体 @ref vsf_dma_cfg_t 的指针
 @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码

 @note 失败后不需要调用 vsf_dma_fini() 反初始化。
       vsf_dma_init() 应该在除 vsf_dma_capability() 之外的其他 DMA API 之前调用。
 */
extern vsf_err_t vsf_dma_init(vsf_dma_t *dma_ptr, vsf_dma_cfg_t *cfg_ptr);

/**
 \~english
 @brief Finalize a DMA instance
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @return none

 \~chinese
 @brief 终止一个 DMA 实例
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @return 无
 */
extern void vsf_dma_fini(vsf_dma_t *dma_ptr);

/**
 \~english
 @brief Get the current configuration of DMA instance
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[out] cfg_ptr: a pointer to structure @ref vsf_dma_cfg_t to store the current configuration
 @return vsf_err_t: VSF_ERR_NONE if successful, otherwise returns error code

 \~chinese
 @brief 获取 DMA 实例的当前配置
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[out] cfg_ptr: 指向结构体 @ref vsf_dma_cfg_t 的指针，用于存储当前配置
 @return vsf_err_t: 如果成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_dma_get_configuration(vsf_dma_t *dma_ptr, vsf_dma_cfg_t *cfg_ptr);

/**
 \~english
 @brief Get the capabilities of DMA instance
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @return vsf_dma_capability_t: All capabilities of current DMA @ref vsf_dma_capability_t

 \~chinese
 @brief 获取 DMA 实例的能力
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @return vsf_dma_capability_t: 返回当前 DMA 的所有能力 @ref vsf_dma_capability_t
 */
extern vsf_dma_capability_t vsf_dma_capability(vsf_dma_t *dma_ptr);

/**
 \~english
 @brief DMA request a new channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in,out] channel_hint_ptr: a pointer to DMA channel hint. User should provide appropriate hint information based on actual requirements. If the actually allocated channel or other configuration differs from user's expectation, the function may modify channel_hint_ptr to notify the user of the actual allocation. The allocated channel number will be stored in channel_hint_ptr->channel.
 @return vsf_err_t: VSF_ERR_NONE if the request was successful, otherwise returns error code

 \~chinese
 @brief DMA 请求一个新的通道
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in,out] channel_hint_ptr: 指向 DMA 通道提示的指针。用户应根据实际情况提供合适的提示信息。如果实际分配的通道或其他配置与用户预期不一致，函数可能会修改 channel_hint_ptr 来通知用户实际分配的结果。分配的通道号将存储在 channel_hint_ptr->channel 中。
 @return vsf_err_t: 如果请求成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_dma_channel_request(vsf_dma_t *dma_ptr, vsf_dma_channel_hint_t *channel_hint_ptr);

/**
 \~english
 @brief Release a DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return none

 \~chinese
 @brief DMA 释放一个通道
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道号
 @return 无
 */
extern void vsf_dma_channel_release(vsf_dma_t *dma_ptr, uint8_t channel);

/**
 \~english
 @brief Configure a DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @param[in] cfg_ptr: a pointer to DMA channel configuration
 @return vsf_err_t: VSF_ERR_NONE if the configuration was successful, otherwise returns error code

 \~chinese
 @brief DMA 通道配置
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @param[in] cfg_ptr: 指向 DMA 通道配置的指针
 @return vsf_err_t: 如果配置成功返回 VSF_ERR_NONE，否则返回错误码
 @note: 对于指定通道，API vsf_dma_channel_sg_config_desc() 和 vsf_dma_channel_sg_start() 必须配合使用。
        API vsf_dma_channel_config() 和 vsf_dma_channel_start() 配对使用。
        这两组API不能混用，只有在当前DMA传输完全结束后，才能切换到另一组API使用。
 */
extern vsf_err_t vsf_dma_channel_config(vsf_dma_t *dma_ptr, uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr);

/**
 \~english
 @brief Get the current configuration of a DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @param[out] cfg_ptr: a pointer to structure @ref vsf_dma_channel_cfg_t to store the current configuration
 @return vsf_err_t: VSF_ERR_NONE if successful, otherwise returns error code

 \~chinese
 @brief 获取 DMA 通道的当前配置
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @param[out] cfg_ptr: 指向结构体 @ref vsf_dma_channel_cfg_t 的指针，用于存储当前配置
 @return vsf_err_t: 如果成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_dma_channel_get_configuration(vsf_dma_t *dma_ptr, uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr);

/**
 * \~english
 * @brief Start a DMA channel transfer
 * @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 * @param[in] channel: channel number
 * @param[in] src_address: source address
 * @param[in] dst_address: destination address
 * @param[in] count: number of data items to be transferred
 * @return vsf_err_t:
 *         - VSF_ERR_NONE if the start request was successful
 *         - VSF_ERR_BUSY if the channel is currently busy with another transfer
 *         - Other error codes for other failures
 *
 * \~chinese
 * @brief DMA 开始一个通道的传输
 * @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 * @param[in] channel: 通道序号
 * @param[in] src_address: 源地址
 * @param[in] dst_address: 目标地址
 * @param[in] count: 将要传输的数据项数量
 * @return vsf_err_t:
 *         - VSF_ERR_NONE 如果开始传输成功
 *         - VSF_ERR_BUSY 如果通道当前正忙于另一个传输
 *         - 其他错误码表示其他失败情况
 * @note: 对于指定通道，API vsf_dma_channel_sg_config_desc() 和 vsf_dma_channel_sg_start() 必须配合使用。
        API vsf_dma_channel_config() 和 vsf_dma_channel_start() 配对使用。
        这两组API不能混用，只有在当前DMA传输完全结束后，才能切换到另一组API使用。
 * @note: If the channel is busy, this function returns VSF_ERR_BUSY immediately without modifying the ongoing transfer.
 *        To wait for the current transfer to complete, applications can:
 *        - Poll vsf_dma_channel_status() to check if the channel is no longer busy (is_busy == 0)
 *        - Wait for the transfer completion interrupt (VSF_DMA_IRQ_MASK_CPL) if interrupt is enabled
 *        如果通道正忙，此函数立即返回 VSF_ERR_BUSY，不会修改正在进行的传输。
 *        要等待当前传输完成，应用程序可以：
 *        - 轮询 vsf_dma_channel_status() 检查通道是否不再忙碌（is_busy == 0）
 *        - 如果启用了中断，等待传输完成中断（VSF_DMA_IRQ_MASK_CPL）
 */
extern vsf_err_t vsf_dma_channel_start(vsf_dma_t *dma_ptr, uint8_t channel, vsf_dma_addr_t src_address, vsf_dma_addr_t dst_address, uint32_t count);

/**
 \~english
 @brief Cancel the transmission of a specific DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return vsf_err_t: VSF_ERR_NONE if the cancel request was successful, otherwise returns error code

 \~chinese
 @brief DMA 取消特定通道的传输
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @return vsf_err_t: 如果传输取消成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_dma_channel_cancel(vsf_dma_t *dma_ptr, uint8_t channel);

/**
 \~english
 @brief Configure a DMA channel for scatter-gather transfer
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @param[in] isr: DMA interrupt handler
 @param[in] cfg_ptr: a pointer to DMA channel scatter-gather configuration
 @param[in] sg_count: number of scatter-gather configurations
 @return vsf_err_t: VSF_ERR_NONE if the configuration was successful, otherwise returns error code
 @note The scatter_gather_cfg array should be initialized using the VSF_DMA_CHANNEL_SG_ARRAY macro
       for better compatibility.
 @note The scatter_gather_cfg must point to DMA-accessible memory region.
 @note The caller must ensure scatter_gather_cfg remains valid until the DMA transfer completes.
 @note The contents of scatter_gather_cfg may be modified by the driver to match the actual
       hardware descriptor format required by the DMA controller.
 @note The driver will handle linked list construction internally, including:
       - Setting up the 'next' pointer for each descriptor
       - Terminating the list with NULL or hardware-specific end marker
       Users only need to provide a valid descriptor array with sg_count elements.
 @note For a specific channel, API vsf_dma_channel_sg_config_desc() and vsf_dma_channel_sg_start()
       must be used together. API vsf_dma_channel_config() and vsf_dma_channel_start() are paired
       for use. These two groups of APIs cannot be mixed. Only after the current DMA transfer is
       completely finished, can you switch to the other group of APIs.

 \~chinese
 @brief DMA 通道配置为 scatter-gather 传输
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @param[in] isr: DMA 中断处理函数
 @param[in] cfg_ptr: 指向 DMA 通道 scatter-gather 配置的指针
 @param[in] sg_count: scatter-gather 配置的数量
 @return vsf_err_t: 如果配置成功返回 VSF_ERR_NONE，否则返回错误码
 @note scatter_gather_cfg 指向的结构体应该使用宏 VSF_DMA_CHANNEL_SG_ARRAY 来初始化，以保证更好的兼容性。
 @note scatter_gather_cfg 指向的结构体需要是可以被 DMA 直接访问的内存区域。
 @note scatter_gather_cfg 指向的结构体需要调用者确保在 DMA 传输完成之前不会被释放。
 @note scatter_gather_cfg 指向的结构体的内容可能在配置后被修改成 DMA 实际需要的 Scatter-Gather 描述符结构。
 @note 驱动会在内部处理链表的构建，包括：
       - 为每个描述符设置 'next' 指针
       - 使用 NULL 或硬件特定的结束标记终止链表
       用户只需提供包含 sg_count 个元素的有效描述符数组。
 @note 对于指定通道，API vsf_dma_channel_sg_config_desc() 和 vsf_dma_channel_sg_start() 必须配合使用。
       API vsf_dma_channel_config() 和 vsf_dma_channel_start() 配对使用。
       这两组API不能混用，只有在当前DMA传输完全结束后，才能切换到另一组API使用。
 */
extern vsf_err_t vsf_dma_channel_sg_config_desc(vsf_dma_t *dma_ptr, uint8_t channel, vsf_dma_isr_t isr,
            vsf_dma_channel_sg_desc_t *sg_desc_ptr, uint32_t sg_count);

/**
 \~english
 @brief Start a DMA scatter-gather transfer
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return vsf_err_t:
         - VSF_ERR_NONE if the start request was successful
         - VSF_ERR_BUSY if the channel is currently busy with another transfer
         - Other error codes for other failures
 @note: For a specific channel, API vsf_dma_channel_sg_config_desc() and vsf_dma_channel_sg_start() must be used together.
        API vsf_dma_channel_config() and vsf_dma_channel_start() are paired for use.
        These two groups of APIs cannot be mixed. Only after the current DMA transfer is completely finished,
        can you switch to the other group of APIs.
 @note: If the channel is busy, this function returns VSF_ERR_BUSY immediately without modifying the ongoing transfer.
        To wait for the current transfer to complete, applications can:
        - Poll vsf_dma_channel_status() to check if the channel is no longer busy (is_busy == 0)
        - Wait for the transfer completion interrupt (VSF_DMA_IRQ_MASK_CPL) if interrupt is enabled

 \~chinese
 @brief DMA 开始一个 scatter-gather 传输
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @return vsf_err_t:
         - VSF_ERR_NONE 如果开始传输成功
         - VSF_ERR_BUSY 如果通道当前正忙于另一个传输
         - 其他错误码表示其他失败情况
 @note: 对于指定通道，API vsf_dma_channel_sg_config_desc() 和 vsf_dma_channel_sg_start() 必须配合使用。
        API vsf_dma_channel_config() 和 vsf_dma_channel_start() 配对使用。
        这两组API不能混用，只有在当前DMA传输完全结束后，才能切换到另一组API使用。
 @note: 如果通道正忙，此函数立即返回 VSF_ERR_BUSY，不会修改正在进行的传输。
        要等待当前传输完成，应用程序可以：
        - 轮询 vsf_dma_channel_status() 检查通道是否不再忙碌（is_busy == 0）
        - 如果启用了中断，等待传输完成中断（VSF_DMA_IRQ_MASK_CPL）
 */
extern vsf_err_t vsf_dma_channel_sg_start(vsf_dma_t *dma_ptr, uint8_t channel);

/**
 * \~english
 * @brief Get the count of data items transferred for a specific DMA channel
 * @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 * @param[in] channel: channel number
 * @return uint32_t: Number of data items transferred
 *
 * @note Behavior after completion:
 *       - For normal transfer: Returns 0 after VSF_DMA_IRQ_MASK_CPL interrupt.
 *       - For scatter-gather transfer: Returns 0 after the entire SG chain completes.
 *
 * @note Usage after cancel:
 *       This function is typically called after vsf_dma_channel_cancel() to get the
 *       number of data items that were successfully transferred before cancellation.
 *
 * @note Scatter-Gather semantics:
 *       - Returns the cumulative total of data items transferred across all descriptors
 *         in the SG chain, including completed descriptors plus the partial transfer
 *         of the current (interrupted) descriptor.
 *       - If cancel occurs exactly at a descriptor boundary, the return value includes
 *         all data items from completed descriptors (the current descriptor's transfer
 *         count would be 0).
 *
 * \~chinese
 * @brief DMA 获取特定通道已经传输的数量
 * @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 * @param[in] channel: 通道序号
 * @return uint32_t: 已传输的数据项数量
 *
 * @note 完成后的行为:
 *       - 普通传输: 在 VSF_DMA_IRQ_MASK_CPL 中断之后返回 0。
 *       - Scatter-Gather 传输: 在整个 SG 链完成后返回 0。
 *
 * @note 取消后的用法:
 *       此函数通常在调用 vsf_dma_channel_cancel() 之后调用，用于获取取消前已成功传输的数据项数量。
 *
 * @note Scatter-Gather 语义:
 *       - 返回整个 SG 链中所有描述符的累计传输数据项数量，包括已完成的描述符
 *         加上当前（被中断的）描述符的部分传输量。
 *       - 如果取消恰好发生在描述符边界上，返回值包含所有已完成描述符的数据项数量
 *         （当前描述符的传输计数为 0）。
 */
extern uint32_t vsf_dma_channel_get_transferred_count(vsf_dma_t *dma_ptr, uint8_t channel);

/**
 \~english
 @brief Get status of a DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return vsf_dma_channel_status_t: Status of the DMA channel

 \~chinese
 @brief 获取 DMA 通道状态
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @return vsf_dma_channel_status_t: DMA 通道状态
 */
extern vsf_dma_channel_status_t vsf_dma_channel_status(vsf_dma_t *dma_ptr, uint8_t channel);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_DMA_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_dma_t                                              VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_t)
#   define vsf_dma_init(__DMA, ...)                                 VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_init)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_fini(__DMA)                                      VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_fini)((__vsf_dma_t *)(__DMA))
#   define vsf_dma_get_configuration(__DMA, ...)                    VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_get_configuration)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_capability(__DMA)                                VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_capability)((__vsf_dma_t *)(__DMA))
#   define vsf_dma_channel_request(__DMA, ...)                      VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_request)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_release(__DMA, ...)                      VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_release)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_config(__DMA, ...)                       VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_config)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_get_configuration(__DMA, ...)            VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_get_configuration)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_start(__DMA, ...)                        VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_start)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_sg_config_desc(__DMA, ...)               VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_sg_config_desc)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_sg_start(__DMA, ...)                     VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_sg_start)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_cancel(__DMA, ...)                       VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_cancel)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_get_transferred_count(__DMA, ...)        VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_get_transferred_count)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_status(__DMA, ...)                       VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_status)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_DMA_H__*/
