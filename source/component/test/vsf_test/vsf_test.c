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

#include "./vsf_test.h"

#if VSF_USE_TEST == ENABLED

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_test_init(vsf_test_t *test, vsf_test_case_t *test_case_array,
                   uint32_t test_case_size)
{
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(test_case_array != NULL);
    VSF_ASSERT(test_case_size > 0);

    int i;
    for (i = 0; i < test_case_size; i++) {
        if (test_case_array[i].jmp_fn == NULL) {
            break;
        }
    }
    test->test_case.offset = i;
    test->test_case.size   = test_case_size;
    test->test_case.array  = test_case_array;
}

bool vsf_test_add_ex(vsf_test_t *test, vsf_test_case_t *test_case)
{
    VSF_ASSERT(test != NULL);

    if (test->test_case.offset < test->test_case.size) {
        test->test_case.array[test->test_case.offset] = *test_case;
        test->test_case.offset++;
        return false;
    } else {
        VSF_ASSERT(0);
        return true;
    }
}

bool vsf_test_add(vsf_test_t *test, vsf_test_jmp_fn_t *jmp_fn, char *cfg_str)
{
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(jmp_fn != NULL);

    vsf_test_case_t test_case = {
        .jmp_fn     = jmp_fn,
        .type       = VSF_TEST_TYPE_LONGJMP_FN,
        .expect_wdt = 0,
        .cfg_str    = cfg_str,
    };

    return vsf_test_add_ex(test, &test_case);
}

bool vsf_test_add_bool_fn(vsf_test_t *test, vsf_test_bool_fn_t *b_fn,
                          char *cfg_str)
{
    vsf_test_case_t test_case = {
        .b_fn       = b_fn,
        .type       = VSF_TEST_TYPE_BOOL_FN,
        .expect_wdt = 0,
        .cfg_str    = cfg_str,
    };

    return vsf_test_add_ex(test, &test_case);
}

void __vsf_test_longjmp(vsf_test_t *test, vsf_test_result_t result,
                        const char *file_name, uint32_t line,
                        const char *function_name, const char *condition)
{
    VSF_ASSERT(test != NULL);

    vsf_test_data_t *data     = &test->data;
    data->result              = result;
    data->error.function_name = function_name;
    data->error.file_name     = file_name;
    data->error.condition     = condition;
    data->error.line          = line;

    longjmp(*test->jmp_buf, 1);
}

void vsf_test_reboot(vsf_test_t *test, vsf_test_result_t result,
                     const char *file_name, uint32_t line,
                     const char *function_name, const char *condition)
{
    VSF_ASSERT(test != NULL);

    vsf_test_data_t *data     = &test->data;
    data->result              = result;
    data->error.function_name = function_name;
    data->error.file_name     = file_name;
    data->error.condition     = condition;
    data->error.line          = line;

    data->sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);

    data->status = VSF_TEST_STATUS_IDLE;
    data->sync(data, VSF_TEST_STATUS_WRITE);

    data->idx++;
    data->sync(data, VSF_TEST_TESTCASE_INDEX_WRITE);

    if (test->reboot.external != NULL) {
        test->reboot.external();
    }
    if (test->reboot.internal != NULL) {
        test->reboot.internal();
    }
    while (1);
}

void vsf_test_run_tests(vsf_test_t *test)
{
    if (test->wdt.internal.init != NULL) {
        uint32_t timeout_ms = test->wdt.internal.timeout_ms;
        if (timeout_ms == 0) {
            timeout_ms = VSF_TEST_CFG_INTERNAL_TIMEOUT_MS;
        }
        test->wdt.internal.init(&test->wdt.internal, timeout_ms);
    }
    if (test->wdt.external.init != NULL) {
        uint32_t timeout_ms = test->wdt.external.timeout_ms;
        if (timeout_ms == 0) {
            timeout_ms = VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS;
        }
        test->wdt.external.init(&test->wdt.external, timeout_ms);
    }

    vsf_test_data_t *data = &test->data;
    if (data->init != NULL) {
        data->init(data);
    }

    data->sync(data, VSF_TEST_TESTCASE_INDEX_READ);
    while (1) {
        if (data->idx >= test->test_case.offset) {
            break;
        }
        data->sync(data, VSF_TEST_TESTCASE_INDEX_WRITE);
        vsf_test_case_t *test_case = &test->test_case.array[data->idx];

        data->sync(data, VSF_TEST_STATUS_READ);
        // After powering up, we first check if it was reset before.
        if (data->status != VSF_TEST_STATUS_IDLE) {
            // last testing start and wdt timeout
            data->result = test_case->expect_wdt ? VSF_TEST_RESULT_WDT_PASS
                                                 : VSF_TEST_RESULT_WDT_FAIL;
            data->sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);

            data->status = VSF_TEST_STATUS_IDLE;
            data->sync(data, VSF_TEST_STATUS_WRITE);
            data->idx++;

            continue;
        }

        if (test->wdt.internal.feed != NULL) {
            test->wdt.internal.feed(&test->wdt.internal);
        }
        if (test->wdt.external.feed != NULL) {
            test->wdt.external.feed(&test->wdt.external);
        }

        if (test_case->cfg_str != NULL) {
            data->request_str = test_case->cfg_str;
            data->sync(data, VSF_TEST_TESECASE_REQUEST_WRITE);
            if (data->req_continue == VSF_TEST_REQ_NO_SUPPORT) {
                data->result = VSF_TEST_RESULT_SKIP;
                data->sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);
                data->idx++;
                continue;
            }
        }

        data->error.function_name = NULL;
        data->error.file_name     = NULL;
        data->error.condition     = NULL;
        data->error.line          = 0;

        // first test, IDLE -> RUNNING
        data->status = VSF_TEST_STATUS_RUNNING;
        data->sync(data, VSF_TEST_STATUS_WRITE);

        vsf_test_type_t type = test_case->type;
        switch (type) {
        case VSF_TEST_TYPE_BOOL_FN:
            data->result = test_case->b_fn();
            break;
        case VSF_TEST_TYPE_LONGJMP_FN: {
            jmp_buf buf;
            data->result  = VSF_TEST_RESULT_PASS;
            test->jmp_buf = &buf;
            if (0 == setjmp(buf)) {
                test_case->jmp_fn();
            }
        } break;
        default:
            VSF_ASSERT(0);
            break;
        }

        data->sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);
        data->status = VSF_TEST_STATUS_IDLE;
        data->sync(data, VSF_TEST_STATUS_WRITE);
        data->idx++;
    }

    data->sync(data, VSF_TEST_DONE);

    while (1) {
        if (test->wdt.internal.feed != NULL) {
            test->wdt.internal.feed(&test->wdt.internal);
        }
        if (test->wdt.external.feed != NULL) {
            test->wdt.external.feed(&test->wdt.external);
        }
    }
}

#endif
