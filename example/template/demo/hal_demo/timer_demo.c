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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_TIMER_DEMO == ENABLED && VSF_HAL_USE_TIMER == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_TIMER_DEMO_CFG_TIMER_PREFIX
#   undef VSF_TIMER_CFG_PREFIX
#   define VSF_TIMER_CFG_PREFIX                         APP_TIMER_DEMO_CFG_TIMER_PREFIX
#endif

#ifndef APP_TIMER_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_TIMER_DEMO_CFG_DEFAULT_INSTANCE          vsf_hw_timer0
#endif

#ifndef APP_TIMER_DEMO_CFG_MODE
#   define APP_TIMER_DEMO_CFG_MODE                      VSF_TIMER_BASE_CONTINUES
#endif

#ifndef APP_TIMER_DEMO_CFG_FREQ
#   define APP_TIMER_DEMO_CFG_FREQ                      (1 * 1000 * 1000)   // 1MHz(1us)
#endif

#ifndef APP_TIMER_DEMO_CFG_MAX_COUNT
#   define APP_TIMER_DEMO_CFG_MAX_COUNT                 (2 * 1000 * 1000)   // 2s
#endif

#ifndef APP_TIMER_DEMO_CFG_DEVICES_COUNT
#   define APP_TIMER_DEMO_CFG_DEVICES_COUNT             1
#endif

#ifndef APP_TIMER_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_TIMER_DEMO_CFG_DEVICES_ARRAY_INIT        \
        { .cnt = dimof(vsf_hw_timer_devices), .devices = vsf_hw_timer_devices},
#endif

/*============================ TYPES =========================================*/

typedef enum METHOD_t {
    METHOD_OVERFLOW_ISR  = 0x00 << 0,
    METHOD_PWM           = 0x01 << 0,
    METHOD_MASK          = 0x01 << 0,
} METHOD_t;

typedef enum timer_demo_evt_t {
    VSF_EVT_TIMER_GET = __VSF_EVT_HAL_LAST,
    VSF_EVT_TIMER_GET_TIME,
    VSF_EVT_TIMER_OVERFLOW_ISR_WAIT,
} timer_demo_evt_t;

typedef struct timer_test_t  {
    implement(hal_test_t)
    vsf_timer_cfg_t cfg;
    uint8_t chn;
    uint32_t period;
    uint32_t pulse;
    uint32_t match_cnt;
} timer_test_t;

typedef struct timer_demo_const_t {
    implement(hal_demo_const_t)
    timer_test_t test;
} timer_demo_const_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __mode_options[] = {
    HAL_DEMO_OPTION(VSF_TIMER_MODE_RESTART_MASK, VSF_TIMER_BASE_ONESHOT),
    HAL_DEMO_OPTION(VSF_TIMER_MODE_RESTART_MASK, VSF_TIMER_BASE_CONTINUES),

    HAL_DEMO_OPTION(VSF_TIMER_MODE_PWM_MASK, VSF_TIMER_MODE_PWM),
    HAL_DEMO_OPTION(VSF_TIMER_MODE_PWM_MASK, VSF_TIMER_MODE_NO_PWM),
};

static const hal_option_t __irq_options[] = {
    HAL_DEMO_OPTION(VSF_TIMER_IRQ_MASK_OVERFLOW, VSF_TIMER_IRQ_MASK_OVERFLOW),
};

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_MASK, METHOD_OVERFLOW_ISR, "isr"),
    HAL_DEMO_OPTION_EX(METHOD_MASK, METHOD_PWM,          "pwm"),
};

HAL_DEMO_INIT(timer, APP_TIMER,
    "timer-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [pwm|isr]" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE            i.e. vsf_hw_timer0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT            repeat count" VSF_TRACE_CFG_LINEEND
    "  -c, --mode MODE                @ref vsf_timer_mode_t" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT          test timeout" VSF_TRACE_CFG_LINEEND
    "  -f, --freq                     @ref freq in vsf_timer_cfg_t" VSF_TRACE_CFG_LINEEND
    "  -n, --max_count                @ref max_count in vsf_timer_cfg_t" VSF_TRACE_CFG_LINEEND,

    .test.method = METHOD_OVERFLOW_ISR,

    .test.cfg.mode          = VSF_TIMER_BASE_ONESHOT,
    .test.cfg.period        = APP_TIMER_DEMO_CFG_MAX_COUNT,
    .test.cfg.freq          = APP_TIMER_DEMO_CFG_FREQ,

    .init_has_cfg           = true,
    .device_init            = (hal_init_fn_t       )vsf_timer_init,
    .device_fini            = (hal_fini_fn_t       )vsf_timer_fini,
    .device_enable          = (hal_enable_fn_t     )vsf_timer_enable,
    .device_disable         = (hal_disable_fn_t    )vsf_timer_disable,
    .device_irq_enable      = (hal_irq_enable_fn_t )vsf_timer_irq_enable,
    .device_irq_disable     = (hal_irq_disable_fn_t)vsf_timer_irq_disable,

    .mode.options           = __mode_options,
    .mode.cnt               = dimof(__mode_options),
    .irq.options            = __irq_options,
    .irq.cnt                = dimof(__irq_options),
    .method.options         = __method_options,
    .method.cnt             = dimof(__method_options),
);

/*============================ IMPLEMENTATION ================================*/

