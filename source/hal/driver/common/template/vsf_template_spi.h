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

// multi-class support enabled by default for maximum availability.
#ifndef VSF_SPI_CFG_MULTI_CLASS
#   define VSF_SPI_CFG_MULTI_CLASS                      ENABLED
#endif

#if defined(VSF_HW_SPI_COUNT) && !defined(VSF_HW_SPI_MASK)
#   define VSF_HW_SPI_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_SPI_COUNT)
#endif

#if defined(VSF_HW_SPI_MASK) && !defined(VSF_HW_SPI_COUNT)
#   define VSF_HW_SPI_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_SPI_MASK)
#endif

// application code can redefine it
#ifndef VSF_SPI_CFG_PREFIX
#   if (VSF_SPI_CFG_MULTI_CLASS == DISABLED) && defined(VSF_HW_SPI_COUNT) && (VSF_HW_SPI_COUNT != 0)
#       define VSF_SPI_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_SPI_CFG_PREFIX                       vsf
#   endif
#endif

#ifndef VSF_SPI_CFG_FUNCTION_RENAME
#   define VSF_SPI_CFG_FUNCTION_RENAME                  ENABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE to redefine vsf_spi_mode_t as needed.
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE            DISABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK to redefine vsf_spi_irq_mask_t as needed.
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS to redefine vsf_spi_status_t as needed.
#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS          DISABLED
#endif

//! Redefine struct vsf_spi_cfg_t. The vsf_spi_isr_handler_t type also needs to
//! be redefined For compatibility, members should not be deleted when struct
//! @ref vsf_spi_cfg_t redefining.
#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

//! Redefine struct vsf_spi_capability_t.
//! For compatibility, members should not be deleted when struct @ref
//! vsf_spi_capability_t redefining.
#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS to reimplement the function
//! vsf_spi_mode_to_data_bits as needed.
#ifndef VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS
#   define VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS    DISABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS to reimplement the function
//! vsf_spi_data_bits_to_mode as needed.
#ifndef VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE
#   define VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE    DISABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS to reimplement the function
//! vsf_spi_data_bits_to_mode as needed.
#ifndef VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_BYTES
#   define VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_BYTES   DISABLED
#endif

#ifndef VSF_SPI_CFG_INHERT_HAL_CAPABILITY
#   define VSF_SPI_CFG_INHERT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_SPI_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, init,                 VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, vsf_spi_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, fini,                 VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             spi, enable,               VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             spi, disable,              VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, irq_enable,           VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, vsf_spi_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, irq_disable,          VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, vsf_spi_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_spi_status_t,     spi, status,               VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_spi_capability_t, spi, capability,           VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, cs_active,            VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, uint_fast8_t index) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, cs_inactive,          VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, uint_fast8_t index) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, fifo_transfer,        VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, \
                                                                                           void *out_buffer_ptr, uint_fast32_t *out_offset_ptr, \
                                                                                           void *in_buffer_ptr, uint_fast32_t *in_offset_ptr, uint_fast32_t cnt) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, request_transfer,     VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, void *out_buffer_ptr, \
                                                                                           void *in_buffer_ptr, uint_fast32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, cancel_transfer,      VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, get_transferred_count, VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, uint_fast32_t * tx_count, uint_fast32_t *rx_count)


