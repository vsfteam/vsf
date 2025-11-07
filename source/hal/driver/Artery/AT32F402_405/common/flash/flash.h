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
 *  See the License for the flashecific language governing permissions and   *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_FLASH_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_FLASH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_FLASH == ENABLED

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
/*\note hw FLASH driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS for vsf_flash_status_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_flash_irq_mask_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_CTRL for vsf_flash_ctrl_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG for vsf_flash_cfg_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_flash_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED

// HW end

/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_flash_irq_mask_t {
    // 1: RESERVED in FLASH_STS
    VSF_FLASH_IRQ_ERASE_MASK        = (1 << 1),
    // 3: RESERVED in FLASH_STS
    VSF_FLASH_IRQ_WRITE_MASK        = (1 << 3),
    // 6: RESERVED in FLASH_STS
    VSF_FLASH_IRQ_READ_MASK         = (1 << 6),
    // 4: EPPERR(4) in FLASH_STS
    VSF_FLASH_IRQ_ERASE_ERROR_MASK  = (1 << 4),
    // 2|4: PRGMERR(2)|EPPERR(4) in FLASH_STS
    VSF_FLASH_IRQ_WRITE_ERROR_MASK  = (1 << 2) | (1 << 4),
    // 7: RESERVED in FLASH_STS
    VSF_FLASH_IRQ_READ_ERROR_MASK   = (1 << 7),
} vsf_flash_irq_mask_t;
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_flash_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_flash_status_t;
#endif
// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_FLASH
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_FLASH_H__
/* EOF */
