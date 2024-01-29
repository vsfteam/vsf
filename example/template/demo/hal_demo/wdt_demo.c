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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_WDT_DEMO == ENABLED && VSF_HAL_USE_WDT == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_WDT_DEMO_CFG_WDT_PREFIX
#   undef VSF_WDT_CFG_PREFIX
#   define VSF_WDT_CFG_PREFIX                         APP_WDT_DEMO_CFG_WDT_PREFIX
#endif

#ifndef APP_WDT_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_WDT_DEMO_CFG_DEFAULT_INSTANCE          vsf_hw_wdt0
#endif

#ifndef APP_WDT_DEMO_CFG_MODE
#   define APP_WDT_DEMO_CFG_MODE                      (VSF_WDT_MODE_NO_EARLY_WAKEUP | VSF_WDT_MODE_RESET_SOC)
#endif

#ifndef APP_WDT_DEMO_CFG_MAX_TIMEOUT_MS
#   define APP_WDT_DEMO_CFG_MAX_TIMEOUT_MS            (5 * 1000)
#endif

#ifndef APP_WDT_DEMO_CFG_MIN_TIMEOUT_MS
#   define APP_WDT_DEMO_CFG_MIN_TIMEOUT_MS            (5 * 100)
#endif

#ifndef APP_WDT_DEMO_CFG_FREQ
#   define APP_WDT_DEMO_CFG_FREQ                      (1 * 1000 * 1000)   // 1MHz(1us)
#endif

#ifndef APP_WDT_DEMO_CFG_MAX_COUNT
#   define APP_WDT_DEMO_CFG_MAX_COUNT                 (2 * 1000 * 1000)   // 2s
#endif

#ifndef APP_WDT_DEMO_CFG_DEVICES_COUNT
#   define APP_WDT_DEMO_CFG_DEVICES_COUNT             1
#endif

#ifndef APP_WDT_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_WDT_DEMO_CFG_DEVICES_ARRAY_INIT        \
        { .cnt = dimof(vsf_hw_wdt_devices), .devices = vsf_hw_wdt_devices},
#endif

/*============================ TYPES =========================================*/

typedef enum METHOD_t {
    METHOD_FEED_IN_ISR  = 0x00 << 0,
    METHOD_FEED_IN_TASK = 0x01 << 0,
    METHOD_FEED_MASK    = 0x01 << 0,

    METHOD_TEST_DISABLE = 0x00 << 1,
    METHOD_TEST_TIMEOUT = 0x01 << 1,
    METHOD_TEST_MASK    = 0x01 << 1,
} METHOD_t;


typedef struct wdt_test_t  {
    implement(hal_test_t)
    vsf_wdt_cfg_t cfg;
} wdt_test_t;

typedef struct wdt_demo_const_t {
    implement(hal_demo_const_t)
    wdt_test_t test;
} wdt_demo_const_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __mode_options[] = {
    HAL_DEMO_OPTION(VSF_WDT_MODE_EARLY_WAKEUP_MASK, VSF_WDT_MODE_NO_EARLY_WAKEUP),
    HAL_DEMO_OPTION(VSF_WDT_MODE_EARLY_WAKEUP_MASK, VSF_WDT_MODE_EARLY_WAKEUP),

    HAL_DEMO_OPTION(VSF_WDT_MODE_RESET_MASK, VSF_WDT_MODE_RESET_NONE),
    HAL_DEMO_OPTION(VSF_WDT_MODE_RESET_MASK, VSF_WDT_MODE_RESET_CPU),
    HAL_DEMO_OPTION(VSF_WDT_MODE_RESET_MASK, VSF_WDT_MODE_RESET_SOC),
};

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_FEED_MASK, METHOD_FEED_IN_TASK, "task-feed"),
    HAL_DEMO_OPTION_EX(METHOD_FEED_MASK, METHOD_FEED_IN_ISR,  "isr-feed"),

    HAL_DEMO_OPTION_EX(METHOD_TEST_MASK, METHOD_TEST_DISABLE,  "disable"),
    HAL_DEMO_OPTION_EX(METHOD_TEST_MASK, METHOD_TEST_TIMEOUT,  "timeout"),
};

