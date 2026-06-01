/*****************************************************************************
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
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#ifndef __VSF_TEST_H__
#   define __VSF_TEST_H__

//! \note do not move this pre-processor statement to other places
#   include "component/vsf_component_cfg.h"
#   include "utilities/vsf_utilities.h"

/* example:

    // 0. Include vsf header file
    #include "vsf.h"

    // 1. Defining vsf_test variable
    static vsf_test_wdt_t __wdt_entries[] = {
        // 1.1 Internal WDT (chip's own watchdog)
        {
            .init = vsf_test_hal_wdt_init,
            .feed = vsf_test_hal_wdt_feed,
        },
        // 1.2 External WDT (assist device controlling power/reset pin)
        {
            .init = vsf_test_stdio_wdt_init,
            .feed = vsf_test_stdio_wdt_feed,
        },
    };
    static vsf_test_reboot_t *__reboot_entries[] = {
        // 1.3 External reboot first (assist device)
        vsf_test_stdio_reboot,
        // 1.4 Internal reboot fallback (chip reset)
        vsf_arch_reset,
    };
    static vsf_test_t __test = {
        .wdt = {
            .entries = __wdt_entries,
            .count   = dimof(__wdt_entries),
        },
        .reboot = {
            .entries = __reboot_entries,
            .count   = dimof(__reboot_entries),
        },
    };
    static vsf_test_t *test = &__test;

    // 3. Optionally, call vsf_test_reboot in the callback function for all
    // exceptions to abort the test and provide additional error messages if
    // possible information.
    // Here is an example of HardFault_Handler in Cortex-M:
    void HardFault_Handler(void)
    {
        vsf_test_reboot(VSF_TEST_RESULT_FAULT_HANDLER_FAIL, __FILE__,
                        __LINE__, __FUNCTION__, "More Info");
    }

    // 4. Here are some test examples. including:
    // - test succeeded
    // - test failed
    // - test with watchdog reset
    // - test with an exception

    // test succeeded
    static void __test_pass(void)
    {
        int a = 100;
        int b = 50 + 50;

        VSF_TEST_ASSERT(a == b);
    }

    // test failed
    static void __test_fail(void)
    {
        int a = 100;
        int b = 50 + 50;

        VSF_TEST_ASSERT(a != b);
    }

    // test with watchdog reset
    static void __test_wdt(void)
    {
        while (1);
    }

    // test with an exception, current only support cortex-m
    void __test_unalign(void)
    {
    #ifdef __CORTEX_M
        SCB->CCR |= (1 << 3);
        volatile uint32_t *p     = (volatile uint32_t *)0x03;
        uint32_t           value = *p;
    #else
    #    error "TODO"
    #endif
    }

    int main(void)
    {
        // 5. Configure and initialize the test framework
        static vsf_test_wdt_t __test_wdt_entries[] = {
            {
                .init = vsf_test_hal_wdt_init,
                .feed = vsf_test_hal_wdt_feed,
            },
        };
        static vsf_test_reboot_t *__test_reboot_entries[] = {
            vsf_arch_reset,
        };
        __vsf_test.wdt.entries = __test_wdt_entries;
        __vsf_test.wdt.count   = dimof(__test_wdt_entries);
        __vsf_test.reboot.entries = __test_reboot_entries;
        __vsf_test.reboot.count   = dimof(__test_reboot_entries);
        __vsf_test.restart_on_done = false;  // Set to true to restart when test completes or errors occur
        vsf_test_run(NULL);

        // 6. We support two styles of adding test cases.
        // - The first is the static way. We can use macros to initialize test
        //   cases. It is more RAM efficient.
        // - The second is the dynamic way of adding, which is closer to the
        //   traditional testing framework.
    #if 1
        vsf_test_add(__test_pass, "test_pass hw_req=none");
        vsf_test_add(__test_fail, "test_fail hw_req=none");
        vsf_test_add(__test_wdt, "test_wdt hw_req=none");
        vsf_test_add(__test_unalign, "test_invalid_address hw_req=none");
    #endif

        // 7. vsf_test_run runs all tests and optionally starts the shell REPL.
        // No explicit vsf_test_run_tests call is needed.

        return 0;
    }
*/

