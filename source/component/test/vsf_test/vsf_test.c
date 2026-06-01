/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "./vsf_test.h"
#include <string.h>

#if VSF_USE_TEST == ENABLED

#if VSF_TEST_CFG_USE_TRACE == ENABLED
#   if VSF_USE_TRACE != ENABLED
#       error "VSF_USE_TRACE must be ENABLED when VSF_TEST_CFG_USE_TRACE is ENABLED"
#   endif
#   include "service/trace/vsf_trace.h"
#endif

/*============================ MACROS ========================================*/

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

static vsf_test_t *__vsf_test_self;

extern vsf_err_t vsf_systimer_init(void);
extern void vsf_systick_enable(void);
extern vsf_systimer_tick_t vsf_systimer_get(void);
extern vsf_systimer_tick_t vsf_systimer_tick_to_us(vsf_systimer_tick_t tick);

void vsf_test_trace(uint8_t level, const char *format, ...)
{
    if (__vsf_test_self != NULL && level <= __vsf_test_self->shell.trace_level) {
        // Use uint32_t for formatting because newlib-nano vsnprintf does not
        // support %llu.  vsf_systimer_tick_t is uint64_t, but us fits in 32 bits
        // for test durations under ~4294 seconds (71 minutes).
        uint32_t us = (uint32_t)vsf_systimer_tick_to_us(vsf_systimer_get());
        vsf_trace(level, "[vsf-test] [%lu.%06lu s] ",
                  (unsigned long)(us / 1000000),
                  (unsigned long)(us % 1000000));
        va_list ap;
        va_start(ap, format);
        vsf_trace_arg(level, format, ap);
        va_end(ap);
    }
}

#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
static void __emit_case_start(const char *suite_name, uint8_t case_idx, bool needs_ready)
{
    VSF_TEST_TRACE_INFO("%s:case:%u\r\n", suite_name, (unsigned)case_idx);
    if (needs_ready) {
        VSF_TEST_TRACE_INFO("%s:case:%u:ready\r\n", suite_name, (unsigned)case_idx);
    }
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);
}

static void __emit_case_done(const char *suite_name, uint8_t case_idx)
{
    VSF_TEST_TRACE_INFO("%s:case:%u:done\r\n", suite_name, (unsigned)case_idx);
}

static void __emit_suite_end(const char *suite_name)
{
    VSF_TEST_TRACE_INFO("%s:end\r\n", suite_name);
}
#endif

