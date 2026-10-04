/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  You may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *****************************************************************************/

#ifndef __VSF_INPUT_GT9XX_H__
#define __VSF_INPUT_GT9XX_H__

/*  Driver for GT9xx series from Goodix(GT911/GT9147/GT9157/GT1151...)  */

/*============================ INCLUDES ======================================*/

#include "component/input/vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_GT9XX == ENABLED

#include "hal/vsf_hal.h"
#include "kernel/vsf_kernel.h"

#if     defined(__VSF_INPUT_GT9XX_CLASS_IMPLEMENT)
#   undef __VSF_INPUT_GT9XX_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_I2C != ENABLED
#   error Touch Screen chip GT9xx series need I2C enabled
#endif

// frame trigger selection: with EXTI support compiled in, a board whose
// TP_INT pad is wired sets gpio_int and runs interrupt driven; gpio_int
// NULL(boards without a routed data-ready pad) makes the driver poll with
// an internal callback timer instead
#ifndef VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI
#   define VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI       VSF_HAL_USE_GPIO
#endif

// poll period in milliseconds(poll mode only)
#ifndef VSF_INPUT_GT9XX_CFG_POLL_MS
#   define VSF_INPUT_GT9XX_CFG_POLL_MS            15
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_input_gt9xx_t) {
    public_member(
        union {
            vsf_i2c_regacc_t regacc;
            vsf_i2c_regacc_t;
        };

#if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI == ENABLED
        // data-ready interrupt pad. NULL selects poll mode. the driver does
        // not run the vendor power-on sequence itself: the reset rising
        // edge latches one of the two slave addresses with the int pad
        // level, so boards that wire the reset output run their sequence
        // BEFORE vk_input_gt9xx_init and the driver probes both latched
        // addresses instead
        vsf_gpio_t *gpio_int;
        uint8_t gpio_int_pin;
#endif

        uint16_t width;
        uint16_t height;

        // probe outcome, polled from THREAD context: the driver NEVER
        // traces from its interrupt paths - vsf_trace is not isr-safe on
        // every config, so consumers read these flags instead.
        // is_ready=1 after a product-id match, probe_failed=1 after both
        // latched slave addresses stayed silent
        uint8_t is_ready;
        uint8_t probe_failed;
    )
    private_member(
        // status byte + up to 5 points x 8 bytes, one regacc read from
        // the status register(the point fifo sits right behind it)
        uint8_t buffer[1 + 5 * 8];
        uint8_t last_down;
        uint8_t addr_idx;
        uint8_t state;
        vsf_arch_prio_t prio;
        vsf_callback_timer_t poll_timer;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

// asynchronous init: i2c master + product-id probe over both latched slave
// addresses, then arms the frame trigger - the int pad EXTI when wired, a
// periodic timer otherwise(all flow afterwards is interrupt driven)
extern vsf_err_t vk_input_gt9xx_init(vk_input_gt9xx_t *gt9xx, vsf_arch_prio_t prio);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_INPUT && VSF_INPUT_USE_GT9XX
#endif      // __VSF_INPUT_GT9XX_H__
