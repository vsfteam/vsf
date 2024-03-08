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

#ifndef __HAL_DRIVER_${SERIES/GPIO_IP}_GPIO_H__
#define __HAL_DRIVER_${SERIES/GPIO_IP}_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripherial drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 *
 *      Usually, there is no IPCore driver for GPIO.
 */

/*\note Incudes CAN ONLY be put here. */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// HW
/*\note hw GPIO driver can reimplement vsf_gpio_mode_t/vsf_gpio_interrupt_mode_t.
 *      To enable reimplementation, please enable macro below:
 *          VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE for vsf_gpio_mode_t, mutually exclusive with VSF_GPIO_USE_IO_MODE_TYPE
 *          VSF_GPIO_CFG_REIMPLEMENT_TYPE_EXT_MODE for vsf_gpio_interrupt_mode_t
 *          VSF_GPIO_USE_IO_MODE_TYPE to use vsf_io_mode_t to implement vsf_gpio_mode_t, mutually exclusive with VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

//#define VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE      ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_TYPE_EXT_MODE  ENABLED
#define VSF_GPIO_USE_IO_MODE_TYPE               ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW
typedef enum vsf_gpio_interrupt_mode_t {
    VSF_GPIO_INT_MODE_NONE                      = (0x0 << 0),
    VSF_GPIO_INT_MODE_LOW_LEVEL                 = (0x1 << 0),
    VSF_GPIO_INT_MODE_HIGH_LEVEL                = (0x2 << 0),
    VSF_GPIO_INT_MODE_RISING                    = (0x3 << 0),
    VSF_GPIO_INT_MODE_FALLING                   = (0x4 << 0),
    VSF_GPIO_INT_MODE_RISING_FALLING            = (0x5 << 0),
} vsf_gpio_interrupt_mode_t;
// HW end

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_GPIO
#endif      // __HAL_DRIVER_${SERIES/GPIO_IP}_GPIO_H__
/* EOF */
