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

#ifndef __VSF_TEMPLATE_SPI_H__
#define __VSF_TEMPLATE_SPI_H__

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
#ifndef VSF_SPI_CFG_MULTI_CLASS
#   define VSF_SPI_CFG_MULTI_CLASS                      ENABLED
#endif

/**
 * \~english
 * @brief Define SPI hardware mask if count is defined
 *
 * \~chinese
 * @brief 如果定义了 SPI 硬件数量，则定义对应的掩码
 */
#if defined(VSF_HW_SPI_COUNT) && !defined(VSF_HW_SPI_MASK)
#   define VSF_HW_SPI_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_SPI_COUNT)
#endif

/**
 * \~english
 * @brief Define SPI hardware count if mask is defined
 *
 * \~chinese
 * @brief 如果定义了 SPI 硬件掩码，则定义对应的数量
 */
#if defined(VSF_HW_SPI_MASK) && !defined(VSF_HW_SPI_COUNT)
#   define VSF_HW_SPI_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_SPI_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_SPI_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 * \~chinese
 * @brief 可以重新定义宏 VSF_SPI_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动函数。
 */
#ifndef VSF_SPI_CFG_PREFIX
#   if VSF_SPI_CFG_MULTI_CLASS == ENABLED
#       define VSF_SPI_CFG_PREFIX                       vsf
#   elif defined(VSF_HW_SPI_COUNT) && (VSF_HW_SPI_COUNT != 0)
#       define VSF_SPI_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_SPI_CFG_PREFIX                       vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_SPI_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_spi_init()).
 * \~chinese
 * @brief 禁用 VSF_SPI_CFG_FUNCTION_RENAME 以使用原始函数名
 * (例如 vsf_spi_init())。
 */
#ifndef VSF_SPI_CFG_FUNCTION_RENAME
#   define VSF_SPI_CFG_FUNCTION_RENAME                  ENABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE in specific hardware
 * drivers to redefine enum @ref vsf_spi_mode_t.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE
 * 来重新定义枚举 @ref vsf_spi_mode_t。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE            DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK to redefine enum
 * @ref vsf_spi_irq_mask_t.
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK
 * 来重新定义枚举 @ref vsf_spi_irq_mask_t。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS to redefine struct
 * @ref vsf_spi_status_t.
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS
 * 来重新定义结构体 @ref vsf_spi_status_t。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS          DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG to redefine struct
 * @ref vsf_spi_cfg_t. For compatibility, members should not be
 * deleted when redefining @ref vsf_spi_cfg_t. The type
 * @ref vsf_spi_isr_handler_t also needs to be redefined.
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG
 * 来重新定义结构体 @ref vsf_spi_cfg_t。为保证兼容性，重新定义时不应删除成员，
 * 同时需要重新定义类型 @ref vsf_spi_isr_handler_t。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG
#    define VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG            DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL to redefine enum
 * @ref vsf_spi_ctrl_t.
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL
 * 来重新定义枚举 @ref vsf_spi_ctrl_t。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL            DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY to redefine struct
 * @ref vsf_spi_capability_t. For compatibility, members should not
 * be deleted.
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY
 * 来重新定义结构体 @ref vsf_spi_capability_t。为保证兼容性，重新定义时不应删除成员。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY
#    define VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY     DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS to reimplement the
 * function vsf_spi_mode_to_data_bits().
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS
 * 来重新实现函数 vsf_spi_mode_to_data_bits()。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS
#   define VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS    DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE to reimplement the
 * function vsf_spi_data_bits_to_mode().
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE
 * 来重新实现函数 vsf_spi_data_bits_to_mode()。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE
#   define VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE    DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_REIMPLEMENT_MODE_TO_BYTES to reimplement the function
 * vsf_spi_mode_to_data_bytes().
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_REIMPLEMENT_MODE_TO_BYTES
 * 来重新实现函数 vsf_spi_mode_to_data_bytes()。
 */
#ifndef VSF_SPI_CFG_REIMPLEMENT_MODE_TO_BYTES
#   define VSF_SPI_CFG_REIMPLEMENT_MODE_TO_BYTES        DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, enable macro
 * VSF_SPI_CFG_INHERIT_HAL_CAPABILITY to inherit the structure
 * @ref vsf_peripheral_capability_t.
 * \~chinese
 * @brief 在特定硬件驱动中，启用宏 VSF_SPI_CFG_INHERIT_HAL_CAPABILITY
 * 来继承结构体 @ref vsf_peripheral_capability_t。
 */
#ifndef VSF_SPI_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_SPI_CFG_INHERIT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief SPI API template, used to generate SPI type, specific prefix function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating SPI functions.
 * \~chinese
 * @brief SPI API 模板，用于生成 SPI 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 SPI 函数的前缀。
 */
#define VSF_SPI_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, init,                  VSF_MCONNECT(__prefix_name, _t) *spi_ptr, vsf_spi_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, fini,                  VSF_MCONNECT(__prefix_name, _t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             spi, enable,                VSF_MCONNECT(__prefix_name, _t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             spi, disable,               VSF_MCONNECT(__prefix_name, _t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, irq_enable,            VSF_MCONNECT(__prefix_name, _t) *spi_ptr, vsf_spi_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, irq_disable,           VSF_MCONNECT(__prefix_name, _t) *spi_ptr, vsf_spi_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_spi_status_t,     spi, status,                VSF_MCONNECT(__prefix_name, _t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_spi_capability_t, spi, capability,            VSF_MCONNECT(__prefix_name, _t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, cs_active,             VSF_MCONNECT(__prefix_name, _t) *spi_ptr, uint_fast8_t index) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, cs_inactive,           VSF_MCONNECT(__prefix_name, _t) *spi_ptr, uint_fast8_t index) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, fifo_transfer,         VSF_MCONNECT(__prefix_name, _t) *spi_ptr, void *out_buffer_ptr, uint_fast32_t *out_offset_ptr, void *in_buffer_ptr, uint_fast32_t *in_offset_ptr, uint_fast32_t cnt) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, request_transfer,      VSF_MCONNECT(__prefix_name, _t) *spi_ptr, void *out_buffer_ptr, void *in_buffer_ptr, uint_fast32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, cancel_transfer,       VSF_MCONNECT(__prefix_name, _t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, get_transferred_count, VSF_MCONNECT(__prefix_name, _t) *spi_ptr, uint_fast32_t * tx_count, uint_fast32_t *rx_count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, ctrl,                  VSF_MCONNECT(__prefix_name, _t) *spi_ptr, vsf_spi_ctrl_t ctrl, void* param)


/*============================ TYPES =========================================*/

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF SPI modes that can be reimplemented in specific HAL drivers.
 *
 * SPI Mode Bit Field Layout (32-bit):
 * Bits 31-16: Reserved for hardware-specific extensions
 * Bits 15-8:  Data size configuration
 * Bits 7-4:   Hardware/Software CS control and protocol modes
 * Bit  3:     Clock phase (CPHA): 0=first edge, 1=second edge
 * Bit  2:     Clock polarity (CPOL): 0=idle low, 1=idle high
 * Bit  1:     Bit order: 0=MSB first, 1=LSB first
 * Bit  0:     Direction: 0=master, 1=slave
 *
 * \~chinese
 * @brief 预定义的 VSF SPI 模式，可以在具体的 HAL 驱动重新实现。
 *
 * SPI 模式位字段布局（32位）：
 * 位 31-16: 为硬件特定扩展保留
 * 位 15-8:  数据大小配置
 * 位 7-4:   硬件/软件片选控制和协议模式
 * 位 3:     时钟相位 (CPHA): 0=第一个边沿，1=第二个边沿
 * 位 2:     时钟极性 (CPOL): 0=空闲低电平，1=空闲高电平
 * 位 1:     位顺序: 0=MSB优先，1=LSB优先
 * 位 0:     方向: 0=主机，1=从机
 *
 * \~english
 * If we want to add optional modes in the specific driver, we need to provide
 * one or more enum options, and a macro with the same name to determine if they
 * are supported at runtime. If the feature supports more than one option, it
 * also needs to provide the corresponding MASK option, so that the user can
 * switch to different modes at compile-time.
 * \~chinese
 * 在特定驱动里，添加可选模式时需要提供一个或者多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果该特性支持多个选项，那也需要提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
