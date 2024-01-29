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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_IO_DEMO == ENABLED && VSF_HAL_USE_IO == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_IO_DEMO_CFG_IO_PREFIX
#   undef VSF_IO_CFG_PREFIX
#   define VSF_IO_CFG_PREFIX                        APP_IO_DEMO_CFG_IO_PREFIX
#endif

#ifndef APP_IO_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_IO_DEMO_CFG_DEFAULT_INSTANCE         vsf_hw_io
#endif

#ifndef APP_IO_DEMO_CFG_DEFAULT_PORT_PIN
#   define APP_IO_DEMO_CFG_DEFAULT_PORT_PIN         VSF_PA0
#endif

#ifndef APP_IO_DEMO_CFG_DEFAULT_FUNCTION
#   define APP_IO_DEMO_CFG_DEFAULT_FUNCTION         0
#endif

#ifndef APP_IO_DEMO_CFG_DEFAULT_FEATURE
#   define APP_IO_DEMO_CFG_DEFAULT_FEATURE          0
#endif

#ifndef APP_IO_DEMO_CFG_DEVICES_COUNT
#   define APP_IO_DEMO_CFG_DEVICES_COUNT            1
#endif

#ifndef APP_IO_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_IO_DEMO_CFG_DEVICES_ARRAY_INIT       \
        { .cnt = dimof(vsf_hw_io_devices), .devices = vsf_hw_io_devices},
#endif

#ifndef APP_IO_DEMO_CFG_CONFIG_NUMBER
#   define APP_IO_DEMO_CFG_CONFIG_NUMBER            32
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum io_method_t {
    METHOD_CONFIG          = 0x00 << 0,
    METHOD_CONFIG_ONE_PIN  = 0x01 << 0,
    METHOD_CONFIG_MASK     = 0x01 << 0,
} io_method_t;

typedef struct io_test_t  {
    implement(hal_test_t)
    vsf_io_cfg_t cfgs[APP_IO_DEMO_CFG_CONFIG_NUMBER];
    uint8_t index;
    uint8_t cnt;
} io_test_t;

typedef struct io_demo_const_t {
    implement(hal_demo_const_t)
    io_test_t test;
} io_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __mode_options[] = {
    HAL_DEMO_OPTION(VSF_IO_OUTPUT_MASK, VSF_IO_PULL_UP),
    HAL_DEMO_OPTION(VSF_IO_OUTPUT_MASK, VSF_IO_PULL_DOWN),
    HAL_DEMO_OPTION(VSF_IO_OUTPUT_MASK, VSF_IO_OPEN_DRAIN),

    HAL_DEMO_OPTION(VSF_IO_INPUT_MASK, VSF_IO_NORMAL_INPUT),
    HAL_DEMO_OPTION(VSF_IO_INPUT_MASK, VSF_IO_INVERT_INPUT),
    HAL_DEMO_OPTION(VSF_IO_INPUT_MASK, VSF_IO_DISABLE_INPUT),

    HAL_DEMO_OPTION(VSF_IO_HIGH_DRIVE_STRENGTH, VSF_IO_HIGH_DRIVE_STRENGTH),

    HAL_DEMO_OPTION(VSF_IO_FILTER_MASK, VSF_IO_FILTER_2CLK),
    HAL_DEMO_OPTION(VSF_IO_FILTER_MASK, VSF_IO_FILTER_4CLK),
    HAL_DEMO_OPTION(VSF_IO_FILTER_MASK, VSF_IO_FILTER_8CLK),

    HAL_DEMO_OPTION(VSF_IO_FILTER_CLK_MASK, VSF_IO_FILTER_CLK_SRC0),
    HAL_DEMO_OPTION(VSF_IO_FILTER_CLK_MASK, VSF_IO_FILTER_CLK_SRC1),
    HAL_DEMO_OPTION(VSF_IO_FILTER_CLK_MASK, VSF_IO_FILTER_CLK_SRC2),
    HAL_DEMO_OPTION(VSF_IO_FILTER_CLK_MASK, VSF_IO_FILTER_CLK_SRC3),
    HAL_DEMO_OPTION(VSF_IO_FILTER_CLK_MASK, VSF_IO_FILTER_CLK_SRC4),
    HAL_DEMO_OPTION(VSF_IO_FILTER_CLK_MASK, VSF_IO_FILTER_CLK_SRC5),
    HAL_DEMO_OPTION(VSF_IO_FILTER_CLK_MASK, VSF_IO_FILTER_CLK_SRC6),
    HAL_DEMO_OPTION(VSF_IO_FILTER_CLK_MASK, VSF_IO_FILTER_CLK_SRC7),
};

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_CONFIG_MASK, METHOD_CONFIG_ONE_PIN, "config-one-pin"),
    HAL_DEMO_OPTION_EX(METHOD_CONFIG_MASK, METHOD_CONFIG,         "config"),
};

