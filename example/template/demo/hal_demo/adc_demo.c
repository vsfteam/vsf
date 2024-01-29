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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_ADC_DEMO == ENABLED && VSF_HAL_USE_ADC == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_ADC_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#   undef VSF_ADC_CFG_PREFIX
#   define VSF_ADC_CFG_PREFIX                       APP_ADC_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#endif

#ifndef APP_ADC_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_ADC_DEMO_CFG_DEFAULT_INSTANCE        vsf_hw_adc0
#endif

#ifndef APP_ADC_DEMO_CFG_DEVICES_COUNT
#   define APP_ADC_DEMO_CFG_DEVICES_COUNT           1
#endif

#ifndef APP_ADC_DEMO_CFG_CHANNEL_COUNT
#   define APP_ADC_DEMO_CFG_CHANNEL_COUNT           8
#endif

#ifndef APP_ADC_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_ADC_DEMO_CFG_DEVICES_ARRAY_INIT      \
        { .cnt = dimof(vsf_hw_adc_devices), .devices = vsf_hw_adc_devices},
#endif

#ifndef APP_ADC_DEMO_CFG_DEFAULT_MODE
#   define APP_ADC_DEMO_CFG_DEFAULT_MODE            (VSF_ADC_SCAN_CONV_SEQUENCE_MODE | VSF_ADC_REF_VDD_1)
#endif

#ifndef APP_ADC_DEMO_CFG_DEFAULT_CHANNEL
#   define APP_ADC_DEMO_CFG_DEFAULT_CHANNEL         0
#endif

#ifndef APP_ADC_DEMO_CFG_DEFAULT_SAMPLE_CYCLES
#   define APP_ADC_DEMO_CFG_DEFAULT_SAMPLE_CYCLES   0
#endif

#ifndef APP_ADC_DEMO_CFG_DEFAULT_CLOCK
#   define APP_ADC_DEMO_CFG_DEFAULT_CLOCK           (1 * 1000 * 1000)
#endif

#ifndef APP_ADC_DEMO_CFG_CHANNEL_DEFAULT_MODE
#   define APP_ADC_DEMO_CFG_CHANNEL_DEFAULT_MODE    (VSF_ADC_CHANNEL_GAIN_1 | VSF_ADC_CHANNEL_REF_VDD_1)
#endif

#ifndef APP_ADC_DEMO_CFG_MAX_CHANNEL
#   define APP_ADC_DEMO_CFG_MAX_CHANNEL             8
#endif

#define __ADC_CHN_INIT(__N, __)                                                 \
    .test.chn_cfgs[__N].mode          = APP_ADC_DEMO_CFG_CHANNEL_DEFAULT_MODE,  \
    .test.chn_cfgs[__N].sample_cycles = APP_ADC_DEMO_CFG_DEFAULT_SAMPLE_CYCLES, \
    .test.chn_cfgs[__N].channel       = APP_ADC_DEMO_CFG_DEFAULT_CHANNEL,

/*============================ IMPLEMENTATION ================================*/
/*============================ TYPES =========================================*/

typedef enum adc_method_t {
    METHOD_REQUEST         = 0x00 << 0,
    METHOD_REQUEST_ONCE    = 0x01 << 0,
    METHOD_TRANSFER_MASK   = 0x01 << 0,

    METHOD_WAIT_POLL       = 0x00 << 1,
    METHOD_WAIT_ISR        = 0x01 << 1,
    METHOD_WAIT_MASK       = 0x01 << 1,
} adc_method_t;

typedef enum adc_demo_evt_test_t {
    VSF_TEST_ADC_REQUEST_ONE = 0x00 << 0x00,
    VSF_TEST_ADC_REQUEST     = 0x01 << 0x00,
} adc_demo_evt_test_t;

typedef enum adc_demo_evt_t {
    VSF_EVT_ADC_REQUEST_POLL_WAIT = __VSF_EVT_HAL_LAST,
    VSF_EVT_ADC_REQUEST_ONE_ISR_WAIT,
    VSF_EVT_ADC_REQUEST_ISR_WAIT,
} adc_demo_evt_t;