HAL_DEMO_INIT(wdt, APP_WDT,
    "wdt-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [task-feed|isr-feed] [disable|timeout]" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE            i.e. vsf_hw_wdt0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT            repeat count" VSF_TRACE_CFG_LINEEND
    "  -c, --mode MODE                @ref vsf_wdt_mode_t" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT         timer timeout" VSF_TRACE_CFG_LINEEND
    "  -a, --max-ms TIMEOUT          wdt max_ms timeout" VSF_TRACE_CFG_LINEEND
    "  -s, --min-ms TIMEOUT          wdt min_ms timeout" VSF_TRACE_CFG_LINEEND,


    .test.method            = METHOD_FEED_IN_TASK | METHOD_TEST_DISABLE,

    .test.cfg.mode          = APP_WDT_DEMO_CFG_MODE,
    .test.cfg.max_ms        = APP_WDT_DEMO_CFG_MAX_TIMEOUT_MS,
    .test.cfg.min_ms        = APP_WDT_DEMO_CFG_MIN_TIMEOUT_MS,

    .init_has_cfg           = true,
    .device_init            = (hal_init_fn_t       )vsf_wdt_init,
    .device_fini            = (hal_fini_fn_t       )vsf_wdt_fini,
    .device_enable          = (hal_enable_fn_t     )vsf_wdt_enable,

    .mode.options           = __mode_options,
    .mode.cnt               = dimof(__mode_options),
    .method.options         = __method_options,
    .method.cnt             = dimof(__method_options),
);

/*============================ IMPLEMENTATION ================================*/

static void __wdt_demo_feed(wdt_test_t *test)
{
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(test->device != NULL);

    vsf_wdt_feed(test->device);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_wdt_feed(&%s);" VSF_TRACE_CFG_LINEEND, test->device_name);
    }
}

static fsm_rt_t __wdt_disable(wdt_test_t *test)
{
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(test->device != NULL);

    fsm_rt_t rt = vsf_wdt_disable(test->device);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_wdt_disable(&%s) = %d" VSF_TRACE_CFG_LINEEND, test->device_name, (int)rt);
    }
    if (rt == fsm_rt_on_going) {
        if (hal_test_is_timeout(&test->use_as__hal_test_t, 200)) {       // TODO: use timeout from args
            rt = fsm_rt_err;
        }
    }

    return rt;
}

static void __wdt_isr_handler(void *target_ptr, vsf_wdt_t *wdt_ptr)
{
    wdt_test_t *test = (wdt_test_t *)target_ptr;

    if ((test->method & METHOD_FEED_MASK) == METHOD_FEED_IN_ISR) {
        __wdt_demo_feed(test);
        vsf_eda_post_evt(&test->teda.use_as__vsf_eda_t, VSF_EVT_HAL_TEST_NEXT);
    }
}

static bool __wdt_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    wdt_test_t *test = vsf_container_of(hal_test, wdt_test_t, use_as__hal_test_t);

    VSF_ASSERT(test->device != NULL);

    vsf_wdt_capability_t cap = vsf_wdt_capability(test->device);

    if ((test->method & METHOD_FEED_MASK) == METHOD_FEED_IN_ISR) {
        test->cfg.mode |= (test->cfg.mode & ~VSF_WDT_MODE_EARLY_WAKEUP_MASK) | VSF_WDT_MODE_EARLY_WAKEUP;
    }

    if ((test->cfg.mode & VSF_WDT_MODE_EARLY_WAKEUP_MASK) == VSF_WDT_MODE_EARLY_WAKEUP) {
        if (!cap.support_early_wakeup) {
            vsf_trace_error("%s does not support interrupt!" VSF_TRACE_CFG_LINEEND, test->device_name);
            return false;
        }

        test->cfg.isr.handler_fn = __wdt_isr_handler;
        test->cfg.isr.target_ptr = test;
        test->cfg.isr.prio = test->isr_arch_prio;
   }

    if ((test->method & METHOD_TEST_MASK) == METHOD_TEST_DISABLE) {
        if (!cap.support_disable) {
            vsf_trace_error("%s does not support disable!" VSF_TRACE_CFG_LINEEND, test->device_name);
            return false;
        }
    }

    if (test->verbose >= 2) {
        hal_options_trace(VSF_TRACE_DEBUG, "vsf_wdt_cfg_t cfg = { .mode = ", __mode_options, dimof(__mode_options), test->cfg.mode);
        vsf_trace_debug(", .max_ms = %u, .min_ms = %u, .isr = { .prio = vsf_arch_prio_%u }};" VSF_TRACE_CFG_LINEEND, test->cfg.max_ms, test->cfg.min_ms, hal_demo_arch_prio_to_num(test->cfg.isr.prio));
    }

    test->hal_cfg = (void *)&test->cfg;

    return true;
}

