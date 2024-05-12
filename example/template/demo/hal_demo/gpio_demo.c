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

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_GPIO_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED

#    if VSF_USE_LINUX == ENABLED
#        include <getopt.h>
#    endif

#    include "hal_demo.h"

/*============================ MACROS ========================================*/

#    ifdef APP_GPIO_DEMO_CFG_GPIO_PREFIX
#        undef VSF_GPIO_CFG_PREFIX
#        define VSF_GPIO_CFG_PREFIX APP_GPIO_DEMO_CFG_GPIO_PREFIX
#    endif

#   ifndef APP_GPIO_DEMO_CFG_DEVICES_COUNT
#       define APP_GPIO_DEMO_CFG_DEVICES_COUNT              1
#   endif

#   ifndef APP_GPIO_DEMO_CFG_DEVICES_ARRAY_INIT
#       define APP_GPIO_DEMO_CFG_DEVICES_ARRAY_INIT            \
            { .cnt = dimof(vsf_hw_gpio_devices), .devices = vsf_hw_gpio_devices},
#   endif

#    ifndef APP_GPIO_DEMO_CFG_DEFAULT_INSTANCE
#        define APP_GPIO_DEMO_CFG_DEFAULT_INSTANCE          vsf_hw_gpio0
#    endif

#    ifndef APP_GPIO_DEMO_CFG_DEFAULT_INPUT_PORT
#        define APP_GPIO_DEMO_CFG_DEFAULT_INPUT_PORT        APP_GPIO_DEMO_CFG_DEFAULT_INSTANCE
#    endif

#    ifndef APP_GPIO_DEMO_CFG_DEFAULT_OUTPUT_PORT
#        define APP_GPIO_DEMO_CFG_DEFAULT_OUTPUT_PORT       APP_GPIO_DEMO_CFG_DEFAULT_INSTANCE
#    endif

#    ifndef APP_GPIO_DEMO_CFG_DEFAULT_INPUT_PIN_MASK
#        define APP_GPIO_DEMO_CFG_DEFAULT_INPUT_PIN_MASK    VSF_PIN10_MASK
#    endif

#    ifndef APP_GPIO_DEMO_CFG_DEFAULT_OUTPUT_PIN_MASK
#        define APP_GPIO_DEMO_CFG_DEFAULT_OUTPUT_PIN_MASK   VSF_PIN3_MASK
#    endif

#    ifndef APP_GPIO_DEMO_CFG_INPUT_MODE
#        define APP_GPIO_DEMO_CFG_INPUT_MODE                (VSF_GPIO_PULL_UP)
#    endif

#    ifndef APP_GPIO_DEMO_CFG_OUTPUT_MODE
#        define APP_GPIO_DEMO_CFG_OUTPUT_MODE               (VSF_GPIO_PULL_UP)
#    endif

#    ifndef APP_GPIO_DEMO_DELAY_MS
#        define APP_GPIO_DEMO_DELAY_MS                      500
#    endif

#    ifndef APP_GPIO_DEMO_TOGGLE_CNT
#        define APP_GPIO_DEMO_TOGGLE_CNT                    10
#    endif

#    ifndef APP_GPIO_DEMO_CFG_GPIO_INSTANCE_MASK
#        define APP_GPIO_DEMO_CFG_GPIO_INSTANCE_MASK        VSF_HAL_COUNT_TO_MASK(APP_GPIO_DEMO_CFG_GPIO_INSTANCE_COUNT)
#    endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum gpio_demo_method_t {
    METHOD_CONFIG_PIN               = 0x0 << 0,
    METHOD_NO_CONFIG_PIN            = 0x1 << 0,
    METHOD_CONFIG_PIN_MASK          = 0x1 << 0,

    METHOD_CONFIRM_PIN              = 0x0 << 1,
    METHOD_NO_CONFIRM_PIN           = 0x1 << 1,
    METHOD_CONFIRM_PIN_MASK         = 0x1 << 1,

    METHOD_DIR_SET                  = 0x0 << 2,
    METHOD_DIR_SET_INPUT_OUTPUT     = 0x1 << 2,
    METHOD_DIR_SWITCH_DIR           = 0x2 << 2,
    METHOD_DIR_NO_SET               = 0x3 << 2,
    METHOD_DIR_MASK                 = 0x3 << 2,

    METHOD_OUT_WRITE                = 0x0 << 4,
    METHOD_OUT_SET_CLEAR            = 0x1 << 4,
    METHOD_OUT_TOGGLE               = 0x2 << 4,
    METHOD_OUT_MASK                 = 0x3 << 4,

    METHOD_OUTPUT_AND_SET           = 0x1 << 6,
    METHOD_OUTPUT_AND_CLEAR         = 0x1 << 7,

    METHOD_PIN_INT_NONE             = 0x0 << 8,
    METHOD_PIN_INT_LOW_LEVEL        = 0x1 << 8,
    METHOD_PIN_INT_HIGH_LEVEL       = 0x2 << 8,
    METHOD_PIN_INT_RISING           = 0x3 << 8,
    METHOD_PIN_INT_FALLING          = 0x4 << 8,
    METHOD_PIN_INT_RISING_FALLING   = 0x5 << 8,
    METHOD_PIN_INT_MASK             = 0x7 << 8,
} gpio_demo_method_t;

