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

#if APP_USE_HAL_DEMO == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if APP_USE_LINUX_DEMO == ENABLED
static void __print_usage(const hal_demo_t *demo);
#endif

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_arch_prio_t __arch_prios[VSF_ARCH_PRI_NUM] = {
#define HAL_DEMO_ARCH_PRIO(__N, __X)  vsf_arch_prio_ ## __N,

    VSF_MREPEAT(VSF_ARCH_PRI_NUM, HAL_DEMO_ARCH_PRIO, NULL)
};

static const vsf_prio_t __prios[VSF_ARCH_PRI_NUM] = {
#define HAL_DEMO_PRIO_M(__N, __X)  vsf_prio_ ## __N,
#define MFUNC_IN_U8_DEC_VALUE                    VSF_OS_CFG_PRIORITY_NUM
#   include "utilities/preprocessor/mf_u8_dec2str.h"

    VSF_MREPEAT(MFUNC_OUT_DEC_STR, HAL_DEMO_PRIO_M, NULL)
};

/*============================ IMPLEMENTATION ================================*/

vsf_arch_prio_t hal_demo_num_to_arch_prio(uint32_t i)
{
    if (i >= VSF_ARCH_PRI_NUM) {
        return vsf_arch_prio_invalid;
    }
    return __arch_prios[i];
}

int hal_demo_arch_prio_to_num(vsf_arch_prio_t prio)
{
    for (int i = 0; i < VSF_ARCH_PRI_NUM; i++) {
        if (__arch_prios[i] == prio) {
            return i;
        }
    }

    return -1;
}

vsf_prio_t hal_demo_num_to_prio(uint32_t i)
{
    if (i >= VSF_OS_CFG_PRIORITY_NUM) {
        return vsf_prio_inherit;
    }
    return (vsf_prio_t)__prios[i];
}

int hal_demo_prio_to_num(vsf_prio_t prio)
{
    for (int i = 0; i < VSF_ARCH_PRI_NUM; i++) {
        if (__prios[i] == prio) {
            return i;
        }
    }

    return -1;
}

static void __scan_device(hal_demo_t * demo)
{
    VSF_ASSERT(demo != NULL);
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    if (demo_const_ptr->scan != NULL) {
        demo_const_ptr->scan(demo);
    }
}

const vsf_hal_device_t *hal_demo_find_device(hal_demo_t * demo, char *name)
{
    VSF_ASSERT(demo != NULL);
    VSF_ASSERT(name != NULL);

    __scan_device(demo);
    VSF_ASSERT(demo->array_cnt != 0);

    for (int i = 0; i < demo->array_cnt; i++) {
        uint8_t devices_cnt = demo->devices_array[i].cnt;
        const vsf_hal_device_t *devices = demo->devices_array[i].devices;
        VSF_ASSERT(devices_cnt != 0);
        VSF_ASSERT(devices != NULL);

        for (int j = 0; j < devices_cnt; j++) {
            if (strcmp(name, devices[j].name) == 0) {
                return &devices[j];
            }
        }
    }

    return NULL;
}

void __list_devices(hal_demo_t * demo, bool verbose)
{
    VSF_ASSERT(demo != NULL);
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    __scan_device(demo);
    VSF_ASSERT(demo->array_cnt != 0);

    for (int i = 0; i < demo->array_cnt; i++) {

        const vsf_hal_device_t * devices = demo->devices_array[i].devices;
        uint8_t devices_cnt = demo->devices_array[i].cnt;
        VSF_ASSERT(devices_cnt != 0);
        VSF_ASSERT(devices != NULL);

        for (int j = 0; j < devices_cnt; j++) {
            vsf_trace_info("%s" VSF_TRACE_CFG_LINEEND, devices[j].name);
        }
    }
}

const char *hal_option_to_str(const hal_option_t *options, uint8_t cnt, uint32_t value)
{
    VSF_ASSERT(options != NULL);
    VSF_ASSERT(cnt > 0);

    for (int i = 0; i < cnt; i++) {
        if ((value & options[i].mask) == options[i].value) {
            return options[i].name;
        }
    }

    return NULL;
}