/*
 * Protocol A — Shell REPL (host → device, real-time over UART):
 *   vsf-test suite --list          → enumerate registered suites
 *   vsf-test run <name>            → trigger execution
 *   vsf-test config shuffle <N>    → set random seed
 *   Suite ack: <name>              ← confirmation
 *   Pass: N, Fail: N, Skip: N      ← per-suite summary
 *
 * Protocol B — Capture Markers (device → host, offline via LA decode):
 *   <suite>:CASE:<N>               ← case start (framework)
 *   <suite>:CASE:<N>:READY         ← DUT ready for host input (framework, optional)
 *   <suite>:CASE:<N>:DONE          ← case end (framework)
 *   <suite>:END                    ← suite boundary (framework)
 *
 * Host-less standalone mode (data sync via assist device) is not supported
 * in the current configuration. Legacy port files are preserved under
 * component/test/vsf_test/port/legacy/ for reference.
 */

#    if VSF_USE_TEST == ENABLED

/*============================ MACROS ========================================*/

//!< Using longjmp/setjmp for assertions
#        ifndef VSF_TEST_CFG_LONGJMP
#            define VSF_TEST_CFG_LONGJMP ENABLED
#        endif

//!< Internal watchdog default timeout
#        ifndef VSF_TEST_CFG_INTERNAL_TIMEOUT_MS
#            define VSF_TEST_CFG_INTERNAL_TIMEOUT_MS 1000
#        endif

//!< External Watchdog Default Timeout
#        ifndef VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS
#            define VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS 1500
#        endif

//!< Using the hal wdt device
#        ifndef VSF_TEST_CFG_USE_HAL_WDT
#            define VSF_TEST_CFG_USE_HAL_WDT DISABLED
#        endif

//!< Enable trace output for test framework
#        ifndef VSF_TEST_CFG_USE_TRACE
#            define VSF_TEST_CFG_USE_TRACE ENABLED
#        endif

//!< Emit Capture Markers (CASE:N / READY / DONE / :END) for host-side LA decode.
//!< Disabled by default — only needed when vsf-bench decodes LA captures.
#        ifndef VSF_TEST_CFG_EMIT_MARKERS
#            define VSF_TEST_CFG_EMIT_MARKERS DISABLED
#        endif

//!< Behaviour of the weak (un-overridden) vsf_test_hw_config().
//!< DISABLED (default): assert — a board that needs per-test pin muxing but
//!< forgot to provide vsf_test_hw_config() fails loud instead of silently
//!< leaving pins unconfigured. ENABLED: just log, for boards that
//!< intentionally configure all pins elsewhere (e.g. boot-time pinmux).
#        ifndef VSF_TEST_CFG_HW_CONFIG_LOG_ONLY
#            define VSF_TEST_CFG_HW_CONFIG_LOG_ONLY DISABLED
#        endif

//!< Marker settle delay in milliseconds. Framework emits CASE/READY then
//!< waits this long before invoking the test function, ensuring marker bytes
//!< are fully on the wire before any test-driven UART activity begins.
//!< Overridden by test_params_generated.h when the test-params generator runs.
#        ifndef VSF_TEST_MARKER_DELAY_MS
#            define VSF_TEST_MARKER_DELAY_MS 2
#        endif

//!< Loop iterations per millisecond for vsf_test_busy_wait_ms. CPU-frequency
//!< dependent; default tuned for RP2040 @ 133MHz. Override per-board if needed.
#        ifndef VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS
#            define VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS 22000
#        endif