static void __wdt_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    wdt_test_t *test = vsf_container_of(eda, wdt_test_t, teda);
    vsf_wdt_t * wdt_ptr = test->device;
    VSF_ASSERT(wdt_ptr != NULL);

    switch (evt) {
    case VSF_EVT_HAL_TEST_RUN:
        if ((test->method & METHOD_FEED_MASK) == METHOD_FEED_IN_TASK) {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
        }
        break;

    case VSF_EVT_TIMER:
        if ((test->method & METHOD_FEED_MASK) == METHOD_FEED_IN_TASK) {
            __wdt_demo_feed(test);
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        }
        break;

    case VSF_EVT_HAL_TEST_END:
        if ((test->method & METHOD_TEST_MASK) == METHOD_TEST_DISABLE) {
            fsm_rt_t rt = __wdt_disable(test);
            if (rt == fsm_rt_on_going) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_END);
            } else if (rt != fsm_rt_cpl) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
            } else {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_FINI);
            }
        }
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __wdt_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    wdt_test_t *test = (wdt_test_t *)hal_test;
    VSF_ASSERT(test != NULL);
    wdt_demo_t *demo = (wdt_demo_t *)test->demo;
    VSF_ASSERT(demo != NULL);
    const wdt_demo_const_t *demo_const_ptr = (const wdt_demo_const_t *)demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    vsf_err_t err = VSF_ERR_NONE;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::m:r:d:c:f:p:i:t:a:s:";
    static const struct option __long_options[] = {
        { "help",        no_argument,       NULL, 'h'  },
        { "list-device", no_argument,       NULL, 'l'  },
        { "verbose",     optional_argument, NULL, 'v'  },
        { "method",      required_argument, NULL, 'm'  },
        { "repeat",      required_argument, NULL, 'r'  },
        { "device",      required_argument, NULL, 'd'  },
        { "mode",        required_argument, NULL, 'c'  },
        { "prio",        required_argument, NULL, 'p'  },
        { "isr_prio",    required_argument, NULL, 'i'  },
        { "timeout",     required_argument, NULL, 't'  },
        { "max-ms",      required_argument, NULL, 'a'  },
        { "min-ms",      required_argument, NULL, 's'  },
        { NULL,          0                , NULL, '\0' },
    };

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch (c) {
        case 's':
            test->cfg.min_ms = strtol(optarg, NULL, 0);
            break;

        case 'a':
            test->cfg.max_ms = strtol(optarg, NULL, 0);
            break;

        default:
            err = hal_test_evthandler(hal_test, argc, argv, c);
            if (err != VSF_ERR_NONE) {
                return err;
            }
            break;
        }
    }

    if (test->cfg.min_ms >= test->cfg.max_ms) {
        vsf_trace_error("wdt time range error, min: %u, max: %u!" VSF_TRACE_CFG_LINEEND, test->cfg.min_ms, test->cfg.max_ms);
        return VSF_ERR_FAIL;
    }

    test->cfg.mode = (test->cfg.mode & ~test->mode_mask) | test->mode_value;

    return err;
}

int wdt_main(int argc, char *argv[])
{
    return hal_main(&__wdt_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__wdt_demo.use_as__hal_demo_t);
}
#endif

#endif