// TODO: use vsf_gpio_config or vsf_io_config
// TODO: cap: read out pin is (real pin or out reg)

typedef struct gpio_test_t {
    implement(hal_test_t)

    struct {
        vsf_gpio_t *gpio;
        vsf_gpio_mode_t mode;
        vsf_gpio_pin_mask_t pin_mask;
        char name[HAL_DEMO_DEVICE_NAME_MAX_LENGTH];
    } in, out;

    vsf_gpio_interrupt_mode_t int_mode;

    bool is_output_high;

} gpio_test_t;

typedef struct gpio_demo_const_t {
    implement(hal_demo_const_t)
    gpio_test_t test;
} gpio_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_CONFIG_PIN_MASK, METHOD_CONFIG_PIN,             "confin"),
    HAL_DEMO_OPTION_EX(METHOD_CONFIG_PIN_MASK, METHOD_NO_CONFIG_PIN,          "no-config"),

    HAL_DEMO_OPTION_EX(METHOD_CONFIRM_PIN_MASK, METHOD_CONFIRM_PIN,           "read-dir"),
    HAL_DEMO_OPTION_EX(METHOD_CONFIRM_PIN_MASK, METHOD_NO_CONFIRM_PIN,        "no-read-dir"),

    HAL_DEMO_OPTION_EX(METHOD_DIR_MASK, METHOD_DIR_SET,                       "set-dir"),
    HAL_DEMO_OPTION_EX(METHOD_DIR_MASK, METHOD_DIR_SET_INPUT_OUTPUT,          "set-input-output"),
    HAL_DEMO_OPTION_EX(METHOD_DIR_MASK, METHOD_DIR_SWITCH_DIR,                "switch-dir"),
    HAL_DEMO_OPTION_EX(METHOD_DIR_MASK, METHOD_DIR_NO_SET,                    "not-set"),

    HAL_DEMO_OPTION_EX(METHOD_OUT_MASK, METHOD_OUT_WRITE,                     "write"),
    HAL_DEMO_OPTION_EX(METHOD_DIR_MASK, METHOD_OUT_SET_CLEAR,                 "set-clear"),
    HAL_DEMO_OPTION_EX(METHOD_DIR_MASK, METHOD_OUT_TOGGLE,                    "toggle"),

    HAL_DEMO_OPTION_EX(METHOD_OUTPUT_AND_SET, METHOD_OUTPUT_AND_SET,          "output-and-set"),
    HAL_DEMO_OPTION_EX(METHOD_OUTPUT_AND_CLEAR, METHOD_OUTPUT_AND_CLEAR,      "output-and-clear"),

    HAL_DEMO_OPTION_EX(METHOD_PIN_INT_MASK, METHOD_PIN_INT_NONE,              "pin-int-none"),
    HAL_DEMO_OPTION_EX(METHOD_PIN_INT_MASK, METHOD_PIN_INT_LOW_LEVEL,         "pin-int-low"),
    HAL_DEMO_OPTION_EX(METHOD_PIN_INT_MASK, METHOD_PIN_INT_HIGH_LEVEL,        "pin-int-high"),
    HAL_DEMO_OPTION_EX(METHOD_PIN_INT_MASK, METHOD_PIN_INT_RISING,            "pin-int-rising"),
    HAL_DEMO_OPTION_EX(METHOD_PIN_INT_MASK, METHOD_PIN_INT_FALLING,           "pin-int-falling"),
    HAL_DEMO_OPTION_EX(METHOD_PIN_INT_MASK, METHOD_PIN_INT_RISING_FALLING,    "pin-int-rising-falling"),
};