typedef enum vsf_spi_mode_t {
    // Direction control (bit 0)
    VSF_SPI_MASTER                  = 0x00ul << 0,  //! \~english Master mode (controller) \~chinese 主机模式（控制器）
    VSF_SPI_SLAVE                   = 0x01ul << 0,  //! \~english Slave mode (peripheral) \~chinese 从机模式（外设）

    // Bit order control (bit 1)
    VSF_SPI_MSB_FIRST               = 0x00ul << 1,  //! \~english Most Significant Bit (MSB) first \~chinese 最高有效位（MSB）优先
    VSF_SPI_LSB_FIRST               = 0x01ul << 1,  //! \~english Least Significant Bit (LSB) first \~chinese 最低有效位（LSB）优先

    // Clock polarity control (bit 2)
    VSF_SPI_CPOL_LOW                = 0x00ul << 2,  //! \~english Clock polarity: idle state is low \~chinese 时钟极性：空闲状态为低电平
    VSF_SPI_CPOL_HIGH               = 0x01ul << 2,  //! \~english Clock polarity: idle state is high \~chinese 时钟极性：空闲状态为高电平

    // Clock phase control (bit 3)
    VSF_SPI_CPHA_LOW                = 0x00ul << 3,  //! \~english Clock phase: sample on first edge \~chinese 时钟相位：第一个边沿采样
    VSF_SPI_CPHA_HIGH               = 0x01ul << 3,  //! \~english Clock phase: sample on second edge \~chinese 时钟相位：第二个边沿采样

    VSF_SPI_MODE_0                  = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_LOW,     //! \~english Mode 0: CPOL=0 (idle low), CPHA=0 (sample on first edge) \~chinese 模式 0：CPOL=0（空闲低），CPHA=0（第一个边沿采样）
    VSF_SPI_MODE_1                  = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_HIGH,    //! \~english Mode 1: CPOL=0 (idle low), CPHA=1 (sample on second edge) \~chinese 模式 1：CPOL=0（空闲低），CPHA=1（第二个边沿采样）
    VSF_SPI_MODE_2                  = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_LOW,     //! \~english Mode 2: CPOL=1 (idle high), CPHA=0 (sample on first edge) \~chinese 模式 2：CPOL=1（空闲高），CPHA=0（第一个边沿采样）
    VSF_SPI_MODE_3                  = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_HIGH,    //! \~english Mode 3: CPOL=1 (idle high), CPHA=1 (sample on second edge) \~chinese 模式 3：CPOL=1（空闲高），CPHA=1（第二个边沿采样）

    // Chip select control (bit 4)
    VSF_SPI_CS_SOFTWARE_MODE        = 0x00ul << 4,  //! \~english Software controlled chip select \~chinese 软件控制片选
    VSF_SPI_CS_HARDWARE_MODE        = 0x01ul << 4,  //! \~english Hardware controlled chip select \~chinese 硬件控制片选

    // Data size control (bits 8-15)
    VSF_SPI_DATASIZE_8              = 0x00ul << 8,  //! \~english 8-bit data transfer size \~chinese 8 位数据传输大小
    VSF_SPI_DATASIZE_16             = 0x01ul << 8,  //! \~english 16-bit data transfer size \~chinese 16 位数据传输大小
    VSF_SPI_DATASIZE_32             = 0x02ul << 8,  //! \~english 32-bit data transfer size \~chinese 32 位数据传输大小
    /*
    //! \~english Optional data size, if the hardware supports more data bits, we can define it inside the specific driver
    //! \~chinese 可选数据位大小，如果硬件支持更多数据位，我们可以在特定驱动里定义它
    VSF_SPI_DATASIZE_VALUE_OFFSET   = 1,
    VSF_SPI_DATASIZE_BIT_OFFSET     = 8,
    VSF_SPI_DATASIZE_4              = ( 4ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_5              = ( 5ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_6              = ( 6ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_7              = ( 7ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_8              = ( 8ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_9              = ( 9ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_10             = (10ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_11             = (11ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_12             = (12ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_13             = (13ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_14             = (14ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_15             = (15ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_16             = (16ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_17             = (17ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_18             = (18ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_19             = (19ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_20             = (20ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_21             = (21ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_22             = (22ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_23             = (23ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_24             = (24ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_25             = (25ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_26             = (26ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_27             = (27ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_28             = (28ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_29             = (29ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_30             = (30ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_31             = (31ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_32             = (32ul - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET,
    #define VSF_SPI_DATASIZE_BIT_OFFSET     VSF_SPI_DATASIZE_BIT_OFFSET
    #define VSF_SPI_DATASIZE_VALUE_OFFSET   VSF_SPI_DATASIZE_VALUE_OFFSET
    #define VSF_SPI_DATASIZE_4              VSF_SPI_DATASIZE_4
    #define VSF_SPI_DATASIZE_5              VSF_SPI_DATASIZE_5
    #define VSF_SPI_DATASIZE_6              VSF_SPI_DATASIZE_6
    #define VSF_SPI_DATASIZE_7              VSF_SPI_DATASIZE_7
    #define VSF_SPI_DATASIZE_8              VSF_SPI_DATASIZE_8
    #define VSF_SPI_DATASIZE_9              VSF_SPI_DATASIZE_9
    #define VSF_SPI_DATASIZE_10             VSF_SPI_DATASIZE_10
    #define VSF_SPI_DATASIZE_11             VSF_SPI_DATASIZE_11
    #define VSF_SPI_DATASIZE_12             VSF_SPI_DATASIZE_12
    #define VSF_SPI_DATASIZE_13             VSF_SPI_DATASIZE_13
    #define VSF_SPI_DATASIZE_14             VSF_SPI_DATASIZE_14
    #define VSF_SPI_DATASIZE_15             VSF_SPI_DATASIZE_15
    #define VSF_SPI_DATASIZE_16             VSF_SPI_DATASIZE_16
    #define VSF_SPI_DATASIZE_17             VSF_SPI_DATASIZE_17
    #define VSF_SPI_DATASIZE_18             VSF_SPI_DATASIZE_18
    #define VSF_SPI_DATASIZE_19             VSF_SPI_DATASIZE_19
    #define VSF_SPI_DATASIZE_20             VSF_SPI_DATASIZE_20
    #define VSF_SPI_DATASIZE_21             VSF_SPI_DATASIZE_21
    #define VSF_SPI_DATASIZE_22             VSF_SPI_DATASIZE_22
    #define VSF_SPI_DATASIZE_23             VSF_SPI_DATASIZE_23
    #define VSF_SPI_DATASIZE_24             VSF_SPI_DATASIZE_24
    #define VSF_SPI_DATASIZE_25             VSF_SPI_DATASIZE_25
    #define VSF_SPI_DATASIZE_26             VSF_SPI_DATASIZE_26
    #define VSF_SPI_DATASIZE_27             VSF_SPI_DATASIZE_27
    #define VSF_SPI_DATASIZE_28             VSF_SPI_DATASIZE_28
    #define VSF_SPI_DATASIZE_29             VSF_SPI_DATASIZE_29
    #define VSF_SPI_DATASIZE_30             VSF_SPI_DATASIZE_30
    #define VSF_SPI_DATASIZE_31             VSF_SPI_DATASIZE_31
    #define VSF_SPI_DATASIZE_32             VSF_SPI_DATASIZE_32
    #define VSF_SPI_DATASIZE_MASK          (  VSF_SPI_DATASIZE_4   \
                                            | VSF_SPI_DATASIZE_5   \
                                            | VSF_SPI_DATASIZE_6   \
                                            | VSF_SPI_DATASIZE_7   \
                                            | VSF_SPI_DATASIZE_8   \
                                            | VSF_SPI_DATASIZE_9   \
                                            | VSF_SPI_DATASIZE_10  \
                                            | VSF_SPI_DATASIZE_11  \
                                            | VSF_SPI_DATASIZE_12  \
                                            | VSF_SPI_DATASIZE_13  \
                                            | VSF_SPI_DATASIZE_14  \
                                            | VSF_SPI_DATASIZE_15  \
                                            | VSF_SPI_DATASIZE_16  \
                                            | VSF_SPI_DATASIZE_17  \
                                            | VSF_SPI_DATASIZE_18  \
                                            | VSF_SPI_DATASIZE_19  \
                                            | VSF_SPI_DATASIZE_20  \
                                            | VSF_SPI_DATASIZE_21  \
                                            | VSF_SPI_DATASIZE_22  \
                                            | VSF_SPI_DATASIZE_23  \
                                            | VSF_SPI_DATASIZE_24  \
                                            | VSF_SPI_DATASIZE_25  \
                                            | VSF_SPI_DATASIZE_26  \
                                            | VSF_SPI_DATASIZE_27  \
                                            | VSF_SPI_DATASIZE_28  \
                                            | VSF_SPI_DATASIZE_29  \
                                            | VSF_SPI_DATASIZE_30  \
                                            | VSF_SPI_DATASIZE_31  \
                                            | VSF_SPI_DATASIZE_32 )
    */

    /*
    //! \~english Optional data line, if the hardware supports more data lines, we can define it inside the specific driver
    //! \~chinese 可选数据线，如果硬件支持更多数据线，我们可以在特定驱动里定义它
    VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX    = (0x00ul << 16),    // 2 line, full-duplex, standard spi
    VSF_SPI_DATALINE_2_LINE_RX_ONLY        = (0x01ul << 16),    // 2 line, but rx only
    VSF_SPI_DATALINE_2_LINE_TX_ONLY        = (0x02ul << 16),    // 2 line, but tx only
    VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX    = (0x03ul << 16),    // 1 line, half-duplex
    VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX    = (0x04ul << 16),    // 2 line, half-duplex, dual spi
    VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX    = (0x05ul << 16),    // 4 line, half-duplex, quad spi
    VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX    = (0x06ul << 16),    // 8 line, half-duplex, octal spi
    #define VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX     VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX
    #define VSF_SPI_DATALINE_2_LINE_RX_ONLY         VSF_SPI_DATALINE_2_LINE_RX_ONLY
    #define VSF_SPI_DATALINE_2_LINE_TX_ONLY         VSF_SPI_DATALINE_2_LINE_TX_ONLY
    #define VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX     VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX
    #define VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX     VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX
    #define VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX     VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX
    #define VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX     VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX
    #define VSF_SPI_DATALINE_MASK                  (  VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX \
                                                    | VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX \
                                                    | VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX \
                                                    | VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX \
                                                    | VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX)
    */

    /*
    //! \~english Optional mode, if the hardware supports more mode like TI mode etc, we can define it inside the specific driver
    //! \~chinese 可选模式，如果硬件支持更多的模式，例如 TI 模式等，我们可以在特定驱动里定义它
    VSF_SPI_MOTOROLA_MODE       = (0x00ul << 20),    //! \~english Motorola SPI: Standard 4-wire SPI protocol \~chinese 摩托罗拉 SPI：标准 4 线 SPI 协议
    VSF_SPI_TI_MODE             = (0x01ul << 20),    //! \~english TI SSI: Chip select timing differs from Motorola SPI \~chinese TI SSI：片选时序与摩托罗拉 SPI 不同
    #define VSF_SPI_MOTOROLA_MODE       VSF_SPI_MOTOROLA_MODE
    #define VSF_SPI_TI_MODE             VSF_SPI_TI_MODE
    #define VSF_SPI_MOTOROLA_TI_MASK    (VSF_SPI_MOTOROLA_MODE | VSF_SPI_TI_MODE)
    */

    /*
    //! \~english
    // @brief Optional CRC calculation support for data integrity
    // @note Hardware CRC calculation is platform-dependent
    // @note When enabled, CRC is automatically calculated and verified
    //! \~chinese
    // @brief 可选的 CRC 数据完整性校验支持
    // @note 硬件 CRC 计算依赖于平台
    // @note 当使能时，CRC 将自动计算和验证
    VSF_SPI_CRC_DISABLED        = (0x00ul << 21),    //! \~english CRC calculation disabled (no integrity check) \~chinese CRC 计算禁用（无完整性检查）
    VSF_SPI_CRC_ENABLED         = (0x01ul << 21),    //! \~english CRC calculation enabled (hardware computes and verifies) \~chinese CRC 计算使能（硬件计算和验证）
    #define VSF_SPI_CRC_DISABLED        VSF_SPI_CRC_DISABLED
    #define VSF_SPI_CRC_ENABLED         VSF_SPI_CRC_ENABLED
    #define VSF_SPI_CRC_MASK            (VSF_SPI_CRC_DISABLED | VSF_SPI_CRC_ENABLED)
    */

    /*
    //! \~english Optional prescaler, if the hardware supports prescaler, we can define it inside the specific driver
    //! \~chinese 可选 prescaler，如果硬件支持 prescaler，我们可以在特定驱动里定义它
    VSF_SPI_CLOCK_PRESCALER_2    = (0x00ul << 22),    //! \~english Divide system clock by 2 (highest frequency) \~chinese 系统时钟除以 2（最高频率）
    VSF_SPI_CLOCK_PRESCALER_4    = (0x01ul << 22),    //! \~english Divide system clock by 4 \~chinese 系统时钟除以 4
    VSF_SPI_CLOCK_PRESCALER_8    = (0x02ul << 22),    //! \~english Divide system clock by 8 \~chinese 系统时钟除以 8
    VSF_SPI_CLOCK_PRESCALER_16   = (0x03ul << 22),    //! \~english Divide system clock by 16 \~chinese 系统时钟除以 16
    VSF_SPI_CLOCK_PRESCALER_32   = (0x04ul << 22),    //! \~english Divide system clock by 32 \~chinese 系统时钟除以 32
    VSF_SPI_CLOCK_PRESCALER_64   = (0x05ul << 22),    //! \~english Divide system clock by 64 \~chinese 系统时钟除以 64
    VSF_SPI_CLOCK_PRESCALER_128  = (0x06ul << 22),    //! \~english Divide system clock by 128 \~chinese 系统时钟除以 128
    VSF_SPI_CLOCK_PRESCALER_256  = (0x07ul << 22),    //! \~english Divide system clock by 256 (lowest frequency) \~chinese 系统时钟除以 256（最低频率）
    #define VSF_SPI_CLOCK_PRESCALER_2     VSF_SPI_CLOCK_PRESCALER_2
    #define VSF_SPI_CLOCK_PRESCALER_4     VSF_SPI_CLOCK_PRESCALER_4
    #define VSF_SPI_CLOCK_PRESCALER_8     VSF_SPI_CLOCK_PRESCALER_8
    #define VSF_SPI_CLOCK_PRESCALER_16    VSF_SPI_CLOCK_PRESCALER_16
    #define VSF_SPI_CLOCK_PRESCALER_32    VSF_SPI_CLOCK_PRESCALER_32
    #define VSF_SPI_CLOCK_PRESCALER_64    VSF_SPI_CLOCK_PRESCALER_64
    #define VSF_SPI_CLOCK_PRESCALER_128   VSF_SPI_CLOCK_PRESCALER_128
    #define VSF_SPI_CLOCK_PRESCALER_256   VSF_SPI_CLOCK_PRESCALER_256
    #define VSF_SPI_CLOCK_PRESCALER_MASK  (  VSF_SPI_CLOCK_PRESCALER_2    \
                                            | VSF_SPI_CLOCK_PRESCALER_4    \
                                            | VSF_SPI_CLOCK_PRESCALER_8    \
                                            | VSF_SPI_CLOCK_PRESCALER_16   \
                                            | VSF_SPI_CLOCK_PRESCALER_32   \
                                            | VSF_SPI_CLOCK_PRESCALER_64   \
                                            | VSF_SPI_CLOCK_PRESCALER_128  \
                                            | VSF_SPI_CLOCK_PRESCALER_256)
    */
} vsf_spi_mode_t;
#endif

/**
 * \~english
 * @brief SPI mode completion, used to simplify the definition of SPI mode.
 * \~chinese
 * @brief SPI 模式补全，用来简化 SPI 模式的定义
 */
enum {
    /**
     * \~english
     * @brief SPI direction mode mask for master/slave selection
     * @note Combines VSF_SPI_MASTER and VSF_SPI_SLAVE
     * \~chinese
     * @brief SPI 主从模式选择掩码
     * @note 包含 VSF_SPI_MASTER 和 VSF_SPI_SLAVE
     */
    VSF_SPI_DIR_MODE_MASK           = VSF_SPI_MASTER
                                    | VSF_SPI_SLAVE,