//!< Default poll tick (ms) used by vsf_test_busy_wait_ms() in polling loops.
//!< All test suites share this granularity; change globally in one place.
#        ifndef VSF_TEST_POLL_TICK_MS
#            define VSF_TEST_POLL_TICK_MS 1
#        endif

/*!
    \def VSF_TEST_ASSERT(__v)
    \brief Add an assertion to the test case
    \param __v expressions for test conditions
*/
#        define VSF_TEST_ASSERT(__v)                                           \
            do {                                                               \
                if (!(__v)) {                                                  \
                    vsf_test_assert(VSF_TEST_RESULT_FAIL, __FILE__,         \
                                       __LINE__, __FUNCTION__, #__v);          \
                }                                                              \
            } while (0)

/*!
    \def VSF_TEST_ASSERT_INST(_inst, _pt, _field)
    \brief Validate a test instance matches expected type and has non-null fixture
    \param _inst  const vsf_test_inst_t pointer
    \param _pt    expected vsf_peripheral_type_t value
    \param _field fixture union field name (e.g. gpio, usart, i2c)
*/
#        define VSF_TEST_ASSERT_INST(_inst, _pt, _field)                          \
            do {                                                                   \
                VSF_TEST_ASSERT((_inst) != NULL);                                  \
                VSF_TEST_ASSERT((_inst)->peripheral_type == (_pt));                \
                VSF_TEST_ASSERT((_inst)->fixture._field != NULL);                  \
                VSF_TEST_ASSERT((_inst)->name != NULL);                            \
            } while (0)

/*!
    \def VSF_TEST_ASSERT_ERR(__err, __expected, ...)
    \brief Assert that an error code equals the expected value, with a
           pre-assertion TRACE_ERROR if it does not.
    \param __err      vsf_err_t expression (evaluated once)
    \param __expected expected vsf_err_t value
    \param __VA_ARGS__ format string and args for VSF_TEST_TRACE_ERROR
*/
#        define VSF_TEST_ASSERT_ERR(__err, __expected, ...)                      \
            do {                                                                   \
                vsf_err_t _vsf_err = (vsf_err_t)(__err);                           \
                if (_vsf_err != (vsf_err_t)(__expected)) {                         \
                    VSF_TEST_TRACE_ERROR(__VA_ARGS__);                             \
                }                                                                  \
                VSF_TEST_ASSERT(_vsf_err == (vsf_err_t)(__expected));              \
            } while (0)

/*!
    \def VSF_TEST_ASSERT_ERR_NONE(__err, ...)
    \brief Convenience wrapper: assert err == VSF_ERR_NONE with TRACE_ERROR.
*/
#        define VSF_TEST_ASSERT_ERR_NONE(__err, ...)                             \
            VSF_TEST_ASSERT_ERR(__err, VSF_ERR_NONE, __VA_ARGS__)

/*!
    \def VSF_TEST_WAIT_FOR(__cond, __timeout_ms)
    \brief Busy-wait for a condition with a 1ms polling tick and timeout
    \param __cond       expression that becomes true when ready
    \param __timeout_ms timeout in milliseconds
*/
#        define VSF_TEST_WAIT_FOR(__cond, __timeout_ms)                        \
            do {                                                               \
                uint32_t _vsf_t = (uint32_t)(__timeout_ms);                    \
                while (!(__cond) && _vsf_t-- > 0) {                            \
                    vsf_test_busy_wait_ms(1);                                  \
                }                                                              \
            } while (0)

/*!
    \def VSF_TEST_SPIN_FOR(__cond, __iterations)
    \brief Tight spin-wait for a condition with an iteration limit (no delay)
    \param __cond       expression that becomes true when ready
    \param __iterations max loop iterations
*/
#        define VSF_TEST_SPIN_FOR(__cond, __iterations)                        \
            do {                                                               \
                uint32_t _vsf_i = (uint32_t)(__iterations);                    \
                while (!(__cond) && _vsf_i-- > 0) {}                           \
            } while (0)

/*!
    \def VSF_TEST_TRACE_INFO(...)
    \brief Level-filtered trace output via the test shell's trace_level
*/
extern void vsf_test_trace(uint8_t level, const char *format, ...);

#        define VSF_TEST_TRACE_ERROR(...)                                      \
            vsf_test_trace(VSF_TRACE_ERROR, __VA_ARGS__)
#        define VSF_TEST_TRACE_WARNING(...)                                    \
            vsf_test_trace(VSF_TRACE_WARNING, __VA_ARGS__)
#        define VSF_TEST_TRACE_INFO(...)                                       \
            vsf_test_trace(VSF_TRACE_INFO, __VA_ARGS__)
#        define VSF_TEST_TRACE_DEBUG(...)                                      \
            vsf_test_trace(VSF_TRACE_DEBUG, __VA_ARGS__)

/*============================ INCLUDES ======================================*/

#        include "./vsf_test_shell.h"

/*============================ TYPES =========================================*/

typedef enum vsf_test_result_t {
    VSF_TEST_RESULT_PASS = 0,
    VSF_TEST_RESULT_SKIP = 1,
    VSF_TEST_RESULT_FAIL = 2,
} vsf_test_result_t;

typedef enum vsf_peripheral_type_t {
    VSF_PERIPHERAL_TYPE_NONE   = 0,
    VSF_PERIPHERAL_TYPE_GPIO,
    VSF_PERIPHERAL_TYPE_USART,
    VSF_PERIPHERAL_TYPE_SPI,
    VSF_PERIPHERAL_TYPE_I2C,
    VSF_PERIPHERAL_TYPE_ADC,
    VSF_PERIPHERAL_TYPE_PWM,
    VSF_PERIPHERAL_TYPE_TIMER,
    VSF_PERIPHERAL_TYPE_RTC,
    VSF_PERIPHERAL_TYPE_WDT,
    VSF_PERIPHERAL_TYPE_RNG,
    VSF_PERIPHERAL_TYPE_DMA,
    VSF_PERIPHERAL_TYPE_FLASH,
    VSF_PERIPHERAL_TYPE_I2C_SLAVE,
    VSF_PERIPHERAL_TYPE_GPIO_PINMUX,
    VSF_PERIPHERAL_TYPE_ARCH,
} vsf_peripheral_type_t;

typedef void vsf_test_reboot_t(void);

vsf_class(vsf_test_wdt_t) {
    public_member(
        //! Watchdog driver. In hardware, the watchdog usually cannot be
        //! reconfigured after initialization, so here the initialization function
        //! is called just once after power-up
        void (*init)(vsf_test_wdt_t *wdt, uint32_t timeout_ms);
        //! The feed function will be called once after each test is completed.
        void (*feed)(vsf_test_wdt_t *wdt);
        //! Watchdog timeout time (in milliseconds), if not set, the default time
        //! (VSF_TEST_CFG_INTERNAL_TIMEOUT_MS or VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS)
        //! will be used
        uint32_t timeout_ms;
    )
};

typedef struct vsf_test_case_t vsf_test_case_t;
typedef void vsf_test_jmp_fn_t(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

dcl_simple_class(vsf_test_suite_t)

//! \brief Test Suite — pure logic, no HAL binding.
//!
//! Each scenario extends vsf_test_suite_t via PLOOC and adds its
//! scenario-specific fields (typically a HAL handle slot that the framework
//! fills at runtime from the matching instance).

vsf_class(vsf_test_case_t) {
    public_member(
        uint8_t            case_idx;
        void              *params;
    )
};

vsf_class(vsf_test_suite_t) {
    public_member(
        const char                     *name;          //!< also used as Capture Marker tag
        vsf_test_jmp_fn_t             *jmp_fn;         //!< run function for all cases in this suite
        uint16_t                       case_count;     //!< number of cases
        uint16_t                       params_stride;  //!< sizeof(scenario-specific params struct)
        vsf_peripheral_type_t          peripheral_type;//!< which HAL type this suite needs (0 = none)
        uint8_t                        needs_ready;    //!< emit :READY marker before each case
    )
};

/* Forward declarations of HAL types — only pointers are needed in the union. */
typedef struct vsf_i2c_t          vsf_i2c_t;
typedef struct vsf_usart_t        vsf_usart_t;
typedef struct vsf_spi_t          vsf_spi_t;
typedef struct vsf_adc_t          vsf_adc_t;
typedef struct vsf_pwm_t          vsf_pwm_t;
typedef struct vsf_dma_t          vsf_dma_t;
typedef struct vsf_timer_t        vsf_timer_t;
typedef struct vsf_rtc_t          vsf_rtc_t;
typedef struct vsf_flash_t        vsf_flash_t;
typedef struct vsf_wdt_t          vsf_wdt_t;
typedef struct vsf_rng_t          vsf_rng_t;
typedef struct vsf_gpio_t         vsf_gpio_t;

/*! Multi-device context for I2C slave test scenarios (master + slave pair). */
typedef struct vsf_test_i2c_slave_ctx_t {
    vsf_i2c_t *master_i2c;
    vsf_i2c_t *slave_i2c;
} vsf_test_i2c_slave_ctx_t;

/*! Multi-device context for GPIO pinmux test scenarios (GPIO + alternate-function peripheral). */
typedef struct vsf_test_gpio_pinmux_ctx_t {
    vsf_gpio_t  *gpio;
    vsf_usart_t *usart;
} vsf_test_gpio_pinmux_ctx_t;

typedef struct vsf_test_inst_t {
    vsf_peripheral_type_t        peripheral_type; //!< which HAL interface this instance satisfies
    const char                  *name;            //!< human-readable instance description for logs
    union {
        void                         *raw;           //!< generic fallback / pass-through
        vsf_i2c_t                    *i2c;
        vsf_usart_t                  *usart;
        vsf_spi_t                    *spi;
        vsf_adc_t                    *adc;
        vsf_pwm_t                    *pwm;
        vsf_dma_t                    *dma;
        vsf_timer_t                  *timer;
        vsf_rtc_t                    *rtc;
        vsf_flash_t                  *flash;
        vsf_wdt_t                    *wdt;
        vsf_rng_t                    *rng;
        vsf_gpio_t                   *gpio;
        vsf_test_i2c_slave_ctx_t     *i2c_slave;
        vsf_test_gpio_pinmux_ctx_t   *gpio_pinmux;
    } fixture;                                      //!< typed hardware fixture passed to test suites
} vsf_test_inst_t;

typedef struct vsf_test_t {
    //! Without a watchdog, we can still can test.
    //! But the watchdog provides stronger guarantees for tests:
    //! if a test is abnormal, the next test continues to run after the watchdog
    //! times out.
    struct {
        vsf_test_wdt_t *entries;
        uint8_t         count;
    } wdt;

    //! We perform a reset when the test program goes into exception.
    //! Reboot functions are called in array order; typically external first,
    //! then internal fallback. If none succeed, the framework enters a dead
    //! loop and waits for the watchdog to reset.
    struct {
        vsf_test_reboot_t **entries;
        uint8_t             count;
    } reboot;

    //! Current test case pointer — set by vsf_test_run_case before invoking
    //! the test function, used by vsf_test_assert and vsf_test_reboot.
    const vsf_test_case_t *current_case;

    //! Current suite pointer — set before running cases in a suite.
    const vsf_test_suite_t *current_suite;

    //! Result and error info for the currently running case — written by
    //! vsf_test_assert / vsf_test_reboot, read by the runner after the case
    //! function returns. Only one copy exists; reused across all cases.
    uint8_t result;
    struct {
        const char *function_name;
        const char *file_name;
        const char *condition;
        uint32_t    line;
    } error;

#        if VSF_TEST_CFG_LONGJMP == ENABLED
    jmp_buf *jmp_buf;
#        endif

    //! Registered suites array and count — populated at compile time.
    const vsf_test_suite_t **suites;
    uint8_t                  suite_count;

    //! Peripheral instances array and count — populated at compile time.
    const vsf_test_inst_t  *instances;
    uint8_t            instance_count;

    //! Embedded shell REPL — started by vsf_test_run() after init.
    vsf_test_shell_t shell;
} vsf_test_t;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/**
 @brief initialize vsf test
 @param[in] test: test instance pointer (must not be NULL)
 */
extern void vsf_test_run(vsf_test_t *test);

/**
 @brief rong jump. the user does not need to directly call this API
 @param[in] result: test result,  @ref vsf_test_result_t
 @param[in] file_name: then name of the file where the assertion occurred
 @param[in] line: the line number of the code where the assertion occurred
 @param[in] function_name: the name of the function where the assertion occurred
 @param[in] condition: String of asserted code

 @note This function will be called when the test case asserts that the
 condition is not satisfied.
 */
extern void vsf_test_assert(vsf_test_result_t result,
                               const char *file_name, uint32_t line,
                               const char *function_name,
                               const char *condition);

/**
 @brief reboot, usually called inside an exception.
 @param[in] result: test result,  @ref vsf_test_result_t
 @param[in] file_name: then name of the file where the assertion occurred
 @param[in] line: the line number of the code where the assertion occurred
 @param[in] function_name: the name of the function where the assertion occurred
 @param[in] additional_str: provide additional exception information

 */
extern void vsf_test_reboot(vsf_test_result_t result,
                            const char *file_name, uint32_t line,
                            const char *function_name,
                            const char *additional_str);

/**
 @brief Busy-wait for approximately the given milliseconds. Useful for
 simple inter-step delays in test scenarios where vsf_systimer is not
 initialized. Calibrated by VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS.
 @param[in] ms: milliseconds to wait
 */
extern void vsf_test_busy_wait_ms(uint32_t ms);
extern void vsf_test_busy_wait_us(uint32_t us);

/**
 @brief Board-provided hardware init for the test framework.
 Fills wdt, reboot entries, peripheral instances, and instance count.
 Called once from main before vsf_test_run().
 @param[in,out] test: pointer to vsf_test_t to initialize hardware fields
 */
extern void vsf_test_hw_setup(vsf_test_t *test);

/**
 @brief GPIO config hook called before/after running test cases on an instance.
 Board override (weak): configure pinmux / GPIO for the peripheral under test.
 @param[in] peripheral_type: VSF_PERIPHERAL_TYPE_* of the instance under test
 @param[in] arg: HAL handle (or array of handles) for the instance, same as suite->arg
 @param[in] init: true before cases, false after cases (teardown)
 */
extern void vsf_test_hw_config(vsf_peripheral_type_t peripheral_type, const vsf_test_inst_t *inst, bool init);

/* ========================== Test Suite primitive ========================== */

/**
 @brief Run a single test case within a suite. No setup/teardown is performed
 — the caller is responsible for calling setup before the first case and
 teardown after the last case.
 @param[in] suite: pointer to the suite containing the case
 @param[in] local_idx: index of the case within the suite's cases array
 */
extern vsf_test_result_t vsf_test_run_suite_case(const vsf_test_suite_t *suite, uint16_t local_idx, const vsf_test_inst_t *inst);

/**
 @brief Run all cases in a suite. Calls setup before the first case and
 teardown after the last case. If setup returns false, all cases are skipped.
 @param[in] suite: pointer to the suite to run
 */
extern void vsf_test_run_suite(const vsf_test_suite_t *suite);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

#    endif
#endif
/* EOF */