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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_RTC_DEMO == ENABLED && VSF_HAL_USE_RTC == ENABLED

#    if VSF_USE_LINUX == ENABLED
#        include <getopt.h>
#    endif

#    include "hal_demo.h"
#    include <time.h>

/*============================ MACROS ========================================*/

#    ifdef APP_RTC_DEMO_CFG_RTC_PREFIX
#        undef VSF_RTC_CFG_PREFIX
#        define VSF_RTC_CFG_PREFIX                  APP_RTC_DEMO_CFG_RTC_PREFIX
#    endif

#    ifndef APP_RTC_DEMO_CFG_DEFAULT_INSTANCE
#        define APP_RTC_DEMO_CFG_DEFAULT_INSTANCE   vsf_hw_rtc0
#    endif

#   ifndef APP_RTC_DEMO_CFG_DEVICES_COUNT
#       define APP_RTC_DEMO_CFG_DEVICES_COUNT                 1
#   endif

#   ifndef APP_RTC_DEMO_CFG_DEVICES_ARRAY_INIT
#       define APP_RTC_DEMO_CFG_DEVICES_ARRAY_INIT            \
            { .cnt = dimof(vsf_hw_rtc_devices), .devices = vsf_hw_rtc_devices},
#   endif

#    ifndef APP_RTC_DEMO_CFG_TIME_FORMAT
#        define APP_RTC_DEMO_CFG_TIME_FORMAT        "%a, %b %Y %H:%M:%S GMT"
#    endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum METHOD_t {
    METHOD_NO_SET        = 0x00 << 0,
    METHOD_SET           = 0x01 << 0,
    METHOD_SET_TIME      = 0x02 << 0,
    METHOD_SET_MASK      = 0x03 << 0,

    METHOD_NO_GET        = 0x00 << 2,
    METHOD_GET           = 0x01 << 2,
    METHOD_GET_MASK      = 0x01 << 2,

    METHOD_NO_GET_TIME   = 0x00 << 3,
    METHOD_GET_TIME      = 0x01 << 3,
    METHOD_GET_TIME_MASK = 0x01 << 3,

    METHOD_NO_ALARM_ISR  = 0x00 << 4,
    METHOD_ALARM_ISR     = 0x01 << 4,
    METHOD_ALARM_ISR_MASK= 0x01 << 4,
} METHOD_t;

typedef enum rtc_demo_error_t {
    RTC_ERROR_NONE           = 0,
    RTC_ERROR_INIT_FAILD = 0,
} rtc_demo_error_t;

typedef enum rtc_demo_evt_t {
    VSF_EVT_RTC_GET = __VSF_EVT_HAL_LAST,
    VSF_EVT_RTC_GET_TIME,
    VSF_EVT_RTC_ISR_WAIT,
} rtc_demo_evt_t;

typedef struct rtc_test_t {
    implement(hal_test_t)
    vsf_rtc_cfg_t cfg;
    vsf_rtc_time_t second;
    vsf_rtc_time_t millisecond;
    vsf_rtc_tm_t rtc_tm;
} rtc_test_t;

typedef struct rtc_demo_const_t {
    implement(hal_demo_const_t)
    rtc_test_t test;
} rtc_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_SET_MASK, METHOD_NO_SET,             "no-set"),
    HAL_DEMO_OPTION_EX(METHOD_SET_MASK, METHOD_SET,                "set"),
    HAL_DEMO_OPTION_EX(METHOD_SET_MASK, METHOD_SET_TIME,           "set-time"),

    HAL_DEMO_OPTION_EX(METHOD_GET_MASK, METHOD_GET,                "get"),
    HAL_DEMO_OPTION_EX(METHOD_GET_MASK, METHOD_NO_GET,             "no-get"),

    HAL_DEMO_OPTION_EX(METHOD_GET_TIME_MASK, METHOD_GET_TIME,      "get-time"),
    HAL_DEMO_OPTION_EX(METHOD_GET_TIME_MASK, METHOD_NO_GET_TIME,   "no-get-time"),

    HAL_DEMO_OPTION_EX(METHOD_ALARM_ISR_MASK, METHOD_NO_ALARM_ISR, "no-alarm"),
    HAL_DEMO_OPTION_EX(METHOD_ALARM_ISR_MASK, METHOD_ALARM_ISR,    "alarm"),
};

