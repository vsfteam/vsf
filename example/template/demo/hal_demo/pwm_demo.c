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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_PWM_DEMO == ENABLED && VSF_HAL_USE_PWM == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_PWM_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#   undef VSF_PWM_CFG_PREFIX
#   define VSF_PWM_CFG_PREFIX                           APP_PWM_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#endif

#ifndef APP_PWM_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_PWM_DEMO_CFG_DEFAULT_INSTANCE            vsf_hw_pwm0
#endif

#ifndef APP_PWM_DEMO_CFG_PRIO
#   define APP_PWM_DEMO_CFG_PRIO                        vsf_prio_0
#endif

#ifndef APP_PWM_DEMO_CFG_DEVICES_COUNT
#   define APP_PWM_DEMO_CFG_DEVICES_COUNT               1
#endif

#ifndef APP_PWM_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_PWM_DEMO_CFG_DEVICES_ARRAY_INIT          \
        { .cnt = dimof(vsf_hw_pwm_devices), .devices = vsf_hw_pwm_devices},
#endif

#ifndef APP_PWM_DEMO_CFG_CHNNAL
#   define APP_PWM_DEMO_CFG_CHNNAL                      0
#endif

#ifndef APP_PWM_DEMO_CFG_DEFAULT_FREQ
#   define APP_PWM_DEMO_CFG_DEFAULT_FREQ                (1 * 1000 * 1000) // 1MHz
#endif

#ifndef APP_PWM_DEMO_CFG_PREIOD
#   define APP_PWM_DEMO_CFG_PREIOD                      1000
#endif

#ifndef APP_PWM_DEMO_CFG_PULSE
#   define APP_PWM_DEMO_CFG_PULSE                       500
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum METHOD_t {
    METHOD_TEST_TICK = 0x00 << 0,
    METHOD_TEST_MS   = 0x01 << 0,
    METHOD_TEST_US   = 0x02 << 0,
    METHOD_TEST_NS   = 0x03 << 0,
    METHOD_TEST_MASK = 0x03 << 0,
} METHOD_t;

typedef struct pwm_test_t  {
    implement(hal_test_t)
    vsf_pwm_cfg_t cfg;
    uint8_t chn;
    uint32_t period;
    uint32_t pulse;
} pwm_test_t;

typedef struct pwm_demo_const_t {
    implement(hal_demo_const_t)
    pwm_test_t test;
} pwm_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_TEST_MASK, METHOD_TEST_TICK, "tick"),
    HAL_DEMO_OPTION_EX(METHOD_TEST_MASK, METHOD_TEST_MS,   "ms"),
    HAL_DEMO_OPTION_EX(METHOD_TEST_MASK, METHOD_TEST_US,   "us"),
    HAL_DEMO_OPTION_EX(METHOD_TEST_MASK, METHOD_TEST_NS,   "ns"),
};

HAL_DEMO_INIT(pwm, APP_PWM,
    "pwm-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [tick|ms|us|ns]" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE            i.e. vsf_hw_pwm0" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT          test timeout" VSF_TRACE_CFG_LINEEND
    "  -f, --freq FREQ                @ref freq in vsf_pwm_cfg_t" VSF_TRACE_CFG_LINEEND,

    .test.cfg.freq    = APP_PWM_DEMO_CFG_DEFAULT_FREQ,
    .test.period      = APP_PWM_DEMO_CFG_PREIOD,
    .test.pulse       = APP_PWM_DEMO_CFG_PULSE,

    .init_has_cfg     = true,
    .device_init      = (hal_init_fn_t       )vsf_pwm_init,
    .device_fini      = (hal_fini_fn_t       )vsf_pwm_fini,
    .device_enable    = (hal_enable_fn_t     )vsf_pwm_enable,
    .device_disable   = (hal_disable_fn_t    )vsf_pwm_disable,
    .method.options   = __method_options,
    .method.cnt       = dimof(__method_options),
);

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static bool __pwm_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    pwm_test_t *test = vsf_container_of(hal_test, pwm_test_t, use_as__hal_test_t);

    VSF_ASSERT(test->device != NULL);

    vsf_pwm_capability_t cap = vsf_pwm_capability(test->device);
    if (test->freq > cap.max_freq) {
        vsf_trace_error("pwm frequency(%u) is greater than the maximum frequency(%u)!"VSF_TRACE_CFG_LINEEND, test->freq, cap.max_freq);
        return false;
    }
    if (test->freq < cap.min_freq) {
        vsf_trace_error("pwm frequency(%u) is less  than the minimum frequency(%u)!"VSF_TRACE_CFG_LINEEND, test->freq, cap.min_freq);
        return false;
    }

    test->hal_cfg = (void *)&test->cfg;

    return true;
}