bool hal_demo_options_get_value(const hal_option_t *options, uint8_t cnt, char *str, uint32_t *value_ptr)
{
    VSF_ASSERT(options != NULL);
    VSF_ASSERT(cnt >= 1);
    VSF_ASSERT(str != NULL);
    VSF_ASSERT(value_ptr != NULL);

    for (int i = 0; i < cnt; i++) {
        VSF_ASSERT(options[i].name != NULL);
        if (strcmp(options[i].name , str) == 0) {
            *value_ptr = options[i].value;
            return true;
        }
    }

    return false;
}

bool hal_options_get_value(int argc, char *argv[],
                          const hal_option_t *options, uint8_t cnt,
                          uint32_t *value_ptr, uint32_t *mask_ptr)
{
    VSF_ASSERT(options != NULL);
    VSF_ASSERT(cnt >= 1);
    VSF_ASSERT(argc >= 1);
    VSF_ASSERT(argv != NULL);
    VSF_ASSERT(value_ptr != NULL);
    VSF_ASSERT(mask_ptr != NULL);

    uint32_t value = 0;
    uint32_t mask = 0;

    for (; optind < argc; optind++) {
        char *name = argv[optind];
        if (name[0] == '-') {
            break;
        }

        // TODO: support hex value

        int j = 0;
        for (; j < cnt; j++) {
            VSF_ASSERT(options[j].name != NULL);
            if (strcmp(name, options[j].name) == 0) {
                value |= options[j].value;
                mask |= options[j].mask;
                break;
            }
        }
        if (j >= cnt) {
            vsf_trace_error("unknown options: %s" VSF_TRACE_CFG_LINEEND, name);
            return false;
        }
    }

    if (mask == 0) {
        return false;
    } else {
        *value_ptr = value;
        *mask_ptr = mask;
        return true;
    }
}

void hal_options_trace(vsf_trace_level_t level, const char * prefix_str,
                            const hal_option_t *options, uint8_t cnt,
                            uint32_t mode)
{
    VSF_ASSERT(options != NULL);
    VSF_ASSERT(cnt > 0);
    bool is_first = true;

    vsf_trace(level, prefix_str);

    uint32_t mask = 0;

    for (int i = 0; i < cnt; i++) {
        if ((mode & options[i].mask) == options[i].value) {

            vsf_trace(level, is_first ? "%s" : " | %s", options[i].name);
            mask |= options[i].mask;
            is_first = false;
        }
    }

    if (mode & ~mask) {
        vsf_trace(level, "| 0x%08X /*unknown*/", mode & ~mask);
    } else if (is_first) {
        vsf_trace(level, "0");
    }

    //vsf_trace(level, VSF_TRACE_CFG_LINEEND);
}

void hal_test_reset_timeout(hal_test_t *test)
{
    test->start_tick = 0;
}

bool hal_test_is_timeout(hal_test_t *test, vsf_systimer_tick_t timeout_ms)
{
    if (test->start_tick == 0) {
        test->start_tick = vsf_systimer_get_tick();
    }

    vsf_systimer_tick_t now = vsf_systimer_get_tick();
    vsf_systimer_tick_t duration = vsf_systimer_get_duration(test->start_tick, now);
    vsf_systimer_tick_t duration_ms = vsf_systimer_tick_to_ms(duration);

    if (duration_ms >= timeout_ms) {
        test->start_tick = 0;
        return true;
    } else {
        return false;
    }
}

static void __hal_demo_fini(hal_test_t *test)
{
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(test->demo != NULL);

    if (!test->test_pass) {
        vsf_trace_error("%s test failed!", test->device_name);
    }

    if (test->send.buffer) {
        vsf_heap_free(test->send.buffer);
        test->send.buffer = NULL;
    } else if (test->recv.buffer) {
        vsf_heap_free(test->recv.buffer);
        test->recv.buffer = NULL;
    }

    vsf_heap_free(test);
}