HAL_DEMO_INIT(rtc, APP_RTC,
    "rtc-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [no-set|set|set-time] [get|no-get] [get-time|no-get-time] [alarm|no-alarm]" VSF_TRACE_CFG_LINEEND
    "  -d, --device vsf_hw_rtcX       i.e. vsf_hw_rtc0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT            repeat count" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT          test timeout" VSF_TRACE_CFG_LINEEND
    "  -s, --tm tm-string [tm-format] @ref strptime()" VSF_TRACE_CFG_LINEEND
    "  -e, --time second [millisecond] @ref second and millisecond in vsf_rtc_set_time()" VSF_TRACE_CFG_LINEEND,

    .test.method            = METHOD_NO_SET |METHOD_GET | METHOD_GET_TIME | METHOD_NO_ALARM_ISR,
    .test.timeout_ms        = 1000,

    .init_has_cfg           = true,
    .device_init            = (hal_init_fn_t       )vsf_rtc_init,
    .device_fini            = (hal_fini_fn_t       )vsf_rtc_fini,
    .device_enable          = (hal_enable_fn_t     )vsf_rtc_enable,
    .device_disable         = (hal_disable_fn_t    )vsf_rtc_disable,
    .method.options         = __method_options,
    .method.cnt             = dimof(__method_options),
);
/*============================ IMPLEMENTATION ================================*/

static void __rtc_isr_handler(void *target_ptr, vsf_rtc_t *rtc_ptr, vsf_rtc_irq_mask_t irq_mask)
{
    rtc_test_t *test = (rtc_test_t *)target_ptr;
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(rtc_ptr != NULL);
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    if (!hal_test_irq_check(&test->use_as__hal_test_t, irq_mask)) {
        return;
    }

    if (irq_mask & VSF_RTC_IRQ_MASK_ALARM) {
        vsf_eda_post_evt(eda, VSF_EVT_RTC_ISR_WAIT);
    }
}


