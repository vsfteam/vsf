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

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_LED_SCAN == ENABLED

#define __VSF_LED_SCAN_CLASS_IMPLEMENT
#include "./vsf_led_scan.h"

/*============================ MACROS ========================================*/

#ifndef VSF_LED_SCAN_CFG_INTERVAL_MS
#   define VSF_LED_SCAN_CFG_INTERVAL_MS     1
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_led_scan_on_timer(vsf_callback_timer_t *timer)
{
    vsf_led_scan_t *scan = container_of(timer, vsf_led_scan_t, timer);
    const vsf_io_mapper_t *io_mapper = scan->hw->io_mapper;
    const vsf_led_scan_pin_t *pin = &scan->hw->pins[scan->cur_pin];
    vsf_protect_t orig;

    // set current pin FLOATING
    vsf_io_mapper_set_input(io_mapper, pin->pin_set);
    vsf_io_mapper_set_input(io_mapper, pin->pin_clr);

next_led:
    orig = vsf_protect_int();
    if (0 == scan->value) {
        scan->is_running = false;
        vsf_unprotect_int(orig);
    } else {
        vsf_unprotect_int(orig);
        if (++scan->cur_pin >= scan->hw->led_num) {
            scan->cur_pin = 0;
        }

        if (!(scan->value & (1 << scan->cur_pin))) {
            goto next_led;
        }

        pin = &scan->hw->pins[scan->cur_pin];
        vsf_io_mapper_set_output(io_mapper, pin->pin_set);
        vsf_io_mapper_set_output(io_mapper, pin->pin_clr);
        vsf_io_mapper_set(io_mapper, pin->pin_set);
        vsf_io_mapper_clear(io_mapper, pin->pin_clr);

        vsf_callback_timer_add_ms(timer, VSF_LED_SCAN_CFG_INTERVAL_MS);
    }
}

static void __vsf_led_scan_check_start(vsf_led_scan_t *scan)
{
    vsf_protect_t orig = vsf_protect_int();
    if ((scan->value != 0) && !scan->is_running) {
        scan->is_running = true;
        vsf_unprotect_int(orig);
        vsf_callback_timer_add_ms(&scan->timer, VSF_LED_SCAN_CFG_INTERVAL_MS);
    } else {
        vsf_unprotect_int(orig);
    }
}

void vsf_led_scan_config_pin(vsf_led_scan_t *scan, uint32_t pin_mask, uint32_t feature)
{
}

void vsf_led_scan_set_direction(vsf_led_scan_t *scan, uint32_t direction_mask, uint32_t pin_mask)
{
    VSF_HAL_ASSERT((direction_mask & pin_mask) == pin_mask);

    if (!scan->is_inited) {
        scan->is_inited = true;

        const vsf_led_scan_hw_t *hw = scan->hw;
        for (uint8_t i = 0; i < hw->led_num; i++) {
            vsf_io_mapper_set_input(hw->io_mapper, hw->pins[i].pin_set);
            vsf_io_mapper_set_input(hw->io_mapper, hw->pins[i].pin_clr);
        }

        vsf_callback_timer_init(&scan->timer);
        scan->timer.on_timer = __vsf_led_scan_on_timer;
    }
}

uint32_t vsf_led_scan_get_direction(vsf_led_scan_t *scan, uint32_t pin_mask)
{
    return pin_mask;
}

void vsf_led_scan_switch_direction(vsf_led_scan_t *scan, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(false);
}

void vsf_led_scan_set_input(vsf_led_scan_t *scan, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(false);
}

void vsf_led_scan_set_output(vsf_led_scan_t *scan, uint32_t pin_mask)
{
    vsf_led_scan_set_direction(scan, pin_mask, pin_mask);
}

uint32_t vsf_led_scan_read(vsf_led_scan_t *scan)
{
    return scan->value;
}

void vsf_led_scan_write(vsf_led_scan_t *scan, uint32_t value, uint32_t pin_mask)
{
    scan->value &= ~pin_mask;
    scan->value |= value & pin_mask;
    __vsf_led_scan_check_start(scan);
}

void vsf_led_scan_set(vsf_led_scan_t *scan, uint32_t pin_mask)
{
    scan->value |= pin_mask;
    __vsf_led_scan_check_start(scan);
}

void vsf_led_scan_clear(vsf_led_scan_t *scan, uint32_t pin_mask)
{
    scan->value &= ~pin_mask;
}

void vsf_led_scan_toggle(vsf_led_scan_t *scan, uint32_t pin_mask)
{
    scan->value ^= pin_mask;
    __vsf_led_scan_check_start(scan);
}

#endif      // VSF_USE_LED_SCAN