/*============================ TYPES =========================================*/

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF SPI modes that can be reimplemented in specific hal drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF SPI 模式，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these modes, the following modes must be kept，
 * If the hardware supports more modes, e.g. more databits, more slave select mode,
 * we can implement it in the hardware driver:
 * \~chinese
 * 即使硬件不支持这些功能，但是以下的模式必须保留，如果硬件支持更多模式，例如更多的数据位大小，
 * 我们可以在硬件驱动里实现它：
 *
 * - VSF_SPI_MASTER
 * - VSF_SPI_SLAVE
 * - VSF_SPI_MSB_FIRST
 * - VSF_SPI_LSB_FIRST
 * - VSF_SPI_MODE_0
 * - VSF_SPI_MODE_1
 * - VSF_SPI_MODE_2
 * - VSF_SPI_MODE_3
 * - VSF_SPI_CS_SOFTWARE_MODE
 * - VSF_SPI_CS_SOFTWARE_MODE
 * - VSF_SPI_CS_HARDWARE_MODE
 * - VSF_SPI_DATASIZE_8
 * - VSF_SPI_DATASIZE_16
 * - VSF_SPI_DATASIZE_32
 *
 * \~english
 * If more new modes are added to the driver, then the corresponding MASK macros need to
 * be defined to include the values of the new modes. For example, Adding the new databit option
 * requires that the macro VSF_SPI_DATASIZE_MASK be defined.
 * \~chinese
 * 驱动里如果添加更多的新模式的时候，那也需要定义对应的 MASK 宏 来包含新的模式的值。例如添加了新的
 * 数据位选项，就需要定义宏 VSF_SPI_DATASIZE_MASK。
 *
 * \~english
 *  Optional features require one or more enumeration options and a macro with the same
 *  name to determine if they are supported at runtime. If the feature supports more than
 *  one option, it is recommended to provide the corresponding MASK option, so that the
 *  user can switch to different modes at compile-time.
 * \~chinese
 * 可选特性需要提供一个或者多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果它特性支持多个选项，建议提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