static const hal_option_t __mode_options[] = {
    HAL_DEMO_OPTION(VSF_GPIO_OUTPUT_MASK, VSF_GPIO_PULL_UP),
    HAL_DEMO_OPTION(VSF_GPIO_OUTPUT_MASK, VSF_GPIO_PULL_DOWN),
    HAL_DEMO_OPTION(VSF_GPIO_OUTPUT_MASK, VSF_GPIO_OPEN_DRAIN),

    HAL_DEMO_OPTION(VSF_GPIO_INPUT_MASK, VSF_GPIO_NORMAL_INPUT),
    HAL_DEMO_OPTION(VSF_GPIO_INPUT_MASK, VSF_GPIO_INVERT_INPUT),
    HAL_DEMO_OPTION(VSF_GPIO_INPUT_MASK, VSF_GPIO_DISABLE_INPUT),

    HAL_DEMO_OPTION(VSF_GPIO_HIGH_DRIVE_STRENGTH, VSF_GPIO_HIGH_DRIVE_STRENGTH),

    HAL_DEMO_OPTION(VSF_GPIO_FILTER_MASK, VSF_GPIO_FILTER_BYPASS),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_MASK, VSF_GPIO_FILTER_2CLK),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_MASK, VSF_GPIO_FILTER_4CLK),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_MASK, VSF_GPIO_FILTER_8CLK),

    HAL_DEMO_OPTION(VSF_GPIO_FILTER_CLK_MASK, VSF_GPIO_FILTER_CLK_SRC0),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_CLK_MASK, VSF_GPIO_FILTER_CLK_SRC1),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_CLK_MASK, VSF_GPIO_FILTER_CLK_SRC2),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_CLK_MASK, VSF_GPIO_FILTER_CLK_SRC3),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_CLK_MASK, VSF_GPIO_FILTER_CLK_SRC4),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_CLK_MASK, VSF_GPIO_FILTER_CLK_SRC5),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_CLK_MASK, VSF_GPIO_FILTER_CLK_SRC6),
    HAL_DEMO_OPTION(VSF_GPIO_FILTER_CLK_MASK, VSF_GPIO_FILTER_CLK_SRC7),

    HAL_DEMO_OPTION(VSF_GPIO_HIGH_DRIVE_STRENGTH_MASK, VSF_GPIO_HIGH_DRIVE_STRENGTH),
    HAL_DEMO_OPTION(VSF_GPIO_HIGH_DRIVE_STRENGTH_MASK, VSF_IO_HIGH_DRIVE_NO_STRENGTH),

    HAL_DEMO_OPTION(VSF_GPIO_EXTI_MASK, VSF_GPIO_EXTI_DISABLED),
    HAL_DEMO_OPTION(VSF_GPIO_EXTI_MASK, VSF_GPIO_EXTI_ENABLED),
};

static const hal_option_t __int_mode_options[] = {
    HAL_DEMO_OPTION(VSF_GPIO_INT_MODE_MASK, VSF_GPIO_INT_MODE_NONE),
    HAL_DEMO_OPTION(VSF_GPIO_INT_MODE_MASK, VSF_GPIO_INT_MODE_LOW_LEVEL),
    HAL_DEMO_OPTION(VSF_GPIO_INT_MODE_MASK, VSF_GPIO_INT_MODE_HIGH_LEVEL),
    HAL_DEMO_OPTION(VSF_GPIO_INT_MODE_MASK, VSF_GPIO_INT_MODE_RISING),
    HAL_DEMO_OPTION(VSF_GPIO_INT_MODE_MASK, VSF_GPIO_INT_MODE_FALLING),
    HAL_DEMO_OPTION(VSF_GPIO_INT_MODE_MASK, VSF_GPIO_INT_MODE_RISING_FALLING),
};

