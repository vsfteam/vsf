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

//! \note do not move this pre-processor statement to other places
#include "component/vsf_component_cfg.h"
#include "utilities/vsf_utilities.h"

/* example:

    // 0. Include vsf header file
    #include "vsf.h"

    // 1. Defining vsf_test variable
    static vsf_test_t __test = {
        .wdt =
            {
                .internal =
                    {
                        // 1.1 If you are using a device that already supports
                        // vsf_hal_wdt, then you can just use vsf_test_hal_wdt_*
                        .init = vsf_test_hal_wdt_init,
                        .feed = vsf_test_hal_wdt_feed,
                    },
                .external =
                    {
                        // 1.2 If you are using an assist device that can
                        // control the power pin or reset pin of the device,
                        // then you can use vsf_test_stdio_wdt_* to control it
                        .init = vsf_test_stdio_wdt_init,
                        .feed = vsf_test_stdio_wdt_feed,
                    },
            },
        .reboot =
            {
                // 1.3 Reset can be done using the functions provided by
                // vsf_arch or the chip's APIs
                .internal = vsf_arch_reset,

                // 1.4 To control the reset or power pins of the device, we can
                // use the stdio method to communicate.
                .external = vsf_test_stdio_reboot,
            },
        .data = {
            // We use stdio to communicate with assist devices for data
            // persistence. This way we only need to implement the stdio stub
            // function on the device to come.
            .init = vsf_test_stdio_data_init,
            .sync = vsf_test_stdio_data_sync,
        }};
    static vsf_test_t *test = &__test;

    // 3. Optionally, call vsf_test_reboot in the callback function for all
    // exceptions to abort the test and provide additional error messages if
    // possible information.
    // Here is an example of HardFault_Handler in Cortex-M:
    void HardFault_Handler(void)
    {
        vsf_test_reboot(test, VSF_TEST_RESULT_FAULT_HANDLER_FAIL, __FILE__,
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

        VSF_TEST_ASSERT(test, a == b);
    }

    // test failed
    static void __test_fail(void)
    {
        int a = 100;
        int b = 50 + 50;

        VSF_TEST_ASSERT(test, a != b);
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
        // 5. We support two styles of adding test cases.
        // - The first is the static way. We can use macros to initialize test
        //   cases. It is more RAM efficient.
        // - The second is the dynamic way of adding, which is closer to the
        //   traditional testing framework.
    #if 1
        static const vsf_test_case_t __test_cases[] = {
            VSF_TEST_ADD(__test_pass, "test_pass hw_req=none", 0),
            VSF_TEST_ADD(__test_fail, "test_fail hw_req=none", 0),
            VSF_TEST_ADD(__test_wdt, "test_wdt hw_req=none", 0),
            VSF_TEST_ADD(__test_unalign, "test_invalid_address hw_req=none", 0),
        };
        vsf_test_init(test, (vsf_test_case_t *)__test_cases,
                      dimof(__test_cases));
    #else static vsf_test_case_t __test_cases[10];
        vsf_test_init(test, __test_cases, dimof(__test_cases));
        vsf_test_add(test, __test_pass, "test_pass hw_req=none");
        vsf_test_add(test, __test_fail, "test_fail hw_req=none");
        vsf_test_add(test, __test_wdt, "test_wdt hw_req=none");
        vsf_test_add(test, __test_unalign, "test_invalid_address hw_req=none");
    #endif

        // 6. Here the test will start running
        vsf_test_run_tests(test);

        return 0;
    }
*/

#ifndef __VSF_TEST_H__
#    define __VSF_TEST_H__

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

//!< Using stdio to save persistent data to assist device
#        ifndef VSF_TEST_CFG_USE_STDIO_DATA_SYNC
#            define VSF_TEST_CFG_USE_STDIO_DATA_SYNC DISABLED
#        endif