typedef enum vsf_spi_mode_t {
    VSF_SPI_MASTER                  = 0x00ul << 0,      //!< select master mode
    VSF_SPI_SLAVE                   = 0x01ul << 0,      //!< select slave mode

    VSF_SPI_MSB_FIRST               = 0x00ul << 1,      //!< default enable MSB
    VSF_SPI_LSB_FIRST               = 0x01ul << 1,      //!< transfer LSB first

    // CPOL and CPHA are not recommended to use, not every SPI driver provide this
    VSF_SPI_CPOL_LOW                = 0x00ul << 2,      //!< SCK clock polarity is low
    VSF_SPI_CPOL_HIGH               = 0x01ul << 2,      //!< SCK clock polarity is high
    VSF_SPI_CPHA_LOW                = 0x00ul << 2,      //!< SCK clock phase is low
    VSF_SPI_CPHA_HIGH               = 0x01ul << 2,      //!< SCK clock phase is high
    // SPI_MODE is recommended
    VSF_SPI_MODE_0                  = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_LOW,
    VSF_SPI_MODE_1                  = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_HIGH,
    VSF_SPI_MODE_2                  = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_LOW,
    VSF_SPI_MODE_3                  = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_HIGH,

    //! SCK Chip Select by software(vsf_spi_cs_active/vsf_spi_cs_inactive)
    VSF_SPI_CS_SOFTWARE_MODE        = 0x00ul << 4,
    //! Hardware dependent, possibly low all the time spi is enabled, possibly
    //! low only when spi is transmitted
    VSF_SPI_CS_HARDWARE_MODE        = 0x01ul << 4,
    /*
    // Some hardware supports multimaster mode
    VSF_SPI_CS_HARDWARE_INPUT_MODE  = 0x02ul << 4,
    #define VSF_SPI_CS_HARDWARE_INPUT_MODE VSF_SPI_CS_HARDWARE_INPUT_MODE
    */

    VSF_SPI_DATASIZE_8              = 0x00ul << 8,
    VSF_SPI_DATASIZE_16             = 0x01ul << 8,
    VSF_SPI_DATASIZE_32             = 0x02ul << 8,
    /*
    // Some hardware supports more data bits, we can redefine it inside the specific driver
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
    // Some hardware supports a different number of data line
    VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX    = (0x00ul << 16),    // 2 line, full-duplex, standard spi
    VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX    = (0x01ul << 16),    // 1 line, half-duplex
    VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX    = (0x02ul << 16),    // 2 line, half-duplex, dual spi
    VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX    = (0x03ul << 16),    // 4 line, half-duplex, qual spi
    VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX    = (0x04ul << 16),    // 8 line, half-duplex, octal spi

    #define VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX     VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX
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
    // Some hardware supports TI mode
    VSF_SPI_MOTORALA_MODE    = (0x00 << 20),        // SPI, serial peripheral interface
    VSF_SPI_TI_MODE          = (0x01 << 20),        // SSI, synchronous serial interface
    #define VSF_SPI_MOTORALA_MODE       VSF_SPI_MOTORALA_MODE
    #define VSF_SPI_TI_MODE             VSF_SPI_TI_MODE
    #define VSF_SPI_MOTORALA_TI_MASK    (VSF_SPI_MOTORALA_MODE | VSF_SPI_TI_MODE)
    */

    /*
    // Some hardware supports crc
    VSF_SPI_CRC_DISABLED     = (0x00 << 21),
    VSF_SPI_CRC_ENABLED      = (0x01 << 21),
    #define VSF_SPI_CRC_DISABLED        VSF_SPI_CRC_DISABLED
    #define VSF_SPI_CRC_ENABLED         VSF_SPI_CRC_ENABLED
    #define VSF_SPI_CRC_MASK            (VSF_SPI_CRC_DISABLED | VSF_SPI_CRC_ENABLED)
    */

    /*
    // Some hardware supports prescaler
    VSF_SPI_CLOCK_PRESCLER_2    = (0x00 << 22),
    VSF_SPI_CLOCK_PRESCLER_4    = (0x01 << 22),
    VSF_SPI_CLOCK_PRESCLER_8    = (0x02 << 22),
    VSF_SPI_CLOCK_PRESCLER_16   = (0x03 << 22),
    VSF_SPI_CLOCK_PRESCLER_32   = (0x04 << 22),
    VSF_SPI_CLOCK_PRESCLER_64   = (0x05 << 22),
    VSF_SPI_CLOCK_PRESCLER_128  = (0x06 << 22),
    VSF_SPI_CLOCK_PRESCLER_256  = (0x07 << 22),

    #define VSF_SPI_CLOCK_PRESCLER_MASK  VSF_SPI_CLOCK_PRESCLER_2
    #define VSF_SPI_CLOCK_PRESCLER_2     VSF_SPI_CLOCK_PRESCLER_2
    #define VSF_SPI_CLOCK_PRESCLER_4     VSF_SPI_CLOCK_PRESCLER_4
    #define VSF_SPI_CLOCK_PRESCLER_8     VSF_SPI_CLOCK_PRESCLER_8
    #define VSF_SPI_CLOCK_PRESCLER_16    VSF_SPI_CLOCK_PRESCLER_16
    #define VSF_SPI_CLOCK_PRESCLER_32    VSF_SPI_CLOCK_PRESCLER_32
    #define VSF_SPI_CLOCK_PRESCLER_64    VSF_SPI_CLOCK_PRESCLER_64
    #define VSF_SPI_CLOCK_PRESCLER_128   VSF_SPI_CLOCK_PRESCLER_128
    #define VSF_SPI_CLOCK_PRESCLER_256   VSF_SPI_CLOCK_PRESCLER_256

    #define VSF_SPI_CLOCK_PRESCLER_MASK  (  VSF_SPI_CLOCK_PRESCLER_2    \
                                            | VSF_SPI_CLOCK_PRESCLER_4    \
                                            | VSF_SPI_CLOCK_PRESCLER_8    \
                                            | VSF_SPI_CLOCK_PRESCLER_16   \
                                            | VSF_SPI_CLOCK_PRESCLER_32   \
                                            | VSF_SPI_CLOCK_PRESCLER_64   \
                                            | VSF_SPI_CLOCK_PRESCLER_128  \
                                            | VSF_SPI_CLOCK_PRESCLER_256)
    */
} vsf_spi_mode_t;
#endif