    /**
     * \~english
     * @brief SPI bit order mask for MSB/LSB first selection
     * @note Combines VSF_SPI_MSB_FIRST and VSF_SPI_LSB_FIRST
     * \~chinese
     * @brief SPI 位顺序选择掩码
     * @note 包含 VSF_SPI_MSB_FIRST 和 VSF_SPI_LSB_FIRST
     */
    VSF_SPI_BIT_ORDER_MASK          = VSF_SPI_MSB_FIRST
                                    | VSF_SPI_LSB_FIRST,

    /**
     * \~english
     * @brief SPI mode mask for clock polarity and phase configuration
     * @note Combines all four modes (0-3) with different CPOL/CPHA combinations
     * \~chinese
     * @brief SPI 时钟极性和相位配置掩码
     * @note 包含全部四种模式（0-3）的不同 CPOL/CPHA 组合
     */
    VSF_SPI_MODE_MASK               = VSF_SPI_MODE_0
                                    | VSF_SPI_MODE_1
                                    | VSF_SPI_MODE_2
                                    | VSF_SPI_MODE_3,

    /**
     * \~english Some hardware supports more chip select mode, can be redefined inside the specific driver.
     * \~chinese 一些硬件支持更多的芯片选择模式，我们可以在具体的驱动里重新定义它.
     */
#ifndef VSF_SPI_CS_MODE_MASK
    VSF_SPI_CS_MODE_MASK            = VSF_SPI_CS_SOFTWARE_MODE
                                    | VSF_SPI_CS_HARDWARE_MODE
    /**
     * \~english Some hardware supports cs input mode, can be redefined inside the specific driver.
     * \~chinese 一些硬件支持 cs 输入模式，我们可以在具体的驱动里重新定义它.
     */
#ifdef VSF_SPI_CS_HARDWARE_INPUT_MODE
                                    | VSF_SPI_CS_HARDWARE_INPUT_MODE
#endif
                                    ,
#endif

    /**
     * \~english Some hardware supports more data bits, default defined as 8, 16, 32 bits.
     * \~chinese 一些硬件支持更多的数据位，默认定义 8, 16, 32 位.
     */
#ifndef VSF_SPI_DATASIZE_MASK
    VSF_SPI_DATASIZE_MASK           = VSF_SPI_DATASIZE_8
                                    | VSF_SPI_DATASIZE_16
                                    | VSF_SPI_DATASIZE_32,
#endif

#ifndef VSF_SPI_MODE_ALL_BITS_MASK
    /**
     * \~english SPI Mode All Bits Mask.
     * \~chinese SPI 模式所有位掩码.
     */
    VSF_SPI_MODE_ALL_BITS_MASK      = VSF_SPI_DIR_MODE_MASK
                                    | VSF_SPI_BIT_ORDER_MASK
                                    | VSF_SPI_MODE_MASK
                                    | VSF_SPI_CS_MODE_MASK
                                    | VSF_SPI_DATASIZE_MASK
#ifdef VSF_SPI_DATALINE_MASK
                                    | VSF_SPI_DATALINE_MASK
#endif
#ifdef VSF_SPI_MOTOROLA_TI_MASK
                                    | VSF_SPI_MOTOROLA_TI_MASK
#endif
#ifdef VSF_SPI_CRC_MASK
                                    | VSF_SPI_CRC_MASK
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_MASK
                                    | VSF_SPI_CLOCK_PRESCALER_MASK
#endif
#endif
};

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/**
 * \~english
 * @brief Predefined VSF SPI interrupt masks that can be reimplemented in specific HAL drivers.
 * Optional interrupts need to provide one or more enum options and a macro with the same name
 * to determine if they are supported at runtime. If multiple options are supported, a corresponding
 * MASK option should be provided to allow switching between modes at runtime.
 * \~chinese
 * @brief 预定义的 VSF SPI 中断掩码，可以在具体的 HAL 驱动重新实现。
 * 可选中断需要提供一个或者多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果支持多个选项，那需要提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
typedef enum vsf_spi_irq_mask_t {
    VSF_SPI_IRQ_MASK_TX             = 0x01ul << 0,  //! \~english TX FIFO threshold interrupt (triggers when TX FIFO level is below threshold) \~chinese TX FIFO 阈值中断（当 TX FIFO 水平低于阈值时触发）
    VSF_SPI_IRQ_MASK_RX             = 0x01ul << 1,  //! \~english RX FIFO threshold interrupt (triggers when RX FIFO level is above threshold) \~chinese RX FIFO 阈值中断（当 RX FIFO 水平高于阈值时触发）
    VSF_SPI_IRQ_MASK_TX_CPL         = 0x01ul << 2,  //! \~english TX complete interrupt (triggers when all TX data has been sent) \~chinese TX 完成中断（当所有 TX 数据发送完成时触发）
    VSF_SPI_IRQ_MASK_CPL            = 0x01ul << 3,  //! \~english Transfer complete interrupt (triggers when both TX and RX are complete) \~chinese 传输完成中断（当 TX 和 RX 都完成时触发）
    VSF_SPI_IRQ_MASK_OVERFLOW_ERR   = 0x01ul << 4,  //! \~english Overflow error interrupt (triggers when RX FIFO overflows) \~chinese 溢出错误中断（当 RX FIFO 溢出时触发）
} vsf_spi_irq_mask_t;
#endif

/**
 * \~english @brief SPI IRQ mask completion, used to simplify the definition of SPI IRQ mask.
 * \~chinese @brief SPI 中断的补全，用来简化 SPI 中断的定义
 */
enum {
    VSF_SPI_IRQ_MASK_TX_FIFO_THRESHOLD  = VSF_SPI_IRQ_MASK_TX,  //! \~english Alias for VSF_SPI_IRQ_MASK_TX \~chinese VSF_SPI_IRQ_MASK_TX 的别名
    VSF_SPI_IRQ_MASK_RX_FIFO_THRESHOLD  = VSF_SPI_IRQ_MASK_RX,  //! \~english Alias for VSF_SPI_IRQ_MASK_RX \~chinese VSF_SPI_IRQ_MASK_RX 的别名

    /**
     * \~english For SPI transfer, if RX transfer is done, then TX is done too.
     * \~chinese 对于 SPI 传输，如果 RX 传输完成了，那么 TX 也肯定完成了。
     */
    VSF_SPI_IRQ_MASK_RX_CPL             = VSF_SPI_IRQ_MASK_CPL,

#ifndef VSF_SPI_IRQ_MASK_ERR
    /**
     * \~english Some hardware supports more different error interrupts, we can redefine it inside the specific driver.
     * \~chinese 一些硬件支持更多不同类型的错误中断，可以在特定的驱动中重新定义此中断掩码。
     */
    VSF_SPI_IRQ_MASK_ERR                = VSF_SPI_IRQ_MASK_OVERFLOW_ERR,
#endif


#ifndef VSF_SPI_IRQ_ALL_BITS_MASK
    /**
     * \~english Combined IRQ mask for all SPI interrupts.
     * \~chinese 所有 SPI 中断的组合掩码。
     */
    VSF_SPI_IRQ_ALL_BITS_MASK           = VSF_SPI_IRQ_MASK_TX
                                        | VSF_SPI_IRQ_MASK_RX
                                        | VSF_SPI_IRQ_MASK_TX_CPL
                                        | VSF_SPI_IRQ_MASK_CPL
                                        | VSF_SPI_IRQ_MASK_ERR,
#endif
};

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
/**
 * \~english
 * @brief Predefined VSF SPI status that can be reimplemented in specific HAL drivers.
 * Even if the hardware doesn't support these features, these status must be kept.
 * - is_busy: indicates if the SPI is currently busy
 * \~chinese
 * @brief 预定义的 VSF SPI 状态，可以在特定的 HAL 驱动中重新实现。
 * 即使硬件不支持这些功能，这些状态也必须保留：
 * - is_busy: 表示 SPI 当前是否忙于传输
 */
typedef struct vsf_spi_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;            //! \~english SPI busy status (1: busy with current transfer, 0: idle)
                                             //! \~chinese SPI 忙状态（1：当前正在传输，0：空闲）
        };
    };
} vsf_spi_status_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
/**
 * \~english
 * @brief Predefined VSF SPI capability that can be reimplemented in specific HAL drivers.
 * Even if the hardware doesn't support these features, these capabilities must be kept.
 * \~chinese
 * @brief 预定义的 VSF SPI 能力，可以在特定的 HAL 驱动中重新实现。
 * 即使硬件不支持这些功能，这些能力也必须保留。
 */
typedef struct vsf_spi_capability_t {
#if VSF_SPI_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)    //! \~english Inherit peripheral capabilities \~chinese 继承外设能力
#endif
    vsf_spi_irq_mask_t irq_mask;           //! \~english Supported interrupt masks for SPI operations \~chinese SPI 操作支持的中断掩码

    uint8_t support_hardware_cs : 1;        //! \~english Hardware chip select support (1: supported, 0: not supported) \~chinese 硬件片选支持（1：支持，0：不支持）
    uint8_t support_software_cs : 1;        //! \~english Software chip select support (1: supported, 0: not supported) \~chinese 软件片选支持（1：支持，0：不支持）
    uint8_t cs_count            : 6;        //! \~english Number of available chip select lines (0-63) \~chinese 可用片选线数量（0-63）

    uint32_t max_clock_hz;                  //! \~english Maximum supported SPI clock frequency in Hz \~chinese 支持的最大 SPI 时钟频率（Hz）
    uint32_t min_clock_hz;                  //! \~english Minimum supported SPI clock frequency in Hz \~chinese 支持的最小 SPI 时钟频率（Hz）
} vsf_spi_capability_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
/**
 * \~english
 * @brief SPI forward declaration.
 * \~chinese
 * @brief SPI 前置声明.
 */
typedef struct vsf_spi_t vsf_spi_t;
/**
 * \~english
 * @brief SPI interrupt handler type declaration.
 * @param[in,out] target_ptr: User defined target pointer passed to the handler
 * @param[in,out] spi_ptr: SPI instance pointer @ref vsf_spi_t
 * @param[in] irq_mask: Interrupt mask indicating which events occurred
 * \~chinese
 * @brief SPI 中断处理函数类型声明。
 * @param[in,out] target_ptr: 传递给处理函数的用户自定义目标指针
 * @param[in,out] spi_ptr: SPI 实例指针 @ref vsf_spi_t
 * @param[in] irq_mask: 指示发生哪些事件的中断掩码
 */
typedef void vsf_spi_isr_handler_t(void *target_ptr,
                                   vsf_spi_t *spi_ptr,
                                   vsf_spi_irq_mask_t irq_mask);
/**
 * \~english
 * @brief SPI interrupt service routine configuration structure
 * @note The interrupt will not be enabled if handler_fn is NULL
 * \~chinese
 * @brief SPI 中断服务程序配置结构体
 * @note 如果 handler_fn 为 NULL，中断将不会被启用
 */
typedef struct vsf_spi_isr_t {
    vsf_spi_isr_handler_t *handler_fn;  //! \~english Interrupt handler function (NULL to disable interrupts) \~chinese 中断处理函数（NULL 表示禁用中断）
    void                  *target_ptr;  //! \~english User context pointer passed to handler \~chinese 传递给处理函数的用户上下文指针
    vsf_arch_prio_t        prio;        //! \~english Hardware-specific interrupt priority \~chinese 硬件特定的中断优先级
} vsf_spi_isr_t;

/**
 * \~english
 * @brief Configuration structure for SPI.
 * \~chinese
 * @brief SPI 配置结构体。
 */
typedef struct vsf_spi_cfg_t {
    vsf_spi_mode_t   mode;          //! \~english SPI operating mode (master/slave, CPOL/CPHA, bit order, data size) \~chinese SPI 工作模式（主/从、CPOL/CPHA、位顺序、数据大小）
    uint32_t         clock_hz;      //! \~english SPI clock frequency in Hz (must be between min_clock_hz and max_clock_hz) \~chinese SPI 时钟频率（Hz）（必须在 min_clock_hz 和 max_clock_hz 之间）
    vsf_spi_isr_t    isr;           //! \~english Interrupt configuration (handler, target pointer, priority) \~chinese 中断配置（处理函数、目标指针、优先级）
    uint8_t          auto_cs_index; //! \~english Hardware CS pin index (0 to cs_count-1) for auto chip select, if supported \~chinese 用于自动片选的硬件 CS 引脚索引（0 到 cs_count-1），如果支持
} vsf_spi_cfg_t;
#endif


