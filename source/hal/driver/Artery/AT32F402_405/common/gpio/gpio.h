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

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_GPIO_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

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
 *      Usually, there is no IPCore driver for GPIO.
 */

/*\note Includes CAN ONLY be put here. */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// HW
/*\note hw GPIO driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE for vsf_gpio_mode_t
 *          VSF_GPIO_CFG_REIMPLEMENT_TYPE_CFG for vsf_gpio_cfg_t
 *          VSF_GPIO_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_gpio_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
// HW end

// AF_MODE, internal usage only
#define __VSF_HW_AF_MODE_OUTPUT_PP      1
#define __VSF_HW_AF_MODE_OUTPUT_PP_PU   2
#define __VSF_HW_AF_MODE_OUTPUT_PP_PD   3
#define __VSF_HW_AF_MODE_OUTPUT_OD      4
#define __VSF_HW_AF_MODE_OUTPUT_OD_PU   5
#define __VSF_HW_AF_MODE_OUTPUT_OD_PD   6
#define __VSF_HW_AF_MODE_INPUT          7
#define __VSF_HW_AF_MODE_INPUT_PU       8
#define __VSF_HW_AF_MODE_INPUT_PD       9

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_gpio_mode_t {
    // GPIO_CFGR.IOMC:  bit0..bit1
    // GPIO_OMODE.OM:   bit2
    VSF_GPIO_INPUT                      = (0 << 0),
    VSF_GPIO_ANALOG                     = (3 << 0),
    __VSF_GPIO_AF                       = (2 << 0),
    VSF_GPIO_OUTPUT_PUSH_PULL           = (1 << 0) | (0 << 2),
    VSF_GPIO_OUTPUT_OPEN_DRAIN          = (1 << 0) | (1 << 2),
    __VSF_GPIO_EXTI                     = (1 << 8),
    VSF_GPIO_EXTI                       = VSF_GPIO_INPUT | __VSF_GPIO_EXTI,

    VSF_GPIO_AF_OUTPUT_PUSH_PULL        = __VSF_GPIO_AF | (0 << 2),
    VSF_GPIO_AF_OUTPUT_OPEN_DRAIN       = __VSF_GPIO_AF | (1 << 2),
    VSF_GPIO_AF_INPUT                   = __VSF_GPIO_AF /*| (x[0|1] << 2)*/,
#define VSF_GPIO_AF_OUTPUT_PUSH_PULL    VSF_GPIO_AF_OUTPUT_PUSH_PULL
#define VSF_GPIO_AF_OUTPUT_OPEN_DRAIN   VSF_GPIO_AF_OUTPUT_OPEN_DRAIN
#define VSF_GPIO_AF_INPUT               VSF_GPIO_AF_INPUT

    // GPIO_PULL.PULL:  bit3..bit4
    VSF_GPIO_NO_PULL_UP_DOWN            = (0 << 3),
    VSF_GPIO_PULL_UP                    = (1 << 3),
    VSF_GPIO_PULL_DOWN                  = (2 << 3),

    // GPIO_ODRVR.ODRV: bit5..bit6
    // GPIO_HDRV.HDRV:  bit7
    VSF_GPIO_DRIVE_STRENGTH_LOW         = (0 << 5) | (0 << 7),
    VSF_GPIO_DRIVE_STRENGTH_MEDIUM      = (0 << 5) | (1 << 7),
    VSF_GPIO_DRIVE_STRENGTH_HIGH        = (1 << 5) | (1 << 7),
    VSF_GPIO_DRIVE_STRENGTH_MASK        = (3 << 5) | (1 << 7),
#define VSF_GPIO_DRIVE_STRENGTH_LOW     VSF_GPIO_DRIVE_STRENGTH_LOW
#define VSF_GPIO_DRIVE_STRENGTH_MEDIUM  VSF_GPIO_DRIVE_STRENGTH_MEDIUM
#define VSF_GPIO_DRIVE_STRENGTH_HIGH    VSF_GPIO_DRIVE_STRENGTH_HIGH
#define VSF_GPIO_DRIVE_STRENGTH_MASK    VSF_GPIO_DRIVE_STRENGTH_MASK

    // todo: exti support
    VSF_GPIO_EXTI_MODE_NONE             = 0,
    VSF_GPIO_EXTI_MODE_LOW_LEVEL        = 1 << 16,
    VSF_GPIO_EXTI_MODE_HIGH_LEVEL       = 2 << 16,
    VSF_GPIO_EXTI_MODE_RISING           = 3 << 16,
    VSF_GPIO_EXTI_MODE_FALLING          = 4 << 16,
    VSF_GPIO_EXTI_MODE_RISING_FALLING   = 5 << 16,
} vsf_gpio_mode_t;
#endif

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_GPIO
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_GPIO_H__
/* EOF */