enum {
    VSF_SPI_DIR_MODE_MASK           = VSF_SPI_MASTER
                                    | VSF_SPI_SLAVE,

    VSF_SPI_BIT_ORDER_MASK          = VSF_SPI_MSB_FIRST
                                    | VSF_SPI_LSB_FIRST,

    VSF_SPI_MODE_MASK               = VSF_SPI_MODE_0
                                    | VSF_SPI_MODE_1
                                    | VSF_SPI_MODE_2
                                    | VSF_SPI_MODE_3,

#ifndef VSF_SPI_CS_MODE_MASK
    VSF_SPI_CS_MODE_MASK            = VSF_SPI_CS_SOFTWARE_MODE
                                    | VSF_SPI_CS_HARDWARE_MODE
#ifdef VSF_SPI_CS_HARDWARE_INPUT_MODE
                                    | VSF_SPI_CS_HARDWARE_INPUT_MODE
#endif
                                    ,
#endif

#ifndef VSF_SPI_DATASIZE_MASK
    VSF_SPI_DATASIZE_MASK           = VSF_SPI_DATASIZE_8
                                    | VSF_SPI_DATASIZE_16
                                    | VSF_SPI_DATASIZE_32,
#endif

    VSF_SPI_MODE_ALL_BITS_MASK      = VSF_SPI_DIR_MODE_MASK
                                    | VSF_SPI_BIT_ORDER_MASK
                                    | VSF_SPI_MODE_MASK
                                    | VSF_SPI_DIR_MODE_MASK
                                    | VSF_SPI_CS_MODE_MASK
                                    | VSF_SPI_DATASIZE_MASK
#ifdef VSF_SPI_DATALINE_MASK
                                    | VSF_SPI_DATALINE_MASK
#endif
#ifdef VSF_SPI_MOTORALA_TI_MASK
                                    | VSF_SPI_MOTORALA_TI_MASK
#endif
#ifdef VSF_SPI_CRC_MASK
                                    | VSF_SPI_CRC_MASK
#endif
#ifdef VSF_SPI_CLOCK_PRESCLER_MASK
                                    | VSF_SPI_CLOCK_PRESCLER_MASK
#endif
};

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/**
 * \~english
 * @brief Predefined VSF SPI interrupt that can be reimplemented in specific hal drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF SPI 中断，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these interrupt must be kept.
 * \~chinese
 * 即使硬件不支持这些中断，但是这些中断是必须实现的：
 *
 * - VSF_SPI_IRQ_MASK_TX
 * - VSF_SPI_IRQ_MASK_RX
 * - VSF_SPI_IRQ_MASK_TX_CPL
 * - VSF_SPI_IRQ_MASK_CPL
 * - VSF_SPI_IRQ_MASK_OVERFLOW_ERR
 * - VSF_SPI_IRQ_MASK_ERROR
 */
typedef enum vsf_spi_irq_mask_t {
    // TX/RX reach fifo threshold, threshold on some devices is bound to 1
    VSF_SPI_IRQ_MASK_TX             = 0x01ul << 0,
    VSF_SPI_IRQ_MASK_RX             = 0x01ul << 1,

    // request_rx/request_tx complete
    VSF_SPI_IRQ_MASK_TX_CPL         = 0x01ul << 2,
    VSF_SPI_IRQ_MASK_CPL            = 0x01ul << 3,

    VSF_SPI_IRQ_MASK_OVERFLOW_ERR   = 0x01ul << 4,
    VSF_SPI_IRQ_MASK_ERROR          = 0x01ul << 5,
} vsf_spi_irq_mask_t;
#endif

enum {
    VSF_SPI_IRQ_MASK_TX_FIFO_THRESHOLD  = VSF_SPI_IRQ_MASK_TX,
    VSF_SPI_IRQ_MASK_RX_FIFO_THRESHOLD  = VSF_SPI_IRQ_MASK_RX,

    //! For SPI transfers, send completion is definitely earlier than receive completion.
    VSF_SPI_IRQ_MASK_RX_CPL             = VSF_SPI_IRQ_MASK_CPL,