#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL == DISABLED
/**
 *
 * \~english
 * @brief Predefined VSF SPI control commands that can be reimplemented in specific HAL drivers.
 * @note Used as the 'ctrl' parameter (second parameter) in vsf_spi_ctrl(vsf_spi_t *spi_ptr, vsf_spi_ctrl_t ctrl, void *param) function
 * @note The 'param' parameter (third parameter) type depends on the specific control command
 * \~chinese
 * @brief 预定义的 VSF SPI 控制命令，可以在特定的 HAL 驱动中重新实现。
 * @note 作为 vsf_spi_ctrl(vsf_spi_t *spi_ptr, vsf_spi_ctrl_t ctrl, void *param) 函数中的 'ctrl' 参数（第二个参数）
 * @note 'param' 参数（第三个参数）的类型取决于具体的控制命令
 *
 * \~english
 * Optional control commands require one or more enumeration options and a macro with the same
 * name to determine if they are supported at runtime. If the feature supports more than
 * one option, it is recommended to provide the corresponding MASK option, so that the
 * user can check for supported features at compile-time.
 * \~chinese
 * 可选控制命令需要提供一个或多个枚举选项，还需要提供同名的宏，以便用户在运行时判断是否支持。
 * 如果该功能支持多个选项，建议提供相应的 MASK 选项，以便用户在编译时检查支持的功能。
 */
typedef enum vsf_spi_ctrl_t {
    __VSF_SPI_CTRL_DUMMY = 0,               //! \~english Dummy value for compilation \~chinese 编译占位值

    /*
    //! \~english Optional control commands for pause/resume functionality
    //! \~chinese 可选的暂停/恢复控制命令
    VSF_SPI_CTRL_REQUEST_PAUSE  = (0x1ul << 0),    //! \~english Request to pause SPI transfer \~chinese 请求暂停 SPI 传输
    VSF_SPI_CTRL_REQUEST_RESUME = (0x1ul << 1),    //! \~english Request to resume SPI transfer \~chinese 请求恢复 SPI 传输
    #define VSF_SPI_CTRL_REQUEST_PAUSE VSF_SPI_CTRL_REQUEST_PAUSE
    #define VSF_SPI_CTRL_REQUEST_RESUME VSF_SPI_CTRL_REQUEST_RESUME
    */

    /*
    //!
    //! \~english
    //! @brief Optional control commands for Quad SPI functionality (template-only, disabled in this header)
    //! @note QSPI transfer sequence typically includes: Command phase, Address phase, Dummy phase, and Data phase
    //! @note Data phase MUST be configured first, as its configuration directly determines which other phases can be used
    //! @note Some hardware may impose restrictions between data phase line mode and other phases' configurations
    //! @warning Once QSPI mode is enabled, standard SPI mode operations are typically incompatible and unsupported
    //! \~chinese
    //! @brief 可选的四线 SPI 功能控制命令（仅作为模板示例，默认在本头文件中禁用）
    //! @note QSPI 传输序列通常包括: 命令阶段、地址阶段、空闲阶段和数据阶段
    //! @note 必须首先配置数据阶段，因为其配置直接决定了可以使用哪些其他阶段
    //! @note 某些硬件可能在数据阶段线模式与其他阶段配置之间施加限制
    //! @warning 一旦启用 QSPI 模式，标准 SPI 模式操作通常不兼容且不受支持
    //!

    //! \~english Enable Quad SPI mode for enhanced data transfer (param: NULL)
    //! \~english Note: After enabling QSPI mode, standard SPI operations are typically no longer available
    //! \~chinese 启用四线 SPI 模式以增强数据传输 (参数: NULL)
    //! \~chinese 注意：启用 QSPI 模式后，标准 SPI 操作通常不再可用
    VSF_SPI_CTRL_QSPI_ENABLE                                = 0x00ul,
    //! \~english Disable Quad SPI mode and return to standard SPI operation (param: NULL)
    //! \~english Note: This restores compatibility with standard SPI operations
    //! \~chinese 禁用四线 SPI 模式并恢复到标准 SPI 操作 (参数: NULL)
    //! \~chinese 注意：这将恢复与标准 SPI 操作的兼容性
    VSF_SPI_CTRL_QSPI_DISABLE                               = 0x01ul,
    #define VSF_SPI_CTRL_QSPI_ENABLE                        VSF_SPI_CTRL_QSPI_ENABLE
    #define VSF_SPI_CTRL_QSPI_DISABLE                       VSF_SPI_CTRL_QSPI_DISABLE

    //!
    //! \~english
    //! @brief Optional control commands for Quad SPI command phase
    //! @note Command phase typically sends instruction codes to the target device
    //! @note IMPORTANT: Even if only command/address phases are used (no data phase),
    //!       you must still call vsf_spi_fifo_transfer() or vsf_spi_request_transfer()
    //!       with count=0 to execute the actual transaction
    //! \~chinese
    //! @brief 可选的四线 SPI 命令阶段操作控制命令
    //! @note 命令阶段通常向目标设备发送指令代码
    //! @note 重要: 即使仅使用命令/地址阶段（无数据阶段），仍然必须调用
    //!       vsf_spi_fifo_transfer() 或 vsf_spi_request_transfer()，
    //!       并设置 count=0 来执行实际的传输
    //!

    //! \~english Enable command phase in Quad SPI transfer (param: NULL)
    //! \~chinese 在四线 SPI 传输中启用命令阶段 (参数: NULL)
    VSF_SPI_CTRL_QSPI_CMD_PHASE_ENABLE                      = 0x02ul,
    //! \~english Disable command phase in Quad SPI transfer (param: NULL)
    //! \~chinese 在四线 SPI 传输中禁用命令阶段 (参数: NULL)
    VSF_SPI_CTRL_QSPI_CMD_PHASE_DISABLE                     = 0x03ul,
    //! \~english Set command phase size in bits (param: uint32_t *)
    //! \~chinese 设置命令阶段的位大小 (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_SIZE                    = 0x04ul,
    //! \~english Get command phase size in bits (param: uint32_t *)
    //! \~chinese 获取命令阶段的位大小 (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_SIZE                    = 0x05ul,
    //! \~english Set command phase line mode (param: uint32_t * pointing to one of:
    //!           VSF_SPI_CTRL_QSPI_CMD_SINGLE (if supported), VSF_SPI_CTRL_QSPI_CMD_DUAL (if supported),
    //!           or VSF_SPI_CTRL_QSPI_CMD_QUAD (if supported))
    //! \~chinese 设置命令阶段的线模式 (参数: uint32_t * 指向以下之一:
    //!           VSF_SPI_CTRL_QSPI_CMD_SINGLE (如果支持)、VSF_SPI_CTRL_QSPI_CMD_DUAL (如果支持)
    //!           或 VSF_SPI_CTRL_QSPI_CMD_QUAD (如果支持))
    VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_LINE_MODE               = 0x06ul,
    //! \~english Get command phase line mode (param: uint32_t * pointing to one of:
    //!           VSF_SPI_CTRL_QSPI_CMD_SINGLE (if supported), VSF_SPI_CTRL_QSPI_CMD_DUAL (if supported),
    //!           or VSF_SPI_CTRL_QSPI_CMD_QUAD (if supported))
    //! \~chinese 获取命令阶段的线模式 (参数: uint32_t * 指向以下之一:
    //!           VSF_SPI_CTRL_QSPI_CMD_SINGLE (如果支持)、VSF_SPI_CTRL_QSPI_CMD_DUAL (如果支持)
    //!           或 VSF_SPI_CTRL_QSPI_CMD_QUAD (如果支持))
    VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_LINE_MODE               = 0x07ul,
    //! \~english Set command value (param: uint8_t *)
    //! \~chinese 设置命令值 (参数: uint8_t *)
    VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_VALUE                   = 0x08ul,
    //! \~english Get command value (param: uint8_t *)
    //! \~chinese 获取命令值 (参数: uint8_t *)
    VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_VALUE                   = 0x09ul,
    #define VSF_SPI_CTRL_QSPI_CMD_PHASE_ENABLE              VSF_SPI_CTRL_QSPI_CMD_PHASE_ENABLE
    #define VSF_SPI_CTRL_QSPI_CMD_PHASE_DISABLE             VSF_SPI_CTRL_QSPI_CMD_PHASE_DISABLE
    #define VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_SIZE            VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_SIZE
    #define VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_SIZE            VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_SIZE
    #define VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_LINE_MODE       VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_LINE_MODE
    #define VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_LINE_MODE       VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_LINE_MODE
    #define VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_VALUE           VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_VALUE
    #define VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_VALUE           VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_VALUE

    //!
    //! \~english
    //! @brief Optional control commands for Quad SPI address phase operations
    //! @note Address phase typically specifies memory location in the target device
    //! \~chinese
    //! @brief 可选的四线 SPI 地址阶段操作控制命令
    //! @note 地址阶段通常指定目标设备中的存储位置
    //!

    //! \~english Enable address phase in Quad SPI transfer (param: NULL)
    //! \~chinese 在四线 SPI 传输中启用地址阶段 (参数: NULL)
    VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_ENABLE                  = 0x0Aul,
    //! \~english Disable address phase in Quad SPI transfer (param: NULL)
    //! \~chinese 在四线 SPI 传输中禁用地址阶段 (参数: NULL)
    VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_DISABLE                 = 0x0Bul,
    //! \~english Set address phase size (8/16/24/32 bits) (param: uint32_t *)
    //! \~chinese 设置地址阶段大小 (8/16/24/32 位) (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_SIZE                = 0x0Cul,
    //! \~english Get address phase size (param: uint32_t *)
    //! \~chinese 获取地址阶段大小 (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_SIZE                = 0x0Dul,
    //! \~english Set address phase line mode (param: uint32_t * pointing to one of:
    //!           VSF_SPI_CTRL_QSPI_ADDRESS_SINGLE (if supported), VSF_SPI_CTRL_QSPI_ADDRESS_DUAL (if supported),
    //!           or VSF_SPI_CTRL_QSPI_ADDRESS_QUAD (if supported))
    //! \~chinese 设置地址阶段的线模式 (参数: uint32_t * 指向以下之一:
    //!           VSF_SPI_CTRL_QSPI_ADDRESS_SINGLE (如果支持)、VSF_SPI_CTRL_QSPI_ADDRESS_DUAL (如果支持)
    //!           或 VSF_SPI_CTRL_QSPI_ADDRESS_QUAD (如果支持))
    VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_LINE_MODE           = 0x0Eul,

    //! \~english Get address phase line mode (param: uint32_t * to receive one of:
    //!           VSF_SPI_CTRL_QSPI_ADDRESS_SINGLE, VSF_SPI_CTRL_QSPI_ADDRESS_DUAL, or VSF_SPI_CTRL_QSPI_ADDRESS_QUAD)
    //! \~chinese 获取地址阶段线模式 (参数: uint32_t * 用于接收以下之一:
    //!           VSF_SPI_CTRL_QSPI_ADDRESS_SINGLE、VSF_SPI_CTRL_QSPI_ADDRESS_DUAL 或 VSF_SPI_CTRL_QSPI_ADDRESS_QUAD)
    VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_LINE_MODE           = 0x0Ful,
    //! \~english Set address value (param: uint32_t *)
    //! \~chinese 设置地址值 (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_ADDRESS_SET                           = 0x10ul,
    //! \~english Get address value (param: uint32_t *)
    //! \~chinese 获取地址值 (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_ADDRESS_GET                           = 0x11ul,
    #define VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_ENABLE          VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_ENABLE
    #define VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_DISABLE         VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_DISABLE
    #define VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_SIZE        VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_SIZE
    #define VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_SIZE        VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_SIZE
    #define VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_LINE_MODE   VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_LINE_MODE
    #define VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_LINE_MODE   VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_LINE_MODE
    #define VSF_SPI_CTRL_QSPI_ADDRESS_SET                   VSF_SPI_CTRL_QSPI_ADDRESS_SET
    #define VSF_SPI_CTRL_QSPI_ADDRESS_GET                   VSF_SPI_CTRL_QSPI_ADDRESS_GET

    //!
    //! \~english
    //! @brief Optional control commands for Quad SPI dummy phase operations
    //! @note Dummy phase provides wait states required by some devices before data transfer
    //! \~chinese
    //! @brief 可选的四线 SPI 空闲阶段操作控制命令
    //! @note 空闲阶段提供某些设备在数据传输前所需的等待状态
    //!

    //! \~english Enable dummy phase (waiting cycles) in Quad SPI transfer (param: uint8_t *)
    //! \~chinese 在四线 SPI 传输中启用空闲阶段（等待周期） (参数: uint8_t *)
    VSF_SPI_CTRL_QSPI_DUMMY_PHASE_ENABLE                    = 0x18ul,
    //! \~english Disable dummy phase in Quad SPI transfer (param: uint8_t *)
    //! \~chinese 在四线 SPI 传输中禁用空闲阶段 (参数: uint8_t *)
    VSF_SPI_CTRL_QSPI_DUMMY_PHASE_DISABLE                   = 0x19ul,
    //! \~english Set number of dummy cycles (param: uint8_t *)
    //! \~chinese 设置空闲周期数 (参数: uint8_t *)
    VSF_SPI_CTRL_QSPI_DUMMY_PHASE_SET_CYCLES                = 0x1Aul,
    //! \~english Get number of dummy cycles (param: uint8_t *)
    //! \~chinese 获取空闲周期数 (参数: uint8_t *)
    VSF_SPI_CTRL_QSPI_DUMMY_PHASE_GET_CYCLES                = 0x1Bul,
    #define VSF_SPI_CTRL_QSPI_DUMMY_PHASE_ENABLE            VSF_SPI_CTRL_QSPI_DUMMY_PHASE_ENABLE
    #define VSF_SPI_CTRL_QSPI_DUMMY_PHASE_DISABLE           VSF_SPI_CTRL_QSPI_DUMMY_PHASE_DISABLE
    #define VSF_SPI_CTRL_QSPI_DUMMY_PHASE_SET_CYCLES        VSF_SPI_CTRL_QSPI_DUMMY_PHASE_SET_CYCLES
    #define VSF_SPI_CTRL_QSPI_DUMMY_PHASE_GET_CYCLES        VSF_SPI_CTRL_QSPI_DUMMY_PHASE_GET_CYCLES

    //!
    //! \~english
    //! @brief Optional control commands for Quad SPI data phase operations
    //! @note Data phase is where actual data exchange (read/write) occurs
    //! @note This phase MUST be configured first, as its configuration directly determines valid options for other phases
    //! @note Hardware typically imposes restrictions between data phase line mode and other phases' configurations
    //! \~chinese
    //! @brief 可选的四线 SPI 数据阶段操作控制命令
    //! @note 数据阶段是实际数据交换（读/写）发生的阶段
    //! @note 必须首先配置此阶段，因为其配置直接决定了其他阶段的有效选项
    //! @note 硬件通常在数据阶段线模式与其他阶段配置之间施加限制
    //!

    //! \~english Enable data phase in Quad SPI transfer (param: NULL)
    //! \~chinese 在四线 SPI 传输中启用数据阶段 (参数: NULL)
    VSF_SPI_CTRL_QSPI_DATA_PHASE_ENABLE                     = 0x12ul,
    //! \~english Disable data phase in Quad SPI transfer (param: NULL)
    //! \~chinese 在四线 SPI 传输中禁用数据阶段 (参数: NULL)
    VSF_SPI_CTRL_QSPI_DATA_PHASE_DISABLE                    = 0x13ul,
    //! \~english Set data size (8/16/32 bits) (param: uint32_t *)
    //! \~chinese 设置数据大小 (8/16/32 位) (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_SIZE                   = 0x14ul,
    //! \~english Get data size (param: uint32_t *)
    //! \~chinese 获取数据大小 (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_SIZE                   = 0x15ul,

    //! \~english Set data phase line mode (param: uint32_t * pointing to one of:
    //!           VSF_SPI_CTRL_QSPI_DATA_PHASE_SINGLE (if supported), VSF_SPI_CTRL_QSPI_DATA_PHASE_DUAL (if supported),
    //!           or VSF_SPI_CTRL_QSPI_DATA_PHASE_QUAD (if supported))
    //! \~chinese 设置数据阶段线模式 (参数: uint32_t * 指向以下之一:
    //!           VSF_SPI_CTRL_QSPI_DATA_PHASE_SINGLE (如果支持)、VSF_SPI_CTRL_QSPI_DATA_PHASE_DUAL (如果支持)
    //!           或 VSF_SPI_CTRL_QSPI_DATA_PHASE_QUAD (如果支持))
    VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_LINE_MODE              = 0x16ul,

    //! \~english Get data phase line mode  (param: uint32_t * pointing to one of:
    //!           VSF_SPI_CTRL_QSPI_DATA_PHASE_SINGLE (if supported), VSF_SPI_CTRL_QSPI_DATA_PHASE_DUAL (if supported),
    //!           or VSF_SPI_CTRL_QSPI_DATA_PHASE_QUAD (if supported))
    //! \~chinese 获取数据阶段线模式(参数: uint32_t * 指向以下之一:
    //!           VSF_SPI_CTRL_QSPI_DATA_PHASE_SINGLE (如果支持)、VSF_SPI_CTRL_QSPI_DATA_PHASE_DUAL (如果支持)
    //!           或 VSF_SPI_CTRL_QSPI_DATA_PHASE_QUAD (如果支持))
    VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_LINE_MODE              = 0x17ul,
    #define VSF_SPI_CTRL_QSPI_DATA_PHASE_ENABLE             VSF_SPI_CTRL_QSPI_DATA_PHASE_ENABLE
    #define VSF_SPI_CTRL_QSPI_DATA_PHASE_DISABLE            VSF_SPI_CTRL_QSPI_DATA_PHASE_DISABLE
    #define VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_SIZE           VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_SIZE
    #define VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_SIZE           VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_SIZE
    #define VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_LINE_MODE      VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_LINE_MODE
    #define VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_LINE_MODE      VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_LINE_MODE

    //! \~english Optional control commands for Quad SPI transfer mode configuration
    //! \~chinese 可选的四线 SPI 传输模式配置控制命令
    //! \~english Set transfer mode (write/read/combined operations) (param: uint32_t *)
    //! \~chinese 设置传输模式（写入/读取/组合操作） (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_TRANSFER_SET_MODE                     = 0x1Cul,
    //! \~english Get current transfer mode (param: uint32_t *)
    //! \~chinese 获取当前传输模式 (参数: uint32_t *)
    VSF_SPI_CTRL_QSPI_TRANSFER_GET_MODE                     = 0x1Dul,
    #define VSF_SPI_CTRL_QSPI_TRANSFER_SET_MODE             VSF_SPI_CTRL_QSPI_TRANSFER_SET_MODE
    #define VSF_SPI_CTRL_QSPI_TRANSFER_GET_MODE             VSF_SPI_CTRL_QSPI_TRANSFER_GET_MODE
    */
} vsf_spi_ctrl_t;
#endif