HAL_DEMO_INIT(gpio, APP_GPIO,
    "gpio-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT            repeat count" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT          test timeout" VSF_TRACE_CFG_LINEEND
    "  -m, --method MODE" VSF_TRACE_CFG_LINEEND
    "     METHOD: [confin|no-config] [read-dir|no-read-dir] [set-dir|set-input-output|switch-dir|not-set]" VSF_TRACE_CFG_LINEEND
    "             [wirte|set-clear-toggle] [output-and-set|output-and-clear] [pin-int-[none|low|high|rising|falling|rising-falling]]" VSF_TRACE_CFG_LINEEND
    "  -i, --input DEVICE PIN_MASK MODE" VSF_TRACE_CFG_LINEEND
    "  -o, --output DEVICE PIN_MASK MODE" VSF_TRACE_CFG_LINEEND
    "     DEVICE: i.e. vsf_hw_gpio0" VSF_TRACE_CFG_LINEEND
    "     PIN_MASK: i.e. vsf_hw_gpio0" VSF_TRACE_CFG_LINEEND
    "     MODE: @ref \"vsf_gpio_mode_t\"" VSF_TRACE_CFG_LINEEND,


    .test.int_mode = VSF_GPIO_INT_MODE_NONE,
    .test.in.gpio = (vsf_gpio_t *)&APP_GPIO_DEMO_CFG_DEFAULT_INPUT_PORT,
    .test.in.mode = APP_GPIO_DEMO_CFG_INPUT_MODE,
    .test.out.gpio = (vsf_gpio_t *)&APP_GPIO_DEMO_CFG_DEFAULT_INPUT_PORT,
    .test.out.mode = APP_GPIO_DEMO_CFG_INPUT_MODE,

    .method.options = __method_options,
    .method.cnt     = dimof(__method_options),
);
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static bool __gpio_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    gpio_test_t *test = vsf_container_of(hal_test, gpio_test_t, use_as__hal_test_t);
    VSF_ASSERT(test->device != NULL);

    vsf_gpio_capability_t in_cap;
    vsf_gpio_capability_t out_cap;

    if ((test->in.gpio == NULL) && (test->out.gpio == NULL)) {
        vsf_trace_error("Specify at least one input or output device!" VSF_TRACE_CFG_LINEEND, test->in.pin_mask & ~in_cap.pin_mask);
        return false;
    }

    if (test->in.gpio != NULL) {
        in_cap = vsf_gpio_capability(test->in.gpio);
        if (test->in.pin_mask & ~in_cap.pin_mask) {
            vsf_trace_error("input device unsupport pin mask: 0x%08x" VSF_TRACE_CFG_LINEEND, test->in.pin_mask & ~in_cap.pin_mask);
            return false;
        }
    }

    if (test->out.gpio != NULL) {
        out_cap = vsf_gpio_capability(test->out.gpio);
        if (test->out.pin_mask & ~out_cap.pin_mask) {
            vsf_trace_error("output device unsupport pin mask: 0x%08x" VSF_TRACE_CFG_LINEEND, test->out.pin_mask & ~out_cap.pin_mask);
            return false;
        }

        if ((test->method & METHOD_OUTPUT_AND_SET) && !out_cap.support_output_and_set) {
            vsf_trace_error("output device does not support vsf_gpio_output_and_set()" VSF_TRACE_CFG_LINEEND);
            return false;
        }

        if ((test->method & METHOD_OUTPUT_AND_CLEAR) && !out_cap.support_output_and_clear) {
            vsf_trace_error("output device does not support vsf_gpio_output_and_clear()" VSF_TRACE_CFG_LINEEND);
            return false;
        }
    }

    if ((test->in.gpio != NULL) && (test->out.gpio != NULL)) {
        if (test->in.gpio != test->out.gpio) {
            if (in_cap.is_async || out_cap.is_async) {
                vsf_trace_warning("The timing of the two gpio's may not be synchronized." VSF_TRACE_CFG_LINEEND);
            }
        }

        if ((test->in.pin_mask == 0) && (test->out.pin_mask == 0)) {
            vsf_trace_error("No test pins were selected" VSF_TRACE_CFG_LINEEND);
            return false;
        }
    }

    switch (test->method & METHOD_PIN_INT_MASK) {
    case METHOD_PIN_INT_NONE:
        test->int_mode = VSF_GPIO_INT_MODE_NONE;
        test->in.mode = (test->in.mode & ~VSF_GPIO_EXTI_MASK) | VSF_GPIO_EXTI_DISABLED;
        return true;

    case METHOD_PIN_INT_LOW_LEVEL:
        test->int_mode = VSF_GPIO_INT_MODE_LOW_LEVEL;
        break;
    case METHOD_PIN_INT_HIGH_LEVEL:
        test->int_mode = VSF_GPIO_INT_MODE_HIGH_LEVEL;
        break;
    case METHOD_PIN_INT_RISING:
        test->int_mode = VSF_GPIO_INT_MODE_RISING;
        break;
    case METHOD_PIN_INT_FALLING:
        test->int_mode = VSF_GPIO_INT_MODE_FALLING;
        break;
    case METHOD_PIN_INT_RISING_FALLING:
        test->int_mode = VSF_GPIO_INT_MODE_RISING_FALLING;
        break;
    }
    test->in.mode = (test->in.mode & ~VSF_GPIO_EXTI_MASK) | VSF_GPIO_EXTI_ENABLED;

    if (!in_cap.support_interrupt) {
        vsf_trace_error("External interrupts are not supported!" VSF_TRACE_CFG_LINEEND);
        return false;
    }


    return true;
}

