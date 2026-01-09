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
#include <string.h>
#if VSF_TEST_CFG_USE_FILE_DATA_SYNC == ENABLED
#   include "./port/vsf_test_port_file.h"
#endif
#if VSF_TEST_CFG_USE_APPCFG_DATA_SYNC == ENABLED
#   include "./port/vsf_test_port_appcfg.h"
#endif

#if VSF_USE_TEST == ENABLED

#if VSF_TEST_CFG_USE_TRACE == ENABLED
#   if VSF_USE_TRACE != ENABLED
#       error "VSF_USE_TRACE must be ENABLED when VSF_TEST_CFG_USE_TRACE is ENABLED"
#   endif
#   include "service/trace/vsf_trace.h"
#endif

/*============================ MACROS ========================================*/

#if VSF_TEST_CFG_USE_TRACE == ENABLED
#   define __VSF_TEST_TRACE_INFO(...)    vsf_trace_info(__VA_ARGS__)
#   define __VSF_TEST_TRACE_DEBUG(...)   vsf_trace_debug(__VA_ARGS__)
#   define __VSF_TEST_TRACE_ERROR(...)   vsf_trace_error(__VA_ARGS__)
#else
#   define __VSF_TEST_TRACE_INFO(...)
#   define __VSF_TEST_TRACE_DEBUG(...)
#   define __VSF_TEST_TRACE_ERROR(...)
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

static vsf_test_t __vsf_test;

/*============================ IMPLEMENTATION ================================*/

void vsf_test_init(const vsf_test_cfg_t *cfg)
{
    VSF_ASSERT(cfg != NULL);

    // 配置看门狗
    __vsf_test.wdt.internal = cfg->wdt.internal;
    __vsf_test.wdt.external = cfg->wdt.external;

    // 配置复位函数
    __vsf_test.reboot.internal = cfg->reboot.internal;
    __vsf_test.reboot.external = cfg->reboot.external;

    // 配置数据持久化
    __vsf_test.data.init = cfg->data.init;
    __vsf_test.data.sync = cfg->data.sync;

    // 配置完成时重启选项
    __vsf_test.restart_on_done = cfg->restart_on_done;

    // 初始化测试用例计数
    __vsf_test.test_case_count = 0;

    // 初始化数据同步
    if (__vsf_test.data.init != NULL) {
        __vsf_test.data.init(&__vsf_test.data);
    }

    __VSF_TEST_TRACE_INFO("[TEST] Initialized with capacity %u\r\n", VSF_TEST_CFG_ARRAY_SIZE);
}

bool vsf_test_add_ex(vsf_test_case_t *test_case)
{
    if (__vsf_test.test_case_count < VSF_TEST_CFG_ARRAY_SIZE) {
        __vsf_test.test_case_array[__vsf_test.test_case_count] = *test_case;
        __VSF_TEST_TRACE_DEBUG("vsf_test_add_ex: added test case at index %u, type=%u\r\n",
                              __vsf_test.test_case_count, test_case->type);
        __vsf_test.test_case_count++;
        return false;
    } else {
        __VSF_TEST_TRACE_ERROR("vsf_test_add_ex: test case array is full (count=%u, capacity=%u)\r\n",
                              __vsf_test.test_case_count, VSF_TEST_CFG_ARRAY_SIZE);
        VSF_ASSERT(0);
        return true;
    }
}

bool vsf_test_add_simple_case(vsf_test_jmp_fn_t *jmp_fn, char *cfg_str)
{
    return vsf_test_add_case(jmp_fn, cfg_str, 0);
}

bool vsf_test_add_bool_fn(vsf_test_bool_fn_t *b_fn, char *cfg_str)
{
    vsf_test_case_t test_case = {
        .b_fn       = b_fn,
        .type       = VSF_TEST_TYPE_BOOL_FN,
        .expect_wdt = 0,
        .cfg_str    = cfg_str,
    };

    __VSF_TEST_TRACE_DEBUG("vsf_test_add_bool_fn: adding BOOL_FN test case, cfg_str=%s\r\n",
                          cfg_str ? cfg_str : "NULL");

    return vsf_test_add_ex(&test_case);
}