static bool __rtc_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    rtc_test_t *test = vsf_container_of(hal_test, rtc_test_t, use_as__hal_test_t);
    VSF_ASSERT(test->device != NULL);

    vsf_rtc_capability_t cap = vsf_rtc_capability(test->device);
    if ((test->method & METHOD_ALARM_ISR) && ((cap.irq_mask & VSF_RTC_IRQ_MASK_ALARM) != VSF_RTC_IRQ_MASK_ALARM)) {
        vsf_trace_error("this rtc does not support VSF_RTC_IRQ_MASK_ALARM interrupt" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    if ((test->method & METHOD_ALARM_ISR_MASK) == METHOD_ALARM_ISR) {
        test->irq_mask = VSF_RTC_IRQ_MASK_ALARM;
        test->expected_irq_mask = VSF_RTC_IRQ_MASK_ALARM;
        test->cfg.isr.handler_fn = __rtc_isr_handler;
        test->cfg.isr.target_ptr = test;
    }

    test->hal_cfg = (void *)&test->cfg;

    return true;
}

static vsf_err_t __rtc_demo_set(rtc_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_rtc_t *rtc_ptr = test->device;
    VSF_ASSERT(rtc_ptr != NULL);

    vsf_err_t err = vsf_rtc_set(rtc_ptr, &test->rtc_tm);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_rtc_set(&%s, %p/*sbuf*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, &test->rtc_tm, err);
    }
    if (err != VSF_ERR_NONE) {
        vsf_trace_error("vsf_rtc_set faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }

    return err;
}

static vsf_err_t __rtc_demo_get(rtc_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_rtc_t *rtc_ptr = test->device;
    VSF_ASSERT(rtc_ptr != NULL);

    vsf_err_t err = vsf_rtc_get(rtc_ptr, &test->rtc_tm);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_rtc_get(&%s, %p/*rtc_tm*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, &test->rtc_tm, err);
    }
    if (err != VSF_ERR_NONE) {
        vsf_trace_error("vsf_rtc_get faild: %d" VSF_TRACE_CFG_LINEEND, err);
        return err;
    }

    return err;
}

static vsf_err_t __rtc_demo_set_time(rtc_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_rtc_t *rtc_ptr = test->device;
    VSF_ASSERT(rtc_ptr != NULL);

    vsf_err_t err = vsf_rtc_set_time(rtc_ptr, test->second, test->millisecond);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_rtc_set_time(&%s, %u, %u) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->second, test->millisecond, err);
    }
    if (err != VSF_ERR_NONE) {
        vsf_trace_error("vsf_rtc_set_time faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }

    return err;
}

static vsf_err_t __rtc_demo_get_time(rtc_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_rtc_t *rtc_ptr = test->device;
    VSF_ASSERT(rtc_ptr != NULL);

    vsf_err_t err = vsf_rtc_get_time(rtc_ptr, &test->second, &test->millisecond);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_rtc_get_time(&%s, %p, %p) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, &test->second, &test->millisecond, err);
    }
    if (err != VSF_ERR_NONE) {
        vsf_trace_error("vsf_rtc_get_time faild: %d" VSF_TRACE_CFG_LINEEND, err);
        return err;
    }

    return err;
}

static void __rtc_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    rtc_test_t *test = vsf_container_of(eda, rtc_test_t, teda);
    vsf_rtc_t *rtc_ptr        = test->device;
    VSF_ASSERT(rtc_ptr != NULL);

    switch (evt) {
    case VSF_EVT_HAL_TEST_START:
        if ((test->method & METHOD_SET_MASK) == METHOD_SET) {
            if (VSF_ERR_NONE != __rtc_demo_set(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else if ((test->method & METHOD_SET_MASK) == METHOD_SET_TIME) {
            if (VSF_ERR_NONE != __rtc_demo_set_time(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        }

    case VSF_EVT_HAL_TEST_RUN:
        if ((test->method & METHOD_GET_MASK) == METHOD_GET) {
            if (VSF_ERR_NONE != __rtc_demo_get(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);;
                break;
            }
            struct tm tm = {
                .tm_sec = test->rtc_tm.tm_sec,
                .tm_min = test->rtc_tm.tm_min,
                .tm_hour = test->rtc_tm.tm_hour,
                .tm_mday = test->rtc_tm.tm_mday,
                .tm_mon = test->rtc_tm.tm_mon - 1,
                .tm_year = test->rtc_tm.tm_year - 1900,
                .tm_wday = test->rtc_tm.tm_wday - 1,
                .tm_yday = 0,
                .tm_isdst = 0,
                .tm_gmtoff = 0,
                .tm_zone = NULL,
            };

            char buffer[80];
            size_t offset = strftime(buffer, sizeof(buffer) - 1, APP_RTC_DEMO_CFG_TIME_FORMAT, &tm);
            VSF_ASSERT(offset != 0);
            vsf_trace_info("current Time: %s" VSF_TRACE_CFG_LINEEND, buffer);
        }

        if ((test->method & METHOD_GET_TIME_MASK) == METHOD_GET_TIME) {
            if (VSF_ERR_NONE != __rtc_demo_get_time(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
            vsf_trace_info("current time: %lld.%lld" VSF_TRACE_CFG_LINEEND, (long long)test->second, (long long)test->millisecond);
        }

        if ((test->method & METHOD_ALARM_ISR_MASK) != METHOD_ALARM_ISR) {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
        } else {
            vsf_teda_set_timer_ms(test->timeout_ms.test + 10);        // add little delay
        }
        break;

    case VSF_EVT_RTC_ISR_WAIT:
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        break;

    case VSF_EVT_TIMER:
        if ((test->method & METHOD_ALARM_ISR_MASK) != METHOD_ALARM_ISR) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        } else {
            test->test_pass = false;
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_END);
        }
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __rtc_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    VSF_ASSERT(hal_test != NULL);
    rtc_test_t *test = (rtc_test_t *)hal_test;

    char *tm_format_str = NULL;

    vsf_err_t err = VSF_ERR_NONE;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::m:r:d:p:i:t:s:e:";
    static const struct option __long_options[] = {
        { "help",        no_argument,       NULL, 'h'  },
        { "list-device", no_argument,       NULL, 'l'  },
        { "verbose",     optional_argument, NULL, 'v'  },
        { "method",      required_argument, NULL, 'm'  },
        { "repeat",      required_argument, NULL, 'r'  },
        { "device",      required_argument, NULL, 'd'  },
        { "prio",        required_argument, NULL, 'p'  },
        { "isr_prio",    required_argument, NULL, 'i'  },
        { "timeout",     required_argument, NULL, 't'  },
        { "tm",          required_argument, NULL, 's' },
        { "time",        required_argument, NULL, 'e' },
        {NULL,          0,                  NULL, '\0'},
    };

    optind = 1;
    while (EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch (c) {
        case 's':
            test->method = (test->method & ~METHOD_SET_MASK) | METHOD_SET;
            if ((optind < argc) && (argv[optind][0] != '-')) {
                tm_format_str = argv[optind++];
            } else {
                tm_format_str = "%Y-%m-%u %H:%M:%S";
            }
            //struct tm tm;
            //if (NULL == strptime(optarg, tm_format_str, &tm)) {
            //    vsf_trace_error("time string or time format error: %s, %s" VSF_TRACE_CFG_LINEEND, optarg, tm_format_str);
            //}
            vsf_trace_error("TODO: support strptime(\"%s\", \"%s\", &tm)" VSF_TRACE_CFG_LINEEND, optarg, tm_format_str);
            break;
        case 'e':
            test->method = (test->method & ~METHOD_SET_MASK) | METHOD_SET_TIME;
            test->second = strtoul(optarg, NULL, 0);
            if ((optind < argc) && (argv[optind][0] != '-')) {
                test->millisecond = strtoul(argv[optind], NULL, 0);
                if (test->millisecond >= 1000) {
                    vsf_trace_error("milli second must be less than 1000!" VSF_TRACE_CFG_LINEEND);
                    return VSF_ERR_FAIL;
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

    return err;
}

int rtc_main(int argc, char *argv[])
{
    return hal_main(&__rtc_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
   return hal_main(&__rtc_demo.use_as__hal_demo_t);
}
#endif

#endif
