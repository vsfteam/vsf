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

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_EXTI_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_EXTI_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_EXTI == ENABLED

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
 *
 *      Usually, there is no IPCore driver for EXTI.
 */

/*\note Includes CAN ONLY be put here. */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// HW
/*\note hw EXTI driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_EXTI_CFG_REIMPLEMENT_TYPE_MODE for vsf_exti_mode_t
 *          VSF_EXTI_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_exti_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_EXTI_CFG_REIMPLEMENT_TYPE_MODE          ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_EXTI_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_exti_mode_t {
    VSF_EXTI_MODE_NONE                              = 0,
    VSF_EXTI_MODE_RISING                            = 1 << 0,
    VSF_EXTI_MODE_FALLING                           = 1 << 1,
    VSF_EXTI_MODE_RISING_FALLING                    = VSF_EXTI_MODE_RISING | VSF_EXTI_MODE_FALLING,
} vsf_exti_mode_t;
#endif

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_EXTI
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_EXTI_H__
/* EOF */