bool vsf_test_add_case(vsf_test_jmp_fn_t *fn, char *cfg, uint8_t expect_wdt)
{
    vsf_test_case_t test_case = {
        .jmp_fn      = fn,
        .cfg_str     = cfg,
        .type        = VSF_TEST_TYPE_LONGJMP_FN,
        .expect_wdt  = expect_wdt,
        .expect_assert = 0,
    };
    return vsf_test_add_ex(&test_case);
}

bool vsf_test_add_bool_fn_case(vsf_test_bool_fn_t *fn, char *cfg, uint8_t expect_wdt)
{
    vsf_test_case_t test_case = {
        .b_fn       = fn,
        .cfg_str    = cfg,
        .type       = VSF_TEST_TYPE_BOOL_FN,
        .expect_wdt = expect_wdt,
        .expect_assert = 0,
    };
    return vsf_test_add_ex(&test_case);
}

bool vsf_test_add_ex_case(vsf_test_jmp_fn_t *fn, char *cfg,
                          vsf_test_type_t type,
                          uint8_t expect_wdt,
                          uint8_t expect_assert)
{
    vsf_test_case_t test_case = {
        .jmp_fn      = fn,
        .cfg_str     = cfg,
        .type        = type,
        .expect_wdt  = expect_wdt,
        .expect_assert = expect_assert,
    };
    return vsf_test_add_ex(&test_case);
}

bool vsf_test_add_expect_assert_case(vsf_test_jmp_fn_t *fn,
                                     char *cfg,
                                     uint8_t expect_wdt)
{
    vsf_test_case_t test_case = {
        .jmp_fn      = fn,
        .cfg_str     = cfg,
        .type        = VSF_TEST_TYPE_LONGJMP_FN,
        .expect_wdt  = expect_wdt,
        .expect_assert = 1,
    };
    return vsf_test_add_ex(&test_case);
}