static vsf_err_t __hal_device_init(hal_test_t *test)
{
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);
    vsf_err_t err;

    if (demo_const_ptr->device_init == NULL) {
        return VSF_ERR_NONE;
    }

    if (demo_const_ptr->init_has_cfg) {
        err = demo_const_ptr->device_init(test->device, test->hal_cfg);
        if (test->verbose >= 2) {
            vsf_trace_debug("vsf_%s_init(&%s, &cfg) = %d" VSF_TRACE_CFG_LINEEND,
                            demo_const_ptr->name, test->device_name, (int)err);
        }
    } else {
        err = demo_const_ptr->device_no_cfg_init(test->device);
        if (test->verbose >= 2) {
            vsf_trace_debug("vsf_%s_init(&%s) = %d" VSF_TRACE_CFG_LINEEND, demo_const_ptr->name, test->device_name, (int)err);
        }
    }

    if (err != VSF_ERR_NONE) {
        vsf_trace_error("%s init failed: %d" VSF_TRACE_CFG_LINEEND, demo_const_ptr->name, err);
    }

    return err;
}

static void __hal_device_fini(hal_test_t *test)
{
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    if (demo_const_ptr->device_fini == NULL) {
        return ;
    }

    demo_const_ptr->device_fini(test->device);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_%s_fini(&%s)" VSF_TRACE_CFG_LINEEND,
                        demo_const_ptr->name, test->device_name);
    }
}

static fsm_rt_t __hal_device_enable(hal_test_t *test)
{
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    if (demo_const_ptr->device_enable == NULL) {
        return fsm_rt_cpl;
    }

    fsm_rt_t rt = demo_const_ptr->device_enable(test->device);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_%s_enable(&%s) = %d" VSF_TRACE_CFG_LINEEND,
                        demo_const_ptr->name, test->device_name, (int)rt);
    }
    if (rt == fsm_rt_on_going) {
        if (hal_test_is_timeout(test, 200)) {       // TODO: use timeout from args
            rt = fsm_rt_err;
        }
    }

    return rt;
}

static fsm_rt_t __hal_device_disable(hal_test_t *test)
{
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    if (demo_const_ptr->device_disable == NULL) {
        return fsm_rt_cpl;
    }

    fsm_rt_t rt = demo_const_ptr->device_disable(test->device);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_%s_disable(&%s) = %d" VSF_TRACE_CFG_LINEEND,
                        demo_const_ptr->name, test->device_name, (int)rt);
    }
    if (rt == fsm_rt_on_going) {
        if (hal_test_is_timeout(test, 200)) {       // TODO: use timeout from args
            rt = fsm_rt_err;
        }
    }

    return rt;
}

void hal_device_irq_enable(hal_test_t *test)
{
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    if (demo_const_ptr->device_irq_enable == NULL) {
        return ;
    }

    if (test->irq_mask == 0) {
        return ;
    }

    if (test->verbose >= 2) {
        VSF_ASSERT(demo_const_ptr->irq.options != NULL);
        VSF_ASSERT(demo_const_ptr->irq.cnt != 0);

        vsf_trace_debug("vsf_%s_irq_enable(&%s, 0x%08x)",
                        demo_const_ptr->name, test->device_name, test->irq_mask);
        hal_options_trace(VSF_TRACE_DEBUG, " // ",
                               demo_const_ptr->irq.options,
                               demo_const_ptr->irq.cnt,
                               test->irq_mask);
        vsf_trace_debug(VSF_TRACE_CFG_LINEEND);

    }

    demo_const_ptr->device_irq_enable(test->device, test->irq_mask);
}

static void __hal_device_irq_disable(hal_test_t *test)
{
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    if (demo_const_ptr->device_irq_disable == NULL) {
        return;
    }

    if (test->irq_mask == 0) {
        return;
    }

    if (test->verbose >= 2) {
        VSF_ASSERT(demo_const_ptr->irq.options != NULL);
        VSF_ASSERT(demo_const_ptr->irq.cnt != 0);

        vsf_trace_debug("vsf_%s_irq_disable(&%s, 0x%08x)",
                        demo_const_ptr->name, test->device_name, test->irq_mask);
        hal_options_trace(VSF_TRACE_DEBUG, " // ",
                               demo_const_ptr->irq.options,
                               demo_const_ptr->irq.cnt,
                               test->irq_mask);
        vsf_trace_debug(VSF_TRACE_CFG_LINEEND);
    }

    demo_const_ptr->device_irq_disable(test->device, test->irq_mask);
}

