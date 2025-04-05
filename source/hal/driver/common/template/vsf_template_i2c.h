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

#ifndef __VSF_TEMPLATE_I2C_H__
#define __VSF_TEMPLATE_I2C_H__

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
 * @brief 默认启用多类支持，以获得最大可用性。
 */
#ifndef VSF_I2C_CFG_MULTI_CLASS
#   define VSF_I2C_CFG_MULTI_CLASS                  ENABLED
#endif

/**
 * \~english
 * @brief In specific hardware drivers, either VSF_HW_I2C_COUNT or
 * VSF_HW_I2C_MASK must be defined.
 * \~chinese
 * @brief 在特定硬件驱动中，必须定义 VSF_HW_I2C_COUNT 或
 * VSF_HW_I2C_MASK 其中之一。
 */
#if defined(VSF_HW_I2C_COUNT) && !defined(VSF_HW_I2C_MASK)
#   define VSF_HW_I2C_MASK                          VSF_HAL_COUNT_TO_MASK(VSF_HW_I2C_COUNT)
#endif

/**
 * \~english
 * @brief In specific hardware drivers, either VSF_HW_I2C_COUNT or
 * VSF_HW_I2C_MASK must be defined.
 * \~chinese
 * @brief 在特定硬件驱动中，必须定义 VSF_HW_I2C_COUNT 或
 * VSF_HW_I2C_MASK 其中之一。
 */
#if !defined(VSF_HW_I2C_COUNT) && defined(VSF_HW_I2C_MASK)
#   define VSF_HW_I2C_COUNT                         VSF_HAL_MASK_TO_COUNT(VSF_HW_I2C_MASK)
#endif

/**
 * \~english
 * @brief Macro VSF_I2C_CFG_PREFIX can be redefined in application code to specify
 * a prefix for directly calling a specific driver. This allows applications to
 * access different I2C implementations through unique function names.
 * \~chinese
 * @brief 可以在应用代码中重新定义宏 VSF_I2C_CFG_PREFIX，以指定直接调用
 * 特定驱动时使用的前缀。这使得应用程序可以通过唯一的函数名访问不同的
 * I2C 实现。
 */
#ifndef VSF_I2C_CFG_PREFIX
#   if VSF_I2C_CFG_MULTI_CLASS == ENABLED
#       define VSF_I2C_CFG_PREFIX                   vsf
#   elif defined(VSF_HW_I2C_COUNT) && (VSF_HW_I2C_COUNT != 0)
#       define VSF_I2C_CFG_PREFIX                   vsf_hw
#   else
#       define VSF_I2C_CFG_PREFIX                   vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_I2C_CFG_FUNCTION_RENAME to use original function
 * names (e.g., vsf_i2c_init()) without prefix.
 * \~chinese
 * @brief 禁用 VSF_I2C_CFG_FUNCTION_RENAME 以使用不带前缀的原始函数名
 * (例如 vsf_i2c_init())。
 */
#ifndef VSF_I2C_CFG_FUNCTION_RENAME
#   define VSF_I2C_CFG_FUNCTION_RENAME              ENABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE in specific hardware
 * drivers to redefine enum @ref vsf_i2c_mode_t.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE
 * 来重新定义枚举 @ref vsf_i2c_mode_t。
 */
#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE        DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD in specific hardware
 * drivers to redefine enum @ref vsf_i2c_cmd_t.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD
 * 来重新定义枚举 @ref vsf_i2c_cmd_t。
 */
#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD         DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK in specific hardware
 * drivers to redefine enum @ref vsf_i2c_irq_mask_t.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK
 * 来重新定义枚举 @ref vsf_i2c_irq_mask_t。
 */
#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK    DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS in specific hardware
 * drivers to redefine struct @ref vsf_i2c_status_t. This allows hardware-specific
 * status information to be included in the status structure.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS
 * 来重新定义结构体 @ref vsf_i2c_status_t。这允许在状态结构中包含
 * 特定硬件的状态信息。
 */
#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS      DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL in specific hardware
 * drivers to redefine enum @ref vsf_i2c_ctrl_t. This allows hardware-specific
 * control commands to be added to the control enumeration.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL
 * 来重新定义枚举 @ref vsf_i2c_ctrl_t。这允许在控制枚举中添加特定
 * 硬件的控制命令。
 */
#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL        DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG in specific hardware
 * drivers to redefine struct @ref vsf_i2c_cfg_t. This allows hardware-specific
 * configuration options to be added while maintaining compatibility with the
 * base structure.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG
 * 来重新定义结构体 @ref vsf_i2c_cfg_t。这允许在保持与基础结构兼容性
 * 的同时添加特定硬件的配置选项。
 */
#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_I2C_CFG_INHERIT_HAL_CAPABILITY in specific hardware
 * drivers to incorporate HAL capability structure into @ref vsf_i2c_capability_t.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_I2C_CFG_INHERIT_HAL_CAPABILITY
 * 来将 HAL 能力结构继承到 @ref vsf_i2c_capability_t 中。
 */
#ifndef VSF_I2C_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_I2C_CFG_INHERIT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief I2C API template, used to generate I2C type, specific prefix function declarations, etc.
 * \~chinese
 * @brief I2C API 模板，用于生成 I2C 类型、特定前缀的函数声明等。
 */
