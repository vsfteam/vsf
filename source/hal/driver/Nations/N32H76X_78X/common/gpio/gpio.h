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

#ifndef __HAL_DRIVER_N32H76X_N32H78X_GPIO_H__
#define __HAL_DRIVER_N32H76X_N32H78X_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

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
#define VSF_GPIO_CFG_REIMPLEMENT_TYPE_CFG          DISABLED
#define VSF_GPIO_CFG_REIMPLEMENT_TYPE_CAPABILITY   DISABLED
// HW end

// HW/IPCore
/*\note To redefine these two macros, vsf_gpio_pin_mask_t type must be defined according to actual situation,
 * and macro with the same name must be defined.
 */
#if (VSF_GPIO_CFG_REIMPLEMENT_TYPE_CFG == ENABLED) || (VSF_GPIO_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED)
typedef uint32_t vsf_gpio_pin_mask_t;
#define vsf_gpio_pin_mask_t vsf_gpio_pin_mask_t
#endif
// HW/IPCore end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_gpio_mode_t {
    // 0..1, GPIO_PMODE: INPUT(0)/OUTPUT(1)/AF(2)/ANALOG(3)
    // 2: GPIO_POTYPE: PUSHPULL(0)/OPENDRAIN(1)
    VSF_GPIO_INPUT                              = (0 << 0),     //!< enable input mode
    VSF_GPIO_ANALOG                             = (3 << 0),     //!< enable analog function
    VSF_GPIO_OUTPUT_PUSH_PULL                   = (1 << 0) | (0 << 2),  //!< enable output push-pull mode
    VSF_GPIO_OUTPUT_OPEN_DRAIN                  = (1 << 0) | (1 << 2),  //!< enable output open-drain mode
    VSF_GPIO_AF                                 = (2 << 0),     //!< enable AF mode
    VSF_GPIO_AF_PUSH_PULL                       = (2 << 0) | (0 << 2),  //!< enable output push-pull mode
    VSF_GPIO_AF_OPEN_DRAIN                      = (2 << 0) | (1 << 2),  //!< enable output open-drain mode
    VSF_GPIO_EXTI                               = VSF_GPIO_INPUT,

    // 3..4, GPIO_PUPD: FLOATING(0)/PULLUP(1)/PULLDOWN(2)
    VSF_GPIO_NO_PULL_UP_DOWN                    = (0 << 3),     //!< enable floating
    VSF_GPIO_PULL_UP                            = (1 << 3),     //!< enable pull-up resistor
    VSF_GPIO_PULL_DOWN                          = (2 << 3),     //!< enable pull-down resistor

    // 5, GPIO_SR: FAST(0)/SLOW(1)
    VSF_GPIO_SPEED_LOW                          = (1 << 5),
    VSF_GPIO_SPEED_HIGH                         = (0 << 5),
    VSF_GPIO_SPEED_MASK                         = VSF_GPIO_SPEED_LOW
                                                | VSF_GPIO_SPEED_HIGH,
#define VSF_GPIO_SPEED_LOW                      VSF_GPIO_SPEED_LOW
#define VSF_GPIO_SPEED_HIGH                     VSF_GPIO_SPEED_HIGH
#define VSF_GPIO_SPEED_MASK                     VSF_GPIO_SPEED_MASK

    // 6..7: GPIO_DS: LOW(0), MEDIUM(2), HIGH(1), VERY_HIGH(3)
    VSF_GPIO_HIGH_DRIVE_STRENGTH_LOW            = (0 << 6),
    VSF_GPIO_HIGH_DRIVE_STRENGTH_MEDIUM         = (2 << 6),
    VSF_GPIO_HIGH_DRIVE_STRENGTH_HIGH           = (1 << 6),
    VSF_GPIO_HIGH_DRIVE_STRENGTH_VERY_HIGH      = (3 << 6),
    VSF_GPIO_HIGH_DRIVE_STRENGTH_MASK           = (3 << 6),
#define VSF_GPIO_DRIVE_STRENGTH_LOW             VSF_GPIO_DRIVE_STRENGTH_LOW
#define VSF_GPIO_DRIVE_STRENGTH_MEDIUM          VSF_GPIO_DRIVE_STRENGTH_MEDIUM
#define VSF_GPIO_DRIVE_STRENGTH_HIGH            VSF_GPIO_DRIVE_STRENGTH_HIGH
#define VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH       VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH
#define VSF_GPIO_HIGH_DRIVE_STRENGTH_MASK       VSF_GPIO_HIGH_DRIVE_STRENGTH_MASK

    // TODO: add exti support
    VSF_GPIO_EXTI_MODE_NONE                     = 0,
    VSF_GPIO_EXTI_MODE_LOW_LEVEL                = 1 << 16,
    VSF_GPIO_EXTI_MODE_HIGH_LEVEL               = 2 << 16,
    VSF_GPIO_EXTI_MODE_RISING                   = 3 << 16,
    VSF_GPIO_EXTI_MODE_FALLING                  = 4 << 16,
    VSF_GPIO_EXTI_MODE_RISING_FALLING           = 5 << 16,

    __VSF_HW_GPIO_MODE_ALL_BITS                 = 0xFF,
} vsf_gpio_mode_t;
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_gpio_t vsf_gpio_t;
typedef void vsf_gpio_exti_isr_handler_t(void *target_ptr, vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);
typedef struct vsf_gpio_exti_irq_cfg_t {
    vsf_gpio_exti_isr_handler_t *handler_fn;
    void                        *target_ptr;
    vsf_arch_prio_t              prio;
} vsf_gpio_exti_irq_cfg_t;
typedef struct vsf_gpio_cfg_t {
    vsf_gpio_mode_t     mode;
    uint16_t            alternate_function;
} vsf_gpio_cfg_t;
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_gpio_capability_t {
#if VSF_GPIO_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    uint8_t is_async                     : 1;
    uint8_t support_output_and_set       : 1;
    uint8_t support_output_and_clear     : 1;
    uint8_t support_interrupt            : 1;
    uint8_t pin_count;
    vsf_gpio_pin_mask_t pin_mask;
} vsf_gpio_capability_t;
#endif
// HW/IPCore end

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_GPIO
#endif      // __HAL_DRIVER_N32H76X_N32H78X_GPIO_H__
/* EOF */