void hal_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    hal_test_t *test = vsf_container_of(eda, hal_test_t, teda);
    hal_demo_t *demo = test->demo;
    VSF_ASSERT(demo != NULL);

    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    fsm_rt_t rt;

    switch (evt) {
    case VSF_EVT_INIT:
        test->test_pass = true;
        if ((test->verbose > 1) && (demo_const_ptr->method.options != NULL)) {
            hal_options_trace(VSF_TRACE_DEBUG, "test method : ",
                                   demo_const_ptr->method.options,
                                   demo_const_ptr->method.cnt,
                                   test->method);
            vsf_trace_debug(VSF_TRACE_CFG_LINEEND);
        }
        // fall through

    case VSF_EVT_HAL_CHECK:
        VSF_ASSERT(demo_const_ptr->check_capability != NULL);
        if (!demo_const_ptr->check_capability(test)) {
            test->test_pass = false;
            vsf_eda_post_evt(eda, VSF_EVT_HAL_FREE_AND_REPORT);
            break;
        }
        // fall through

    case VSF_EVT_HAL_ALLOC:
        if ((test->send.size != 0) || (test->recv.size != 0)) {
            size_t size = test->send.size + test->recv.size;
            uint8_t *buffer = vsf_heap_malloc(size);
            if (buffer == NULL) {
                vsf_trace_error("%s alloc buffer (size: 0x%08x bytes) failed!" VSF_TRACE_CFG_LINEEND, size);
                vsf_eda_post_evt(eda, VSF_EVT_HAL_FREE_AND_REPORT);
                break;
            }
            if (test->send.size != 0) {
                test->send.buffer = buffer;
                buffer += test->send.size;
            }
            if (test->recv.size != 0) {
                test->recv.buffer = buffer;
            }
        }
        // fall through

    case VSF_EVT_HAL_CALL_INIT:
        if (VSF_ERR_NONE != __hal_device_init(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_FREE_AND_REPORT);
            break;
        }
        // fall through

    case VSF_EVT_HAL_CALL_ENABLE:
        rt = __hal_device_enable(test);
        if (rt == fsm_rt_on_going) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_ENABLE);
            break;
        } else if (rt != fsm_rt_cpl) {
            // the reset fails by default
            vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_FINI);
            break;
        }
        // fall through

    case VSF_EVT_HAL_CALL_IRQ_ENABLE:
        hal_device_irq_enable(test);
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_START);
        break;

    case VSF_EVT_HAL_TEST_START:
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_RUN);
        break;

    case VSF_EVT_HAL_TEST_RUN:
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        break;

    case VSF_EVT_HAL_TEST_NEXT:
        if (--test->repeat_cnt > 0) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_RUN);
        } else {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_END);
        }
        break;

    case VSF_EVT_HAL_TEST_FAILED:
        test->test_pass = false;
        vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_IRQ_DISABLE);
        break;

    case VSF_EVT_HAL_TEST_END:
        vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_IRQ_DISABLE);
        break;

    case VSF_EVT_HAL_CALL_IRQ_DISABLE:
        __hal_device_irq_disable(test);
        // fall through

    case VSF_EVT_HAL_CALL_DISABLE:
        rt = __hal_device_disable(test);
        if (rt == fsm_rt_on_going) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_DISABLE);
        } else if (rt != fsm_rt_cpl) {
            test->test_pass = false;
        }
        // fall through

    case VSF_EVT_HAL_CALL_FINI:
        __hal_device_fini(test);
        // fall through

    case VSF_EVT_HAL_FREE_AND_REPORT:
        __hal_demo_fini(test);
        break;

    default:
        VSF_ASSERT(0);
        break;
    }
}