/*!
    \def VSF_TEST_ASSERT(__t, __v)
    \brief Add an assertion to the test case
    \param __t pointer to @ref vsf_test_t
    \param __v expressions for test conditions
*/
#        define VSF_TEST_ASSERT(__t, __v)                                      \
            do {                                                               \
                if (!(__v)) {                                                  \
                    __vsf_test_longjmp(__t, VSF_TEST_RESULT_FAIL, __FILE__,    \
                                       __LINE__, __FUNCTION__, #__v);          \
                }                                                              \
            } while (0)

/*!
    \def VSF_TEST_ADD_EX(__FN, __CFG, __TYPE, ...)
    \brief Initialize to add a test case of any type
    \param __FN pointer to @ref vsf_test_bool_fn_t or @ref vsf_test_jmp_fn_t
    \param __CFG qequest string for test cases
    \param __TYPE type @ref vsf_test_type_t
    \param ...  More configuration options, such as expect_wdt for whether to
   expect a watchdog reset or not
*/
#        define VSF_TEST_ADD_EX(__FN, __CFG, __TYPE, ...)                      \
            {.jmp_fn = __FN, .cfg_str = __CFG, .type = __TYPE, __VA_ARGS__}

/*!
    \def VSF_TEST_ADD_BOOL_FN(__FN, __CFG, __TYPE, ...)
    \brief Initialize to add a test case of VSF_TEST_TYPE_BOOL_FN type
    \param __FN pointer to @ref vsf_test_bool_fn_t or @ref vsf_test_jmp_fn_t
    \param __CFG qequest string for test cases
    \param ...  More configuration options, such as expect_wdt for whether to
   expect a watchdog reset or not
*/
#        define VSF_TEST_ADD_BOOL_FN(__FN, __CFG, ...)                         \
            {.jmp_fn  = __FN,                                                  \
             .cfg_str = __CFG,                                                 \
             .type    = VSF_TEST_TYPE_BOOL_FN,                                 \
             __VA_ARGS__}

/*!
    \def VSF_TEST_ADD(__FN, __CFG, __TYPE, ...)
    \brief Initialize to add a test case of VSF_TEST_TYPE_LONGJMP_FN type
    \param __FN pointer to @ref vsf_test_bool_fn_t or @ref vsf_test_jmp_fn_t
    \param __CFG qequest string for test cases
    \param ...  More configuration options, such as expect_wdt for whether to
   expect a watchdog reset or not
*/
#        define VSF_TEST_ADD(__FN, __CFG, ...)                                 \
            {.jmp_fn  = __FN,                                                  \
             .cfg_str = __CFG,                                                 \
             .type    = VSF_TEST_TYPE_LONGJMP_FN,                              \
             __VA_ARGS__}

/*============================ TYPES =========================================*/

typedef enum vsf_test_status_t {
    VSF_TEST_STATUS_IDLE    = 0,
    VSF_TEST_STATUS_RUNNING = 1,
} vsf_test_status_t;

typedef enum vsf_test_req_t {
    VSF_TEST_REQ_NO_SUPPORT = 0,
    VSF_TEST_REQ_SUPPORT    = 1,
} vsf_test_req_t;

typedef enum vsf_test_result_t {
    VSF_TEST_RESULT_PASS               = 0x0u << 0,
    VSF_TEST_RESULT_SKIP               = 0x1u << 0,
    VSF_TEST_RESULT_WDT_PASS           = 0x2u << 0,
    VSF_TEST_RESULT_FAIL               = 0x3u << 0,
    VSF_TEST_RESULT_ASSERT_FAIL        = VSF_TEST_RESULT_FAIL,
    VSF_TEST_RESULT_WDT_FAIL           = 0x4u << 0,
    VSF_TEST_RESULT_ASSIST_FAIL        = 0x5u << 0,
    VSF_TEST_RESULT_FAULT_HANDLER_FAIL = 0x6u << 0,
} vsf_test_result_t;

//! Test the type of the function,
typedef enum vsf_test_type_t {
    //! Functions with no return value can use VSF_TEST_ASSERT, which depends on
    //! setjmp/longjmp.
    VSF_TEST_TYPE_LONGJMP_FN = 0,
    //! Functions with boolean return values do not depend on longjmp but cannot
    //! use VSF_TEST_ASSERT
    VSF_TEST_TYPE_BOOL_FN,
} vsf_test_type_t;

typedef void vsf_test_reboot_t(void);

typedef struct vsf_test_wdt_t vsf_test_wdt_t;
struct vsf_test_wdt_t {
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
};

//! Device commands during data synchronization
typedef enum vsf_test_data_cmd_t {
    //! Reading the current state requires a response from the assist device
    VSF_TEST_STATUS_READ,
    //! Write current state, no assist device response required
    VSF_TEST_STATUS_WRITE,

    //! Reading the current index requires a response from the assist device
    VSF_TEST_TESTCASE_INDEX_READ,
    //! Write current index, no assist device response required
    VSF_TEST_TESTCASE_INDEX_WRITE,

    //! Send test request information, requires an assist device response.
    VSF_TEST_TESECASE_REQUEST_WRITE,

    //! Write the result of the current test, does not require a response from
    //! the assist device.
    VSF_TEST_TESTCASE_RESULT_WRITE,

    //! All tests are completed without a response from the assist device. The
    //! Assist Device should output a test report and be ready for the next
    //! test.
    VSF_TEST_DONE,
} vsf_test_data_cmd_t;

//! All data here needs to be synchronized with the assist device after
//! modification. After powering up, we need to read the data from the assist
//! device first.
typedef struct vsf_test_data_t vsf_test_data_t;
struct vsf_test_data_t {
    //! This function is called after each power-up and before performing data
    //! synchronization. In it, we can initialize the peripherals needed to
    //! synchronize the data.
    void (*init)(vsf_test_data_t *data);
    //! Synchronize data according to different commands. All commands are
    //! actively sent by the device, some commands are only sent, and some
    //! commands need to wait for a response from the auxiliary device after
    //! they are sent. All commands are not sent during the test case run, so
    //! there is no need to worry about real-time.
    void (*sync)(vsf_test_data_t *data, vsf_test_data_cmd_t cmd);

    //! No matter how many test cases there are, there is only one copy of the
    //! persistent data on the device side. So using a 32bit variable to save
    //! the data here is not significantly wasteful.

    //! The current test case, the initial value is 0. It will be incremented
    //! once every test is completed, and will be set to 0 after all tests are
    //! completed.
    uint32_t idx;

    //! Test status, @ref vsf_test_status_t
    uint32_t status;

    //! The test request information from the device, which will be sent to the
    //! assist device and confirm from the assist device whether the test is
    //! supported or not.
    char *request_str;
    //! Before the start of the current test, the device will confirm with the
    //! secondary device whether the current test is supported. If it is not
    //! supported this variable will have a value of VSF_TEST_REQ_NO_SUPPORT, if
    //! it is supported this variable will have a value of VSF_TEST_REQ_SUPPORT.
    uint32_t req_continue;

    //! The results of the device's current test.
    //! Please note that a successful test on the device side does not represent
    //! the final result, and the assist device may modify the test result to a
    //! test failure depending on the actual situation.
    uint32_t result;

    //! More information is logged here when the test is asserted or goes to an
    //! exception.
    struct {
        const char *function_name;
        const char *file_name;
        const char *condition;
        uint32_t    line;
    } error;
};

typedef bool vsf_test_bool_fn_t(void);
typedef void vsf_test_jmp_fn_t(void);

typedef struct vsf_test_case_t {
    union {
        //! The test function uses Boolean return value of function that returns
        //! true for a successful test and false for a failed test. You cannot
        //! use VSF_TEST_ASSERT inside this function.
        vsf_test_bool_fn_t *b_fn;

        //! Test functions that use no return value use VSF_TEST_ASSERT to
        //! assert failure. Inside a function, if it is not asserted, the test
        //! succeeds
        vsf_test_jmp_fn_t *jmp_fn;
    };
    char *cfg_str; // json or other configuration format

    //! Use different test function prototypes depending on the type.
    //! @ref vsf_test_type_t
    //! VSF_TEST_TYPE_BOOL_FN: use b_fn
    //! VSF_TEST_TYPE_LONGJMP_FN : use jmp_fn
    uint8_t type;

    //! If the result of the test is expected to be a watchdog reset. Then set
    //! this variable to one
    uint8_t expect_wdt;
} vsf_test_case_t;

typedef struct vsf_test_t {
    //! Without a watchdog, we can still can test.
    //! But the watchdog provides stronger guarantees for tests:
    //! if a test is abnormal, the next test continues to run after the watchdog
    //! times out.
    struct {
        //! internal watchdog means the watchdog
        //! inside the device
        vsf_test_wdt_t internal;

        //! Use an external method to implement a watchdog,
        //! possibly via the reset pin or the device's power pin.
        vsf_test_wdt_t external;
    } wdt;

    //! We perform a reset when the test program goes into exception.
    //! 1. First execute the external reset function.
    //! 2. If the external reset is not successful, then we continue to execute
    //! the internal reset function.
    //! 3. If the internal reset is not successful, then we enter a dead loop
    //! and wait for the watchdog to reset.
    struct {
        //! Use the chip's internal reset, possibly a hot reset
        vsf_test_reboot_t *internal;
        //! Use an external method to reset, possibly via the
        //! reset pin or the device's power pin.
        vsf_test_reboot_t *external;
    } reboot;

    //! Persistent data, each time the data in this structure is modified it
    //! should be actively synchronized to the assist device
    vsf_test_data_t data;

#        if VSF_TEST_CFG_LONGJMP == ENABLED
    jmp_buf *jmp_buf;
#        endif

    //! Information for each test
    struct {
        uint32_t         size;
        uint32_t         offset;
        vsf_test_case_t *array;
    } test_case;
} vsf_test_t;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/**
 @brief initialize vsf test
 @param[in] test: a pointer to structure @ref vsf_test_t
 @param[in] tc_array: a pointer to array @ref vsf_test_case_t
 @param[in] array_size: size of tc_array
 */
extern void vsf_test_init(vsf_test_t *test, vsf_test_case_t *tc_array,
                          uint32_t array_size);

/**
 @brief Add to add a test case of any type
 @param[in] test: a pointer to structure @ref vsf_test_t
 @param[in] test_case: a pointer to array @ref vsf_test_case_t
 @return bool: true if add was successfully, or false
 */
extern bool vsf_test_add_ex(vsf_test_t *test, vsf_test_case_t *test_case);

/**
 @brief Add to add a test case of VSF_TEST_TYPE_LONGJMP_FN type
 @param[in] test: a pointer to structure @ref vsf_test_t
 @param[in] jmp_fn: a pointer to function @ref vsf_test_jmp_fn_t
 @param[in] cfg: a string of request information for the test case
 @return bool: true if add was successfully, or false
 */
extern bool vsf_test_add(vsf_test_t *test, vsf_test_jmp_fn_t *jmp_fn,
                         char *cfg);

/**
 @brief Add to add a test case of VSF_TEST_TYPE_BOOL_FN type
 @param[in] test: a pointer to structure @ref vsf_test_t
 @param[in] b_fn: a pointer to function @ref vsf_test_bool_fn_t
 @param[in] cfg: a string of request information for the test case
 @return bool: true if add was successfully, or false
 */
extern bool vsf_test_add_bool_fn(vsf_test_t *test, vsf_test_bool_fn_t *b_fn,
                                 char *cfg);

/**
 @brief Run all tests. Should be called after all use cases have been
 initialized.
 @param[in] test: a pointer to structure @ref vsf_test_t
 */
extern void vsf_test_run_tests(vsf_test_t *test);

/**
 @brief rong jump. the user does not need to directly call this API
 @param[in] test: a pointer to structure @ref vsf_test_t
 @param[in] result: test result,  @ref vsf_test_result_t
 @param[in] file_name: then name of the file where the assertion occurred
 @param[in] line: the line number of the code where the assertion occurred
 @param[in] function_name: the name of the function where the assertion occurred
 @param[in] condition: String of asserted code

 @note This function will be called when the test case asserts that the
 condition is not satisfied.
 */
extern void __vsf_test_longjmp(vsf_test_t *test, vsf_test_result_t result,
                               const char *file_name, uint32_t line,
                               const char *function_name,
                               const char *condition);

/**
 @brief reboot, usually called inside an exception.
 @param[in] test: a pointer to structure @ref vsf_test_t
 @param[in] result: test result,  @ref vsf_test_result_t
 @param[in] file_name: then name of the file where the assertion occurred
 @param[in] line: the line number of the code where the assertion occurred
 @param[in] function_name: the name of the function where the assertion occurred
 @param[in] additional_str: provide additional exception information

 */
extern void vsf_test_reboot(vsf_test_t *test, vsf_test_result_t result,
                            const char *file_name, uint32_t line,
                            const char *function_name,
                            const char *additional_str);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ INCLUDES ======================================*/

#        include "./port/vsf_test_port_hal.h"
#        include "./port/vsf_test_port_stdio.h"

#    endif
#endif
/* EOF */