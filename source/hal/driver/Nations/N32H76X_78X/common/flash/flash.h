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

#ifndef __HAL_DRIVER_N32H76X_N32H78X_FLASH_H__
#define __HAL_DRIVER_N32H76X_N32H78X_FLASH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_FLASH == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// HW
/*\note hw FLASH driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_MODE for vsf_flash_mode_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS for vsf_flash_status_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_flash_irq_mask_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_CTRL for vsf_flash_ctrl_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG for vsf_flash_cfg_t
 *          VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_flash_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_MODE         DISABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS       DISABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK     DISABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_CTRL         DISABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG          DISABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY   DISABLED

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
#   undef VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE   ENABLED
#endif
// HW end

/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_flash_irq_mask_t {
    VSF_FLASH_IRQ_ERASE_MASK        = (0x1ul << 0),
    VSF_FLASH_IRQ_WRITE_MASK        = (0x1ul << 1),
    VSF_FLASH_IRQ_READ_MASK         = (0x1ul << 2),
    VSF_FLASH_IRQ_ERASE_ERROR_MASK  = (0x1ul << 3),
    VSF_FLASH_IRQ_WRITE_ERROR_MASK  = (0x1ul << 4),
    VSF_FLASH_IRQ_READ_ERROR_MASK   = (0x1ul << 5),
} vsf_flash_irq_mask_t;
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE == ENABLED
typedef uint_fast32_t vsf_flash_size_t;
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_flash_t vsf_flash_t;
typedef void vsf_flash_isr_handler_t(void *target_ptr, vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t mask);
typedef struct vsf_flash_isr_t {
    vsf_flash_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t          prio;
} vsf_flash_isr_t;
typedef struct vsf_flash_cfg_t {
    vsf_flash_isr_t isr;
} vsf_flash_cfg_t;
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

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_flash_capability_t {
#if VSF_FLASH_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_flash_irq_mask_t irq_mask;
    vsf_flash_size_t base_address;
    vsf_flash_size_t max_size;
    vsf_flash_size_t erase_sector_size;
    vsf_flash_size_t write_sector_size;
    struct {
        uint8_t none_sector_aligned_write : 1;
        uint8_t none_sector_aligned_read  : 1;
    };
} vsf_flash_capability_t;
#endif
// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_FLASH
#endif      // __HAL_DRIVER_N32H76X_N32H78X_FLASH_H__
/* EOF */