#define VSF_I2C_APIS(__PREFIX_NAME)                                                                                                                                             \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            i2c, init,                          VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)       \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 i2c, fini,                          VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                               \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, fsm_rt_t,             i2c, enable,                        VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                               \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, fsm_rt_t,             i2c, disable,                       VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                               \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 i2c, irq_enable,                    VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)  \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 i2c, irq_disable,                   VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)  \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_i2c_status_t,     i2c, status,                        VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                               \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_i2c_capability_t, i2c, capability,                    VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                               \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 i2c, master_fifo_transfer,          VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, uint16_t address,             \
        vsf_i2c_cmd_t cmd, uint_fast16_t count, uint8_t* buffer_ptr, vsf_i2c_cmd_t *cur_cmd_ptr, uint_fast16_t *offset_ptr)                                                     \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            i2c, master_request,                VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, uint16_t address,             \
        vsf_i2c_cmd_t cmd, uint_fast16_t count, uint8_t* buffer_ptr)                                                                                                            \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, uint_fast16_t,        i2c, master_get_transferred_count,  VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                               \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, uint_fast16_t,        i2c, slave_fifo_transfer,           VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, bool transmit_or_receive,     \
        uint_fast16_t count, uint8_t* buffer_ptr)                                                                                                                               \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            i2c, slave_request,                 VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, bool transmit_or_receive,     \
        uint_fast16_t count, uint8_t* buffer_ptr)                                                                                                                               \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, uint_fast16_t,        i2c, slave_get_transferred_count, VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                                 \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            i2c, ctrl,                          VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, vsf_i2c_ctrl_t ctrl, void* param)

/*============================ TYPES =========================================*/

/**
 * \~english
 * @brief Predefined VSF I2C modes that can be reimplemented in specific HAL drivers.
 * \~chinese
 * @brief 预定义的 VSF I2C 模式，可以在具体的 HAL 驱动中重新实现。
 *
 * \~english
 * If we want to add optional modes in the specific driver, we need to provide
 * one or more enum options, and a macro with the same name to determine if they
 * are supported at runtime. If the feature supports more than one option, it
 * also needs to provide the corresponding MASK option, so that the user can
 * switch to different modes at compile-time.
 * \~chinese
 * 在特定驱动中添加可选模式时，需要提供一个或多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果该特性支持多个选项，还需要提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
#if VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
typedef enum vsf_i2c_mode_t {
    /**
     * \~english
     * @brief Select master mode for I2C operation.
     * The device initiates and controls the I2C bus communication.
     * \~chinese
     * @brief 选择 I2C 主机模式。
     * 设备将初始化并控制 I2C 总线通信。
     */
    VSF_I2C_MODE_MASTER           = (0x1ul << 28),

    /**
     * \~english
     * @brief Select slave mode for I2C operation.
     * The device responds to I2C bus commands from master devices.
     * \~chinese
     * @brief 选择 I2C 从机模式。
     * 设备响应来自主机设备的 I2C 总线命令。
     */
    VSF_I2C_MODE_SLAVE            = (0x0ul << 28),

    /**
     * \~english
     * @brief Standard mode, up to 100 kbit/s.
     * The lowest I2C speed specification, compatible with most devices.
     * \~chinese
     * @brief 标准模式，最高 100 kbit/s。
     * 最低的 I2C 速度规格，与大多数设备兼容。
     */
    VSF_I2C_SPEED_STANDARD_MODE   = (0x0ul << 29),

    /**
     * \~english
     * @brief Fast mode, up to 400 kbit/s.
     * Commonly used medium speed I2C mode.
     * \~chinese
     * @brief 快速模式，最高 400 kbit/s。
     * 常用的中速 I2C 模式。
     */
    VSF_I2C_SPEED_FAST_MODE       = (0x1ul << 29),

    /**
     * \~english
     * @brief Fast mode plus, up to 1 Mbit/s.
     * Enhanced fast mode for higher throughput applications.
     * \~chinese
     * @brief 快速模式加强版，最高 1 Mbit/s。
     * 适用于更高吞吐量应用的增强型快速模式。
     */
    VSF_I2C_SPEED_FAST_MODE_PLUS  = (0x2ul << 29),

    /**
     * \~english
     * @brief High speed mode, up to 3.4 Mbit/s.
     * Highest specified I2C speed for performance-critical applications.
     * \~chinese
     * @brief 高速模式，最高 3.4 Mbit/s。
     * 性能关键型应用的最高指定 I2C 速度。
     */
    VSF_I2C_SPEED_HIGH_SPEED_MODE = (0x3ul << 29),

    /**
     * \~english
     * @brief 7-bit address mode.
     * Standard addressing mode supporting up to 128 unique devices.
     * \~chinese
     * @brief 7 位地址模式。
     * 标准寻址模式，支持最多 128 个唯一设备。
     */
    VSF_I2C_ADDR_7_BITS           = (0x0ul << 31),

    /**
     * \~english
     * @brief 10-bit address mode.
     * Extended addressing mode supporting up to 1024 unique devices.
     * \~chinese
     * @brief 10 位地址模式。
     * 扩展寻址模式，支持最多 1024 个唯一设备。
     */
    VSF_I2C_ADDR_10_BITS          = (0x1ul << 31),
} vsf_i2c_mode_t;
#endif

/**
 * \~english
 * @brief I2C mode masks for different operation modes and configurations.
 * These masks are used to extract or set specific mode bits from the I2C mode value.
 * \~chinese
 * @brief I2C 模式掩码，用于不同的操作模式和配置。
 * 这些掩码用于从 I2C 模式值中提取或设置特定的模式位。
 */
enum {
    /**
     * \~english
     * @brief Mask for master/slave mode selection.
     * Extracts VSF_I2C_MODE_MASTER and VSF_I2C_MODE_SLAVE bits.
     * \~chinese
     * @brief 主从模式选择掩码。
     * 用于提取 VSF_I2C_MODE_MASTER 和 VSF_I2C_MODE_SLAVE 位。
     */
    VSF_I2C_MODE_MASK             = VSF_I2C_MODE_MASTER |
                                    VSF_I2C_MODE_SLAVE,

