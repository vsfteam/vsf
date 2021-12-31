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

#ifndef __VSF_LED_SCAN_H__
#define __VSF_LED_SCAN_H__

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_LED_SCAN == ENABLED

#include "hal/vsf_hal.h"
#include "kernel/vsf_kernel.h"

#if     defined(__VSF_LED_SCAN_CLASS_IMPLEMENT)
#   undef __VSF_LED_SCAN_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __describe_led_scan_led(__pin_set, __pin_clr)                           \
    { .pin_set = (__pin_set), .pin_clr = (__pin_clr) }
#define describe_led_scan_led(__pin_set, __pin_clr)                             \
    __describe_led_scan_led((__pin_set), (__pin_clr))

#define __describe_led_scan_hw(__name, __io_mapper, __led_num, ...)             \
    typedef struct VSF_MCONNECT(__, __name, _hw_t) {                            \
        const vsf_io_mapper_t *io_mapper;                                       \
        uint8_t led_num;                                                        \
        vsf_led_scan_pin_t __pins[(__led_num)];                                 \
    } VSF_MCONNECT(__, __name, _hw_t);                                          \
    const VSF_MCONNECT(__, __name, _hw_t) VSF_MCONNECT(__, __name, _hw) = {     \
        .io_mapper      = (const vsf_io_mapper_t *)(__io_mapper),               \
        .led_num        = (__led_num),                                          \
        .__pins         = {                                                     \
            __VA_ARGS__                                                         \
        },                                                                      \
    };
#define describe_led_scan_hw(__name, __io_mapper, __led_num, ...)               \
    __describe_led_scan_hw(__name, (__io_mapper), (__led_num), __VA_ARGS__)

#define __describe_led_scan(__name, __io_mapper, __led_num, ...)                \
    describe_led_scan_hw(__name, (__io_mapper), (__led_num), __VA_ARGS__)       \
    vsf_led_scan_t __name = {                                                   \
        .hw             = (const vsf_led_scan_hw_t *)&VSF_MCONNECT(__, __name, _hw),\
    };
#define describe_led_scan(__name, __io_mapper, __led_num, ...)                  \
    __describe_led_scan(__name, (__io_mapper), (__led_num), __VA_ARGS__)

/*============================ TYPES =========================================*/

typedef struct vsf_led_scan_pin_t {
    uint8_t pin_set;
    uint8_t pin_clr;
} vsf_led_scan_pin_t;

typedef struct vsf_led_scan_hw_t {
    const vsf_io_mapper_t *io_mapper;
    uint8_t led_num;
    vsf_led_scan_pin_t pins[0];
} vsf_led_scan_hw_t;

vsf_class(vsf_led_scan_t) {
    public_member(
        const vsf_led_scan_hw_t *hw;
    )
    private_member(
        uint32_t value;
        uint8_t cur_pin;
        bool is_inited;
        bool is_running;
        vsf_callback_timer_t timer;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_led_scan_config_pin(vsf_led_scan_t *scan, uint32_t pin_mask, uint32_t feature);
extern void vsf_led_scan_set_direction(vsf_led_scan_t *scan, uint32_t direction_mask, uint32_t pin_mask);
extern uint32_t vsf_led_scan_get_direction(vsf_led_scan_t *scan, uint32_t pin_mask);
extern void vsf_led_scan_switch_direction(vsf_led_scan_t *scan, uint32_t pin_mask);
extern void vsf_led_scan_set_input(vsf_led_scan_t *scan, uint32_t pin_mask);
extern void vsf_led_scan_set_output(vsf_led_scan_t *scan, uint32_t pin_mask);
extern uint32_t vsf_led_scan_read(vsf_led_scan_t *scan);
extern void vsf_led_scan_write(vsf_led_scan_t *scan, uint32_t value, uint32_t pin_mask);
extern void vsf_led_scan_set(vsf_led_scan_t *scan, uint32_t pin_mask);
extern void vsf_led_scan_clear(vsf_led_scan_t *scan, uint32_t pin_mask);
extern void vsf_led_scan_toggle(vsf_led_scan_t *scan, uint32_t pin_mask);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_LED_SCAN
#endif      // __VSF_LED_SCAN_H__
/* EOF */