typedef struct adc_test_t  {
    implement(hal_test_t)
    vsf_adc_cfg_t cfg;
    uint8_t bytes;
    uint8_t chn_count;
    uint16_t req_cnt;
    uint16_t offset;
    vsf_adc_channel_cfg_t chn_cfgs[APP_ADC_DEMO_CFG_MAX_CHANNEL];
} adc_test_t;

typedef struct adc_demo_const_t {
    implement(hal_demo_const_t)
    adc_test_t test;
} adc_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __mode_options[] = {
    HAL_DEMO_OPTION(VSF_ADC_REF_VDD_MASK, VSF_ADC_REF_VDD_1),
    HAL_DEMO_OPTION(VSF_ADC_REF_VDD_MASK, VSF_ADC_REF_VDD_1_2),
    HAL_DEMO_OPTION(VSF_ADC_REF_VDD_MASK, VSF_ADC_REF_VDD_1_3),
    HAL_DEMO_OPTION(VSF_ADC_REF_VDD_MASK, VSF_ADC_REF_VDD_1_4),

    HAL_DEMO_OPTION(VSF_ADC_DATA_ALIGN_MASK, VSF_ADC_DATA_ALIGN_RIGHT),
    HAL_DEMO_OPTION(VSF_ADC_DATA_ALIGN_MASK, VSF_ADC_DATA_ALIGN_LEFT),

    HAL_DEMO_OPTION(VSF_ADC_SCAN_CONV_MASK, VSF_ADC_SCAN_CONV_SINGLE_MODE),
    HAL_DEMO_OPTION(VSF_ADC_SCAN_CONV_MASK, VSF_ADC_SCAN_CONV_SEQUENCE_MODE),

    HAL_DEMO_OPTION(VSF_ADC_EXTERN_TRIGGER_MASK, VSF_ADC_EXTERN_TRIGGER_0),
    HAL_DEMO_OPTION(VSF_ADC_EXTERN_TRIGGER_MASK, VSF_ADC_EXTERN_TRIGGER_1),
    HAL_DEMO_OPTION(VSF_ADC_EXTERN_TRIGGER_MASK, VSF_ADC_EXTERN_TRIGGER_2),
};

static const hal_option_t __irq_options[] = {
    HAL_DEMO_OPTION(VSF_ADC_IRQ_MASK_CPL, VSF_ADC_IRQ_MASK_CPL),
};

static const hal_option_t __chn_mode_options[] = {
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_GAIN_MASK, VSF_ADC_CHANNEL_GAIN_1_6),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_GAIN_MASK, VSF_ADC_CHANNEL_GAIN_1_5),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_GAIN_MASK, VSF_ADC_CHANNEL_GAIN_1_5),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_GAIN_MASK, VSF_ADC_CHANNEL_GAIN_1_4),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_GAIN_MASK, VSF_ADC_CHANNEL_GAIN_1_3),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_GAIN_MASK, VSF_ADC_CHANNEL_GAIN_1_2),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_GAIN_MASK, VSF_ADC_CHANNEL_GAIN_1),

    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_REF_VDD_MASK, VSF_ADC_CHANNEL_REF_VDD_1),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_REF_VDD_MASK, VSF_ADC_CHANNEL_REF_VDD_1_2),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_REF_VDD_MASK, VSF_ADC_CHANNEL_REF_VDD_1_3),
    HAL_DEMO_OPTION(VSF_ADC_CHANNEL_REF_VDD_MASK, VSF_ADC_CHANNEL_REF_VDD_1_4),
};

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_TRANSFER_MASK, METHOD_REQUEST, "req"),
    HAL_DEMO_OPTION_EX(METHOD_TRANSFER_MASK, METHOD_REQUEST_ONCE, "req-once"),

    HAL_DEMO_OPTION_EX(METHOD_WAIT_MASK, METHOD_WAIT_POLL, "poll"),
    HAL_DEMO_OPTION_EX(METHOD_WAIT_MASK, METHOD_WAIT_ISR,  "isr"),
};