static bool __gpio_demo_config_pin(gpio_test_t *test)
{
    if (test->in.pin_mask != 0) {
        if ((test->method & METHOD_CONFIG_PIN_MASK) == METHOD_CONFIG_PIN) {
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_gpio_config_pin(&%s, 0x%08x/*pin mask*/, ", test->in.name, test->in.pin_mask);
                hal_options_trace(VSF_TRACE_DEBUG, "", __mode_options, dimof(__mode_options), test->in.mode);
                vsf_trace_debug(");" VSF_TRACE_CFG_LINEEND);
            }
            vsf_gpio_config_pin(test->in.gpio, test->in.pin_mask, test->in.mode);
        }

        if ((test->method & METHOD_PIN_INT_MASK) == METHOD_PIN_INT_NONE) {
            if ((test->method & METHOD_DIR_MASK) == METHOD_DIR_SET) {
                if (test->verbose >= 2) {
                    vsf_trace_debug("vsf_gpio_set_direction(&%s, 0x%08x/*pin mask*/, 0x%08x/*dir mask*/);" VSF_TRACE_CFG_LINEEND,
                                    test->in.name, test->in.pin_mask, 0);
                }
                vsf_gpio_set_direction(test->in.gpio, test->in.pin_mask, 0);
            } else if ((test->method & METHOD_DIR_MASK) == METHOD_DIR_SET_INPUT_OUTPUT) {
                if (test->verbose >= 2) {
                    vsf_trace_debug("vsf_gpio_set_input(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND,
                                    test->in.name, test->in.pin_mask);
                }
                vsf_gpio_set_input(test->in.gpio, test->in.pin_mask);
            } else if ((test->method & METHOD_DIR_MASK) == METHOD_DIR_SWITCH_DIR) {
                vsf_gpio_pin_mask_t cur_dir = vsf_gpio_get_direction(test->in.name, test->in.pin_mask);
                vsf_gpio_pin_mask_t switch_dir = test->in.pin_mask & cur_dir;  // current is 1(out)
                vsf_gpio_switch_direction(test->in.gpio, switch_dir);
                if (test->verbose >= 2) {
                    vsf_trace_debug("vsf_gpio_get_direction(&%s, 0x%08x/*pin mask*/) = 0x%08x/*cur_dir*/" VSF_TRACE_CFG_LINEEND
                                    "vsf_gpio_switch_direction(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND,
                                    test->in.name, test->in.pin_mask, cur_dir,
                                    test->in.name, switch_dir);
                }
            } else if ((test->method & METHOD_DIR_MASK) == METHOD_DIR_NO_SET) {
                // do nothing
            } else {
                VSF_ASSERT(0);
            }

            if ((test->method & METHOD_CONFIRM_PIN_MASK) == METHOD_CONFIRM_PIN) {
                vsf_gpio_pin_mask_t pin_mask = vsf_gpio_get_direction(test->in.gpio, test->in.pin_mask);
                if (test->verbose >= 2) {
                    vsf_trace_debug("vsf_gpio_get_direction(&%s, 0x%08x/*pin mask*/) = 0x%08x/*pin mask*/;" VSF_TRACE_CFG_LINEEND,
                                    test->in.name, test->in.pin_mask, pin_mask);
                }
                if (pin_mask != 0) {
                    vsf_trace_error("input direction is not as expected" VSF_TRACE_CFG_LINEEND,
                                    test->in.name, test->in.pin_mask, pin_mask);
                    return false;
                }
            }
        }
    }

    if (test->out.pin_mask != 0) {
        if ((test->method & METHOD_CONFIG_PIN_MASK) == METHOD_CONFIG_PIN) {
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_gpio_config_pin(&%s, 0x%08x/*pin mask*/, ", test->out.name, test->out.pin_mask);
                hal_options_trace(VSF_TRACE_DEBUG, "", __mode_options, dimof(__mode_options), test->out.mode);
                vsf_trace_debug(");" VSF_TRACE_CFG_LINEEND);
            }
            vsf_gpio_config_pin(test->out.gpio, test->out.pin_mask, test->out.mode);
        }

        if (test->method & METHOD_OUTPUT_AND_SET) {
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_gpio_output_and_set(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND,
                                test->out.name, test->out.pin_mask);
            }
            vsf_gpio_output_and_set(test->out.gpio, test->out.pin_mask);
        } else if (test->method & METHOD_OUTPUT_AND_CLEAR) {
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_gpio_output_and_set(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND,
                                test->out.name, test->out.pin_mask);
            }
            vsf_gpio_output_and_set(test->out.gpio, test->out.pin_mask);
        } else if ((test->method & METHOD_DIR_MASK) == METHOD_DIR_SET) {
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_gpio_set_direction(&%s, 0x%08x/*pin mask*/, 0x%08x/*dir mask*/);" VSF_TRACE_CFG_LINEEND,
                                test->out.name, test->out.pin_mask, 0);
            }
            vsf_gpio_set_direction(test->out.gpio, test->out.pin_mask, test->out.pin_mask);
        } else if ((test->method & METHOD_DIR_MASK) == METHOD_DIR_SET_INPUT_OUTPUT) {
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_gpio_set_output(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND,
                                test->out.name, test->out.pin_mask);
            }
            vsf_gpio_set_output(test->out.gpio, test->out.pin_mask);
        } else if ((test->method & METHOD_DIR_MASK) == METHOD_DIR_SWITCH_DIR) {
            vsf_gpio_pin_mask_t cur_dir = vsf_gpio_get_direction(test->out.name, test->out.pin_mask);
            vsf_gpio_pin_mask_t switch_dir = test->out.pin_mask & ~cur_dir;  // current is 0(in)
            vsf_gpio_switch_direction(test->out.name, switch_dir);
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_gpio_get_direction(&%s, 0x%08x/*pin mask*/) = 0x%08x/*cur_dir*/" VSF_TRACE_CFG_LINEEND
                                "vsf_gpio_switch_direction(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND,
                                test->out.name, test->out.pin_mask, cur_dir,
                                test->out.name, switch_dir);
            }
        } else if ((test->method & METHOD_DIR_MASK) == METHOD_DIR_NO_SET) {
            // do nothing
        } else {
            VSF_ASSERT(0);
        }

        if ((test->method & METHOD_CONFIRM_PIN_MASK) == METHOD_CONFIRM_PIN) {
            vsf_gpio_pin_mask_t pin_mask = vsf_gpio_get_direction(test->out.gpio, test->out.pin_mask);
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_gpio_get_direction(&%s, 0x%08x/*pin mask*/) = 0x%08x/*pin mask*/;" VSF_TRACE_CFG_LINEEND,
                                test->out.name, test->out.pin_mask, pin_mask);
            }
            if (pin_mask != test->out.pin_mask) {
                vsf_trace_error("output direction is not as expected!" VSF_TRACE_CFG_LINEEND);
                return false;
            }
        }
    }

    return true;
}