/**
 * \~english
 * @brief SPI operation function pointer type, used for SPI Multi Class support
 * \~chinese
 * @brief SPI 操作函数指针类型，用于 SPI Multi Class 支持
 */
typedef struct vsf_spi_op_t {
/// @cond
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_SPI_APIS(vsf_spi)
} vsf_spi_op_t;

#if VSF_SPI_CFG_MULTI_CLASS == ENABLED
/**
 * \~english
 * @brief SPI instance structure, used for SPI Multi Class support, not needed in non Multi Class mode
 * \~chinese
 * @brief SPI 实例结构体，用于 SPI Multi Class 支持，在非 Multi Class 模式下不需要
 */
struct vsf_spi_t {
    const vsf_spi_op_t * op;            //! \~english Pointer to operation table \~chinese 指向操作表的指针
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a SPI instance
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[in] cfg_ptr: Pointer to SPI configuration structure @ref vsf_spi_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise returns error code
 * @note The configuration includes clock settings, transfer modes, and interrupt handlers
 * @note This must be called before any other SPI operations except for vsf_spi_capability()
 * \~chinese
 * @brief 初始化 SPI 实例
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @param[in] cfg_ptr: 指向配置结构体 @ref vsf_spi_cfg_t 的指针
 * @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码
 * @note 配置包括时钟设置、传输模式和中断处理程序
 * @note 除了 vsf_spi_capability() 之外，必须在任何其他 SPI 操作之前调用此函数
 */
extern vsf_err_t vsf_spi_init(vsf_spi_t *spi_ptr, vsf_spi_cfg_t *cfg_ptr);

/**
 * \~english
 * @brief Finalize (deinitialize) a SPI instance
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @note This function should be called when the SPI instance is no longer needed
 * @note Ensure all transfers are complete before calling this function
 * \~chinese
 * @brief 终止（反初始化）SPI 实例
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @note 当不再需要 SPI 实例时应调用此函数
 * @note 在调用此函数之前，请确保所有传输都已完成
 */
extern void vsf_spi_fini(vsf_spi_t *spi_ptr);

/**
 * \~english
 * @brief Enable a SPI instance for operation
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @return fsm_rt_t: fsm_rt_cpl if SPI was enabled immediately, fsm_rt_on_going if enabling is in progress
 * @note The instance must be initialized before enabling
 * \~chinese
 * @brief 启用 SPI 实例以进行操作
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @return fsm_rt_t: 如果 SPI 立即启用则返回 fsm_rt_cpl，如果启用正在进行中则返回 fsm_rt_on_going
 * @note 实例必须在启用前完成初始化
 */
extern fsm_rt_t vsf_spi_enable(vsf_spi_t *spi_ptr);

/**
 * \~english
 * @brief Disable a SPI instance from operation
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @return fsm_rt_t: fsm_rt_cpl if SPI was disabled immediately, fsm_rt_on_going if disabling is in progress
 * @note Any ongoing transfers should be completed or cancelled before disabling
 * \~chinese
 * @brief 禁用 SPI 实例
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @return fsm_rt_t: 如果 SPI 立即禁用则返回 fsm_rt_cpl，如果禁用正在进行中则返回 fsm_rt_on_going
 * @note 在禁用前应完成或取消所有正在进行的传输
 */
extern fsm_rt_t vsf_spi_disable(vsf_spi_t *spi_ptr);

/**
 * \~english
 * @brief Enable specific interrupts for a SPI instance
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[in] irq_mask: Interrupt mask(s) to enable from @ref vsf_spi_irq_mask_t
 * @note Clear any pending interrupts before enabling new ones
 * @note Multiple interrupts can be enabled by OR-ing the masks
 * \~chinese
 * @brief 启用 SPI 实例的特定中断
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @param[in] irq_mask: 要启用的中断掩码，来自 @ref vsf_spi_irq_mask_t
 * @note 在启用新中断之前，请清除所有待处理的中断
 * @note 可以通过按位或（OR）组合多个中断掩码来同时启用多个中断
 */
extern void vsf_spi_irq_enable(vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask);

/**
 * \~english
 * @brief Disable specific interrupts for a SPI instance
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[in] irq_mask: Interrupt mask(s) to disable from @ref vsf_spi_irq_mask_t
 * @note Multiple interrupts can be disabled by OR-ing the masks
 * \~chinese
 * @brief 禁用 SPI 实例的特定中断
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @param[in] irq_mask: 要禁用的中断掩码，来自 @ref vsf_spi_irq_mask_t
 * @note 可以通过按位或（OR）组合多个中断掩码来同时禁用多个中断
 */
extern void vsf_spi_irq_disable(vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask);

/**
 * \~english
 * @brief Activate (assert) a SPI chip select line.
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[in] index: Chip select line index (0 to cs_count-1)
 * @return vsf_err_t: VSF_ERR_NONE if successful, otherwise returns error code
 * @note Hardware chip select support must be available if using hardware mode
 * \~chinese
 * @brief 激活（通常为拉低）SPI 片选线。
 * @param[in,out] spi_ptr: 指向 SPI 实例结构体 @ref vsf_spi_t 的指针
 * @param[in] index: 片选线索引（0 到 cs_count-1）
 * @return vsf_err_t: 如果成功返回 VSF_ERR_NONE，否则返回错误码
 * @note 如果使用硬件模式，必须支持硬件片选（CS）控制
 * @note 片选的极性取决于具体设备配置，通常为低电平有效
 */
extern vsf_err_t vsf_spi_cs_active(vsf_spi_t *spi_ptr, uint_fast8_t index);

/**
 * \~english
 * @brief Deactivate (deassert) a SPI chip select line.
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[in] index: Chip select line index (0 to cs_count-1)
 * @return vsf_err_t: VSF_ERR_NONE if successful, otherwise returns error code
 * @note Hardware chip select support must be available if using hardware mode
 * \~chinese
 * @brief 停用（通常为拉高）SPI 片选线。
 * @param[in,out] spi_ptr: 指向 SPI 实例结构体 @ref vsf_spi_t 的指针
 * @param[in] index: 片选线索引（0 到 cs_count-1）
 * @return vsf_err_t: 如果成功返回 VSF_ERR_NONE，否则返回错误码
 * @note 如果使用硬件模式，必须支持硬件片选（CS）控制
 * @note 片选的极性取决于具体设备配置，通常为低电平有效
 */
extern vsf_err_t vsf_spi_cs_inactive(vsf_spi_t *spi_ptr, uint_fast8_t index);

/**
 * \~english
 * @brief Get the current status of a SPI instance
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @return vsf_spi_status_t: Current status flags of the SPI instance
 * @note Status includes busy flag and other peripheral status information
 * \~chinese
 * @brief 获取 SPI 实例的当前状态
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @return vsf_spi_status_t: SPI 实例的当前状态标志
 * @note 状态包括忙状态标志和其他外设状态信息
 */
extern vsf_spi_status_t vsf_spi_status(vsf_spi_t *spi_ptr);

/**
 * \~english
 * @brief Get the capabilities of a SPI instance
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @return vsf_spi_capability_t: Hardware capabilities of the SPI instance
 * @note Capabilities include supported features, modes, and limitations
 * \~chinese
 * @brief 获取 SPI 实例的能力
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @return vsf_spi_capability_t: SPI 实例的硬件能力
 * @note 能力包括支持的特性、模式和限制
 */
extern vsf_spi_capability_t vsf_spi_capability(vsf_spi_t *spi_ptr);

/**
 * \~english
 * @brief Perform FIFO-based data transfer on a SPI instance
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[in] out_buffer_ptr: Pointer to data to transmit (can be NULL for receive-only)
 * @param[inout] out_offset_ptr: Pointer to transmit buffer offset
 * @param[out] in_buffer_ptr: Pointer to buffer for received data (can be NULL for transmit-only)
 * @param[inout] in_offset_ptr: Pointer to receive buffer offset
 * @param[in] count: Number of data units to transfer (can be 0 for QSPI operations with only command/address phases)
 * @note Data unit size is determined by the configured data size in SPI mode
 * @note In master mode, out_buffer_ptr data is sent through MOSI pin, in_buffer_ptr receives from MISO pin
 * @note In slave mode, out_buffer_ptr data is sent through MISO pin, in_buffer_ptr receives from MOSI pin
 * @note When using QSPI after configuring with vsf_spi_ctrl(), the count parameter refers to data phase units only
 * @note Nullability rules: Exactly one of out_buffer_ptr or in_buffer_ptr may be NULL, or both may be non-NULL.
 *       - If out_buffer_ptr is NULL (receive-only), out_offset_ptr must be NULL.
 *       - If in_buffer_ptr is NULL (transmit-only), in_offset_ptr must be NULL.
 *       - If a buffer pointer is non-NULL, its corresponding offset pointer must also be non-NULL.
 *       - It is invalid for both out_buffer_ptr and in_buffer_ptr to be NULL simultaneously.
 * \~chinese
 * @brief 在 SPI 实例上执行基于 FIFO 的数据传输
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @param[in] out_buffer_ptr: 指向要发送数据的缓冲区指针（仅接收时可为 NULL）
 * @param[inout] out_offset_ptr: 指向发送缓冲区偏移的指针
 * @param[out] in_buffer_ptr: 指向用于接收数据的缓冲区指针（仅发送时可为 NULL）
 * @param[inout] in_offset_ptr: 指向接收缓冲区偏移的指针
 * @param[in] count: 要传输的数据单元数（对于仅有命令/地址阶段的 QSPI 操作可以为 0）
 * @note 数据单元大小由 SPI 模式中配置的数据大小决定
 * @note 在主机模式下，out_buffer_ptr 数据通过 MOSI 引脚发送，in_buffer_ptr 从 MISO 引脚接收
 * @note 在从机模式下，out_buffer_ptr 数据通过 MISO 引脚发送，in_buffer_ptr 从 MOSI 引脚接收
 * @note 使用 vsf_spi_ctrl() 配置 QSPI 后，count 参数仅指数据阶段的单元数
 * @note 空指针规则：out_buffer_ptr 与 in_buffer_ptr 不能同时为 NULL；
 *       - 若 out_buffer_ptr 为 NULL（仅接收），则 out_offset_ptr 必须为 NULL；
 *       - 若 in_buffer_ptr 为 NULL（仅读取），则 in_offset_ptr 必须为 NULL；
 *       - 若某个缓冲区指针非 NULL，其对应偏移指针也必须非 NULL。
 */
extern void vsf_spi_fifo_transfer(vsf_spi_t *spi_ptr,
                                  void *out_buffer_ptr, uint_fast32_t* out_offset_ptr,
                                  void *in_buffer_ptr, uint_fast32_t* in_offset_ptr,
                                  uint_fast32_t count);

/**
 * \~english
 * @brief Request a SPI data transfer operation
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[in] out_buffer_ptr: Pointer to data to transmit (NULL for receive-only)
 * @param[out] in_buffer_ptr: Pointer to buffer for received data (NULL for transmit-only)
 * @param[in] count: Number of data units to transfer (can be 0 for QSPI operations with only command/address phases)
 * @return vsf_err_t: VSF_ERR_NONE if transfer started successfully, or error code
 * @note This is an asynchronous operation, register for completion interrupts to be notified
 * @note If both out_buffer_ptr and in_buffer_ptr are not NULL, data is exchanged in full-duplex mode
 * @note In master mode, out_buffer_ptr data is sent through MOSI pin, in_buffer_ptr receives from MISO pin
 * @note In slave mode, out_buffer_ptr data is sent through MISO pin, in_buffer_ptr receives from MOSI pin
 * @note For QSPI operations after calling vsf_spi_ctrl(), this function will execute all configured phases
 *       (command, address, dummy, data). Count parameter refers only to data phase units.
 * @note Nullability rules: out_buffer_ptr and in_buffer_ptr must not be both NULL; if a buffer pointer is NULL,
 *       its corresponding offset pointer (if any in related APIs) must also be NULL; if a buffer pointer is
 *       non-NULL, its corresponding offset pointer must be non-NULL.
 * \~chinese
 * @brief 请求 SPI 数据传输操作
 * @param[in,out] spi_ptr: 指向 SPI 实例结构体 @ref vsf_spi_t 的指针
 * @param[in] out_buffer_ptr: 指向要发送数据的缓冲区指针（仅接收时为 NULL）
 * @param[out] in_buffer_ptr: 指向用于接收数据的缓冲区指针（仅发送时为 NULL）
 * @param[in] count: 要传输的数据单元数量（对于仅有命令/地址阶段的 QSPI 操作可以为 0）
 * @return vsf_err_t: 如果传输成功启动则返回 VSF_ERR_NONE，否则返回错误码
 * @note 这是一个异步操作，注册完成中断以获得通知
 * @note 如果 out_buffer_ptr 和 in_buffer_ptr 都不为 NULL，则以全双工模式交换数据
 * @note 在主机模式下，out_buffer_ptr 数据通过 MOSI 引脚发送，in_buffer_ptr 从 MISO 引脚接收
 * @note 在从机模式下，out_buffer_ptr 数据通过 MISO 引脚发送，in_buffer_ptr 从 MOSI 引脚接收
 * @note 对于调用 vsf_spi_ctrl() 后的 QSPI 操作，此函数将执行所有已配置的阶段（命令、地址、空闲、数据），
 *       count 参数仅指数据阶段的单元数。
 * @note 空指针规则：out_buffer_ptr 与 in_buffer_ptr 不能同时为 NULL；若某个缓冲区指针为 NULL，
 *       则其对应的偏移指针（如适用）也必须为 NULL；若某个缓冲区指针非 NULL，则其对应偏移指针必须非 NULL。
 */
extern vsf_err_t vsf_spi_request_transfer(vsf_spi_t *spi_ptr, void *out_buffer_ptr,
                                          void *in_buffer_ptr, uint_fast32_t count);

/**
 * \~english
 * @brief Cancel an ongoing SPI transfer operation
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @return vsf_err_t: VSF_ERR_NONE if cancellation was successful, or error code
 * @note SPI hardware may not be able to cancel immediately if busy
 * \~chinese
 * @brief 取消正在进行的 SPI 传输操作
 * @param[in,out] spi_ptr: 指向 SPI 实例结构体 @ref vsf_spi_t 的指针
 * @return vsf_err_t: 如果成功取消则返回 VSF_ERR_NONE，否则返回错误码
 * @note 如果硬件忙，可能无法立即取消传输
 */
extern vsf_err_t vsf_spi_cancel_transfer(vsf_spi_t *spi_ptr);

/**
 * \~english
 * @brief Get the number of data units transferred in the current or last SPI operation
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[out] sent_count: Pointer to store the number of data units transmitted (can be NULL if not needed)
 * @param[out] received_count: Pointer to store the number of data units received (can be NULL if not needed)
 * @note Data unit count is based on the configured data size (e.g., for 16-bit data size, count is in 16-bit units)
 * @note For ongoing transfers, returns the current progress; for completed transfers, returns final counts
 * \~chinese
 * @brief 获取当前或最后一次 SPI 操作中已传输的数据单元数
 * @param[in,out] spi_ptr: 指向结构体 @ref vsf_spi_t 的指针
 * @param[out] sent_count: 存储已发送数据单元数的指针（如果不需要可为 NULL）
 * @param[out] received_count: 存储已接收数据单元数的指针（如果不需要可为 NULL）
 * @note 数据单元计数基于配置的数据大小（例如，对于 16 位数据大小，计数以 16 位为单位）
 * @note 对于正在进行的传输，返回当前进度；对于已完成的传输，返回最终计数
 */
extern void vsf_spi_get_transferred_count(vsf_spi_t *spi_ptr, uint_fast32_t *sent_count, uint_fast32_t *received_count);

/**
 * \~english
 * @brief Execute a control command on the SPI instance
 * @param[in,out] spi_ptr: Pointer to SPI instance structure @ref vsf_spi_t
 * @param[in] ctrl: Control command from @ref vsf_spi_ctrl_t enumeration
 * @param[in] param: Command-specific parameter (can be NULL depending on command)
 * @return vsf_err_t: VSF_ERR_NONE if command executed successfully,
 *                    VSF_ERR_NOT_SUPPORT if command is not supported,
 *                    other error codes defined by vsf_err_t for specific failures
 * @note Available commands and their parameters are hardware-dependent
 * @note Some commands may not be supported on all hardware platforms
 * \~chinese
 * @brief 对 SPI 实例执行控制命令
 * @param[in,out] spi_ptr: 指向 SPI 实例结构体 @ref vsf_spi_t 的指针
 * @param[in] ctrl: 控制命令，取值来自 @ref vsf_spi_ctrl_t 枚举
 * @param[in] param: 命令专用参数（根据命令类型可为 NULL）
 * @return vsf_err_t: 命令执行成功返回 VSF_ERR_NONE，
 *                    命令不支持返回 VSF_ERR_NOT_SUPPORT，
 *                    其他特定失败返回 vsf_err_t 定义的错误码
 * @note 可用命令及其参数依赖于具体硬件
 * @note 某些命令可能并非所有硬件平台都支持
 */
extern vsf_err_t vsf_spi_ctrl(vsf_spi_t *spi_ptr, vsf_spi_ctrl_t ctrl, void *param);


/*============================ INLINE FUNCTIONS ==============================*/

#if VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS == DISABLED
/**
 * \~english
 * @brief Extract the number of data bits from SPI mode configuration
 * @param[in] mode: SPI mode value containing data size configuration
 * @return uint8_t: Number of data bits (4-32) if valid, 0 if mode is invalid
 * @note For hardware-specific implementations, data size is encoded in mode bits
 * @note The valid range depends on hardware capabilities and configuration
 * \~chinese
 * @brief 从 SPI 模式配置中提取数据位数
 * @param[in] mode: 包含数据大小配置的 SPI 模式值
 * @return uint8_t: 如果有效则返回数据位数（4-32），如果模式无效则返回 0
 * @note 对于特定硬件实现，数据大小编码在模式位中
 * @note 有效范围取决于硬件能力和配置
 */
#   if defined(VSF_SPI_DATASIZE_BIT_OFFSET) && defined(VSF_SPI_DATASIZE_VALUE_OFFSET)
static inline uint8_t vsf_spi_mode_to_data_bits(vsf_spi_mode_t mode)
{
    int bits = (mode & VSF_SPI_DATASIZE_MASK) >> VSF_SPI_DATASIZE_BIT_OFFSET;
    return bits + VSF_SPI_DATASIZE_VALUE_OFFSET;
}
#   else
static inline uint8_t vsf_spi_mode_to_data_bits(vsf_spi_mode_t mode)
{
    uint32_t m = mode & VSF_SPI_DATASIZE_MASK;
    switch(m) {
#ifdef VSF_SPI_DATASIZE_4
        case VSF_SPI_DATASIZE_4:
            return 4;
#endif
#ifdef VSF_SPI_DATASIZE_5
        case VSF_SPI_DATASIZE_5:
            return 5;
#endif
#ifdef VSF_SPI_DATASIZE_6
        case VSF_SPI_DATASIZE_6:
            return 6;
#endif
#ifdef VSF_SPI_DATASIZE_7
        case VSF_SPI_DATASIZE_7:
            return 7;
#endif
        case VSF_SPI_DATASIZE_8:
            return 8;
#ifdef VSF_SPI_DATASIZE_9
        case VSF_SPI_DATASIZE_9:
            return 9;
#endif
#ifdef VSF_SPI_DATASIZE_10
        case VSF_SPI_DATASIZE_10:
            return 10;
#endif
#ifdef VSF_SPI_DATASIZE_11
        case VSF_SPI_DATASIZE_11:
            return 11;
#endif
#ifdef VSF_SPI_DATASIZE_12
        case VSF_SPI_DATASIZE_12:
            return 12;
#endif
#ifdef VSF_SPI_DATASIZE_13
        case VSF_SPI_DATASIZE_13:
            return 13;
#endif
#ifdef VSF_SPI_DATASIZE_14
        case VSF_SPI_DATASIZE_14:
            return 14;
#endif
#ifdef VSF_SPI_DATASIZE_15
        case VSF_SPI_DATASIZE_15:
            return 15;
#endif
#ifdef VSF_SPI_DATASIZE_16
        case VSF_SPI_DATASIZE_16:
            return 16;
#endif
#ifdef VSF_SPI_DATASIZE_17
        case VSF_SPI_DATASIZE_17:
            return 17;
#endif
#ifdef VSF_SPI_DATASIZE_18
        case VSF_SPI_DATASIZE_18:
            return 18;
#endif
#ifdef VSF_SPI_DATASIZE_19
        case VSF_SPI_DATASIZE_19:
            return 19;
#endif
#ifdef VSF_SPI_DATASIZE_20
        case VSF_SPI_DATASIZE_20:
            return 20;
#endif
#ifdef VSF_SPI_DATASIZE_21
        case VSF_SPI_DATASIZE_21:
            return 21;
#endif
#ifdef VSF_SPI_DATASIZE_22
        case VSF_SPI_DATASIZE_22:
            return 22;
#endif
#ifdef VSF_SPI_DATASIZE_23
        case VSF_SPI_DATASIZE_23:
            return 23;
#endif
#ifdef VSF_SPI_DATASIZE_24
        case VSF_SPI_DATASIZE_24:
            return 24;
#endif
#ifdef VSF_SPI_DATASIZE_25
        case VSF_SPI_DATASIZE_25:
            return 25;
#endif
#ifdef VSF_SPI_DATASIZE_26
        case VSF_SPI_DATASIZE_26:
            return 26;
#endif
#ifdef VSF_SPI_DATASIZE_27
        case VSF_SPI_DATASIZE_27:
            return 27;
#endif
#ifdef VSF_SPI_DATASIZE_28
        case VSF_SPI_DATASIZE_28:
            return 28;
#endif
#ifdef VSF_SPI_DATASIZE_29
        case VSF_SPI_DATASIZE_29:
            return 29;
#endif
#ifdef VSF_SPI_DATASIZE_30
        case VSF_SPI_DATASIZE_30:
            return 30;
#endif
#ifdef VSF_SPI_DATASIZE_31
        case VSF_SPI_DATASIZE_31:
            return 31;
#endif
#ifdef VSF_SPI_DATASIZE_32
        case VSF_SPI_DATASIZE_32:
            return 32;
#endif
        default:
            return 0;
    }
}
#   endif
#endif

#if VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE == DISABLED
/**
 * \~english
 * @brief Convert number of data bits to corresponding SPI mode configuration
 * @param[in] data_bits: Number of data bits (4-32)
 * @return vsf_spi_mode_t: SPI mode with appropriate data size configuration
 * @note Returns 0 (invalid mode) if data_bits is not supported
 * @note The conversion method depends on hardware implementation
 * \~chinese
 * @brief 将数据位数转换为相应的 SPI 模式配置
 * @param[in] data_bits: 数据位数（4-32）
 * @return vsf_spi_mode_t: 具有适当数据大小配置的 SPI 模式
 * @note 如果不支持指定的数据位数，则返回 0（无效模式）
 * @note 转换方法取决于硬件实现
 */
#   if defined(VSF_SPI_DATASIZE_BIT_OFFSET) && defined(VSF_SPI_DATASIZE_VALUE_OFFSET)
static inline vsf_spi_mode_t vsf_spi_data_bits_to_mode(uint8_t data_bits)
{
    return (vsf_spi_mode_t)((data_bits - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET);
}
#   else
static inline vsf_spi_mode_t vsf_spi_data_bits_to_mode(uint8_t data_bits)
{
    switch (data_bits) {
#ifdef VSF_SPI_DATASIZE_4
    case 4:
        return VSF_SPI_DATASIZE_4;
#endif
#ifdef VSF_SPI_DATASIZE_5
    case 5:
        return VSF_SPI_DATASIZE_5;
#endif
#ifdef VSF_SPI_DATASIZE_6
    case 6:
        return VSF_SPI_DATASIZE_6;
#endif
#ifdef VSF_SPI_DATASIZE_7
    case 7:
        return VSF_SPI_DATASIZE_7;
#endif
    case 8:
        return VSF_SPI_DATASIZE_8;
#ifdef VSF_SPI_DATASIZE_9
    case 9:
        return VSF_SPI_DATASIZE_9;
#endif
#ifdef VSF_SPI_DATASIZE_10
    case 10:
        return VSF_SPI_DATASIZE_10;
#endif
#ifdef VSF_SPI_DATASIZE_11
    case 11:
        return VSF_SPI_DATASIZE_11;
#endif
#ifdef VSF_SPI_DATASIZE_12
    case 12:
        return VSF_SPI_DATASIZE_12;
#endif
#ifdef VSF_SPI_DATASIZE_13
    case 13:
        return VSF_SPI_DATASIZE_13;
#endif
#ifdef VSF_SPI_DATASIZE_14
    case 14:
        return VSF_SPI_DATASIZE_14;
#endif
#ifdef VSF_SPI_DATASIZE_15
    case 15:
        return VSF_SPI_DATASIZE_15;
#endif
#ifdef VSF_SPI_DATASIZE_16
    case 16:
        return VSF_SPI_DATASIZE_16;
#endif
#ifdef VSF_SPI_DATASIZE_17
    case 17:
        return VSF_SPI_DATASIZE_17;
#endif
#ifdef VSF_SPI_DATASIZE_18
    case 18:
        return VSF_SPI_DATASIZE_18;
#endif
#ifdef VSF_SPI_DATASIZE_19
    case 19:
        return VSF_SPI_DATASIZE_19;
#endif
#ifdef VSF_SPI_DATASIZE_20
    case 20:
        return VSF_SPI_DATASIZE_20;
#endif
#ifdef VSF_SPI_DATASIZE_21
    case 21:
        return VSF_SPI_DATASIZE_21;
#endif
#ifdef VSF_SPI_DATASIZE_22
    case 22:
        return VSF_SPI_DATASIZE_22;
#endif
#ifdef VSF_SPI_DATASIZE_23
    case 23:
        return VSF_SPI_DATASIZE_23;
#endif
#ifdef VSF_SPI_DATASIZE_24
    case 24:
        return VSF_SPI_DATASIZE_24;
#endif
#ifdef VSF_SPI_DATASIZE_25
    case 25:
        return VSF_SPI_DATASIZE_25;
#endif
#ifdef VSF_SPI_DATASIZE_26
    case 26:
        return VSF_SPI_DATASIZE_26;
#endif
#ifdef VSF_SPI_DATASIZE_27
    case 27:
        return VSF_SPI_DATASIZE_27;
#endif
#ifdef VSF_SPI_DATASIZE_28
    case 28:
        return VSF_SPI_DATASIZE_28;
#endif
#ifdef VSF_SPI_DATASIZE_29
    case 29:
        return VSF_SPI_DATASIZE_29;
#endif
#ifdef VSF_SPI_DATASIZE_30
    case 30:
        return VSF_SPI_DATASIZE_30;
#endif
#ifdef VSF_SPI_DATASIZE_31
    case 31:
        return VSF_SPI_DATASIZE_31;
#endif
#ifdef VSF_SPI_DATASIZE_32
    case 32:
        return VSF_SPI_DATASIZE_32;
#endif
    default:
        return (vsf_spi_mode_t)0;
    }
}
#   endif
#endif

#if VSF_SPI_CFG_REIMPLEMENT_MODE_TO_BYTES == DISABLED
/**
 * \~english
 * @brief Calculate the number of bytes needed for specified SPI mode
 * @param[in] mode: SPI mode containing data size configuration
 * @return uint8_t: Number of bytes (1, 2, or 4) needed to store one data unit
 * @note Returns 1 for 1-8 bits, 2 for 9-16 bits, 4 for 17-32 bits
 * @note Used for buffer size calculations and memory alignment
 * \~chinese
 * @brief 计算指定 SPI 模式所需的字节数
 * @param[in] mode: 包含数据大小配置的 SPI 模式
 * @return uint8_t: 存储一个数据单元所需的字节数（1、2 或 4）
 * @note 1-8 位返回 1，9-16 位返回 2，17-32 位返回 4
 * @note 用于缓冲区大小计算和内存对齐
 */
static inline uint8_t vsf_spi_mode_to_data_bytes(vsf_spi_mode_t mode)
{
    uint8_t bits = vsf_spi_mode_to_data_bits(mode);
    if (bits <= 8) {
        return 1;
    } else if (bits <= 16) {
        return 2;
    } else {
        return 4;
    }
}
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_SPI_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_spi_t                                  VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t)
#   define vsf_spi_init(__SPI, ...)                     VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_init)                 ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#   define vsf_spi_fini(__SPI)                          VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_fini)                 ((__vsf_spi_t *)(__SPI))
#   define vsf_spi_enable(__SPI)                        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_enable)               ((__vsf_spi_t *)(__SPI))
#   define vsf_spi_disable(__SPI)                       VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_disable)              ((__vsf_spi_t *)(__SPI))
#   define vsf_spi_irq_enable(__SPI, ...)               VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_irq_enable)           ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#   define vsf_spi_irq_disable(__SPI, ...)              VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_irq_disable)          ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#   define vsf_spi_status(__SPI)                        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_status)               ((__vsf_spi_t *)(__SPI))
#   define vsf_spi_capability(__SPI)                    VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_capability)           ((__vsf_spi_t *)(__SPI))
#   define vsf_spi_cs_active(__SPI, ...)                VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cs_active)            ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#   define vsf_spi_cs_inactive(__SPI, ...)              VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cs_inactive)          ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#   define vsf_spi_fifo_transfer(__SPI, ...)            VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_fifo_transfer)        ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#   define vsf_spi_request_transfer(__SPI, ...)         VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_request_transfer)     ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#   define vsf_spi_cancel_transfer(__SPI)               VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cancel_transfer)      ((__vsf_spi_t *)(__SPI))
#   define vsf_spi_get_transferred_count(__SPI, ...)    VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_get_transferred_count)((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#   define vsf_spi_ctrl(__SPI, ...)                     VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_ctrl)                 ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)

#   define vsf_qspi_t                                   vsf_spi_t
#   define vsf_qspi_mode_t                              vsf_spi_mode_t
#   define vsf_qspi_irq_mask_t                          vsf_spi_irq_mask_t
#   define vsf_qspi_status_t                            vsf_spi_status_t
#   define vsf_qspi_capability_t                        vsf_spi_capability_t
#   define vsf_qspi_isr_handler_t                       vsf_spi_isr_handler_t
#   define vsf_qspi_isr_t                               vsf_spi_isr_t
#   define vsf_qspi_cfg_t                               vsf_spi_cfg_t
#   define vsf_qspi_ctrl_t                              vsf_spi_ctrl_t

