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

#ifndef __HAL_DEMO_H__
#define __HAL_DEMO_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED

/*============================ MACROS ========================================*/

#ifndef HAL_DEMO_DEVICE_NAME_MAX_LENGTH
#   define HAL_DEMO_DEVICE_NAME_MAX_LENGTH              16
#endif

#ifndef HAL_DEMO_MAX_EVTS
#   define HAL_DEMO_MAX_EVTS                            4
#endif

#ifndef APP_HAL_DEMO_CFG_PRIO
#   define APP_HAL_DEMO_CFG_PRIO                        vsf_prio_0
#endif

#ifndef APP_HAL_DEMO_CFG_IRQ_PRIO
#   define APP_HAL_DEMO_CFG_IRQ_PRIO                    vsf_arch_prio_2
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/


#define __HAL_DEMO_NAME2STR(__NAME)                  # __NAME
#define HAL_DEMO_NAME2STR(__NAME)                    __HAL_DEMO_NAME2STR(__NAME)

#define HAL_DEMO_OPTION_EX(__MASK, __VALUE, __NAME)                 \
    { .name = __NAME, .mask = __MASK, .value = __VALUE }
#define HAL_DEMO_OPTION(__MASK, __VALUE)                                   \
    HAL_DEMO_OPTION_EX(__MASK, __VALUE, #__VALUE)

#if APP_USE_LINUX_DEMO == ENABLED
#   define __HAL_DEMO_CONST_INIT_LINUX(__MODULE, __HELP)                        \
        .name        = # __MODULE,                                       \
        .help       = __HELP,                                           \
        .parser_args        = &VSF_MCONNECT(__, __MODULE, _parser_args),
#else
#   define __HAL_DEMO_CONST_INIT_LINUX(__MODULE, __HELP)
#endif

#define __HAL_DEMO_INIT(__MODULE, __MODULE_UPCASE, __HELP, ...)                                     \
    static bool VSF_MCONNECT(__, __MODULE, _demo_check)(hal_test_t *tet_ptr);       \
    static vsf_err_t VSF_MCONNECT(__, __MODULE, _parser_args)                                       \
        (hal_test_t *test, int argc, char *argv[]);                                        \
    static void VSF_MCONNECT(__, __MODULE, _demo_evthandler)(vsf_eda_t *eda, vsf_evt_t evt);        \
                                                                                                    \
    static const VSF_MCONNECT(__MODULE, _demo_const_t) VSF_MCONNECT(__, __MODULE, _demo_const) = {  \
        __HAL_DEMO_CONST_INIT_LINUX(__MODULE, __HELP)                                               \
        .const_size      = sizeof(VSF_MCONNECT(__MODULE, _test_t)),                         \
        .evthandler         = VSF_MCONNECT(__, __MODULE, _demo_evthandler),                         \
        .check_capability   = &VSF_MCONNECT(__, __MODULE, _demo_check),                  \
        .const_ptr     = &VSF_MCONNECT(__, __MODULE, _demo_const).test.use_as__hal_test_t,\
        .test = {                                                                                   \
            .verbose = 0,                                                                           \
            .task_prio = APP_HAL_DEMO_CFG_PRIO,                                                     \
            .isr_arch_prio = APP_HAL_DEMO_CFG_IRQ_PRIO,                                             \
            .mode_value = 0,                                                                        \
            .mode_mask = 0,                                                                         \
            .device = (VSF_MCONNECT(vsf_, __MODULE, _t) *)                                          \
                                & VSF_MCONNECT(__MODULE_UPCASE, _DEMO_CFG_DEFAULT_INSTANCE),        \
            .device_name =  HAL_DEMO_NAME2STR(                                                      \
                                VSF_MCONNECT(__MODULE_UPCASE, _DEMO_CFG_DEFAULT_INSTANCE)),         \
            .start_tick = 0,                                                                        \
            .timeout_ms = {                                                                         \
                .enable   =  200, /*  200 ms */                                                     \
                .disable  = 1000, /* 1000 ms */                                                     \
                .test     = 5000, /* 5000 ms */                                                     \
            },                                                                                      \
            .repeat_cnt = 1,                                                                        \
        },                                                                                          \
        __VA_ARGS__                                                                                 \
    };                                                                                              \
                                                                                                    \
    typedef struct VSF_MCONNECT(__MODULE, _demo_t) {                                                \
        implement(hal_demo_t)                                                                       \
        struct {                                                                                    \
            uint8_t cnt;                                                                            \
            const vsf_hal_device_t *devices;                                                        \
        } VSF_MCONNECT(__MODULE, _devices_array)[                                                   \
            VSF_MCONNECT(__MODULE_UPCASE, _DEMO_CFG_DEVICES_COUNT)                                  \
        ];                                                                                          \
    } VSF_MCONNECT(__MODULE, _demo_t);                                                              \
                                                                                                    \
    static VSF_MCONNECT(__MODULE, _demo_t) VSF_MCONNECT(__, __MODULE, _demo) = {                    \
        .demo_const_ptr = &VSF_MCONNECT(__, __MODULE, _demo_const).use_as__hal_demo_const_t,        \
        .array_cnt    = VSF_MCONNECT(__MODULE_UPCASE, _DEMO_CFG_DEVICES_COUNT),                     \
        .VSF_MCONNECT(__MODULE, _devices_array) = {                                                 \
            VSF_MCONNECT(__MODULE_UPCASE, _DEMO_CFG_DEVICES_ARRAY_INIT)                             \
        },                                                                                          \
    };

#define HAL_DEMO_INIT(__MODULE, __MODULE_UPCASE, __HELP, ...)                                       \
            __HAL_DEMO_INIT(__MODULE, __MODULE_UPCASE, __HELP, __VA_ARGS__)

/*============================ TYPES =========================================*/

typedef enum hal_test_evt_t {
    VSF_EVT_HAL_CHECK = VSF_EVT_USER,
    VSF_EVT_HAL_ALLOC,
    VSF_EVT_HAL_CALL_INIT,
    VSF_EVT_HAL_CALL_ENABLE,
    VSF_EVT_HAL_CALL_IRQ_ENABLE,

    VSF_EVT_HAL_TEST_START,
    VSF_EVT_HAL_TEST_RUN,
    VSF_EVT_HAL_TEST_NEXT,
    VSF_EVT_HAL_TEST_FAILED,
    VSF_EVT_HAL_TEST_END,

    VSF_EVT_HAL_CALL_IRQ_DISABLE,
    VSF_EVT_HAL_CALL_DISABLE,
    VSF_EVT_HAL_CALL_FINI,

    VSF_EVT_HAL_FREE_AND_REPORT,

    __VSF_EVT_HAL_LAST,
} hal_test_evt_t;

typedef struct hal_option_t {
    const char *name;
    uint32_t mask;
    uint32_t value;
} hal_option_t;

typedef struct hal_test_t hal_test_t;
typedef struct hal_demo_t hal_demo_t;

typedef void (*hal_scan_fn_t)(hal_demo_t *demo);
typedef vsf_err_t (*hal_parser_args_t)(hal_test_t * test, int argc, char *argv[]);
typedef bool (*hal_check_capability_fn_t)(hal_test_t *tet_ptr);
typedef vsf_err_t (*hal_init_fn_t)(void *device_ptr, void *cfg_ptr);
typedef vsf_err_t (*hal_init_no_cfg_fn_t)(void *device_ptr);
typedef void (*hal_fini_fn_t)(void *device_ptr);
typedef fsm_rt_t (*hal_enable_fn_t)(void *device_ptr);
typedef fsm_rt_t (*hal_disable_fn_t)(void *device_ptr);
typedef void(*hal_irq_enable_fn_t)(void *device_ptr, uint32_t irq_mask);
typedef void(*hal_irq_disable_fn_t)(void *device_ptr, uint32_t irq_mask);

struct hal_test_t {
    vsf_teda_t teda;
    hal_demo_t * demo;

    void *device;
    char device_name[HAL_DEMO_DEVICE_NAME_MAX_LENGTH];

    vsf_prio_t task_prio;
    uint32_t method;

    // vsf_xxx_cfg_t
    void *hal_cfg;
    uint32_t mode_value;
    uint32_t mode_mask;
    uint32_t freq;
    vsf_arch_prio_t isr_arch_prio;

    // vsf_xxx_irq_enable() / vsf_xxx_irq_disable()
    uint32_t irq_mask;
    uint32_t expected_irq_mask;

    struct {
        uint8_t *buffer;
        size_t size;
    } send, recv;

    vsf_systimer_tick_t start_tick;
    struct {
        vsf_systimer_tick_t enable;
        vsf_systimer_tick_t disable;
        vsf_systimer_tick_t test;
    } timeout_ms;

    uint16_t repeat_cnt;
    uint8_t verbose;
    bool test_pass;
};

typedef struct hal_demo_const_t {
#if APP_USE_LINUX_DEMO == ENABLED
    char *help;
    hal_parser_args_t parser_args;
#endif
    vsf_eda_evthandler_t evthandler;
    char *name;

    // default value
    hal_test_t const *const const_ptr;
    size_t const_size;

    struct {
        const hal_option_t *options;
        const uint8_t cnt;
    } mode, irq, method;

    hal_scan_fn_t scan;

    bool init_has_cfg;
    union {
        hal_init_fn_t device_init;
        hal_init_no_cfg_fn_t device_no_cfg_init;
    };
    hal_fini_fn_t device_fini;
    hal_enable_fn_t device_enable;
    hal_disable_fn_t device_disable;
    hal_irq_enable_fn_t device_irq_enable;
    hal_irq_disable_fn_t device_irq_disable;
    hal_check_capability_fn_t check_capability;
} hal_demo_const_t;

typedef struct hal_demo_t {
    const hal_demo_const_t *demo_const_ptr;

    uint8_t array_cnt;
    struct {
        uint8_t cnt;
        vsf_hal_device_t *devices;
    } devices_array [0];
} hal_demo_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_arch_prio_t hal_demo_num_to_arch_prio(uint32_t i);
extern int hal_demo_arch_prio_to_num(vsf_arch_prio_t prio);
extern vsf_prio_t hal_demo_num_to_prio(uint32_t i);
extern int hal_demo_prio_to_num(vsf_prio_t prio);

extern const vsf_hal_device_t *hal_demo_find_device(hal_demo_t *demo, char *name);

extern const char *hal_option_to_str(const hal_option_t *options, uint8_t cnt, uint32_t value);
extern bool hal_options_get_value(int argc, char *argv[], const hal_option_t *options, uint8_t cnt, uint32_t *value_ptr, uint32_t *mask_ptr);
extern void hal_options_trace(vsf_trace_level_t level, const char *prefix_str, const hal_option_t *options, uint8_t cnt, uint32_t mode);

extern bool hal_test_irq_check(hal_test_t *test, uint32_t irq_mask);
extern void hal_test_reset_timeout(hal_test_t *test);
extern bool hal_test_is_timeout(hal_test_t *test, vsf_systimer_tick_t timeout_ms);
extern void hal_device_irq_enable(hal_test_t *test);

extern void hal_evthandler(vsf_eda_t *eda, vsf_evt_t evt);
extern vsf_err_t hal_test_evthandler(hal_test_t *test, int argc, char *argv[], char c);

extern int hal_main(void);

#endif      // APP_USE_HAL_DEMO == ENABLED

#endif      // __HAL_DEMO_H__

/* EOF */