static void __gpio_isr_handler(void *target_ptr, vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_ASSERT(target_ptr != NULL);
    gpio_test_t *test = (gpio_test_t *)target_ptr;
    VSF_ASSERT(test->in.gpio == gpio_ptr);

    vsf_trace_debug("gpio interrupt, %s(%p), pin: 0x%08x" VSF_TRACE_CFG_LINEEND, test->in.name, gpio_ptr, pin_mask);
}

static vsf_err_t __gpio_demo_pin_interrupt_init(gpio_test_t *test, vsf_arch_prio_t arch_prio)
{
    VSF_ASSERT(NULL != test);
    vsf_gpio_t *gpio_ptr = test->in.gpio;

    vsf_err_t err = vsf_gpio_exti_irq_enable(gpio_ptr, test->in.pin_mask, arch_prio);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_gpio_exti_irq_enable(&%s, %d, vsf_arch_prio_%d) = %d/*vsf_err_t*/" VSF_TRACE_CFG_LINEEND,
                        test->in.name, test->in.pin_mask, hal_demo_arch_prio_to_num(arch_prio), err);
    }
    return err;
}

static vsf_err_t __gpio_demo_pin_interrupt_config(gpio_test_t *test, vsf_gpio_interrupt_mode_t mode)
{
    VSF_ASSERT(NULL != test);
    vsf_gpio_t *gpio_ptr = test->in.gpio;

    vsf_gpio_pin_irq_cfg_t cfg = {
        .pin_mask = test->in.pin_mask,
        .mode = mode,
        .isr.handler_fn = (mode == VSF_GPIO_INT_MODE_NONE) ? NULL : __gpio_isr_handler,
        .isr.target_ptr = (mode == VSF_GPIO_INT_MODE_NONE) ? NULL : test,
    };
    vsf_err_t err = vsf_gpio_exti_config(gpio_ptr, &cfg);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_gpio_pin_irq_cfg_t cfg = {.pin_mask = 0x%08x, .mode = ", cfg.pin_mask);
        hal_options_trace(VSF_TRACE_DEBUG, "", __int_mode_options, dimof(__int_mode_options), cfg.mode);
        vsf_trace_debug("};" VSF_TRACE_CFG_LINEEND "vsf_gpio_exti_config(&%s, &cfg) = %d/*vsf_err_t*/" VSF_TRACE_CFG_LINEEND,
                        test->in.name, err);
    }
    return err;
}

