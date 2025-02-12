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

#ifndef __HAL_DRIVER_${SERIES/DAC_IP}_DAC_H__
#define __HAL_DRIVER_${SERIES/DAC_IP}_DAC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_DAC == ENABLED

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
#if     defined(__VSF_HAL_${DAC_IP}_DAC_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${DAC_IP}_DAC_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${DAC_IP}_DAC_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_DAC_CFG_MULTI_CLASS should be in dac.c.
 */

// IPCore
#ifndef VSF_${DAC_IP}_DAC_CFG_MULTI_CLASS
#   define VSF_${DAC_IP}_DAC_CFG_MULTI_CLASS    VSF_DAC_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw DAC driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_DAC_CFG_REIMPLEMENT_TYPE_STATUS for vsf_dac_status_t
 *          VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_dac_irq_mask_t
 *          VSF_DAC_CFG_REIMPLEMENT_TYPE_CFG for vsf_dac_cfg_t
 *          VSF_DAC_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_dac_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_DAC_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_DAC_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_DAC_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${dac_ip}_dac_t) {
#if VSF_${DAC_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_dac_t               vsf_dac;
    )
#endif

/*\note You can add more member in vsf_${dac_ip}_dac_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${dac_ip}_dac_reg_t *reg;
        vsf_dac_isr_t           isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
#if VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_dac_irq_mask_t {
    VSF_DAC_IRQ_MASK_IDLE = (0x01ul << 0),
    VSF_DAC_IRQ_MASK_CPL  = (0x1ul << 0),
} vsf_dac_irq_mask_t;
#endif

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_dac_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_dac_status_t;
#endif

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_dac_t vsf_dac_t;
typedef void vsf_dac_isr_handler_t(void *target_ptr, vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask);
typedef struct vsf_dac_isr_t {
    vsf_dac_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_dac_isr_t;
typedef struct vsf_dac_cfg_t {
    vsf_dac_isr_t   isr;
} vsf_dac_cfg_t;
#endif

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_dac_capability_t {
#if VSF_DAC_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_dac_irq_mask_t irq_mask;
    uint8_t max_resolution_bits;
    uint8_t min_resolution_bits;
    uint8_t channel_count;
} vsf_dac_capability_t;
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
#undef __VSF_HAL_${DAC_IP}_DAC_CLASS_IMPLEMENT
#undef __VSF_HAL_${DAC_IP}_DAC_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_DAC
#endif      // __HAL_DRIVER_${SERIES/DAC_IP}_DAC_H__
/* EOF */