static void __timer_isr_handler(void *target_ptr, vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)
{
    timer_test_t *test = (timer_test_t *)target_ptr;
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    if (!hal_test_irq_check(&test->use_as__hal_test_t, irq_mask)) {
        return;
    }

    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        vsf_eda_post_evt(eda, VSF_EVT_TIMER_OVERFLOW_ISR_WAIT);
    }
}

static bool __timer_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    timer_test_t *test = vsf_container_of(hal_test, timer_test_t, use_as__hal_test_t);

    VSF_ASSERT(test->device != NULL);

    vsf_timer_capability_t cap = vsf_timer_capability(test->device);
    if ((test->method & METHOD_MASK) == METHOD_PWM) {
        if (!cap.support_pwm) {
            vsf_trace_error("timer does not support pwm!" VSF_TRACE_CFG_LINEEND);
            return false;
        }
        if (test->chn >= cap.pwm_channel_cnt) {
            vsf_trace_error("chn %u out of range(%u)!" VSF_TRACE_CFG_LINEEND, test->chn, cap.pwm_channel_cnt);
            return false;
        }
    }

    if ((test->method & METHOD_MASK) == METHOD_OVERFLOW_ISR) {
        test->irq_mask = VSF_TIMER_IRQ_MASK_OVERFLOW;
        test->expected_irq_mask = VSF_TIMER_IRQ_MASK_OVERFLOW;
        test->cfg.isr.handler_fn = __timer_isr_handler;
        test->cfg.isr.target_ptr = test;
    }

    test->hal_cfg = (void *)&test->cfg;

    return true;
}

static vsf_err_t __timer_pwm_set(timer_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_timer_t *timer_ptr = test->device;
    VSF_ASSERT(timer_ptr != NULL);

    vsf_err_t err = vsf_timer_pwm_set(timer_ptr, test->chn, test->period, test->pulse);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_timer_pwm_set(&%s, %u, 0x%08x/*period*/, 0x%08x/*pulse*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->chn, test->period, test->pulse, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("timer pwm set faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static void __timer_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    timer_test_t *test = vsf_container_of(eda, timer_test_t, teda);
    vsf_timer_t * timer_ptr = test->device;
    VSF_ASSERT(timer_ptr != NULL);

    switch (evt) {
    case VSF_EVT_HAL_TEST_RUN:
        if ((test->method & METHOD_MASK) == METHOD_PWM) {
            if (VSF_ERR_NONE != __timer_pwm_set(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
            }
        } else /*if ((test->method & METHOD_MASK) == METHOD_OVERFLOW_ISR)*/ {
            // After the timer is initialized and interrupts are enabled, we don't need to do anything more.
        }
        vsf_teda_set_timer_ms(test->timeout_ms.test + 20);
        break;

    case VSF_EVT_TIMER:
        if ((test->method & METHOD_MASK) == METHOD_PWM) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        } else {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        }
        break;

    case VSF_EVT_TIMER_OVERFLOW_ISR_WAIT:
        vsf_teda_cancel_timer();
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __timer_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    timer_test_t *test = (timer_test_t *)hal_test;
    VSF_ASSERT(test != NULL);
    timer_demo_t *demo = (timer_demo_t *)test->demo;
    VSF_ASSERT(demo != NULL);
    const timer_demo_const_t *demo_const_ptr = (const timer_demo_const_t *)demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    vsf_err_t err = VSF_ERR_NONE;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::m:r:d:f:p:i:t:f:p:n";
    static const struct option __long_options[] = {
        { "help",        no_argument,       NULL, 'h'  },
        { "list-device", no_argument,       NULL, 'l'  },
        { "verbose",     optional_argument, NULL, 'v'  },
        { "method",      required_argument, NULL, 'm'  },
        { "repeat",      required_argument, NULL, 'r'  },
        { "device",      required_argument, NULL, 'd'  },
        { "config",      required_argument, NULL, 'c'  },
        { "prio",        required_argument, NULL, 'p'  },
        { "isr_prio",    required_argument, NULL, 'i'  },
        { "timeout",     required_argument, NULL, 't'  },
        { "freq",        required_argument, NULL, 'f'  },
        { "pwm",         required_argument, NULL, 'p'  },
        { "max_count",   required_argument, NULL, 'n'  },
        { NULL,          0                , NULL, '\0' },
    };

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch(c) {
        case 'p':
            if ((optind < argc) && (argv[optind][0] != '-')) {
                test->period = strtol(optarg, NULL, 0);
                test->pulse = strtol(argv[optind++], NULL, 0);

                if (test->period < test->pulse) {
                    vsf_trace_error("timer period(%u) less than pulse(%u)!" VSF_TRACE_CFG_LINEEND, test->period, test->pulse);
                    return VSF_ERR_FAIL;
                }
            }
            break;

        case 'n':
            test->cfg.period = strtol(optarg, NULL, 0);
            if (test->cfg.period == 0) {
                vsf_trace_error("timer max counter cannot be zero!" VSF_TRACE_CFG_LINEEND);
                return VSF_ERR_FAIL;
            }

        default:
            err = hal_test_evthandler(hal_test, argc, argv, c);
            if (err != VSF_ERR_NONE) {
                return err;
            }
            break;
        }
    }

    return err;
}

int timer_main(int argc, char *argv[])
{
    return hal_main(&__timer_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__timer_demo.use_as__hal_demo_t);
}
#endif

#endif