    /**
     * \~english
     * @brief Initialize a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __CFG_PTR: QSPI configuration structure pointer of type vsf_spi_cfg_t*, containing clock, mode, and interrupt settings
     * @return Returns error code of type vsf_err_t
     * @note Parameter meanings reference vsf_spi_init function
     * \~chinese
     * @brief 初始化 QSPI 实例
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __CFG_PTR: QSPI 配置结构体指针，类型为 vsf_spi_cfg_t*，包含时钟、模式和中断配置
     * @return 返回 vsf_err_t 类型错误码
     * @note 参数含义参考 vsf_spi_init 函数
     */
#   define vsf_qspi_init(__SPI, __CFG_PTR)                   vsf_spi_init((__vsf_spi_t *)(__SPI), (__CFG_PTR))

    /**
     * \~english
     * @brief Finalize (deinitialize) a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @return No return value
     * @note Parameter meanings reference vsf_spi_fini function
     * \~chinese
     * @brief 终止（反初始化）QSPI 实例
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @return 无返回值
     * @note 参数含义参考 vsf_spi_fini 函数
     */
#   define vsf_qspi_fini(__SPI)                              vsf_spi_fini((__vsf_spi_t *)(__SPI))

    /**
     * \~english
     * @brief Enable a QSPI instance for operation
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @return Returns status code of type fsm_rt_t
     * @note Parameter meanings reference vsf_spi_enable function
     * \~chinese
     * @brief 启用 QSPI 实例以进行操作
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @return 返回 fsm_rt_t 类型的状态码
     * @note 参数含义参考 vsf_spi_enable 函数
     */
#   define vsf_qspi_enable(__SPI)                            vsf_spi_enable((__vsf_spi_t *)(__SPI))

