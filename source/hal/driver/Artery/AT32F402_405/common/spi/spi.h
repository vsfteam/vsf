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

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_SPI_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_SPI_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

// HW/IPCore
/**
 * \note When vsf_peripheral_status_t is inherited, vsf_template_hal_driver.h needs to be included
 */
#include "hal/driver/common/template/vsf_template_hal_driver.h"
// HW/IPCore end

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// HW
/*\note hw SPI driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE for vsf_spi_mode_t
 *          VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS for vsf_spi_status_t
 *          VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_spi_irq_mask_t
 *          VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL for vsf_spi_ctrl_t
 *          VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG for vsf_spi_cfg_t
 *          VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_spi_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE       ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK   ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_spi_mode_t {
    // 2: SPI_CTRL1.MSTEN(2)
    VSF_SPI_MASTER                              = 1 << 2,
    VSF_SPI_SLAVE                               = 0 << 2,

    // 7: SPI_CTRL1.LTF(7)
    VSF_SPI_MSB_FIRST                           = 0 << 7,
    VSF_SPI_LSB_FIRST                           = 1 << 7,

    // SPI mode (bits 0-1: CPHA and CPOL, hardware specific)
    VSF_SPI_MODE_0                              = 0x00ul,
    VSF_SPI_MODE_1                              = 0x01ul,
    VSF_SPI_MODE_2                              = 0x02ul,
    VSF_SPI_MODE_3                              = 0x03ul,

    // 26: virtual
    VSF_SPI_CS_SOFTWARE_MODE                    = 1 << 26,
    // 18: SPI_CTRL2.HWCSOE(2)
    VSF_SPI_CS_HARDWARE_MODE                    = 1 << 18,

    // 20: SPI_CTRL2.TIEN(4)
    VSF_SPI_MOTOROLA_MODE                       = (0 << 20),
    VSF_SPI_TI_MODE                             = (1 << 20),
#define VSF_SPI_MOTOROLA_MODE                   VSF_SPI_MOTOROLA_MODE
#define VSF_SPI_TI_MODE                         VSF_SPI_TI_MODE
#define VSF_SPI_MOTOROLA_TI_MASK                (VSF_SPI_MOTOROLA_MODE | VSF_SPI_TI_MODE)

    // 11: SPI_CTRL1.FBN(11)
    VSF_SPI_DATASIZE_8                          = 0 << 11,
    VSF_SPI_DATASIZE_16                         = 1 << 11,
#define VSF_SPI_DATASIZE_16                     VSF_SPI_DATASIZE_16
    // 27: not support
    VSF_SPI_DATASIZE_32                         = 1 << 27,

    VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX         = 0,
    // 10: SPI_CTRL1.ORA(10)
    VSF_SPI_DATALINE_2_LINE_RX_ONLY             = 1 << 10,
    // 15: SPI_CTRL1.SLBEN(15)
    VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX         = 1 << 15,
#define VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX     VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX
#define VSF_SPI_DATALINE_2_LINE_RX_ONLY         VSF_SPI_DATALINE_2_LINE_RX_ONLY
#define VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX     VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX

    // 13: SPI_CTRL1.CCEN(13)
    VSF_SPI_CRC_DISABLED                        = 0 << 13,
    VSF_SPI_CRC_ENABLED                         = 1 << 13,

    // Standard Optional: only include these if hardware supports them
    // 1: SPI_CTRL1.CLKPOL(1)
    VSF_SPI_CPOL_LOW                            = 0 << 1,
    #define VSF_SPI_CPOL_LOW                     VSF_SPI_CPOL_LOW
    VSF_SPI_CPOL_HIGH                           = 1 << 1,
    #define VSF_SPI_CPOL_HIGH                    VSF_SPI_CPOL_HIGH
    // 0: SPI_CTRL1.CLKPHA(0)
    VSF_SPI_CPHA_LOW                            = 0 << 0,
    #define VSF_SPI_CPHA_LOW                     VSF_SPI_CPHA_LOW
    VSF_SPI_CPHA_HIGH                           = 1 << 0,
    #define VSF_SPI_CPHA_HIGH                    VSF_SPI_CPHA_HIGH

    // more vendor specified modes can be added here

    __VSF_HW_SPI_CTRL1_MASK                     = VSF_SPI_MASTER
                                                | VSF_SPI_SLAVE
                                                | VSF_SPI_MSB_FIRST
                                                | VSF_SPI_LSB_FIRST
#ifdef VSF_SPI_CPOL_LOW
                                                | VSF_SPI_CPOL_LOW
                                                | VSF_SPI_CPOL_HIGH
#endif
#ifdef VSF_SPI_CPHA_LOW
                                                | VSF_SPI_CPHA_LOW
                                                | VSF_SPI_CPHA_HIGH
#endif
                                                | VSF_SPI_DATASIZE_8
                                                | VSF_SPI_DATASIZE_16
                                                | VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX
                                                | VSF_SPI_DATALINE_2_LINE_RX_ONLY
                                                | VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX
                                                | VSF_SPI_CRC_DISABLED
                                                | VSF_SPI_CRC_ENABLED,
    __VSF_HW_SPI_CTRL2_MASK                     = VSF_SPI_CS_HARDWARE_MODE
                                                | VSF_SPI_TI_MODE
                                                | VSF_SPI_MOTOROLA_MODE,
    __VSF_HW_SPI_CS_MASK                        = VSF_SPI_CS_SOFTWARE_MODE,
} vsf_spi_mode_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_spi_irq_mask_t {
    // 1: SPI_STS.TDBE(1)
    VSF_SPI_IRQ_MASK_TX                         = 1 << 1,
    // 0: SPI_STS.RDBF(0)
    VSF_SPI_IRQ_MASK_RX                         = 1 << 0,
    // 9: virtual, TODO: use DMA interrupt
    VSF_SPI_IRQ_MASK_TX_CPL                     = 1 << 9,
    // 10: virtual, TODO: use DMA interrupt
    VSF_SPI_IRQ_MASK_RX_CPL                     = 1 << 10,
    // 6: SPI_STS.ROERR(6)
    VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR            = 1 << 6,
    // 4: SPI_STS.CCERR(4)
    VSF_SPI_IRQ_MASK_CRC_ERR                    = 1 << 4,

    // more vendor specified irq_masks can be added here
} vsf_spi_irq_mask_t;
#endif

// HW/IPCore end

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_SPI
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_SPI_H__
/* EOF */