    /**
     * \~english
     * @brief Mask for I2C speed mode configuration.
     * Extracts all speed mode bits (standard, fast, fast plus, high speed).
     * \~chinese
     * @brief I2C 速度模式配置掩码。
     * 用于提取所有速度模式位（标准、快速、快速加强、高速）。
     */
    VSF_I2C_SPEED_MASK            = VSF_I2C_SPEED_STANDARD_MODE |
                                    VSF_I2C_SPEED_FAST_MODE |
                                    VSF_I2C_SPEED_FAST_MODE_PLUS |
                                    VSF_I2C_SPEED_HIGH_SPEED_MODE,

    /**
     * \~english
     * @brief Mask for I2C address mode selection.
     * Extracts VSF_I2C_ADDR_7_BITS and VSF_I2C_ADDR_10_BITS bits.
     * \~chinese
     * @brief I2C 地址模式选择掩码。
     * 用于提取 VSF_I2C_ADDR_7_BITS 和 VSF_I2C_ADDR_10_BITS 位。
     */
    VSF_I2C_ADDR_MASK             = VSF_I2C_ADDR_7_BITS |
                                    VSF_I2C_ADDR_10_BITS,

    /**
     * \~english
     * @brief Combined mask for all I2C mode configuration bits.
     * Includes mode, speed, and address mode masks.
     * \~chinese
     * @brief I2C 所有模式配置位的组合掩码。
     * 包含模式、速度和地址模式掩码。
     */
    VSF_I2C_MODE_ALL_BITS_MASK    = VSF_I2C_MODE_MASK |
                                    VSF_I2C_SPEED_MASK |
                                    VSF_I2C_ADDR_MASK,
};

/**
 * \~english
 * @brief Predefined VSF I2C commands that can be reimplemented in specific HAL drivers.
 * \~chinese
 * @brief 预定义的 VSF I2C 命令，可以在具体的 HAL 驱动中重新实现。
 */
#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD == DISABLED
typedef enum vsf_i2c_cmd_t {
    /**
     * \~english
     * @brief Write command for I2C data transfer.
     * Used to write data on the I2C bus.
     * \~chinese
     * @brief I2C 数据写入命令。
     * 用于在 I2C 总线上写入数据。
     */
    VSF_I2C_CMD_WRITE           = (0x00ul << 0),

    /**
     * \~english
     * @brief Read command for I2C data transfer.
     * Used to read data from the I2C bus.
     * \~chinese
     * @brief I2C 数据读取命令。
     * 用于从 I2C 总线上读取数据。
     */
    VSF_I2C_CMD_READ            = (0x01ul << 0),

    /**
     * \~english
     * @brief Start condition command.
     * Generates a START condition on the I2C bus to begin a new transaction.
     * \~chinese
     * @brief 起始条件命令。
     * 在 I2C 总线上产生起始条件以开始新的传输。
     */
    VSF_I2C_CMD_START           = (0x00ul << 28),

    /**
     * \~english
     * @brief Stop condition command.
     * Generates a STOP condition on the I2C bus to end the current transaction.
     * \~chinese
     * @brief 停止条件命令。
     * 在 I2C 总线上产生停止条件以结束当前传输。
     */
    VSF_I2C_CMD_STOP            = (0x00ul << 29),

    /**
     * \~english
     * @brief Restart condition command.
     * Generates a repeated START condition without releasing the bus.
     * Used for changing direction or slave address within a transaction.
     * \~chinese
     * @brief 重新开始条件命令。
     * 在不释放总线的情况下产生重复起始条件。
     * 用于在传输过程中改变方向或从机地址。
     */
    VSF_I2C_CMD_RESTART         = (0x01ul << 30),

    /**
     * \~english
     * @brief 7-bit addressing mode command.
     * Configures the I2C transaction to use 7-bit slave addressing.
     * \~chinese
     * @brief 7 位地址模式命令。
     * 配置 I2C 传输使用 7 位从机寻址。
     */
    VSF_I2C_CMD_7_BITS          = (0x00ul << 31),

    /**
     * \~english
     * @brief 10-bit addressing mode command.
     * Configures the I2C transaction to use 10-bit slave addressing.
     * \~chinese
     * @brief 10 位地址模式命令。
     * 配置 I2C 传输使用 10 位从机寻址。
     */
    VSF_I2C_CMD_10_BITS         = (0x01ul << 31),
} vsf_i2c_cmd_t;
#endif

/**
 * \~english
 * @brief I2C command masks for different command types and configurations.
 * These masks are used to extract or set specific command bits.
 * \~chinese
 * @brief I2C 命令掩码，用于不同的命令类型和配置。
 * 这些掩码用于提取或设置特定的命令位。
 */
enum {
    /**
     * \~english
     * @brief Mask for read/write command selection.
     * Extracts VSF_I2C_CMD_WRITE and VSF_I2C_CMD_READ bits.
     * \~chinese
     * @brief 读写命令选择掩码。
     * 用于提取 VSF_I2C_CMD_WRITE 和 VSF_I2C_CMD_READ 位。
     */
    VSF_I2C_CMD_RW_MASK             = VSF_I2C_CMD_WRITE | VSF_I2C_CMD_READ,

    /**
     * \~english
     * @brief Mask for I2C address bits configuration.
     * Extracts VSF_I2C_CMD_7_BITS and VSF_I2C_CMD_10_BITS bits.
     * \~chinese
     * @brief I2C 地址位配置掩码。
     * 用于提取 VSF_I2C_CMD_7_BITS 和 VSF_I2C_CMD_10_BITS 位。
     */
    VSF_I2C_CMD_BITS_MASK           = VSF_I2C_CMD_7_BITS |
                                      VSF_I2C_CMD_10_BITS,