    /**
     * \~english
     * @brief Disable a QSPI instance from operation
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @return Returns status code of type fsm_rt_t
     * @note Parameter meanings reference vsf_spi_disable function
     * \~chinese
     * @brief 禁用 QSPI 实例
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @return 返回 fsm_rt_t 类型的状态码
     * @note 参数含义参考 vsf_spi_disable 函数
     */
#   define vsf_qspi_disable(__SPI)                           vsf_spi_disable((__vsf_spi_t *)(__SPI))

    /**
     * \~english
     * @brief Enable specific interrupts for a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __IRQ_MASK: Interrupt mask of type vsf_spi_irq_mask_t, indicating which interrupts to enable
     * @return No return value
     * @note Parameter meanings reference vsf_spi_irq_enable function
     * \~chinese
     * @brief 启用 QSPI 实例的特定中断
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __IRQ_MASK: 中断掩码，类型为 vsf_spi_irq_mask_t，指示要启用哪些中断
     * @return 无返回值
     * @note 参数含义参考 vsf_spi_irq_enable 函数
     */
#   define vsf_qspi_irq_enable(__SPI, __IRQ_MASK)            vsf_spi_irq_enable((__vsf_spi_t *)(__SPI), (__IRQ_MASK))

    /**
     * \~english
     * @brief Disable specific interrupts for a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __IRQ_MASK: Interrupt mask of type vsf_spi_irq_mask_t, indicating which interrupts to disable
     * @return No return value
     * @note Parameter meanings reference vsf_spi_irq_disable function
     * \~chinese
     * @brief 禁用 QSPI 实例的特定中断
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __IRQ_MASK: 中断掩码，类型为 vsf_spi_irq_mask_t，指示要禁用哪些中断
     * @return 无返回值
     * @note 参数含义参考 vsf_spi_irq_disable 函数
     */
#   define vsf_qspi_irq_disable(__SPI, __IRQ_MASK)           vsf_spi_irq_disable((__vsf_spi_t *)(__SPI), (__IRQ_MASK))