    VSF_SPI_IRQ_ALL_BITS_MASK           = VSF_SPI_IRQ_MASK_TX
                                        | VSF_SPI_IRQ_MASK_RX
                                        | VSF_SPI_IRQ_MASK_TX_CPL
                                        | VSF_SPI_IRQ_MASK_CPL
                                        | VSF_SPI_IRQ_MASK_OVERFLOW_ERR
                                        | VSF_SPI_IRQ_MASK_ERROR,
};

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_spi_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_spi_status_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_spi_capability_t {
#if VSF_SPI_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_spi_irq_mask_t irq_mask;

    uint8_t support_hardware_cs : 1; // some hardware support
    uint8_t support_software_cs : 1; // some hardware support
    uint8_t cs_count            : 6;

    uint32_t max_clock_hz;
    uint32_t min_clock_hz;
} vsf_spi_capability_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_spi_t vsf_spi_t;

typedef void vsf_spi_isr_handler_t(void *target_ptr,
                                   vsf_spi_t *spi_ptr,
                                   vsf_spi_irq_mask_t irq_mask);

//! spi isr for api
typedef struct vsf_spi_isr_t {
    vsf_spi_isr_handler_t *handler_fn;
    void                  *target_ptr;
    vsf_arch_prio_t        prio;
} vsf_spi_isr_t;

//! spi configuration for api
typedef struct vsf_spi_cfg_t {
    vsf_spi_mode_t   mode;              //!< spi working mode
    uint32_t         clock_hz;
    vsf_spi_isr_t    isr;
    uint8_t          auto_cs_index;     //!< spi auto chip select of pin when multiple cs are supported
} vsf_spi_cfg_t;
#endif

typedef struct vsf_spi_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_SPI_APIS(vsf)
} vsf_spi_op_t;

#if VSF_SPI_CFG_MULTI_CLASS == ENABLED
struct vsf_spi_t  {
    const vsf_spi_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a spi instance.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_spi_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if spi was initialized, or a negative error code