    /**
     * \~english
     * @brief Combined mask for all I2C command bits.
     * Includes read/write, start/stop/restart, and address bits masks.
     * \~chinese
     * @brief I2C 所有命令位的组合掩码。
     * 包含读写、起始/停止/重新开始和地址位掩码。
     */
#ifndef VSF_I2C_CMD_ALL_BITS_MASK
    VSF_I2C_CMD_ALL_BITS_MASK       = VSF_I2C_CMD_RW_MASK
                                    | VSF_I2C_CMD_START
                                    | VSF_I2C_CMD_STOP
                                    | VSF_I2C_CMD_RESTART
                                    | VSF_I2C_CMD_BITS_MASK,
#endif
};

/**
 * \~english
 * @brief Predefined VSF I2C interrupt masks that can be reimplemented in specific HAL drivers.
 * \~chinese
 * @brief 预定义的 VSF I2C 中断掩码，可以在具体的 HAL 驱动中重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these interrupts, these interrupts must be kept:
 * \~chinese
 * 即使硬件不支持这些中断，但是这些中断是必须保留的：
 *
 * - VSF_I2C_IRQ_MASK_MASTER_RX
 * - VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
 * - VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
 * - VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
 * - VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT
 * - VSF_I2C_IRQ_MASK_MASTER_ERR
 * - VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT
 * - VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT
 * - VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT
 * - VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT
 * - VSF_I2C_IRQ_MASK_SLAVE_TX
 * - VSF_I2C_IRQ_MASK_SLAVE_RX
 * - VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE
 * - VSF_I2C_IRQ_MASK_SLAVE_ERR
 *
 * \~chinese
 * 可选中断需要提供一个或者多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果该特性支持多个选项，那需要提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
#if VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_i2c_irq_mask_t {
    /**
     * \~english
     * @brief Master mode: Transmit FIFO threshold interrupt.
     * Triggered when transmit FIFO level falls below threshold in master mode.
     * Note: Some devices have fixed threshold of 1.
     * Only valid in master mode.
     * \~chinese
     * @brief 主机模式：发送 FIFO 阈值中断。
     * 当主机模式下发送 FIFO 水平低于阈值时触发。
     * 注意：某些设备的阈值固定为 1。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_TX                      = (0x1ul <<  0),

    /**
     * \~english
     * @brief Master mode: Receive FIFO threshold interrupt.
     * Triggered when receive FIFO level exceeds threshold in master mode.
     * Note: Some devices have fixed threshold of 1.
     * Only valid in master mode.
     * \~chinese
     * @brief 主机模式：接收 FIFO 阈值中断。
     * 当主机模式下接收 FIFO 水平超过阈值时触发。
     * 注意：某些设备的阈值固定为 1。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_RX                      = (0x1ul <<  1),

    /**
     * \~english
     * @brief Master mode: Transfer completion interrupt.
     * Triggered when the current transfer operation is completed in master mode.
     * Only valid in master mode.
     * \~chinese
     * @brief 主机模式：传输完成中断。
     * 当主机模式下当前传输操作完成时触发。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE       = (0x1ul <<  2),

    /**
     * \~english
     * @brief Master mode: Arbitration lost interrupt.
     * Triggered when the master loses arbitration in multi-master mode.
     * Only valid in master mode.
     * \~chinese
     * @brief 主机模式：仲裁丢失中断。
     * 在多主机模式下，当主机失去仲裁时触发。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST        = (0x1ul <<  3),

    /**
     * \~english
     * @brief Master mode: Address NACK detection interrupt.
     * Triggered when a NACK is received after sending the address in master mode.
     * Only valid in master mode.
     * \~chinese
     * @brief 主机模式：地址 NACK 检测中断。
     * 在主机模式下发送地址后收到 NACK 时触发。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK            = (0x1ul <<  4),

    /**
     * \~english
     * @brief Master mode: data transmit NACK detection interrupt.
     * Triggered when a NACK is received during master transmission.
     * Only valid in master mode.
     * \~chinese
     * @brief 主机模式：发送数据后没有收到 ACK 触发的中断。
     * 当主机传输过程中接收到 NACK 时触发。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT          = (0x1ul <<  5),

    /**
     * \~english
     * @brief Master mode: Error interrupt.
     * Triggered on any error condition in master mode.
     * Only valid in master mode.
     * \~chinese
     * @brief 主机模式：错误中断。
     * 在主机模式下发生任何错误条件时触发。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_ERR                     = (0x1ul <<  6),

    /**
     * \~english
     * @brief Slave mode: START or RESTART condition detection interrupt.
     * Triggered when START condition is detected while in slave mode.
     * Only valid in slave mode.
     * \~chinese
     * @brief 主机模式：起始条件或者重复起始条件检测中断。
     * 当主机模式下检测到起始条件或者重复起始条件时触发。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT = (0x1ul << 7),


    /**
     * \~english
     * @brief Master mode: Start condition detection interrupt.
     * Triggered when the master device generates a START condition on the I2C bus.
     * Only valid in master mode.
     * \~chinese
     * @brief 主机模式：起始条件检测中断。
     * 当主机设备在 I2C 总线上产生起始条件时触发。
     * 仅在主机模式下有效。
     */
    VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT            = (0x1ul << 8),


    /**
     * \~english
     * @brief Slave mode: Repeated START condition detection interrupt.
     * Triggered when repeated START condition is detected while in slave mode.
     * Only valid in slave mode.
     * \~chinese
     * @brief 从机模式：起始条件或者重复起始条件检测中断。
     * 当从机模式下检测到起始条件或者重复起始条件时触发。
     * 仅在从机模式下有效。
     */
    VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT  = (0x1ul << 9),

    /**
     * \~english
     * @brief Slave mode: STOP condition detection interrupt.
     * Triggered when STOP condition is detected while in slave mode.
     * Only valid in slave mode.
     * \~chinese
     * @brief 从机模式：停止条件检测中断。
     * 当从机模式下检测到停止条件时触发。
     * 仅在从机模式下有效。
     */
    VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT              = (0x1ul << 10),

    /**
     * \~english
     * @brief Slave mode: Transmit FIFO threshold interrupt.
     * Triggered when transmit FIFO level falls below threshold in slave mode.
     * Note: Some devices have fixed threshold of 1.
     * Only valid in slave mode.
     * \~chinese
     * @brief 从机模式：发送 FIFO 阈值中断。
     * 当从机模式下发送 FIFO 水平低于阈值时触发。
     * 注意：某些设备的阈值固定为 1。
     * 仅在从机模式下有效。
     */
    VSF_I2C_IRQ_MASK_SLAVE_TX                       = (0x1ul << 11),

    /**
     * \~english
     * @brief Slave mode: Receive FIFO threshold interrupt.
     * Triggered when receive FIFO level exceeds threshold in slave mode.
     * Note: Some devices have fixed threshold of 1.
     * Only valid in slave mode.
     * \~chinese
     * @brief 从机模式：接收 FIFO 阈值中断。
     * 当从机模式下接收 FIFO 水平超过阈值时触发。
     * 注意：某些设备的阈值固定为 1。
     * 仅在从机模式下有效。
     */
    VSF_I2C_IRQ_MASK_SLAVE_RX                       = (0x1ul << 12),

    /**
     * \~english
     * @brief Slave mode: Transfer completion interrupt.
     * Triggered when the current transfer operation is completed in slave mode.
     * Only valid in slave mode.
     * \~chinese
     * @brief 从机模式：传输完成中断。
     * 当从机模式下当前传输操作完成时触发。
     * 仅在从机模式下有效。
     */
    VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE        = (0x1ul << 13),

    /**
     * \~english
     * @brief Slave mode: Error interrupt.
     * Triggered on any error condition in slave mode.
     * Only valid in slave mode.
     * \~chinese
     * @brief 从机模式：错误中断。
     * 在从机模式下发生任何错误条件时触发。
     * 仅在从机模式下有效。
     */
    VSF_I2C_IRQ_MASK_SLAVE_ERR                      = (0x1ul << 14),
} vsf_i2c_irq_mask_t;
#endif

