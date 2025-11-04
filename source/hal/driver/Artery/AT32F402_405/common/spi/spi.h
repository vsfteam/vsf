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

#define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_spi_mode_t {
    VSF_SPI_MASTER                  = 0x00ul << 0,
    VSF_SPI_SLAVE                   = 0x01ul << 0,
    VSF_SPI_MSB_FIRST               = 0x00ul << 1,
    VSF_SPI_LSB_FIRST               = 0x01ul << 1,
    VSF_SPI_CPOL_LOW                = 0x00ul << 2,
    VSF_SPI_CPOL_HIGH               = 0x01ul << 2,
    VSF_SPI_CPHA_LOW                = 0x00ul << 2,
    VSF_SPI_CPHA_HIGH               = 0x01ul << 2,
    VSF_SPI_MODE_0                  = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_LOW,
    VSF_SPI_MODE_1                  = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_HIGH,
    VSF_SPI_MODE_2                  = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_LOW,
    VSF_SPI_MODE_3                  = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_HIGH,
    VSF_SPI_CS_SOFTWARE_MODE        = 0x00ul << 4,
    VSF_SPI_CS_HARDWARE_MODE        = 0x01ul << 4,
    VSF_SPI_DATASIZE_8              = 0x00ul << 8,
    VSF_SPI_DATASIZE_16             = 0x01ul << 8,
    VSF_SPI_DATASIZE_32             = 0x02ul << 8,

    // more vendor specified modes can be added here
} vsf_spi_mode_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_spi_irq_mask_t {
    VSF_SPI_IRQ_MASK_TX                 = 0x01ul << 0,
    VSF_SPI_IRQ_MASK_RX                 = 0x01ul << 1,
    VSF_SPI_IRQ_MASK_TX_CPL             = 0x01ul << 2,
    VSF_SPI_IRQ_MASK_RX_CPL             = 0x01ul << 3,
    VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR    = 0x01ul << 4,

    // more vendor specified irq_masks can be added here
} vsf_spi_irq_mask_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_spi_status_t {
    union {
        struct {
            uint32_t is_busy : 1;
        };
    };

    // more vendor specified status can be added here
} vsf_spi_status_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_spi_capability_t {
    vsf_spi_irq_mask_t irq_mask;
    uint8_t support_hardware_cs : 1;
    uint8_t support_software_cs : 1;
    uint8_t cs_count            : 6;
    uint32_t max_clock_hz;
    uint32_t min_clock_hz;

    // more vendor specified capability can be added here
} vsf_spi_capability_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL == ENABLED
/** \note Redefining vsf_spi_cfg_t usually requires declaring vsf_spi_t
 *        and vsf_spi_isr_handler_t types and define vsf_spi_isr_t.
 */
typedef struct vsf_spi_t vsf_spi_t;
typedef void vsf_spi_isr_handler_t(void *target_ptr,
                                   vsf_spi_t *spi_ptr,
                                   vsf_spi_irq_mask_t irq_mask);
typedef struct vsf_spi_isr_t {
    vsf_spi_isr_handler_t *handler_fn;
    void                  *target_ptr;
    vsf_arch_prio_t        prio;
} vsf_spi_isr_t;
typedef struct vsf_spi_cfg_t {
    vsf_spi_mode_t   mode;
    uint32_t         clock_hz;
    vsf_spi_isr_t    isr;
    uint8_t          auto_cs_index;

    // more vendor specified cfg can be added here
} vsf_spi_cfg_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL == ENABLED
typedef enum vsf_spi_ctrl_t {
    __VSF_SPI_CTRL_DUMMY = 0,
} vsf_spi_ctrl_t;
#endif
// HW/IPCore end

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_SPI
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_SPI_H__
/* EOF */