 \~chinese
 @brief 初始化一个 spi 实例
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @param[in] cfg_ptr: 结构体 vsf_spi_cfg_t 的指针，参考 @ref vsf_spi_cfg_t
 @return vsf_err_t: 如果 spi 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_spi_init(vsf_spi_t *spi_ptr, vsf_spi_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a spi instance.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @return none

 \~chinese
 @brief 终止一个 spi 实例
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @param[in] cfg_ptr: 结构体 vsf_spi_cfg_t 的指针，参考 @ref vsf_spi_cfg_t
 @return 无。
 */
extern void vsf_spi_fini(vsf_spi_t *spi_ptr);

/**
 \~english
 @brief enable interrupt masks of spi instance.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @return none.

 \~chinese
 @brief 使能 spi 实例的中断
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @return 无。
 */
extern fsm_rt_t vsf_spi_enable(vsf_spi_t *spi_ptr);

/**
 \~english
 @brief disable interrupt masks of spi instance.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @return none.

 \~chinese
 @brief 禁能 spi 实例的中断
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @return 无。
 */
extern fsm_rt_t vsf_spi_disable(vsf_spi_t *spi_ptr);

/**
 \~english
 @brief enable interrupt masks of spi instance.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[in] irq_mask: one or more value of enum @ref vsf_spi_irq_mask_t
 @return none.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 使能 spi 实例的中断
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_spi_irq_mask_t 的值的按位或，@ref vsf_spi_irq_mask_t
 @return 无。
 @note 在中断使能之前，应该清除所有悬挂的中断。
 */
extern void vsf_spi_irq_enable(vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of spi instance.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[in] irq_mask: one or more value of enum vsf_spi_irq_mask_t, @ref vsf_spi_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 spi 实例的中断
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_spi_irq_mask_t 的值的按位或，@ref vsf_spi_irq_mask_t
 @return 无。
 */
extern void vsf_spi_irq_disable(vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask);

/**
 \~english
 @brief spi set chip select active
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[in] index: chip select index
 @return none.

 \~chinese
 @brief spi 设置片选有效
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @param[in] index: 片选索引
 @return 无。
 */
extern void vsf_spi_cs_active(vsf_spi_t *spi_ptr, uint_fast8_t index);

/**
 \~english
 @brief spi set chip select inactive
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[in] index: chip select index
 @return none.

 \~chinese
 @brief spi 设置片选无效
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @param[in] index: 片选索引
 @return 无。
 */
extern void vsf_spi_cs_inactive(vsf_spi_t *spi_ptr, uint_fast8_t index);

/**
 \~english
 @brief get the status of spi instance.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @return vsf_spi_status_t: return all status of current spi

 \~chinese
 @brief 获取 spi 实例的状态
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @return vsf_spi_status_t: 返回当前 spi 的所有状态
 */
extern vsf_spi_status_t vsf_spi_status(vsf_spi_t *spi_ptr);

/**
 \~english
 @brief get the capability of spi instance.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @return vsf_spi_capability_t: return all capability of current spi @ref vsf_spi_capability_t

 \~chinese
 @brief 获取 spi 实例的能力
 @param[in] spi_ptr: 结构体 vsf_spi_t 的指针，参考 @ref vsf_spi_t
 @return vsf_spi_capability_t: 返回当前 spi 的所有能力 @ref vsf_spi_capability_t
 */
extern vsf_spi_capability_t vsf_spi_capability(vsf_spi_t *spi_ptr);

/**
 \~english
 @brief spi fifo transfer
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[in] out_buffer_ptr: a pointer to spi send data buffer, it can be a null pointer
 @param[inout] out_offset_ptr: a pointer to spi send data buffer offset
 @param[in] in_buffer_ptr: a pointer to spi receive data buffer, it can be a null pointer
 @param[inout] in_offset_ptr: a pointer to spi send data buffer offset
 @param[in] count: The number of data received or sent
 @return vsf_err_t: VSF_ERR_NONE if spi transfer successfully, or a negative error code

 \~chinese
 @brief spi fifo 传输
 @param[in] spi_ptr: @ref vsf_spi_t 传输结构指针
 @param[in] out_buffer_ptr: spi 发送数据的缓冲区, 它可以是空指针
 @param[in] out_offset_ptr: spi 发送数据的缓冲区的偏移
 @param[in] in_buffer_ptr: spi 接收数据的缓冲区, 它可以是空指针
 @param[in] in_offset_ptr: spi 接收数据的缓冲区的偏移
 @param[in] count: 接收数据的个数或者发送数据的个数
 @return vsf_err_t: 如果传输成功返回 VSF_ERR_NONE, 否则返回负数
 */
extern void vsf_spi_fifo_transfer(vsf_spi_t *spi_ptr,
                                  void *out_buffer_ptr, uint_fast32_t* out_offset_ptr,
                                  void *in_buffer_ptr, uint_fast32_t* in_offset_ptr,
                                  uint_fast32_t count);

/**
 \~english
 @brief start spi transfer. The best way to do this is to use DMA.
        Software implementation is the solution when DMA cannot be used
 @note When using DMA, you need to use DMA to receive data first,
       and then process DMA to send data.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[in] out_buffer_ptr: a pointer to spi send data buffer, it can be a null pointer
 @param[in] in_buffer_ptr: a pointer to spi receive data buffer, it can be a null pointer
 @param[in] count: The number of data received or sent
 @return vsf_err_t: VSF_ERR_NONE if spi transfer successfully, or a negative error code

 \~chinese
 @brief 启动 spi 传输
 @brief 启动 spi 传输. 最好的方式是使用 DMA 来完成，
        无法使用 DMA 完成的时候可以使用软件模拟完成
 @note 当使用 DMA 的时候，需要先使用 DMA 接收数据，然后再使用 DMA 发送数据
 @param[in] spi_ptr: @ref vsf_spi_t 传输结构指针
 @param[in] out_buffer_ptr: spi 发送数据的缓冲区, 它可以是空指针
 @param[in] in_buffer_ptr: spi 接收数据的缓冲区, 它可以是空指针
 @param[in] count: 接收数据的个数或者发送数据的个数
 @return vsf_err_t: 如果传输成功返回 VSF_ERR_NONE, 否则返回负数
 */
extern vsf_err_t vsf_spi_request_transfer(vsf_spi_t *spi_ptr, void *out_buffer_ptr,
                                          void *in_buffer_ptr, uint_fast32_t count);

/**
 \~english
 @brief spi cancel transfer
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @return vsf_err_t: VSF_ERR_NONE if spi cancel transfer successfully, or a negative error code

 \~chinese
 @brief spi 取消传输
 @param[in] spi_ptr: @ref vsf_spi_t 传输结构指针
 @return vsf_err_t: 如果 spi 取消传输成功返回 VSF_ERR_NONE, 否则返回负数
 */
extern vsf_err_t vsf_spi_cancel_transfer(vsf_spi_t *spi_ptr);

/**
 \~english
 @brief spi get transferred counter
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[out] send_count: a pointer to the number of spi data already sent
 @param[out] received_count: a pointer to the number of spi data already received
 @return vsf_err_t: VSF_ERR_NONE if spi cancel transfer successfully, or a negative error code

 \~chinese
 @brief spi 获取已传输的个数
 @param[in] spi_ptr: @ref vsf_spi_t 传输结构指针
 @param[out] send_count: 已经发送的数据数量的指针
 @param[out] received_count: 已经接收的数据数量的指针
 @return vsf_err_t: 如果 spi 取消传输成功返回 VSF_ERR_NONE, 否则返回负数
 */
extern void vsf_spi_get_transferred_count(vsf_spi_t *spi_ptr, uint_fast32_t * sent_count, uint_fast32_t *received_count);


/*============================ INLINE FUNCTIONS ==============================*/

#if VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS == ENABLD
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
        case VSF_SPI_DATASIZE_16:
            return 16;
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
        case VSF_SPI_DATASIZE_32:
            return 32;
        default:
            return 0;
    }
}
#   endif
#endif

#if VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE == ENABLD
#   if defined(VSF_SPI_DATASIZE_BIT_OFFSET) && defined(VSF_SPI_DATASIZE_VALUE_OFFSET)
static inline vsf_spi_mode_t vsf_spi_data_bits_to_mode(uint8_t bits)
{
    return (vsf_spi_mode_t)((bits - VSF_SPI_DATASIZE_VALUE_OFFSET) << VSF_SPI_DATASIZE_BIT_OFFSET);
}
#   else
static inline vsf_spi_mode_t vsf_spi_data_bits_to_mode(uint8_t bit)
{
    switch (bit) {
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
    case 16:
        return VSF_SPI_DATASIZE_16;
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
    case 32:
        return VSF_SPI_DATASIZE_32;
    default:
        return (vsf_spi_mode_t)0;
    }
}
#   endif
#endif

#if VSF_SPI_CFG_REIMPLEMENT_MODE_TO_BYTES == ENABLD
static inline uint8_t vsf_spi_mode_to_data_bytes(vsf_spi_mode_t mode)
{
    int bits = (mode & VSF_SPI_DATASIZE_MASK);
    if (bits <= VSF_SPI_DATASIZE_8) {
        return 1;
    } else if (bits <= VSF_SPI_DATASIZE_16) {
        return 2;
    } else {
        return 4;
    }
}
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SPI_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_spi_t                                  VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t)
#   define vsf_spi_init(__SPI, ...)                     VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_init)                 ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
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
#   define vsf_spi_get_transferred_count(__SPI, ...)    VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_get_transferred_count) ((__vsf_spi_t *)(__SPI), ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_SPI_H__*/