enum {
#ifndef VSF_I2C_IRQ_ALL_BITS_MASK
    VSF_I2C_IRQ_ALL_BITS_MASK                       = VSF_I2C_IRQ_MASK_MASTER_TX
                                                    | VSF_I2C_IRQ_MASK_MASTER_RX
                                                    | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                                                    | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                                    | VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                                                    | VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT
                                                    | VSF_I2C_IRQ_MASK_MASTER_ERR
                                                    | VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT
                                                    | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT
                                                    | VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT
                                                    | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT
                                                    | VSF_I2C_IRQ_MASK_SLAVE_TX
                                                    | VSF_I2C_IRQ_MASK_SLAVE_RX
                                                    | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE
                                                    | VSF_I2C_IRQ_MASK_SLAVE_ERR,
#endif
};

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
/**
 * \~english
 * @brief Predefined VSF I2C status that can be reimplemented in specific hal drivers.
 * \~chinese
 * @brief 预定义的 VSF I2C 状态，可以在具体的 HAL 驱动中重新实现。
 */
typedef struct vsf_i2c_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy    : 1;
        };
        uint32_t value;
    };
} vsf_i2c_status_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
/**
 * \~english
 * @brief Predefined I2C capability structure that can be reimplemented in specific HAL drivers.
 * @note This structure defines the capabilities supported by the I2C interface.
 * \~chinese
 * @brief 预定义的 I2C 能力结构体，可在具体的 HAL 驱动中重新实现。
 * @note 此结构体定义了 I2C 接口支持的能力。
 */
typedef struct vsf_i2c_capability_t {
#if VSF_I2C_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    //! \~english Supported interrupt masks
    //! \~chinese 支持的中断掩码
    vsf_i2c_irq_mask_t irq_mask;

    //! \~english Support for manual START signal control
    //! \~chinese 支持手动控制 START 信号
    uint8_t support_no_start        : 1;

    //! \~english Support for manual STOP signal control
    //! \~chinese 支持手动控制 STOP 信号
    uint8_t support_no_stop         : 1;

    //! \~english Support for RESTART condition
    //! \~chinese 支持 RESTART 条件
    uint8_t support_restart         : 1;

    //! \~english Maximum number of data that can be transferred (0 is invalid)
    //! \~chinese 最大传输数据数量（0为无效值）
    uint_fast16_t max_transfer_size;

    //! \~english Minimum number of data that can be transferred (0 means START condition only)
    //! \~chinese 最小传输数据数量（0表示仅发送 START 条件）
    uint_fast16_t min_transfer_size;
} vsf_i2c_capability_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
/**
 * \~english
 * @brief I2C forward declaration
 * \~chinese
 * @brief I2C 前置声明
 */
typedef struct vsf_i2c_t vsf_i2c_t;

/**
 \~english
 @brief I2C interrupt callback function prototype.
 @note It is called after interrupt occurs.

 @param target_ptr pointer of user.
 @param i2c_ptr pointer of I2C instance.
 @param irq_mask one or more value of enum vsf_i2c_irq_mask_t
 @return None.

 \~chinese
 @brief I2C 中断回调函数原型
 @note 这个回调函数在中断发生之后被调用

 @param target_ptr 用户指针
 @param i2c_ptr I2C 实例的指针
 @param irq_mask 一个或者多个枚举 vsf_i2c_irq_mask_t 的值的按位或
 @return 无。

 \~
 \code {.c}
    static void __user_i2c_irqhandler(void *target_ptr, vsf_i2c_t *i2c_ptr, enum irq_mask)
    {
        if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_STARTED) {
            // do something
        }

        if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_STOPPED) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_i2c_isr_handler_t(void *target_ptr,
                                   vsf_i2c_t *i2c_ptr,
                                   vsf_i2c_irq_mask_t irq_mask);

/**
 \~english
 @brief I2C interrupt configuration

 \~chinese
 @brief I2C 中断配置
 */