HAL_DEMO_INIT(io, APP_IO,
    "io-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE            i.e. vsf_hw_io" VSF_TRACE_CFG_LINEEND
    "  -c, --config PORT_PIN FUNCTION MODE" VSF_TRACE_CFG_LINEEND
    "   PORT_PIN: i.e. VSF_PA1, @ref port_pin_index in vsf_io_cfg_t" VSF_TRACE_CFG_LINEEND
    "   FUNCTION: i.e. 0, @ref function in vsf_io_cfg_t" VSF_TRACE_CFG_LINEEND
    "   MODE    : i.e. VSF_IO_PULL_UP, @ref vsf_io_mode_t" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X           test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X  interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND,

    .init_has_cfg = false,
    .mode.options = __mode_options,
    .mode.cnt = dimof(__mode_options),
    .irq.options = NULL,
    .irq.cnt = 0,
    .method.options = __method_options,
    .method.cnt = dimof(__method_options),
);

/*============================ IMPLEMENTATION ================================*/

static const char __port_name[] = {
#if defined(VSF_IO_CFG_PORTA)
        'A',
#endif
#if defined(VSF_IO_CFG_PORTB)
        'B',
#endif
#if defined(VSF_IO_CFG_PORTC)
        'C',
#endif
#if defined(VSF_IO_CFG_PORTD)
        'D',
#endif
};

static int __get_port_from_char(char ch)
{
    for (int i = 0; i < dimof(__port_name); i++) {
        if (__port_name[i] == ch) {
            return i;
        }
    }
    return -1;
}

static bool __io_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    io_test_t *test = vsf_container_of(hal_test, io_test_t, use_as__hal_test_t);
    VSF_ASSERT(test->device != NULL);

    vsf_io_capability_t cap = vsf_io_capability(test->device);

    for (int i = 0; i < test->cnt; i++) {
        vsf_io_cfg_t *cfg = &test->cfgs[i];
        if (cfg->port_index >= dimof(__port_name)) {
            vsf_trace_error("cfgs[%u] : port out of range(%u),"VSF_TRACE_CFG_LINEEND, i, cfg->port_index, dimof(__port_name));
            return false;
        }
        if ((cap.pin_mask & (1 << cfg->pin_index)) == 0) {
            vsf_trace_error("cfgs[%u] : pin is not supported!(0x%08x)"VSF_TRACE_CFG_LINEEND, i, cfg->port_index, cap.pin_mask);
            return false;
        }
        if (cfg->function > cap.max_function) {
            vsf_trace_error("cfgs[%u] : function out of range(%u)"VSF_TRACE_CFG_LINEEND, i, cfg->function, cap.max_function);
            return false;
        }
        if ((cfg->mode & ~VSF_IO_MODE_ALL_BITS_MASK)) {
            vsf_trace_warning("cfgs[%u] : unknown mode(0x%08x)"VSF_TRACE_CFG_LINEEND, i, cfg->mode & ~VSF_IO_MODE_ALL_BITS_MASK);
        }
    }

    return true;
}

