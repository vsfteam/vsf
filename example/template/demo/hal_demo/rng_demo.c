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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_RNG_DEMO == ENABLED && VSF_HAL_USE_RNG == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_RNG_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#   undef VSF_RNG_CFG_PREFIX
#   define VSF_RNG_CFG_PREFIX                       APP_RNG_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#endif

#ifndef APP_RNG_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_RNG_DEMO_CFG_DEFAULT_INSTANCE        vsf_hw_rng0
#endif

#ifndef APP_RNG_DEMO_CFG_DEVICES_COUNT
#   define APP_RNG_DEMO_CFG_DEVICES_COUNT           1
#endif

#ifndef APP_RNG_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_RNG_DEMO_CFG_DEVICES_ARRAY_INIT      \
        { .cnt = dimof(vsf_hw_rng_devices), .devices = vsf_hw_rng_devices},
#endif

#ifndef APP_RNG_DEMO_CFG_DEFAULT_NUMBER
#   define APP_RNG_DEMO_CFG_DEFAULT_NUMBER          64
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum rng_demo_evt_t {
    VSF_EVT_RNG_WAIT = __VSF_EVT_HAL_LAST,
} rng_demo_evt_t;

typedef struct rng_test_t  {
    implement(hal_test_t)
    uint32_t num;
} rng_test_t;

typedef struct rng_demo_const_t {
    implement(hal_demo_const_t)
    rng_test_t test;
} rng_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

HAL_DEMO_INIT(rng, APP_RNG,
    "rng-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help             show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]  verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device      list device" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE    i.e. vsf_hw_rng0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT    repeat count" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X  test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT  test timeout" VSF_TRACE_CFG_LINEEND
    "  -n, --num              @ref num in vsf_rng_generate_request(..., num)" VSF_TRACE_CFG_LINEEND,

    .test.recv.size = APP_RNG_DEMO_CFG_DEFAULT_NUMBER,
    .init_has_cfg = false,
    .device_init  = (hal_init_fn_t)vsf_rng_init,
    .device_fini  = (hal_fini_fn_t)vsf_rng_fini,
);

/*============================ IMPLEMENTATION ================================*/

static bool __rng_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    rng_test_t *test = vsf_container_of(hal_test, rng_test_t, use_as__hal_test_t);

    VSF_ASSERT(test->device != NULL);

    vsf_rng_capability_t cap = vsf_rng_capability(test->device);
    (void)cap;

    return true;
}

void __rng_on_ready_cb(void *param, uint32_t *buffer, uint32_t num)
{
    rng_test_t *test = (rng_test_t *)param;
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(buffer != NULL);
    VSF_ASSERT(num == test->recv.size);
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    vsf_eda_post_evt(eda, VSF_EVT_RNG_WAIT);
}

static vsf_err_t __rng_generate_request(rng_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_rng_t *rng_ptr = test->device;
    VSF_ASSERT(rng_ptr != NULL);

    vsf_err_t err = vsf_rng_generate_request(rng_ptr, (void *)test->recv.buffer, test->recv.size, test, __rng_on_ready_cb);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_rng_generate_request(&%s, %p/*buf*/, %u/*num*/, %p/*param*/, %p/*ready_cb*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->recv.buffer, test->recv.size, test, __rng_on_ready_cb, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("rng request faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static void __rng_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    rng_test_t *test = vsf_container_of(eda, rng_test_t, teda);
    vsf_rng_t *rng_ptr = test->device;
    VSF_ASSERT(rng_ptr != NULL);

    switch (evt) {
    case VSF_EVT_HAL_TEST_RUN:
        vsf_teda_set_timer_ms(test->timeout_ms.test);
        if (VSF_ERR_NONE != __rng_generate_request(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        }
        break;

    case VSF_EVT_RNG_WAIT:
        vsf_teda_cancel_timer();
        if (test->verbose >= 1) {
            vsf_trace_debug("rng data:" VSF_TRACE_CFG_LINEEND);
            vsf_trace_buffer(VSF_TRACE_DEBUG, test->recv.buffer, test->recv.size);
        }
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        break;

    case VSF_EVT_TIMER:
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __rng_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    rng_test_t *test = (rng_test_t *)hal_test;
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    VSF_ASSERT(demo != NULL);

    vsf_err_t err = VSF_ERR_NONE;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::d:r:p:n:";
    static const struct option __long_options[] = {
        { "help",           no_argument,       NULL, 'h'  },
        { "list-device",    no_argument,       NULL, 'l'  },
        { "verbose",        optional_argument, NULL, 'v'  },
        { "device",         required_argument, NULL, 'd'  },
        { "repeat",     required_argument, NULL, 'r'  },
        { "prio",           required_argument, NULL, 'p'  },
        { "number",         required_argument, NULL, 'n'  },
        { NULL,             0,                 NULL, '\0' },
    };

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch(c) {
        case 'n':
            test->recv.size = strtol(optarg, NULL, 0);
            if (test->recv.size == 0) {
                vsf_trace_error("request counter cannot be zero!" VSF_TRACE_CFG_LINEEND);
                return VSF_ERR_FAIL;
            }
            break;

        default:
            err = hal_test_evthandler(hal_test, argc, argv, c);
            if (err < 0) {
                return err;
            }
            break;
        }
    }

    return err;
}

int rng_main(int argc, char *argv[])
{
    return hal_main(&__rng_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__rng_demo, argc, argv);
}
#endif

#endif