static void __gpio_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    gpio_test_t *test = vsf_container_of(eda, gpio_test_t, teda);
    vsf_gpio_t * gpio_ptr = test->device;
    VSF_ASSERT(gpio_ptr != NULL);

    switch (evt) {
    case VSF_EVT_HAL_TEST_START:
        if (!__gpio_demo_config_pin(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
            break;
        }
        if ((test->int_mode != VSF_GPIO_INT_MODE_NONE) && test->in.pin_mask) {
            if (VSF_ERR_NONE != __gpio_demo_pin_interrupt_init(test, test->isr_arch_prio)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
            if (VSF_ERR_NONE != __gpio_demo_pin_interrupt_config(test, test->int_mode)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        }
        // fall through

    case VSF_EVT_HAL_TEST_RUN:
        if ((test->out.gpio != NULL) && test->out.pin_mask) {
            if ((test->method & METHOD_OUT_MASK) == METHOD_OUT_WRITE) {
                vsf_gpio_pin_mask_t value = test->is_output_high ? test->out.pin_mask : 0;
                if (test->verbose >= 2) {
                    vsf_trace_debug("vsf_gpio_write(&%s, 0x%08x/*pin mask*/, 0x%08x/*value*/);", test->out.name, test->out.pin_mask, value);
                    vsf_trace_debug("" VSF_TRACE_CFG_LINEEND);
                }
                vsf_gpio_write(test->out.gpio, test->out.pin_mask, value);
            } else if ((test->method & METHOD_OUT_MASK) == METHOD_OUT_SET_CLEAR) {
                if (test->is_output_high) {
                    if (test->verbose >= 2) {
                        vsf_trace_debug("vsf_gpio_set(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND, test->out.name, test->out.pin_mask);
                    }
                    vsf_gpio_set(test->out.gpio, test->out.pin_mask);
                } else {
                    if (test->verbose >= 2) {
                        vsf_trace_debug("vsf_gpio_clear(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND, test->out.name, test->out.pin_mask);
                    }
                    vsf_gpio_clear(test->out.gpio, test->out.pin_mask);
                }
            } else if ((test->method & METHOD_OUT_MASK) == METHOD_OUT_TOGGLE) {
                if (test->verbose >= 2) {
                    vsf_trace_debug("vsf_gpio_toggle(&%s, 0x%08x/*pin mask*/);" VSF_TRACE_CFG_LINEEND, test->out.name, test->out.pin_mask);
                }
                vsf_gpio_toggle(test->out.gpio, test->out.pin_mask);
            } else {
                VSF_ASSERT(0);
            }
        }

        if ((test->int_mode == VSF_GPIO_INT_MODE_NONE) && test->in.pin_mask) {
            uint32_t pin_mask = vsf_gpio_read(test->out.gpio);
            if (test->verbose >= 1) {
                vsf_trace_debug("vsf_gpio_read(&%s) = 0x%08x" VSF_TRACE_CFG_LINEEND, test->in.name, pin_mask);
            }
        }

        vsf_teda_set_timer_ms(test->timeout_ms.test);
        break;

    case VSF_EVT_HAL_TEST_END:
        if ((test->int_mode != VSF_GPIO_INT_MODE_NONE) && test->in.pin_mask) {
            if (VSF_ERR_NONE != __gpio_demo_pin_interrupt_config(test, VSF_GPIO_INT_MODE_NONE)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
            if (VSF_ERR_NONE != __gpio_demo_pin_interrupt_init(test, vsf_arch_prio_invalid)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        }
        break;

    case VSF_EVT_TIMER:
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#    if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __gpio_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    gpio_test_t *test = (gpio_test_t *)hal_test;
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    VSF_ASSERT(demo != NULL);

    vsf_err_t err = VSF_ERR_NONE;

    int c                                 = 0;
    int option_index                      = 0;
    static const char *__short_options    = "hlv::m:r:d:c:p:t:i:o:";
    static const struct option __long_options[] = {
        { "help",       no_argument,       NULL, 'h' },
        { "list-device",no_argument,       NULL, 'l' },
        { "verbose",    optional_argument, NULL, 'v' },
        { "method",     required_argument, NULL, 'm' },
        { "repeat",     required_argument, NULL, 'r' },
        { "config",     required_argument, NULL, 'c' },
        { "prio",       required_argument, NULL, 'p' },
        { "timeout",    required_argument, NULL, 't' },
        { "input",      required_argument, NULL, 'i' },
        { "output",     required_argument, NULL, 'o' },
        { NULL,         0,                 NULL, '\0'},
    };

    optind = 1;
    while (EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch (c) {
        case 'i':
            // vsf_hw_gpio0 0x00000001 VSF_GPIO_PULL_UP
            if ((optind < argc)  && (argv[optind][0] != '-')) {
                const vsf_hal_device_t *device_ptr = hal_demo_find_device(test->demo, optarg);
                if (device_ptr == NULL) {
                    vsf_trace_error("unknow input device: %s" VSF_TRACE_CFG_LINEEND, optarg);
                    return VSF_ERR_FAIL;
                }
                strncpy(test->in.name, optarg, dimof(test->in.name) - 1);
                test->in.gpio = device_ptr->pointer;
                test->in.pin_mask = strtol(argv[optind++], NULL, 0);

                strncpy(test->device_name, optarg, sizeof(test->device_name) - 1);

                if ((optind < argc) && (argv[optind][0] != '-')) {
                    uint32_t mode_value = 0;
                    uint32_t mode_mask = 0;
                    if (!hal_options_get_value(argc, argv, __mode_options, dimof(__mode_options), &mode_value, &mode_mask)) {
                        vsf_trace_error("input mode error!" VSF_TRACE_CFG_LINEEND);
                        return VSF_ERR_FAIL;
                    }
                    test->in.mode = (test->in.mode & ~mode_mask) | mode_value;
                }
            }
            break;

        case 'o':
            if ((optind < argc) && (argv[optind][0] != '-')) {
                const vsf_hal_device_t *device_ptr = hal_demo_find_device(test->demo, optarg);
                if (device_ptr == NULL) {
                    vsf_trace_error("unknow output device: %s" VSF_TRACE_CFG_LINEEND, optarg);
                    return VSF_ERR_FAIL;
                }
                strncpy(test->out.name, optarg, dimof(test->out.name) - 1);
                test->out.gpio = device_ptr->pointer;
                test->out.pin_mask = strtol(argv[optind++], NULL, 0);

                if ((optind < argc) && (argv[optind][0] != '-')) {
                    uint32_t mode_value = 0;
                    uint32_t mode_mask = 0;
                    if (!hal_options_get_value(argc, argv, __mode_options, dimof(__mode_options), &mode_value, &mode_mask)) {
                        vsf_trace_error("output mode error!" VSF_TRACE_CFG_LINEEND);
                        return VSF_ERR_FAIL;
                    }
                    test->out.mode = (test->out.mode & ~mode_mask) | mode_value;
                }
            }
            break;

        default:
            err = hal_test_evthandler(hal_test, argc, argv, c);
            if (err != VSF_ERR_NONE) {
                return err;
            }
            break;
        }
    }

    // avoid set some pin in and out same time
    if (test->in.gpio == test->out.gpio) {
        if (test->in.pin_mask & test->out.pin_mask) {
            vsf_trace_error("input and output use same pin" VSF_TRACE_CFG_LINEEND);
            return VSF_ERR_FAIL;
        }
    }

    return VSF_ERR_NONE;
}

//#include "hal/driver/vendor_driver.h"
//#include "gpio/gpio_api.h"

//void __gpio_irq_handler(int event)
//{
//    vsf_trace_debug("PA10 event" VSF_TRACE_CFG_LINEEND);
//}

int gpio_main(int argc, char *argv[])
{
    //gpio_irq_init(10, GPIOIRQ_TYPE_EDGE_RISE, __gpio_irq_handler);
    //return 0;

    return hal_main(&__gpio_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__gpio_demo.use_as__hal_demo_t);
}
#endif

#endif