HAL_DEMO_INIT(adc, APP_ADC,
    "adc-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                      show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]           verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device               list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [req|req-once] [poll|isr]" VSF_TRACE_CFG_LINEEND
    "  -d, --device device             i.e. vsf_hw_adc0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT             repeat count" VSF_TRACE_CFG_LINEEND
    "  -c, --mode VSF_ADC_REF_VDD_1... @ref \"vsf_adc_mode_t\"" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X           test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X  interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT           timeout of sample failure" VSF_TRACE_CFG_LINEEND
    "  -f, --freq FREQ                 clock_hz in @ref \"vsf_adc_cfg_t\"" VSF_TRACE_CFG_LINEEND
    "  -g, --chn_cfg CHANNEL SAMPLE-CYCLES VSF_ADC_CHANNEL_GAIN_1..." VSF_TRACE_CFG_LINEEND
    "                                  @ref \"vsf_adc_channel_mode_t\"" VSF_TRACE_CFG_LINEEND
    "  -s, --sample-count COUNTER      @ref \"vsf_adc_channel_request(..., count)\"" VSF_TRACE_CFG_LINEEND,

    .test.method = METHOD_REQUEST | METHOD_WAIT_POLL,

    .test.cfg.mode                  = APP_ADC_DEMO_CFG_DEFAULT_MODE,
    .test.cfg.clock_hz              = APP_ADC_DEMO_CFG_DEFAULT_CLOCK,

    .test.chn_count                 = 1,
    .test.req_cnt                   = 1,
    VSF_MREPEAT(APP_ADC_DEMO_CFG_MAX_CHANNEL, __ADC_CHN_INIT, NULL)

    .init_has_cfg                   = true,
    .device_init                    = (hal_init_fn_t       )vsf_adc_init,
    .device_fini                    = (hal_fini_fn_t       )vsf_adc_fini,
    .device_enable                  = (hal_enable_fn_t     )vsf_adc_enable,
    .device_disable                 = (hal_disable_fn_t    )vsf_adc_disable,
    .device_irq_enable              = (hal_irq_enable_fn_t )vsf_adc_irq_enable,
    .device_irq_disable             = (hal_irq_disable_fn_t)vsf_adc_irq_disable,
    .mode.options                   = __mode_options,
    .mode.cnt                       = dimof(__mode_options),
    .irq.options                    = __irq_options,
    .irq.cnt                        = dimof(__irq_options),
    .method.options                 = __method_options,
    .method.cnt                     = dimof(__method_options),
);

/*============================ IMPLEMENTATION ================================*/

static void __adc_isr_handler(void *target_ptr, vsf_adc_t *adc_ptr, vsf_adc_irq_mask_t irq_mask)
{
    adc_test_t *test = (adc_test_t *)target_ptr;
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(adc_ptr != NULL);
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    if (!hal_test_irq_check(&test->use_as__hal_test_t, irq_mask)) {
        return;
    }

    if (irq_mask & VSF_ADC_IRQ_MASK_CPL) {
        test->expected_irq_mask &= ~VSF_ADC_IRQ_MASK_CPL;
        vsf_eda_post_evt(eda, VSF_EVT_ADC_REQUEST_ONE_ISR_WAIT);
    }
}

static bool __adc_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    adc_test_t *test = vsf_container_of(hal_test, adc_test_t, use_as__hal_test_t);

    VSF_ASSERT(test->device != NULL);

    if (test->verbose >= 1) {
        hal_options_trace(VSF_TRACE_DEBUG, "vsf_adc_cfg_t cfg = { .mode = ", __mode_options, dimof(__mode_options), test->cfg.mode);
        vsf_trace_debug(", .clock = %d, .isr = { .prio = vsf_arch_prio_%u }};" VSF_TRACE_CFG_LINEEND, test->cfg.clock_hz, hal_demo_arch_prio_to_num(test->cfg.isr.prio));

        vsf_trace_debug("vsf_adc_chn_cfg_t chns[] = {" VSF_TRACE_CFG_LINEEND);
        for (int i = 0; i < test->chn_count; i++) {
            vsf_trace_debug("  { .chn = %d, .sample_cycles = %d, ", test->chn_cfgs[i].channel, test->chn_cfgs[i].sample_cycles);
            hal_options_trace(VSF_TRACE_DEBUG, "mode: ", __chn_mode_options, dimof(__chn_mode_options), test->chn_cfgs[i].mode);
            vsf_trace_debug("},"VSF_TRACE_CFG_LINEEND);
        }
        vsf_trace_debug("};" VSF_TRACE_CFG_LINEEND);
    }

    vsf_adc_capability_t cap = vsf_adc_capability(test->device);

    if (cap.max_data_bits == 0) {
        vsf_trace_error("max_data_bits cannot be zero" VSF_TRACE_CFG_LINEEND, cap.channel_count, test->chn_count);
        return false;
    }

    test->bytes = (cap.max_data_bits + 7) / 8;
    test->recv.size = test->req_cnt * test->bytes;

    if (test->chn_count > cap.channel_count) {
        vsf_trace_error("max chn(%u), too many channels(%u)" VSF_TRACE_CFG_LINEEND, cap.channel_count, test->chn_count);
        return false;
    }

    for (int i = 0; i < test->chn_count; i++) {
        if (test->chn_cfgs[i].channel >= cap.channel_count) {
            vsf_trace_error("chn number(%u) is greater than the maximum number of channels" VSF_TRACE_CFG_LINEEND, test->chn_cfgs[i].channel, cap.channel_count);
            return false;
        }
    }

    if ((test->method & METHOD_WAIT_MASK) == METHOD_WAIT_ISR) {
        test->irq_mask = VSF_ADC_IRQ_MASK_CPL;
        test->expected_irq_mask = VSF_ADC_IRQ_MASK_CPL;
        test->cfg.isr.handler_fn = __adc_isr_handler;
        test->cfg.isr.target_ptr = test;
    }

    test->hal_cfg = (void *)&test->cfg;

    return true;
}