/*============================ LOCAL FUNCTIONS ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_run(vsf_test_t *test)
{
    __vsf_test_self = test;

    __vsf_test_self->current_case  = NULL;
    __vsf_test_self->current_suite = NULL;
    __vsf_test_self->shell.trace_level = VSF_TRACE_DEBUG;

    VSF_TEST_TRACE_INFO("Initialized (%u suites)\r\n", __vsf_test_self->suite_count);

    for (uint8_t i = 0; i < __vsf_test_self->wdt.count; i++) {
        if (__vsf_test_self->wdt.entries[i].init != NULL) {
            uint32_t timeout_ms = __vsf_test_self->wdt.entries[i].timeout_ms;
            if (timeout_ms == 0) {
                timeout_ms = (i == 0) ? VSF_TEST_CFG_INTERNAL_TIMEOUT_MS
                                      : VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS;
            }
            VSF_TEST_TRACE_DEBUG("WDT[%u]: %u ms\r\n",
                                   (unsigned)i, (unsigned)timeout_ms);
            __vsf_test_self->wdt.entries[i].init(&__vsf_test_self->wdt.entries[i], timeout_ms);
        }
    }

    vsf_test_shell_init(&__vsf_test_self->shell,
                        __vsf_test_self->suites, __vsf_test_self->suite_count,
                        __vsf_test_self->instances, __vsf_test_self->instance_count);
    vsf_test_shell_run(&__vsf_test_self->shell);
}

void vsf_test_assert(vsf_test_result_t result,
                        const char *file_name, uint32_t line,
                        const char *function_name, const char *condition)
{
    __vsf_test_self->result              = result;
    __vsf_test_self->error.function_name = function_name;
    __vsf_test_self->error.file_name     = file_name;
    __vsf_test_self->error.condition     = condition;
    __vsf_test_self->error.line          = line;

    VSF_TEST_TRACE_ERROR("Assertion failed: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED
    vsf_trace_dump_stack();
#endif

    longjmp(*__vsf_test_self->jmp_buf, 1);
}

static const char *__vsf_test_get_name(void)
{
    if (__vsf_test_self->current_suite != NULL && __vsf_test_self->current_suite->name != NULL) {
        return __vsf_test_self->current_suite->name;
    }
    return "unknown";
}

void vsf_test_reboot(vsf_test_result_t result,
                     const char *file_name, uint32_t line,
                     const char *function_name, const char *condition)
{
    __vsf_test_self->result              = result;
    __vsf_test_self->error.function_name = function_name;
    __vsf_test_self->error.file_name     = file_name;
    __vsf_test_self->error.condition     = condition;
    __vsf_test_self->error.line          = line;

    VSF_TEST_TRACE_ERROR("Reboot due to error: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED
    vsf_trace_dump_stack();
#endif

    for (uint8_t i = 0; i < __vsf_test_self->reboot.count; i++) {
        if (__vsf_test_self->reboot.entries[i] != NULL) {
            VSF_TEST_TRACE_INFO("Calling reboot[%u]\r\n", (unsigned)i);
            __vsf_test_self->reboot.entries[i]();
        }
    }
    while (1);
}

VSF_CAL_WEAK(vsf_test_hw_config)
void vsf_test_hw_config(vsf_peripheral_type_t peripheral_type, const vsf_test_inst_t *inst, bool init)
{
    (void)peripheral_type;
    (void)inst;
    (void)init;
#if VSF_TEST_CFG_HW_CONFIG_LOG_ONLY == ENABLED
    //!< Board intentionally configures all pins elsewhere (e.g. boot-time
    //!< pinmux); just record that the default hook ran.
    VSF_TEST_TRACE_DEBUG("hw_config: default (no board override) type=%d init=%d\r\n",
                         (int)peripheral_type, (int)init);
#else
    //!< No board override of vsf_test_hw_config: per-test pin muxing never
    //!< happens, which silently breaks peripherals that need it (e.g. HW I2C
    //!< pins stuck at reset FUNCSEL). Fail loud. Set
    //!< VSF_TEST_CFG_HW_CONFIG_LOG_ONLY=ENABLED if this is intentional.
    VSF_ASSERT(0);
#endif
}

VSF_CAL_WEAK(vsf_test_busy_wait_ms)
void vsf_test_busy_wait_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS; i++);
}

VSF_CAL_WEAK(vsf_test_busy_wait_us)
void vsf_test_busy_wait_us(uint32_t us)
{
    for (volatile uint32_t i = 0; i < us * (VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS / 1000); i++);
}

vsf_test_result_t vsf_test_run_suite_case(const vsf_test_suite_t *suite, uint16_t local_idx, const vsf_test_inst_t *inst)
{
    if (suite == NULL || local_idx >= suite->case_count || suite->jmp_fn == NULL) {
        return VSF_TEST_RESULT_SKIP;
    }
    vsf_test_case_t test_case_buf = {.case_idx=(uint8_t)local_idx,.params=(void*)((const uint8_t*)(suite+1)+local_idx*suite->params_stride)};
    const vsf_test_case_t *test_case = &test_case_buf;

    for (uint8_t i = 0; i < __vsf_test_self->wdt.count; i++) {
        if (__vsf_test_self->wdt.entries[i].feed != NULL) {
            __vsf_test_self->wdt.entries[i].feed(&__vsf_test_self->wdt.entries[i]);
        }
    }

    __vsf_test_self->current_case   = test_case;
    __vsf_test_self->current_suite  = suite;
    __vsf_test_self->result         = VSF_TEST_RESULT_PASS;
    __vsf_test_self->error.function_name = NULL;
    __vsf_test_self->error.file_name     = NULL;
    __vsf_test_self->error.condition     = NULL;
    __vsf_test_self->error.line          = 0;

    const char *test_name = __vsf_test_get_name();
    VSF_TEST_TRACE_INFO("Running '%s'\r\n", test_name);
#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
    __emit_case_start(suite->name, test_case->case_idx, suite->needs_ready);
#endif

    jmp_buf buf;
    __vsf_test_self->jmp_buf = &buf;
    if (0 == setjmp(buf)) {
        suite->jmp_fn(suite, test_case, inst);
    }

#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
    __emit_case_done(suite->name, test_case->case_idx);
#endif

    vsf_test_result_t result = (vsf_test_result_t)__vsf_test_self->result;
    __vsf_test_self->current_case  = NULL;
    __vsf_test_self->current_suite = NULL;
    return result;
}

static void __vsf_test_run_suite_all_cases(const vsf_test_suite_t *suite)
{
    for (uint16_t i = 0; i < suite->case_count; i++) {
        vsf_test_run_suite_case(suite, i, NULL);
    }
}

void vsf_test_run_suite(const vsf_test_suite_t *suite)
{
    if (suite == NULL || suite->jmp_fn == NULL) {
        return;
    }
    __vsf_test_run_suite_all_cases(suite);
#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
    __emit_suite_end(suite->name);
#endif
}

#endif
/* EOF */