typedef struct vsf_i2c_isr_t {
    //! \~english interrupt handler
    //! \~chinese 中断处理函数
    vsf_i2c_isr_handler_t *handler_fn;

    //! \~english user data
    //! \~chinese 用户数据
    void                  *target_ptr;

    //! \~english interrupt priority
    //! \~chinese 中断优先级
    vsf_arch_prio_t        prio;
} vsf_i2c_isr_t;

/**
 \~english
 @brief I2C configuration

 \~chinese
 @brief I2C 配置
 */
typedef struct vsf_i2c_cfg_t {
    //! \~english operating mode
    //! \~chinese 工作模式
    vsf_i2c_mode_t mode;

    //! \~english clock frequency in Hz
    //! \~chinese 时钟频率，单位 Hz
    uint32_t clock_hz;

    //! \~english interrupt configuration
    //! \~chinese 中断配置
    vsf_i2c_isr_t isr;

    //! \~english slave address in slave mode
    //! \~chinese 从机模式下的地址
    uint_fast16_t slave_addr;
} vsf_i2c_cfg_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL == DISABLED
/**
 * \~english
 * @brief I2C control commands for hardware-specific operations
 * @note These commands provide additional control beyond basic data transfer
 * \~chinese
 * @brief I2C 控制命令，用于硬件特定操作
 * @note 这些命令提供了基本数据传输之外的额外控制功能
 */
typedef enum vsf_i2c_ctrl_t {
    /*
    //! \~english Abort current transfer in master mode
    //! \~chinese 在主机模式下中止当前传输
    VSF_I2C_CTRL_MASTER_ABORT                     = (0x1ul << 3),
    #define VSF_I2C_CTRL_MASTER_ABORT VSF_I2C_CTRL_MASTER_ABORT
    */

    /*
    //! \~english Abort current transfer in slave mode
    //! \~chinese 在从机模式下中止当前传输
    VSF_I2C_CTRL_SLAVE_ABORT                      = (0x1ul << 4),
    #define VSF_I2C_CTRL_SLAVE_ABORT VSF_I2C_CTRL_SLAVE_ABORT
    */

    /*
    //! \~english Enable analog noise filter on SDA/SCL lines
    //! \~chinese 启用 SDA/SCL 线上的模拟噪声滤波器
    VSF_I2C_CTRL_ANALOG_FILTER_ENABLE            = (0x0ul << 5),
    //! \~english Disable analog noise filter on SDA/SCL lines
    //! \~chinese 禁用 SDA/SCL 线上的模拟噪声滤波器
    VSF_I2C_CTRL_ANALOG_FILTER_DISABLE           = (0x1ul << 5),
    #define VSF_I2C_CTRL_ANALOG_FILTER_ENABLE VSF_I2C_CTRL_ANALOG_FILTER_ENABLE
    #define VSF_I2C_CTRL_ANALOG_FILTER_DISABLE VSF_I2C_CTRL_ANALOG_FILTER_DISABLE
    */

    /*
    //! \~english Enable digital noise filter on SDA/SCL lines
    //! \~chinese 启用 SDA/SCL 线上的数字噪声滤波器
    VSF_I2C_CTRL_DIGITAL_FILTER_ENABLE            = (0x0ul << 7),
    //! \~english Disable digital noise filter on SDA/SCL lines
    //! \~chinese 禁用 SDA/SCL 线上的数字噪声滤波器
    VSF_I2C_CTRL_DIGITAL_FILTER_DISABLE           = (0x1ul << 7),
    #define VSF_I2C_CTRL_DIGITAL_FILTER_ENABLE VSF_I2C_CTRL_DIGITAL_FILTER_ENABLE
    #define VSF_I2C_CTRL_DIGITAL_FILTER_DISABLE VSF_I2C_CTRL_DIGITAL_FILTER_DISABLE
    */

    //! \~english Placeholder enum member to avoid empty enum error
    //! \~chinese 占位枚举成员，用于避免空枚举错误
    __VSF_I2C_CTRL_DUMMP = 0,
} vsf_i2c_ctrl_t;
#endif

/**
 * \~english
 * @brief I2C operation function pointers for multi-class support.
 * @note These function pointers define the interface for I2C operations.
 * \~chinese
 * @brief I2C 操作函数指针，用于多类支持
 * @note 这些函数指针定义了 I2C 操作的接口
 */
typedef struct vsf_i2c_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_I2C_APIS(vsf)
} vsf_i2c_op_t;

#if VSF_I2C_CFG_MULTI_CLASS == ENABLED
/**
 * \~english
 * @brief I2C instance structure for multi-class support.
 * \~chinese
 * @brief I2C 实例结构体，用于多类支持
 */