void __vsf_test_longjmp(vsf_test_result_t result,
                        const char *file_name, uint32_t line,
                        const char *function_name, const char *condition)
{
    vsf_test_data_t *data     = &__vsf_test.data;
    data->result              = result;
    data->error.function_name = function_name;
    data->error.file_name     = file_name;
    data->error.condition     = condition;
    data->error.line          = line;

    __VSF_TEST_TRACE_ERROR("[TEST] Assertion failed: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    longjmp(*__vsf_test.jmp_buf, 1);
}

//! \brief 从 test case 中提取测试名字
static const char *__vsf_test_get_name(vsf_test_case_t *test_case, char *name_buf, size_t name_buf_size)
{
    if (test_case->cfg_str != NULL) {
        // cfg_str 格式通常是 "test_name purpose=... hw_req=..."
        const char *space = strchr(test_case->cfg_str, ' ');
        if (space != NULL) {
            size_t len = space - test_case->cfg_str;
            if (len < name_buf_size) {
                strncpy(name_buf, test_case->cfg_str, len);
                name_buf[len] = '\0';
                return name_buf;
            } else {
                strncpy(name_buf, test_case->cfg_str, name_buf_size - 1);
                name_buf[name_buf_size - 1] = '\0';
                return name_buf;
            }
        } else {
            strncpy(name_buf, test_case->cfg_str, name_buf_size - 1);
            name_buf[name_buf_size - 1] = '\0';
            return name_buf;
        }
    } else {
        strncpy(name_buf, "unknown", name_buf_size - 1);
        name_buf[name_buf_size - 1] = '\0';
        return name_buf;
    }
}

void vsf_test_reboot(vsf_test_result_t result,
                     const char *file_name, uint32_t line,
                     const char *function_name, const char *condition)
{
    vsf_test_data_t *data     = &__vsf_test.data;
    data->result              = result;
    data->error.function_name = function_name;
    data->error.file_name     = file_name;
    data->error.condition     = condition;
    data->error.line          = line;

    __VSF_TEST_TRACE_ERROR("[TEST] Reboot due to error: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    data->sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);

    data->status = VSF_TEST_STATUS_IDLE;
    data->sync(data, VSF_TEST_STATUS_WRITE);

    data->idx++;
    data->sync(data, VSF_TEST_TESTCASE_INDEX_WRITE);

    if (__vsf_test.reboot.external != NULL) {
        __VSF_TEST_TRACE_INFO("[TEST] Calling external reboot\r\n");
        __vsf_test.reboot.external();
    }
    if (__vsf_test.reboot.internal != NULL) {
        __VSF_TEST_TRACE_INFO("[TEST] Calling internal reboot\r\n");
        __vsf_test.reboot.internal();
    }
    while (1);
}

void vsf_test_run_tests(void)
{
    __VSF_TEST_TRACE_INFO("[TEST] Starting test framework\r\n");

    if (__vsf_test.wdt.internal.init != NULL) {
        uint32_t timeout_ms = __vsf_test.wdt.internal.timeout_ms;
        if (timeout_ms == 0) {
            timeout_ms = VSF_TEST_CFG_INTERNAL_TIMEOUT_MS;
        }
        __VSF_TEST_TRACE_DEBUG("[TEST] Internal WDT: %u ms\r\n", timeout_ms);
        __vsf_test.wdt.internal.init(&__vsf_test.wdt.internal, timeout_ms);
    }
    if (__vsf_test.wdt.external.init != NULL) {
        uint32_t timeout_ms = __vsf_test.wdt.external.timeout_ms;
        if (timeout_ms == 0) {
            timeout_ms = VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS;
        }
        __VSF_TEST_TRACE_DEBUG("[TEST] External WDT: %u ms\r\n", timeout_ms);
        __vsf_test.wdt.external.init(&__vsf_test.wdt.external, timeout_ms);
    }

    vsf_test_data_t *data = &__vsf_test.data;
    if (data->init != NULL) {
        data->init(data);
    }

    // 如果设置了完成时重启，重置索引为0
    if (__vsf_test.restart_on_done) {
        data->idx = 0;
        data->sync(data, VSF_TEST_TESTCASE_INDEX_WRITE);
        __VSF_TEST_TRACE_INFO("[TEST] Restart on done: starting from test case #0\r\n");
    } else {
        data->sync(data, VSF_TEST_TESTCASE_INDEX_READ);
        if (data->idx > 0) {
            __VSF_TEST_TRACE_INFO("[TEST] Resuming from test case #%u\r\n", data->idx);
        }
    }

    while (1) {
        if (data->idx >= __vsf_test.test_case_count) {
            break;
        }
        data->sync(data, VSF_TEST_TESTCASE_INDEX_WRITE);
        vsf_test_case_t *test_case = &__vsf_test.test_case_array[data->idx];

        data->sync(data, VSF_TEST_STATUS_READ);
        // After powering up, we first check if it was reset before.
        if (data->status != VSF_TEST_STATUS_IDLE) {
            // last testing start and wdt timeout
            __VSF_TEST_TRACE_INFO("[TEST] #%u: WDT timeout detected\r\n", data->idx);
            data->result = test_case->expect_wdt ? VSF_TEST_RESULT_WDT_PASS
                                                 : VSF_TEST_RESULT_WDT_FAIL;
            data->sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);

            data->status = VSF_TEST_STATUS_IDLE;
            data->sync(data, VSF_TEST_STATUS_WRITE);
            data->idx++;

            continue;
        }

        if (__vsf_test.wdt.internal.feed != NULL) {
            __vsf_test.wdt.internal.feed(&__vsf_test.wdt.internal);
        }
        if (__vsf_test.wdt.external.feed != NULL) {
            __vsf_test.wdt.external.feed(&__vsf_test.wdt.external);
        }

        if (test_case->cfg_str != NULL) {
            data->request_str = test_case->cfg_str;
            data->sync(data, VSF_TEST_TESECASE_REQUEST_WRITE);
            if (data->req_continue == VSF_TEST_REQ_NO_SUPPORT) {
                __VSF_TEST_TRACE_INFO("[TEST] #%u: Not supported, skipping\r\n", data->idx);
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

        // 提取测试用例名称（从 cfg_str 中）
        static char name_buf[64];
        const char *test_name = __vsf_test_get_name(test_case, name_buf, sizeof(name_buf));
        __VSF_TEST_TRACE_INFO("[TEST] #%u: Running '%s'\r\n", data->idx, test_name);

        vsf_test_type_t type = test_case->type;
        switch (type) {
        case VSF_TEST_TYPE_BOOL_FN:
            data->result = test_case->b_fn();
            break;
        case VSF_TEST_TYPE_LONGJMP_FN: {
            jmp_buf buf;
            data->result  = VSF_TEST_RESULT_PASS;
            __vsf_test.jmp_buf = &buf;
            if (0 == setjmp(buf)) {
                test_case->jmp_fn();
            } else {
                // 如果通过 setjmp 捕获到断言，检查是否是预期的断言
                if (test_case->expect_assert) {
                    // 预期的断言，测试通过
                    data->result = VSF_TEST_RESULT_PASS;
                    // 清除错误信息，因为这是预期的
                    data->error.function_name = NULL;
                    data->error.file_name     = NULL;
                    data->error.condition     = NULL;
                    data->error.line          = 0;
                }
                // 如果不是预期的断言，data->result 保持为 VSF_TEST_RESULT_FAIL
                // (由 __vsf_test_longjmp 设置)
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

    __VSF_TEST_TRACE_INFO("[TEST] All test cases completed, entering idle loop\r\n");

    // 输出测试汇总信息
    __VSF_TEST_TRACE_INFO("\r\n[TEST] ========== Test Summary ==========\r\n");
    __VSF_TEST_TRACE_INFO("[TEST] Total test cases: %u\r\n", __vsf_test.test_case_count);

    uint32_t pass_count = 0, fail_count = 0, skip_count = 0, wdt_pass_count = 0, wdt_fail_count = 0;

    // 遍历所有测试用例，读取结果并统计
    uint32_t saved_idx = data->idx;
    for (uint32_t i = 0; i < __vsf_test.test_case_count; i++) {
        // 从持久化存储中读取该测试用例的结果
        data->idx = i;
        data->sync(data, VSF_TEST_TESTCASE_INDEX_READ);

        // 读取该测试用例的结果（文件存储实现会根据 idx 返回对应的结果）
        vsf_test_result_t result = (vsf_test_result_t)data->result;

        // 总是从 test case 数组获取测试名字
        static char saved_name[64];
        vsf_test_case_t *test_case = &__vsf_test.test_case_array[i];
        const char *test_name = __vsf_test_get_name(test_case, saved_name, sizeof(saved_name));

        // 统计结果
        const char *result_str = "UNKNOWN";
        switch (result) {
        case VSF_TEST_RESULT_PASS:
            result_str = "PASS";
            pass_count++;
            break;
        case VSF_TEST_RESULT_SKIP:
            result_str = "SKIP";
            skip_count++;
            break;
        case VSF_TEST_RESULT_WDT_PASS:
            result_str = "WDT_PASS";
            wdt_pass_count++;
            break;
        case VSF_TEST_RESULT_WDT_FAIL:
            result_str = "WDT_FAIL";
            wdt_fail_count++;
            break;
        case VSF_TEST_RESULT_FAIL:
        case VSF_TEST_RESULT_ASSIST_FAIL:
        case VSF_TEST_RESULT_FAULT_HANDLER_FAIL:
            result_str = "FAIL";
            fail_count++;
            break;
        default:
            break;
        }

        __VSF_TEST_TRACE_INFO("[TEST] #%u: %s - %s\r\n", i, test_name, result_str);
    }

    // 恢复原始索引
    data->idx = saved_idx;

    // 输出统计信息
    __VSF_TEST_TRACE_INFO("[TEST] ------------------------------------\r\n");
    __VSF_TEST_TRACE_INFO("[TEST] PASS:        %u\r\n", pass_count);
    __VSF_TEST_TRACE_INFO("[TEST] FAIL:        %u\r\n", fail_count);
    __VSF_TEST_TRACE_INFO("[TEST] SKIP:        %u\r\n", skip_count);
    __VSF_TEST_TRACE_INFO("[TEST] WDT_PASS:    %u\r\n", wdt_pass_count);
    __VSF_TEST_TRACE_INFO("[TEST] WDT_FAIL:    %u\r\n", wdt_fail_count);
    __VSF_TEST_TRACE_INFO("[TEST] ====================================\r\n");

    data->sync(data, VSF_TEST_DONE);

    while (1) {
        if (__vsf_test.wdt.internal.feed != NULL) {
            __vsf_test.wdt.internal.feed(&__vsf_test.wdt.internal);
        }
        if (__vsf_test.wdt.external.feed != NULL) {
            __vsf_test.wdt.external.feed(&__vsf_test.wdt.external);
        }
    }
}

#endif