bool hal_test_irq_check(hal_test_t *test, uint32_t irq_mask)
{
    const hal_demo_t *demo = test->demo;
    VSF_ASSERT(demo != NULL);
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    // TODO: Detection of repeated interruptions
    uint32_t unexpected_irq_mask = irq_mask & ~test->expected_irq_mask;
    if (unexpected_irq_mask) {
        VSF_ASSERT(demo_const_ptr->device_irq_disable != NULL);
        demo_const_ptr->device_irq_disable(test->device, test->irq_mask);

        if (test->verbose >= 2) {
            vsf_trace_debug("vsf_%s_irq_disable(&%s, 0x%08x)", demo_const_ptr->name, test->device_name, test->irq_mask);
            hal_options_trace(VSF_TRACE_DEBUG, " // ", demo_const_ptr->irq.options, demo_const_ptr->irq.cnt, test->irq_mask);
        }

        hal_options_trace(VSF_TRACE_ERROR, "%s unexpected interrupt:",
                               demo_const_ptr->irq.options, demo_const_ptr->irq.cnt,
                               (uint32_t)unexpected_irq_mask);
        vsf_trace_error("" VSF_TRACE_CFG_LINEEND);

        vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_FINI);
    }

    return unexpected_irq_mask ? false : true;
}

#if APP_USE_LINUX_DEMO == ENABLED
static void __print_usage(const hal_demo_t * demo)
{
    VSF_ASSERT(demo != NULL);
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    if (demo_const_ptr->help) {
        vsf_trace_string(VSF_TRACE_INFO, demo_const_ptr->help);
    }
}
#endif

vsf_err_t hal_test_evthandler(hal_test_t * test, int argc, char *argv[], char c)
{
    VSF_ASSERT(test != NULL);
    const hal_demo_t *demo = test->demo;
    VSF_ASSERT(demo != NULL);
    const hal_demo_const_t *demo_const_ptr = test->demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    uint32_t mode_value;
    uint32_t mode_mask;
    uint32_t method_value;
    uint32_t method_mask;
    const vsf_hal_device_t *device_ptr;
    vsf_prio_t prio;
    vsf_arch_prio_t arch_prio;

    vsf_err_t err = VSF_ERR_NONE;

    switch(c) {
    case 'h':
        __print_usage(test->demo);
        err = VSF_ERR_NOT_READY;
        test->test_pass = true;
        break;

    case 'l':
        __list_devices(test->demo, test->verbose);
        err = VSF_ERR_NOT_READY;
        test->test_pass = true;
        break;

    case 'v':
        if ((optarg == NULL) && (optind < argc) && (argv[optind][0] != '-')) {
            test->verbose = strtoul(argv[optind++], NULL, 0);
        } else {
            test->verbose = 1;
        }
        break;

    case 'm':
        method_value = 0;
        method_mask = 0;
        VSF_ASSERT(demo_const_ptr->method.options != NULL);
        optind--;
        if (hal_options_get_value(argc, argv, demo_const_ptr->method.options, demo_const_ptr->method.cnt, &method_value, &method_mask)) {
            test->method = (test->method & ~method_mask) | method_value;
        } else {
            vsf_trace_error("method config failed!" VSF_TRACE_CFG_LINEEND);
            err = VSF_ERR_FAIL;
        }
        break;

    case 'r':
        test->repeat_cnt = strtoul(optarg, NULL, 0);
        if (test->repeat_cnt == 0) {
            vsf_trace_error("repeat counter cannot be 0" VSF_TRACE_CFG_LINEEND);
            err = VSF_ERR_FAIL;
        }
        break;

    case 'd':
        device_ptr = hal_demo_find_device(test->demo, optarg);
        if (device_ptr == NULL) {
            vsf_trace_error("unknown device: %s" VSF_TRACE_CFG_LINEEND, optarg);
            err = VSF_ERR_FAIL;
        }
        strncpy(test->device_name, optarg, sizeof(test->device_name) - 1);
        test->device = device_ptr->pointer;
        break;

    case 'c':
        mode_value = 0;
        mode_mask = 0;
        optind--;
        VSF_ASSERT(demo_const_ptr->method.options != NULL);
        if (hal_options_get_value(argc, argv, demo_const_ptr->mode.options, demo_const_ptr->mode.cnt, &mode_value, &mode_mask)) {
            test->mode_value = mode_value;
            test->mode_mask = mode_mask;
        } else {
            vsf_trace_error("mode config failed!" VSF_TRACE_CFG_LINEEND);
            err = VSF_ERR_FAIL;
        }
        break;

    case 'p':
        if (0 == sscanf(optarg, "vsf_prio_%u", &prio)) {
            vsf_trace_error("unknown prio: %s" VSF_TRACE_CFG_LINEEND, optarg);
            err = VSF_ERR_FAIL;
        } else {
            prio = hal_demo_num_to_prio(prio);
            if (prio == vsf_prio_inherit) {
                vsf_trace_error("%s are not supported: %s" VSF_TRACE_CFG_LINEEND, optarg);
                err = VSF_ERR_FAIL;
            } else {
                test->task_prio = prio;
            }
        }
        break;

    case 'i':
        if (0 == sscanf(optarg, "vsf_arch_prio_%u", &arch_prio)) {
            vsf_trace_error("unknown prio: %s" VSF_TRACE_CFG_LINEEND, optarg);
            err = VSF_ERR_FAIL;
        } else {
            arch_prio = hal_demo_num_to_arch_prio(arch_prio);
            if (arch_prio == vsf_arch_prio_invalid) {
                vsf_trace_error("%s are not supported" VSF_TRACE_CFG_LINEEND, optarg);
                err = VSF_ERR_FAIL;
            } else {
                test->isr_arch_prio = arch_prio;
            }
        }
        break;
    case 't':
        test->timeout_ms.test = strtoul(optarg, NULL, 0);
        break;
    case 'f':
        test->freq = strtoul(optarg, NULL, 0);
        if (test->freq <= 0) {
            vsf_trace_error("freq cannot be 0" VSF_TRACE_CFG_LINEEND);
            err = VSF_ERR_FAIL;
        }
        break;
    }

    return err;
}

