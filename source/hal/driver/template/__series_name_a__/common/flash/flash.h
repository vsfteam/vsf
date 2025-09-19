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

#ifndef __HAL_DRIVER_${SERIES/FLASH_IP}_FLASH_H__
#define __HAL_DRIVER_${SERIES/FLASH_IP}_FLASH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_FLASH == ENABLED

// HW/IPCore
/**
 * \note When vsf_peripheral_status_t is inherited, vsf_template_hal_driver.h needs to be included
 */
#include "hal/driver/common/template/vsf_template_hal_driver.h"
// HW/IPCore end

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

// IPCore
#if     defined(__VSF_HAL_${FLASH_IP}_FLASH_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${FLASH_IP}_FLASH_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${FLASH_IP}_FLASH_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_FLASH_CFG_MULTI_CLASS should be in flash.c.
 */

// IPCore
#ifndef VSF_${FLASH_IP}_FLASH_CFG_MULTI_CLASS
#   define VSF_${FLASH_IP}_FLASH_CFG_MULTI_CLASS    VSF_FLASH_CFG_MULTI_CLASS
#endif
// IPCore end

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

#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
#   undef VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE   ENABLED
#endif

// HW end

/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${flash_ip}_flash_t) {
#if VSF_${FLASH_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_flash_t                 vsf_flash;
    )
#endif

/*\note You can add more member in vsf_${flash_ip}_flash_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${flash_ip}_flash_reg_t *reg;
        vsf_flash_isr_t             isr;
    )
};
// IPCore end

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

// IPCore
/*\note Extern APIs for ip core diriver.
 *      There is no requirement about how APIs of IPCore drivers should be implemented.
 *      Just consider the simplicity for actual peripheral drivers.
 */
// IPCore end

#ifdef __cplusplus
}
#endif

// IPCore
#undef __VSF_HAL_${FLASH_IP}_FLASH_CLASS_IMPLEMENT
#undef __VSF_HAL_${FLASH_IP}_FLASH_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_FLASH
#endif      // __HAL_DRIVER_${SERIES/FLASH_IP}_FLASH_H__
/* EOF */
