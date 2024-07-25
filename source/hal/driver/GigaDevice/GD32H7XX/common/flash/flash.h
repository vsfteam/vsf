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

#ifndef __HAL_DRIVER_GIGADEVICE_GD32H7XX_FLASH_H__
#define __HAL_DRIVER_GIGADEVICE_GD32H7XX_FLASH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_FLASH == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED

/*============================ TYPES =========================================*/

typedef enum vsf_flash_irq_mask_t {
    // 16: ENDIE(16) IN FMC_CTL
    VSF_FLASH_IRQ_ERASE_MASK            = (1 << 16),
    // 15: ENDIE(16) IN FMC_CTL, shift right by 1 to avoid conflict with VSF_FLASH_IRQ_ERASE_MASK
    VSF_FLASH_IRQ_WRITE_MASK            = (1 << 16) >> 1,
    // 14
    VSF_FLASH_IRQ_READ_MASK             = (1 << 14),

    // 17: WPERRIE(17) IN FMC_CTL
    VSF_FLASH_IRQ_ERASE_ERROR_MASK      = (1 << 17),
    // 17..18: GPSERRIE(18)/WPERRIE(17) IN FMC_CTL
    VSF_FLASH_IRQ_WRITE_ERROR_MASK      = (1 << 18) | (1 << 17),
    // 23..24: RSERRIE(24)/RPERRIE(23) IN GMC_CTL
    VSF_FLASH_IRQ_READ_ERROR_MASK       = (1 << 24) | (1 << 23),

    __VSF_HW_FLASH_ERR_IRQ_MASK         = VSF_FLASH_IRQ_ERASE_ERROR_MASK
                                        | VSF_FLASH_IRQ_WRITE_ERROR_MASK
                                        | VSF_FLASH_IRQ_READ_ERROR_MASK,
    __VSF_HW_FLASH_IRQ_MASK             = VSF_FLASH_IRQ_ERASE_MASK
                                        | VSF_FLASH_IRQ_WRITE_MASK
                                        | __VSF_HW_FLASH_ERR_IRQ_MASK,
} vsf_flash_irq_mask_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_FLASH
#endif      // __HAL_DRIVER_GIGADEVICE_GD32H7XX_FLASH_H__
/* EOF */
