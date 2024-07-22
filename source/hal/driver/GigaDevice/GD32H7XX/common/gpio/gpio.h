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

#ifndef __HAL_DRIVER_GIGADEVICE_GD32H7XX_GPIO_H__
#define __HAL_DRIVER_GIGADEVICE_GD32H7XX_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 *
 *      Usually, there is no IPCore driver for GPIO.
 */

/*\note Includes CAN ONLY be put here. */

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
// HW GPIO has no interrupt for GD32H7XX
//#define VSF_GPIO_CFG_REIMPLEMENT_TYPE_EXT_MODE  ENABLED
#define VSF_GPIO_USE_IO_MODE_TYPE               ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_gpio_t vsf_hw_gpio_t;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

// __vsf_hw_gpio_get_regbase is used in io module to access af registers
extern uint32_t __vsf_hw_gpio_get_regbase(vsf_hw_gpio_t *gpio_ptr);

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_GPIO
#endif      // __HAL_DRIVER_GIGADEVICE_GD32H7XX_GPIO_H__
/* EOF */