static vsf_err_t __channel_request_once(adc_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_adc_t *adc_ptr = test->device;
    VSF_ASSERT(adc_ptr != NULL);
    VSF_ASSERT(test->recv.buffer != 0);

    vsf_adc_channel_cfg_t *cfg = &test->chn_cfgs[test->offset % test->chn_count];
    void *value_ptr = test->recv.buffer + test->offset * test->bytes;

    vsf_err_t err = vsf_adc_channel_request_once(adc_ptr, cfg, value_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_adc_channel_request_once(&%s, %p/*cfg addr*/, %p/*value addr*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, cfg, value_ptr, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("adc chn request once faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __channel_request_config(adc_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_adc_t *adc_ptr = test->device;
    VSF_ASSERT(adc_ptr != NULL);

    vsf_err_t err = vsf_adc_channel_config(adc_ptr, test->chn_cfgs, test->chn_count);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_adc_channel_config(&%s, %p/*sbuf*/, %u) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->chn_cfgs, test->chn_count, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("adc chn config faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __channel_request(adc_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_adc_t *adc_ptr = test->device;
    VSF_ASSERT(adc_ptr != NULL);

    vsf_err_t err = vsf_adc_channel_request(adc_ptr, test->recv.buffer, test->req_cnt);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_adc_channel_request(&%s, %p/*sbuf*/, %u) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->recv.buffer, test->req_cnt, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("adc chn request faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static bool __adc_wait_idle(adc_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_adc_t *adc_ptr = test->device;
    VSF_ASSERT(adc_ptr != NULL);

    vsf_adc_status_t status = vsf_adc_status(adc_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_adc_status(&%s); // %s" VSF_TRACE_CFG_LINEEND, test->device_name, status.is_busy ? "busy" : "idle");
    }
    return !status.is_busy;
}

static void __adc_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    adc_test_t *test = vsf_container_of(eda, adc_test_t, teda);
    vsf_adc_t * adc_ptr = test->device;
    VSF_ASSERT(adc_ptr != NULL);

    switch (evt) {
    case VSF_EVT_HAL_TEST_START:
        if ((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST) {
            if (VSF_ERR_NONE != __channel_request_config(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else {
            test->offset = 0;
        }

    case VSF_EVT_HAL_TEST_RUN:
        if ((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST) {
            if (VSF_ERR_NONE != __channel_request(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else if ((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST_ONCE) {
            if (VSF_ERR_NONE != __channel_request_once(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else {
            VSF_ASSERT(0);
        }

        if ((test->method & METHOD_WAIT_MASK) == METHOD_WAIT_POLL) {
            vsf_eda_post_evt(eda, VSF_EVT_ADC_REQUEST_POLL_WAIT);
        } else {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
        }
        break;

    case VSF_EVT_ADC_REQUEST_POLL_WAIT:
        if (!__adc_wait_idle(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_ADC_REQUEST_POLL_WAIT);
            break;
        }

        if ((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST_ONCE) {
            test->offset++;
            if (test->offset < test->req_cnt) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_RUN);
            } else {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
            }
        } else {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        }
        break;

    case VSF_EVT_ADC_REQUEST_ONE_ISR_WAIT:
        VSF_ASSERT((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST_ONCE);
        VSF_ASSERT((test->method & METHOD_WAIT_MASK) == METHOD_WAIT_ISR);
        vsf_teda_cancel_timer();
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        break;

    case VSF_EVT_ADC_REQUEST_ISR_WAIT:
        VSF_ASSERT((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST);
        VSF_ASSERT((test->method & METHOD_WAIT_MASK) == METHOD_WAIT_ISR);
        vsf_teda_cancel_timer();
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);;
        break;

    case VSF_EVT_TIMER:
        vsf_trace_error("adc timeout, not received interrupt" VSF_TRACE_CFG_LINEEND);
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __adc_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    adc_test_t *test = (adc_test_t *)hal_test;
    VSF_ASSERT(test != NULL);
    adc_demo_t *demo = (adc_demo_t *)test->demo;
    VSF_ASSERT(demo != NULL);
    const adc_demo_const_t *demo_const_ptr = (const adc_demo_const_t *)demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    uint8_t chn_count = 0;

    vsf_err_t err = VSF_ERR_NONE;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::r:d:c:m:p:i:t:f:g:s:";
    static const struct option __long_options[] = {
        { "help",           no_argument,       NULL, 'h'  },
        { "list-device",    no_argument,       NULL, 'l'  },
        { "verbose",        optional_argument, NULL, 'v'  },
        { "repeat",         required_argument, NULL, 'r'  },
        { "device",         required_argument, NULL, 'd'  },
        { "config",         required_argument, NULL, 'c'  },
        { "method",         required_argument, NULL, 'm'  },
        { "prio",           required_argument, NULL, 'p'  },
        { "isr_prio",       required_argument, NULL, 'i'  },
        { "timeout",        required_argument, NULL, 't'  },
        { "freq",           required_argument, NULL, 'f'  },
        { "chn_cfg",        required_argument, NULL, 'g' },
        { "simple-count",   required_argument, NULL, 's' },
        { NULL,             0,                 NULL, '\0' },
    };

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch(c) {
        case 'g':
            if ((optind < argc)  && (argv[optind][0] != '-')) {
                test->chn_cfgs[chn_count].channel = strtol(optarg, NULL, 0);
                test->chn_cfgs[chn_count].sample_cycles = strtol(argv[optind++], NULL, 0);
                if ((optind < argc) && (argv[optind][0] != '-')) {
                    uint32_t mode_value = 0;
                    uint32_t mode_mask = 0;
                    if (!hal_options_get_value(argc, argv, __chn_mode_options, dimof(__chn_mode_options),
                                             &mode_value, &mode_mask)) {
                        vsf_trace_error("chn config error!" VSF_TRACE_CFG_LINEEND);
                        return VSF_ERR_FAIL;
                    }
                    test->chn_cfgs[chn_count].mode = (test->chn_cfgs[chn_count].mode & ~mode_mask) | mode_value;
                }
                chn_count++;
            }
            break;
        case 's':
            test->req_cnt = strtol(optarg, NULL, 0);
            if (test->req_cnt == 0) {
                vsf_trace_error("request counter cannot be zero!" VSF_TRACE_CFG_LINEEND);
                return VSF_ERR_FAIL;
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

    if (test->mode_mask) {
        test->cfg.mode = (test->cfg.mode & ~test->mode_mask) | test->mode_value;
    }

    if (chn_count != 0) {
        test->chn_count = chn_count;
    }

    if (test->req_cnt < test->chn_count) {
        vsf_trace_error("the number of reqeust(%u) cannot be smaller than the number of channes(%u)!" VSF_TRACE_CFG_LINEEND,
                        test->req_cnt, test->chn_count);
        return VSF_ERR_FAIL;
    }

    return err;
}

int adc_main(int argc, char *argv[])
{
    return hal_main(&__adc_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__adc_demo.use_as__hal_demo_t);
}
#endif

#endif