static vsf_err_t __io_config(io_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_io_t *io_ptr = test->device;
    VSF_ASSERT(io_ptr != NULL);

    vsf_err_t err = vsf_io_config(io_ptr, test->cfgs, test->cnt);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_io_cfg_t cfgs[] = {" VSF_TRACE_CFG_LINEEND);
        for (int i = 0; i < test->cnt; i++) {
            vsf_io_cfg_t *cfg = &test->cfgs[i];
            vsf_trace_debug(" { .port_index = VSF_PORT%c, .pin_index = VSF_PIN%C, .function = 0x%04x, .mode = ", __port_name[cfg->port_index], cfg->pin_index, cfg->pin_index, cfg->function);
            hal_options_trace(VSF_TRACE_DEBUG, "", __mode_options, dimof(__mode_options), cfg->mode);
            vsf_trace_debug("},"VSF_TRACE_CFG_LINEEND);
        }
        vsf_trace_debug("}" VSF_TRACE_CFG_LINEEND "vsf_io_config(&%s, %p/*cfgs*/, %u/*cnt*/) = % d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->cfgs, test->cnt, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("io config faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __io_config_one_pin(io_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_io_t *io_ptr = test->device;
    VSF_ASSERT(io_ptr != NULL);
    vsf_err_t err = VSF_ERR_NONE;

    for (int i = 0; i < test->cnt; i++) {
        vsf_io_cfg_t *cfg = &test->cfgs[i];
        vsf_err_t err = vsf_io_config_one_pin(io_ptr, cfg);
        if (test->verbose >= 2) {
            vsf_trace_debug("vsf_io_cfg_t cfg = { .port_index = VSF_PORT%C, .pin_index = VSF_PIN%u, .function = 0x%04x, .feature = ", __port_name[cfg->port_index], cfg->pin_index, cfg->function);
            hal_options_trace(VSF_TRACE_DEBUG, "", __mode_options, dimof(__mode_options), cfg->mode);
            vsf_trace_debug("};"VSF_TRACE_CFG_LINEEND "vsf_io_config_one_pin(&%s, %p/*&cfg*/) = %d" VSF_TRACE_CFG_LINEEND, test->device_name, &test->cfgs, err);
        }
        if (VSF_ERR_NONE != err) {
            vsf_trace_error("io config one pin faild: %d" VSF_TRACE_CFG_LINEEND, err);
            break;
        }
    }

    return err;
}

static void __io_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    io_test_t *test = vsf_container_of(eda, io_test_t, teda);
    vsf_io_t *io_ptr = test->device;
    VSF_ASSERT(io_ptr != NULL);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        if ((test->method & METHOD_CONFIG_MASK) == METHOD_CONFIG_ONE_PIN) {
            err = __io_config(test);
        } else {
            err = __io_config_one_pin(test);
        }
        evt = (err == VSF_ERR_NONE) ? VSF_EVT_HAL_TEST_END : VSF_EVT_HAL_TEST_FAILED;
        vsf_eda_post_evt(eda, evt);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __io_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    io_test_t *test = (io_test_t *)hal_test;
    VSF_ASSERT(test != NULL);
    io_demo_t *demo = (io_demo_t *)test->demo;
    VSF_ASSERT(demo != NULL);
    const io_demo_const_t *demo_const_ptr = (const io_demo_const_t *)demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    vsf_err_t err = VSF_ERR_NONE;

    uint32_t mode_value = 0;
    uint32_t mode_mask = 0;
    uint8_t cnt = 0;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::d:c:p:i:";
    static const struct option __long_options[] = {
        { "help",           no_argument,       NULL, 'h'  },
        { "list-device",    no_argument,       NULL, 'l'  },
        { "verbose",        optional_argument, NULL, 'v'  },
        { "device",         required_argument, NULL, 'd'  },
        { "config",         required_argument, NULL, 'c'  },
        { "prio",           required_argument, NULL, 'p'  },
        { "isr_prio",       required_argument, NULL, 'i'  },
        { NULL,             0                , NULL, '\0' },
    };

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch(c) {
        case 'c':
            mode_value = 0;
            mode_mask = 0;
            if ((optind < argc)  && (argv[optind][0] != '-')) {
                char port_char;
                int pin;
                if (sscanf(optarg, "VSF_P%c%u", &port_char, &pin) != 2) {
                    vsf_trace_error("io config error!" VSF_TRACE_CFG_LINEEND);
                    return VSF_ERR_FAIL;
                }
                test->cfgs[cnt].port_index = __get_port_from_char(port_char);
                test->cfgs[cnt].pin_index  = pin;
                test->cfgs[cnt].function = strtol(argv[optind++], NULL, 0);
                if ((optind < argc) && (argv[optind][0] != '-')) {
                    if (!hal_options_get_value(argc, argv, __mode_options, dimof(__mode_options),
                                             &mode_value, &mode_mask)) {
                        vsf_trace_error("io config error!" VSF_TRACE_CFG_LINEEND);
                        return VSF_ERR_FAIL;
                    }
                    test->cfgs[cnt].mode = (test->cfgs[cnt].mode & ~mode_mask) | mode_value;
                }
                cnt++;
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

    if (cnt != 0) {
        test->cnt = cnt;
    }

    return err;
}

int io_main(int argc, char *argv[])
{
    return hal_main(&__io_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__io_demo, argc, argv);
}
#endif

#endif