#if APP_USE_LINUX_DEMO == ENABLED
int hal_main(hal_demo_t * demo, int argc, char *argv[])
{
#else
int hal_main(hal_demo_t * demo)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

    VSF_ASSERT(demo != NULL);
    const hal_demo_const_t *demo_const_ptr = demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);
    VSF_ASSERT(demo_const_ptr->const_size != 0);
    VSF_ASSERT(demo_const_ptr->name != 0);

    hal_test_t *test = vsf_heap_malloc(demo_const_ptr->const_size);
    if (test == NULL) {
        vsf_trace_error("%s alloc instance buffer failed" VSF_TRACE_CFG_LINEEND, demo_const_ptr->name);
        return -1;
    }
    memset(test, 0, demo_const_ptr->const_size);

    if (demo_const_ptr->const_ptr != NULL) {
        memcpy(test, demo_const_ptr->const_ptr, demo_const_ptr->const_size);
    }
    test->demo = demo;

#if APP_USE_LINUX_DEMO == ENABLED
    VSF_ASSERT(demo_const_ptr->parser_args != NULL);
    vsf_err_t err = demo_const_ptr->parser_args(test, argc, argv);
    if (err != VSF_ERR_NONE) {
        if (err == VSF_ERR_NOT_READY) {
            err = VSF_ERR_NONE;
        }
        __hal_demo_fini(test);

        return err;
    }
#endif

    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_eda_cfg_t cfg = {.fn.evthandler = %p/*evthandler*/, .priority = vsf_prio_%u};" VSF_TRACE_CFG_LINEEND
                        "vsf_teda_start(&test->teda, &cfg);" VSF_TRACE_CFG_LINEEND,
                        demo_const_ptr->evthandler, test->task_prio);
    }

    vsf_eda_cfg_t cfg = {
        .fn.evthandler = demo_const_ptr->evthandler,
        .priority = test->task_prio,
    };
    return vsf_teda_start(&test->teda, &cfg);
}

#endif
