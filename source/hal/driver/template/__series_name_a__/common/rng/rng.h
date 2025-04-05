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

#ifndef __HAL_DRIVER_${SERIES/RNG_IP}_RNG_H__
#define __HAL_DRIVER_${SERIES/RNG_IP}_RNG_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_RNG == ENABLED

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
#if     defined(__VSF_HAL_${RNG_IP}_RNG_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${RNG_IP}_RNG_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${RNG_IP}_RNG_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_RNG_CFG_MULTI_CLASS should be in rng.c.
 */

// IPCore
#ifndef VSF_${RNG_IP}_RNG_CFG_MULTI_CLASS
#   define VSF_${RNG_IP}_RNG_CFG_MULTI_CLASS    VSF_RNG_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw RNG driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_RNG_CFG_REIMPLEMENT_TYPE_MODE for vsf_rng_mode_t
 *          VSF_RNG_CFG_REIMPLEMENT_TYPE_STATUS for vsf_rng_status_t
 *          VSF_RNG_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_rng_irq_mask_t
 *          VSF_RNG_CFG_REIMPLEMENT_TYPE_CTRL for vsf_rng_ctrl_t
 *          VSF_RNG_CFG_REIMPLEMENT_TYPE_CFG for vsf_rng_cfg_t
 *          VSF_RNG_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_rng_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_RNG_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_RNG_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_RNG_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_RNG_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_RNG_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_RNG_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${rng_ip}_rng_t) {
#if VSF_${RNG_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_rng_t               vsf_rng;
    )
#endif

/*\note You can add more member in vsf_${rng_ip}_rng_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${rng_ip}_rng_reg_t *reg;
        vsf_rng_isr_t           isr;
    )
};


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
#undef __VSF_HAL_${RNG_IP}_RNG_CLASS_IMPLEMENT
#undef __VSF_HAL_${RNG_IP}_RNG_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_RNG
#endif      // __HAL_DRIVER_${SERIES/RNG_IP}_RNG_H__
/* EOF */