    /**
     * \~english
     * @brief Get the current status of a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @return Returns status information of type vsf_spi_status_t
     * @note Parameter meanings reference vsf_spi_status function
     * \~chinese
     * @brief 获取 QSPI 实例的当前状态
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @return 返回 vsf_spi_status_t 类型状态信息
     * @note 参数含义参考 vsf_spi_status 函数
     */
#   define vsf_qspi_status(__SPI)                            vsf_spi_status((__vsf_spi_t *)(__SPI))

    /**
     * \~english
     * @brief Get the capabilities of a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @return Returns capability information of type vsf_spi_capability_t
     * @note Parameter meanings reference vsf_spi_capability function
     * \~chinese
     * @brief 获取 QSPI 实例的能力
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @return 返回 vsf_spi_capability_t 类型能力信息
     * @note 参数含义参考 vsf_spi_capability 函数
     */
#   define vsf_qspi_capability(__SPI)                        vsf_spi_capability((__vsf_spi_t *)(__SPI))

    /**
     * \~english
     * @brief Perform FIFO write operation on a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __OUT_BUFFER_PTR: Pointer to output buffer containing data to be written
     * @param __OUT_OFFSET_PTR: Pointer to output buffer offset, indicating current write position
     * @param __COUNT: Number of data units to write
     * @return No return value
     * @note This macro sets in_buffer_ptr and in_offset_ptr to NULL to perform write-only operation
     * @note Parameter meanings reference vsf_spi_fifo_transfer function
     * \~chinese
     * @brief 在 QSPI 实例上执行 FIFO 写入操作
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __OUT_BUFFER_PTR: 输出缓冲区指针，包含要写入的数据
     * @param __OUT_OFFSET_PTR: 输出缓冲区偏移指针，指示当前写入位置
     * @param __COUNT: 要写入的数据单元数量
     * @return 无返回值
     * @note 此宏设置 in_buffer_ptr 和 in_offset_ptr 为 NULL 以执行只写操作
     * @note 参数含义参考 vsf_spi_fifo_transfer 函数
     */
#   define vsf_qspi_fifo_write(__SPI, __OUT_BUFFER_PTR, __OUT_OFFSET_PTR, __COUNT) \
                                                             vsf_spi_fifo_transfer((__vsf_spi_t *)(__SPI), (__OUT_BUFFER_PTR), (__OUT_OFFSET_PTR), NULL, NULL, (__COUNT))

    /**
     * \~english
     * @brief Perform FIFO read operation on a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __IN_BUFFER_PTR: Pointer to input buffer for storing read data
     * @param __IN_OFFSET_PTR: Pointer to input buffer offset, indicating current read position
     * @param __COUNT: Number of data units to read
     * @return No return value
     * @note This macro sets out_buffer_ptr and out_offset_ptr to NULL to perform read-only operation
     * @note Parameter meanings reference vsf_spi_fifo_transfer function
     * \~chinese
     * @brief 在 QSPI 实例上执行 FIFO 读取操作
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __IN_BUFFER_PTR: 输入缓冲区指针，用于存储读取的数据
     * @param __IN_OFFSET_PTR: 输入缓冲区偏移指针，指示当前读取位置
     * @param __COUNT: 要读取的数据单元数量
     * @return 无返回值
     * @note 此宏设置 out_buffer_ptr 和 out_offset_ptr 为 NULL 以执行只读操作
     * @note 参数含义参考 vsf_spi_fifo_transfer 函数
     */
#   define vsf_qspi_fifo_read(__SPI, __IN_BUFFER_PTR, __IN_OFFSET_PTR, __COUNT) \
                                                             vsf_spi_fifo_transfer((__vsf_spi_t *)(__SPI), NULL, NULL, (__IN_BUFFER_PTR), (__IN_OFFSET_PTR), (__COUNT))

    /**
     * \~english
     * @brief Request transmit transfer on a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __OUT_BUF: Pointer to output buffer containing data to be sent
     * @param __CNT: Number of data units to transmit
     * @return Returns error code of type vsf_err_t
     * @note This macro sets in_buffer_ptr to NULL to perform transmit-only operation
     * @note Parameter meanings reference vsf_spi_request_transfer function
     * \~chinese
     * @brief 请求 QSPI 实例执行发送传输
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __OUT_BUF: 输出缓冲区指针，包含要发送的数据
     * @param __CNT: 要发送的数据单元数量
     * @return 返回 vsf_err_t 类型错误码
     * @note 此宏设置 in_buffer_ptr 为 NULL 以执行只发送操作
     * @note 参数含义参考 vsf_spi_request_transfer 函数
     */
#   define vsf_qspi_request_tx(__SPI, __OUT_BUF, __CNT)      vsf_spi_request_transfer((__vsf_spi_t *)(__SPI), (__OUT_BUF), NULL, (__CNT))

    /**
     * \~english
     * @brief Request receive transfer on a QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __IN_BUF: Pointer to input buffer for storing received data
     * @param __CNT: Number of data units to receive
     * @return Returns error code of type vsf_err_t
     * @note This macro sets out_buffer_ptr to NULL to perform receive-only operation
     * @note Parameter meanings reference vsf_spi_request_transfer function
     * \~chinese
     * @brief 请求 QSPI 实例执行接收传输
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __IN_BUF: 输入缓冲区指针，用于存储接收的数据
     * @param __CNT: 要接收的数据单元数量
     * @return 返回 vsf_err_t 类型错误码
     * @note 此宏设置 out_buffer_ptr 为 NULL 以执行只接收操作
     * @note 参数含义参考 vsf_spi_request_transfer 函数
     */
#   define vsf_qspi_request_rx(__SPI, __IN_BUF, __CNT)       vsf_spi_request_transfer((__vsf_spi_t *)(__SPI), NULL, (__IN_BUF), (__CNT))

    /**
     * \~english
     * @brief Cancel an ongoing QSPI transfer
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @return Returns error code of type vsf_err_t
     * @note Parameter meanings reference vsf_spi_cancel_transfer function
     * \~chinese
     * @brief 取消正在进行的 QSPI 传输
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @return 返回 vsf_err_t 类型错误码
     * @note 参数含义参考 vsf_spi_cancel_transfer 函数
     */
#   define vsf_qspi_cancel_transfer(__SPI)                   vsf_spi_cancel_transfer((__vsf_spi_t *)(__SPI))

    /**
     * \~english
     * @brief Get the number of data units transferred in the current or last QSPI operation
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __TX_CNT: Pointer to store the number of transmitted data units
     * @param __RX_CNT: Pointer to store the number of received data units
     * @return No return value
     * @note Parameter meanings reference vsf_spi_get_transferred_count function
     * \~chinese
     * @brief 获取当前或最后一次 QSPI 操作中已传输的数据单元数
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __TX_CNT: 存储已发送数据单元数的指针，类型为 uint_fast32_t*
     * @param __RX_CNT: 存储已接收数据单元数的指针，类型为 uint_fast32_t*
     * @return 无返回值
     * @note 参数含义参考 vsf_spi_get_transferred_count 函数
     */
#   define vsf_qspi_get_transferred_count(__SPI, __TX_CNT, __RX_CNT) \
                                                             vsf_spi_get_transferred_count((__vsf_spi_t *)(__SPI), (__TX_CNT), (__RX_CNT))

    /**
     * \~english
     * @brief Execute a control command on the QSPI instance
     * @param __SPI: QSPI instance pointer, converted to __vsf_spi_t* type for underlying SPI function
     * @param __CTRL: Control command of type vsf_spi_ctrl_t, specifying the operation to perform
     * @param __PARAM: Command parameter pointer, type depends on the specific control command
     * @return Returns error code of type vsf_err_t
     * @note Parameter meanings reference vsf_spi_ctrl function
     * \~chinese
     * @brief 执行 QSPI 实例的控制命令
     * @param __SPI: QSPI 实例指针，转换为 __vsf_spi_t* 类型传递给底层 SPI 函数
     * @param __CTRL: 控制命令，类型为 vsf_spi_ctrl_t，指定要执行的操作
     * @param __PARAM: 命令参数指针，类型依赖于具体的控制命令
     * @return 返回 vsf_err_t 类型错误码
     * @note 参数含义参考 vsf_spi_ctrl 函数
     */
#   define vsf_qspi_ctrl(__SPI, __CTRL, __PARAM)             vsf_spi_ctrl((__vsf_spi_t *)(__SPI), (__CTRL), (__PARAM))
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_SPI_H__*/

