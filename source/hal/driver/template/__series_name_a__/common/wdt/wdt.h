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

#ifndef __HAL_DRIVER_${SERIES/WDT_IP}_WDT_H__
#define __HAL_DRIVER_${SERIES/WDT_IP}_WDT_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_WDT == ENABLED

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
#if     defined(__VSF_HAL_${WDT_IP}_WDT_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${WDT_IP}_WDT_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${WDT_IP}_WDT_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_WDT_CFG_MULTI_CLASS should be in wdt.c.
 */

// IPCore
#ifndef VSF_${WDT_IP}_WDT_CFG_MULTI_CLASS
#   define VSF_${WDT_IP}_WDT_CFG_MULTI_CLASS    VSF_WDT_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw WDT driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE for vsf_wdt_mode_t
 *          VSF_WDT_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_wdt_irq_mask_t
 *          VSF_WDT_CFG_REIMPLEMENT_TYPE_CTRL for vsf_wdt_ctrl_t
 *          VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG for vsf_wdt_cfg_t
 *          VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_wdt_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${wdt_ip}_wdt_t) {
#if VSF_${WDT_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_wdt_t               vsf_wdt;
    )
#endif

/*\note You can add more member in vsf_${wdt_ip}_wdt_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${wdt_ip}_wdt_reg_t *reg;
        vsf_wdt_isr_t           isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
#if VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_wdt_mode_t {
    VSF_WDT_MODE_NO_EARLY_WAKEUP = (0 << 0),
    VSF_WDT_MODE_EARLY_WAKEUP    = (1 << 0),
    VSF_WDT_MODE_RESET_NONE      = (0 << 1),
    VSF_WDT_MODE_RESET_CPU       = (1 << 1),
    VSF_WDT_MODE_RESET_SOC       = (2 << 1),

    // more vendor specified modes can be added here
} vsf_wdt_mode_t;
#endif

#if VSF_WDT_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_wdt_irq_mask_t {
    VSF_WDT_IRQ_MASK_OVERFLOW = (0x01 << 0),

    // more vendor specified irq_masks can be added here
} vsf_wdt_irq_mask_t;
#endif

#if VSF_WDT_CFG_REIMPLEMENT_TYPE_CTRL == ENABLED
typedef enum vsf_wdt_ctrl_t {
    __VSF_WDT_CTRL_DUMMY = 0,

    // more vendor specified ctrl can be added here
} vsf_wdt_ctrl_t;
#endif

#if VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_wdt_t vsf_wdt_t;
typedef void vsf_wdt_isr_handler_t(void *target_ptr, vsf_wdt_t *wdt_ptr);
typedef struct vsf_wdt_isr_t {
    vsf_wdt_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t        prio;
} vsf_wdt_isr_t;
typedef struct vsf_wdt_cfg_t {
    vsf_wdt_mode_t  mode;
    uint32_t        max_ms;
    uint32_t        min_ms;
    vsf_wdt_isr_t   isr;
} vsf_wdt_cfg_t;
#endif

#if VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_wdt_capability_t {
    uint8_t support_early_wakeup : 1;
    uint8_t support_reset_none   : 1;
    uint8_t support_reset_cpu    : 1;
    uint8_t support_reset_soc    : 1;
    uint8_t support_disable      : 1;
    uint8_t support_min_timeout  : 1;
    uint32_t max_timeout_ms;

    // more vendor specified capability can be added here
} vsf_wdt_capability_t;
#endif
// HW/IPCore end


/*============================ INCLUDES ======================================*/

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
#undef __VSF_HAL_${WDT_IP}_WDT_CLASS_IMPLEMENT
#undef __VSF_HAL_${WDT_IP}_WDT_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_WDT
#endif      // __HAL_DRIVER_${SERIES/WDT_IP}_WDT_H__
/* EOF */