struct vsf_i2c_t {
    //! \~english Pointer to I2C operation functions
    //! \~chinese I2C 操作函数指针
    const vsf_i2c_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a I2C instance
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @param[in] cfg_ptr: a pointer to configuration structure @ref vsf_i2c_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise returns error code
 *
 * @note It is not necessary to call vsf_i2c_fini() to deinitialization.
 *       vsf_i2c_init() should be called before any other I2C API except vsf_i2c_capability().
 *
 * \~chinese
 * @brief 初始化一个 I2C 实例
 * @param[in] i2c_ptr: 结构体 @ref vsf_i2c_t 的指针
 * @param[in] cfg_ptr: 配置结构体 @ref vsf_i2c_cfg_t 的指针
 * @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 初始化失败后不需要调用 vsf_i2c_fini() 进行反初始化。
 *       vsf_i2c_init() 应该在除 vsf_i2c_capability() 之外的其他 I2C API 之前调用。
 */
extern vsf_err_t vsf_i2c_init(vsf_i2c_t *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr);

/**
 * \~english
 * @brief Finalize a I2C instance
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @return none
 *
 * \~chinese
 * @brief 终止一个 I2C 实例
 * @param[in] i2c_ptr: 指向结构体 @ref vsf_i2c_t 的指针
 * @return 无
 */
extern void vsf_i2c_fini(vsf_i2c_t *i2c_ptr);

/**
 * \~english
 * @brief Enable a I2C instance
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @return fsm_rt_t: fsm_rt_cpl if I2C was enabled, fsm_rt_on_going if I2C is still enabling
 *
 * \~chinese
 * @brief 启用一个 I2C 实例
 * @param[in] i2c_ptr: 指向结构体 @ref vsf_i2c_t 的指针
 * @return fsm_rt_t: 如果 I2C 实例已启用返回 fsm_rt_cpl，如果 I2C 实例正在启用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_i2c_enable(vsf_i2c_t *i2c_ptr);

/**
 * \~english
 * @brief Disable a I2C instance
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @return fsm_rt_t: fsm_rt_cpl if I2C was disabled, fsm_rt_on_going if I2C is still disabling
 *
 * \~chinese
 * @brief 禁用一个 I2C 实例
 * @param[in] i2c_ptr: 指向结构体 @ref vsf_i2c_t 的指针
 * @return fsm_rt_t: 如果 I2C 实例已禁用返回 fsm_rt_cpl，如果 I2C 实例正在禁用过程中返回 fsm_rt_on_going
 *
 * @note 这与 vsf_i2c_fini() 的区别在于，禁用函数不会释放 I2C 实例的资源。之后可以使用 vsf_i2c_enable() 重新启用 I2C 实例
 *       而不需要调用 vsf_i2c_init()
 */
extern fsm_rt_t vsf_i2c_disable(vsf_i2c_t *i2c_ptr);

/**
 * \~english
 * @brief I2C instance enables interrupts
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @param[in] irq_mask: one or more values of enum @ref vsf_i2c_irq_mask_t
 * @return none
 *
 * \~chinese
 * @brief I2C 实例启用中断
 * @param[in] i2c_ptr: 指向结构体 @ref vsf_i2c_t 的指针
 * @param[in] irq_mask: 一个或多个枚举 vsf_i2c_irq_mask_t 的值的按位或，@ref vsf_i2c_irq_mask_t
 * @return 无
 */
extern void vsf_i2c_irq_enable(vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask);

/**
 * \~english
 * @brief I2C instance disables interrupts
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @param[in] irq_mask: one or more values of enum @ref vsf_i2c_irq_mask_t
 * @return none
 *
 * \~chinese
 * @brief I2C 实例禁用中断
 * @param[in] i2c_ptr: 指向结构体 @ref vsf_i2c_t 的指针
 * @param[in] irq_mask: 一个或多个枚举 vsf_i2c_irq_mask_t 的值的按位或，@ref vsf_i2c_irq_mask_t
 * @return 无
 */
extern void vsf_i2c_irq_disable(vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask);

/**
 * \~english
 * @brief Get the status of I2C instance
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @return vsf_i2c_status_t: all status of current I2C
 *
 * \~chinese
 * @brief 获取 I2C 实例的状态
 * @param[in] i2c_ptr: 指向结构体 @ref vsf_i2c_t 的指针
 * @return vsf_i2c_status_t: 返回当前 I2C 的所有状态
 */
extern vsf_i2c_status_t vsf_i2c_status(vsf_i2c_t *i2c_ptr);

/**
 * \~english
 * @brief Get the capabilities of I2C instance
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @return vsf_i2c_capability_t: all capabilities of current I2C @ref vsf_i2c_capability_t
 *
 * \~chinese
 * @brief 获取 I2C 实例的能力
 * @param[in] i2c_ptr: 指向结构体 @ref vsf_i2c_t 的指针
 * @return vsf_i2c_capability_t: 返回当前 I2C 的所有能力 @ref vsf_i2c_capability_t
 */
extern vsf_i2c_capability_t vsf_i2c_capability(vsf_i2c_t *i2c_ptr);

/**
 * \~english
 * @brief I2C instance as master mode performs a FIFO transfer
 * @param[in,out] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @param[in] address: address of I2C transfer
 * @param[in] cmd: I2C command
 * @param[in] count: number of data to transfer
 * @param[in,out] buffer_ptr: I2C transfer buffer
 * @param[in,out] cur_cmd_ptr: current I2C command pointer
 * @param[in,out] offset_ptr: current offset pointer
 * @return uint_fast16_t: number of bytes transferred from current I2C
 *
 * \~chinese
 * @brief I2C 主机进行一次 FIFO 传输
 * @param[in,out] i2c_ptr: 结构体 @ref vsf_i2c_t 的指针
 * @param[in] address: I2C 传输的地址
 * @param[in] cmd: I2C 命令
 * @param[in] count: 要传输的数据数量
 * @param[in,out] buffer_ptr: I2C 传输缓冲区
 * @param[in,out] cur_cmd_ptr: 当前 I2C 命令指针
 * @param[in,out] offset_ptr: 当前偏移指针
 * @return uint_fast16_t: 返回从当前 I2C 传输的字节数
 */
extern void vsf_i2c_master_fifo_transfer(vsf_i2c_t *i2c_ptr,
                                         uint16_t address,
                                         vsf_i2c_cmd_t cmd,
                                         uint_fast16_t count,
                                         uint8_t *buffer_ptr,
                                         vsf_i2c_cmd_t *cur_cmd_ptr,
                                         uint_fast16_t *offset_ptr);

/**
 * \~english
 * @brief I2C instance as slave mode performs a FIFO transfer
 * @param[in,out] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @param[in] transmit_or_receive: true for transmit, false for receive
 * @param[in] count: number of data to transfer
 * @param[in,out] buffer_ptr: I2C transfer buffer
 * @param[in,out] offset_ptr: current offset pointer
 * @return uint_fast16_t: number of bytes transferred from current I2C
 *
 * \~chinese
 * @brief I2C 从机进行一次 FIFO 传输
 * @param[in,out] i2c_ptr: 结构体 @ref vsf_i2c_t 的指针
 * @param[in] transmit_or_receive: true 表示发送，false 表示接收
 * @param[in] count: 要传输的数据数量
 * @param[in,out] buffer_ptr: I2C 传输缓冲区
 * @param[in,out] offset_ptr: 当前偏移指针
 * @return uint_fast16_t: 返回从当前 I2C 传输的字节数
 */
extern uint_fast16_t vsf_i2c_slave_fifo_transfer(vsf_i2c_t *i2c_ptr,
                                                 bool transmit_or_receive,
                                                 uint_fast16_t count,
                                                 uint8_t *buffer_ptr);

/**
 * \~english
 * @brief I2C instance as master mode requests a transfer
 * @param[in,out] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @param[in] address: address of I2C transfer
 * @param[in] cmd: I2C command
 * @param[in] count: number of data to transfer
 * @param[in,out] buffer_ptr: I2C transfer buffer
 * @return vsf_err_t: VSF_ERR_NONE if successful, or a negative error code
 *
 * \~chinese
 * @brief I2C 主机请求一次传输
 * @param[in,out] i2c_ptr: 结构体 @ref vsf_i2c_t 的指针
 * @param[in] address: I2C 传输的地址
 * @param[in] cmd: I2C 命令
 * @param[in] count: 要传输的数据数量
 * @param[in,out] buffer_ptr: I2C 传输缓冲区
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_i2c_master_request(vsf_i2c_t *i2c_ptr,
                                        uint16_t address,
                                        vsf_i2c_cmd_t cmd,
                                        uint_fast16_t count,
                                        uint8_t *buffer_ptr);

/**
 * \~english
 * @brief I2C instance as slave mode requests a transfer
 * @param[in,out] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @param[in] transmit_or_receive: true for transmit, false for receive
 * @param[in] count: number of data to transfer
 * @param[in,out] buffer_ptr: I2C transfer buffer
 * @return vsf_err_t: VSF_ERR_NONE if successful, or a negative error code
 *
 * \~chinese
 * @brief I2C 从机请求一次传输
 * @param[in,out] i2c_ptr: 结构体 @ref vsf_i2c_t 的指针
 * @param[in] transmit_or_receive: true 表示发送，false 表示接收
 * @param[in] count: 要传输的数据数量
 * @param[in,out] buffer_ptr: I2C 传输缓冲区
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_i2c_slave_request(vsf_i2c_t *i2c_ptr,
                                       bool transmit_or_receive,
                                       uint_fast16_t count,
                                       uint8_t *buffer_ptr);

/**
 * \~english
 * @brief get the counter of transfers for current request by the I2C master
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @return uint_fast16_t: number of data transferred
 * @note This API can be used between slave NAK and the next transmission
 *
 * \~chinese
 * @brief 获取 I2C 主机当前请求已经传输的数据数量
 * @param[in] i2c_ptr: 结构体 @ref vsf_i2c_t 的指针
 * @return uint_fast16_t: 已传输的数据数量
 * @note 此 API 可在从机 NAK 之后到下一次传输之前使用
 */
extern uint_fast16_t vsf_i2c_master_get_transferred_count(vsf_i2c_t *i2c_ptr);

/**
 * \~english
 * @brief get the counter of transfers for current request by the I2C slave
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @return uint_fast16_t: number of data transferred
 *
 * \~chinese
 * @brief 获取 I2C 从机当前请求已经传输的数据数量
 * @param[in] i2c_ptr: 结构体 @ref vsf_i2c_t 的指针
 * @return uint_fast16_t: 已传输的数据数量
 */
extern uint_fast16_t vsf_i2c_slave_get_transferred_count(vsf_i2c_t *i2c_ptr);

/**
 * \~english
 * @brief Calls the specified I2C command
 * @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 * @param[in] ctrl: I2C control command @ref vsf_i2c_ctrl_t
 * @param[in,out] param: the parameter of the command, its use is determined by the command
 * @return vsf_err_t: returns VSF_ERR_NONE if successful, or a negative error code
 *
 * \~chinese
 * @brief 调用指定的 I2C 命令
 * @param[in] i2c_ptr: 结构体 @ref vsf_i2c_t 的指针
 * @param[in] ctrl: I2C 控制命令，参考 @ref vsf_i2c_ctrl_t
 * @param[in,out] param: 命令的参数，其用途由命令决定
 * @return vsf_err_t: 成功返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_i2c_ctrl(vsf_i2c_t *i2c_ptr, vsf_i2c_ctrl_t ctrl, void * param);

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/i2c/i2c_request.h"

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_I2C_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_i2c_t                                      VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t)
#   define vsf_i2c_init(__I2C, ...)                         VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_init)                         ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_fini(__I2C)                              VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_fini)                         ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_enable(__I2C)                            VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_enable)                       ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_disable(__I2C)                           VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_disable)                      ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_irq_enable(__I2C, ...)                   VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_irq_enable)                   ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_irq_disable(__I2C, ...)                  VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_irq_disable)                  ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_status(__I2C)                            VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_status)                       ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_capability(__I2C)                        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_capability)                   ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_master_fifo_transfer(__I2C, ...)         VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_master_fifo_transfer)         ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_master_request(__I2C, ...)               VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_master_request)               ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_master_get_transferred_count(__I2C, ...)  VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_master_get_transferred_count) ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_slave_fifo_transfer(__I2C, ...)          VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_slave_fifo_transfer)          ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_slave_request(__I2C, ...)                VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_slave_request)                ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_slave_get_transferred_count(__I2C, ...)  VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_slave_get_transferred_count)  ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_ctrl(__I2C, ...)                         VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_ctrl)                         ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_I2C_H__*/