static vsf_err_t __pwm_set(pwm_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_pwm_t *pwm_ptr = test->device;
    VSF_ASSERT(pwm_ptr != NULL);

    vsf_err_t err = vsf_pwm_set(pwm_ptr, test->chn, test->period, test->pulse);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_pwm_set(&%s, %u, %u/*period*/, %u/*pulse*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->chn, test->period, test->pulse, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("pwm set faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __pwm_set_ms(pwm_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_pwm_t *pwm_ptr = test->device;
    VSF_ASSERT(pwm_ptr != NULL);

    vsf_err_t err = vsf_pwm_set_ms(pwm_ptr, test->chn, test->period, test->pulse);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_pwm_set_ms(&%s, %u, %u/*period*/, %u/*pulse*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->chn, test->period, test->pulse, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("pwm set ms faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __pwm_set_us(pwm_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_pwm_t *pwm_ptr = test->device;
    VSF_ASSERT(pwm_ptr != NULL);

    vsf_err_t err = vsf_pwm_set_us(pwm_ptr, test->chn, test->period, test->pulse);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_pwm_set_us(&%s, %u, %u/*period*/, %u/*pulse*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->chn, test->period, test->pulse, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("pwm set us faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __pwm_set_ns(pwm_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_pwm_t *pwm_ptr = test->device;
    VSF_ASSERT(pwm_ptr != NULL);

    vsf_err_t err = vsf_pwm_set_ns(pwm_ptr, test->chn, test->period, test->pulse);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_pwm_set_ns(&%s, %u, %u/*period*/, %u/*pulse*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->chn, test->period, test->pulse, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("pwm set ns faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static void __pwm_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    pwm_test_t *test = vsf_container_of(eda, pwm_test_t, teda);
    vsf_pwm_t * pwm_ptr = test->device;
    VSF_ASSERT(pwm_ptr != NULL);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_HAL_TEST_RUN:
        if ((test->method & METHOD_TEST_MASK) == METHOD_TEST_TICK) {
            err = __pwm_set(test);
        } else if((test->method &METHOD_TEST_MASK) == METHOD_TEST_MS) {
            err = __pwm_set_ms(test);
        } else if ((test->method & METHOD_TEST_MASK) == METHOD_TEST_US) {
            err = __pwm_set_us(test);
        } else {
            err = __pwm_set_ns(test);
        }
        if (err != VSF_ERR_NONE) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        } else {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
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

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __pwm_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    VSF_ASSERT(hal_test != NULL);
    pwm_test_t *test = vsf_container_of(hal_test, pwm_test_t, use_as__hal_test_t);

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::d:c:p:i:e:u:";
    static const struct option __long_options[] = {
        { "help",           no_argument,       NULL, 'h'  },
        { "list-device",    no_argument,       NULL, 'l'  },
        { "verbose",        optional_argument, NULL, 'v'  },
        { "method",         required_argument, NULL, 'm'  },
        { "repeat",         required_argument, NULL, 'r'  },
        { "device",         required_argument, NULL, 'd'  },
        { "prio",           required_argument, NULL, 'p'  },
        { "timeout",        required_argument, NULL, 't'  },
        { "freq",           required_argument, NULL, 'f'  },
        { "period",         required_argument, NULL, 'e'  },
        { "pulse",          required_argument, NULL, 'u'  },
        { NULL,             0                , NULL, '\0' },
    };
    vsf_err_t err = VSF_ERR_NONE;

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch(c) {
        case 'e':
            test->period = strtol(optarg, NULL, 0);
            break;

        case 'u':
            test->pulse = strtol(optarg, NULL, 0);
            break;

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

int pwm_main(int argc, char *argv[])
{
    return hal_main(&__pwm_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__pwm_demo.use_as__hal_demo_t);
}
#endif

#endif
