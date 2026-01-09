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

#include "../vsf_test.h"
#include "vsf_test_port_appcfg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_TEST == ENABLED && VSF_TEST_CFG_USE_APPCFG_DATA_SYNC == ENABLED

#if VSF_USE_TRACE == ENABLED
#   include "service/trace/vsf_trace.h"
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_TRACE == ENABLED
#   define __VSF_TEST_APPCFG_TRACE_INFO(...)    vsf_trace_info(__VA_ARGS__)
#   define __VSF_TEST_APPCFG_TRACE_DEBUG(...)   vsf_trace_debug(__VA_ARGS__)
#   define __VSF_TEST_APPCFG_TRACE_ERROR(...)   vsf_trace_error(__VA_ARGS__)
#else
#   define __VSF_TEST_APPCFG_TRACE_INFO(...)
#   define __VSF_TEST_APPCFG_TRACE_DEBUG(...)
#   define __VSF_TEST_APPCFG_TRACE_ERROR(...)
#endif

//! \brief 执行 appcfg 命令并获取输出
static int __vsf_test_appcfg_exec(const char *cmd, char *output, size_t output_size)
{
    char full_cmd[256];
    snprintf(full_cmd, sizeof(full_cmd), "appcfg %s", cmd);

    FILE *fp = popen(full_cmd, "r");
    if (fp != NULL) {
        if (output != NULL && output_size > 0) {
            output[0] = '\0';
            // 读取所有输出行，查找实际的数据行
            char line[256];
            while (fgets(line, sizeof(line), fp) != NULL) {
                // 移除换行符
                size_t len = strlen(line);
                if (len > 0 && line[len - 1] == '\n') {
                    line[len - 1] = '\0';
                }
                // 跳过包含 "appcfg:" 或 "succeed" 的日志行
                if (strstr(line, "appcfg:") == NULL &&
                    strstr(line, "succeed") == NULL &&
                    strstr(line, "fail") == NULL) {
                    // 这是实际的数据行
                    strncpy(output, line, output_size - 1);
                    output[output_size - 1] = '\0';
                    break;
                }
            }
        }
        int status = pclose(fp);
        // 检查进程退出状态
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return -1;
    }
    return -1;
}

//! \brief 构建配置键名
static void __vsf_test_appcfg_get_key(char *key, size_t key_size, const char *name)
{
    snprintf(key, key_size, "%s.%s", VSF_TEST_APPCFG_KEY_PREFIX, name);
}

//! \brief 读取配置值（整数）
static bool __vsf_test_appcfg_read_uint32(const char *key, uint32_t *value)
{
    char output[64];

    // appcfg key 返回 value
    if (__vsf_test_appcfg_exec(key, output, sizeof(output)) == 0 && output[0] != '\0') {
        char *endptr;
        unsigned long val = strtoul(output, &endptr, 0);
        // 检查是否成功解析（endptr 指向字符串末尾或空格）
        if (endptr != output && (*endptr == '\0' || *endptr == ' ' || *endptr == '\t')) {
            *value = (uint32_t)val;
            return true;
        }
    }
    return false;
}

//! \brief 写入配置值（整数）
static bool __vsf_test_appcfg_write_uint32(const char *key, uint32_t value)
{
    char cmd[256];
    char output[64];

    // appcfg key value 设置 key value
    snprintf(cmd, sizeof(cmd), "%s %u", key, value);
    return __vsf_test_appcfg_exec(cmd, output, sizeof(output)) == 0;
}

//! \brief 读取配置值（字符串）
static bool __vsf_test_appcfg_read_string(const char *key, char *value, size_t value_size)
{
    // appcfg key 返回 value
    return __vsf_test_appcfg_exec(key, value, value_size) == 0;
}

//! \brief 写入配置值（字符串）
static bool __vsf_test_appcfg_write_string(const char *key, const char *value)
{
    char cmd[256];
    char output[64];

    // appcfg key value 设置 key value
    snprintf(cmd, sizeof(cmd), "%s %s", key, value);
    return __vsf_test_appcfg_exec(cmd, output, sizeof(output)) == 0;
}

VSF_CAL_WEAK(vsf_test_appcfg_data_init)
void vsf_test_appcfg_data_init(vsf_test_data_t *data)
{
    // 初始化时检查 appcfg 命令是否可用
    char output[64];
    if (__vsf_test_appcfg_exec("version", output, sizeof(output)) == 0) {
        __VSF_TEST_APPCFG_TRACE_INFO("[TEST] Appcfg initialized: %s\r\n", output);
    } else {
        __VSF_TEST_APPCFG_TRACE_ERROR("[TEST] Failed to initialize appcfg\r\n");
    }
}

//! \brief 读取测试数据
static void __vsf_test_appcfg_read_data(vsf_test_data_t *data)
{
    char key[128];
    uint32_t value;

    __vsf_test_appcfg_get_key(key, sizeof(key), "idx");
    if (__vsf_test_appcfg_read_uint32(key, &value)) {
        data->idx = value;
    }

    __vsf_test_appcfg_get_key(key, sizeof(key), "status");
    if (__vsf_test_appcfg_read_uint32(key, &value)) {
        data->status = value;
    }

    __vsf_test_appcfg_get_key(key, sizeof(key), "req_continue");
    if (__vsf_test_appcfg_read_uint32(key, &value)) {
        data->req_continue = value;
    }
}

//! \brief 写入测试数据
static void __vsf_test_appcfg_write_data(vsf_test_data_t *data)
{
    char key[128];

    __vsf_test_appcfg_get_key(key, sizeof(key), "idx");
    __vsf_test_appcfg_write_uint32(key, data->idx);

    __vsf_test_appcfg_get_key(key, sizeof(key), "status");
    __vsf_test_appcfg_write_uint32(key, data->status);

    __vsf_test_appcfg_get_key(key, sizeof(key), "req_continue");
    __vsf_test_appcfg_write_uint32(key, data->req_continue);
}

//! \brief 提取测试名字（从 cfg_str 中）
static void __vsf_test_appcfg_extract_name(const char *cfg_str, char *name, size_t name_size)
{
    if (cfg_str != NULL) {
        const char *space = strchr(cfg_str, ' ');
        if (space != NULL) {
            size_t len = space - cfg_str;
            if (len < name_size) {
                strncpy(name, cfg_str, len);
                name[len] = '\0';
            } else {
                strncpy(name, cfg_str, name_size - 1);
                name[name_size - 1] = '\0';
            }
        } else {
            strncpy(name, cfg_str, name_size - 1);
            name[name_size - 1] = '\0';
        }
    } else {
        strncpy(name, "unknown", name_size - 1);
        name[name_size - 1] = '\0';
    }
}

//! \brief 读取特定索引的测试用例结果
static bool __vsf_test_appcfg_read_case_result(uint32_t idx, uint32_t *result)
{
    char key[128];
    snprintf(key, sizeof(key), "%s.result.%u", VSF_TEST_APPCFG_KEY_PREFIX, idx);
    return __vsf_test_appcfg_read_uint32(key, result);
}

void vsf_test_appcfg_data_sync(vsf_test_data_t *data, vsf_test_data_cmd_t index)
{
    VSF_ASSERT(data != NULL);

    switch (index) {
    case VSF_TEST_STATUS_READ:
        __vsf_test_appcfg_read_data(data);
        break;
    case VSF_TEST_STATUS_WRITE:
        __vsf_test_appcfg_write_data(data);
        break;

    case VSF_TEST_TESTCASE_INDEX_READ:
        __vsf_test_appcfg_read_data(data);
        // 尝试从配置中读取对应索引的结果
        // 注意：如果结果不存在，不设置 data->result，保持原值
        uint32_t case_result;
        if (__vsf_test_appcfg_read_case_result(data->idx, &case_result)) {
            data->result = case_result;
        } else {
            // 如果结果不存在，设置为默认值 PASS（0）
            data->result = VSF_TEST_RESULT_PASS;
        }
        break;
    case VSF_TEST_TESTCASE_INDEX_WRITE:
        __vsf_test_appcfg_write_data(data);
        break;

    case VSF_TEST_TESECASE_REQUEST_WRITE:
        // 写入请求信息，然后读取响应
        if (data->request_str != NULL) {
            char key[128];
            __vsf_test_appcfg_get_key(key, sizeof(key), "request");
            __vsf_test_appcfg_write_string(key, data->request_str);

            __vsf_test_appcfg_get_key(key, sizeof(key), "req_continue");
            if (!__vsf_test_appcfg_read_uint32(key, &data->req_continue)) {
                // 默认支持测试
                data->req_continue = VSF_TEST_REQ_SUPPORT;
            }
        } else {
            data->req_continue = VSF_TEST_REQ_SUPPORT;
        }
        break;

    case VSF_TEST_TESTCASE_RESULT_WRITE:
        {
            const char *result_str = (data->result == VSF_TEST_RESULT_PASS) ? "PASS" :
                                     (data->result == VSF_TEST_RESULT_SKIP) ? "SKIP" :
                                     (data->result == VSF_TEST_RESULT_WDT_PASS) ? "WDT_PASS" :
                                     (data->result == VSF_TEST_RESULT_WDT_FAIL) ? "WDT_FAIL" :
                                     (data->result == VSF_TEST_RESULT_FAIL) ? "FAIL" : "UNKNOWN";

            if ((data->error.function_name != NULL) &&
                (data->error.file_name != NULL)) {
                if (data->error.condition == NULL) {
                    data->error.condition = "";
                }
                __VSF_TEST_APPCFG_TRACE_INFO("[TEST] #%u: %s - %s:%u in %s() - %s\r\n",
                              data->idx, result_str, data->error.file_name, data->error.line,
                              data->error.function_name, data->error.condition);
            } else {
                __VSF_TEST_APPCFG_TRACE_INFO("[TEST] #%u: %s\r\n", data->idx, result_str);
            }

            // 提取测试名字（从 request_str 中，因为 request_str 就是 cfg_str）
            char test_name[64] = "unknown";
            if (data->request_str != NULL) {
                __vsf_test_appcfg_extract_name(data->request_str, test_name, sizeof(test_name));
            }

            // 将测试结果写入配置
            char key[128];
            snprintf(key, sizeof(key), "%s.result.%u", VSF_TEST_APPCFG_KEY_PREFIX, data->idx);
            __vsf_test_appcfg_write_uint32(key, data->result);

            // 将测试名字写入配置
            snprintf(key, sizeof(key), "%s.name.%u", VSF_TEST_APPCFG_KEY_PREFIX, data->idx);
            __vsf_test_appcfg_write_string(key, test_name);
        }
        __vsf_test_appcfg_write_data(data);
        break;

    case VSF_TEST_DONE:
        __VSF_TEST_APPCFG_TRACE_INFO("[TEST] All test cases completed\r\n");
        // 所有测试完成，写入完成标记
        __vsf_test_appcfg_write_data(data);
        break;

    default:
        break;
    }
}

//! \brief 公开函数：读取测试用例结果和名字
bool vsf_test_appcfg_read_case_result(uint32_t idx, uint32_t *result,
                                       char *name, size_t name_size)
{
    char key[128];
    bool found = false;

    // 读取结果
    if (result != NULL) {
        snprintf(key, sizeof(key), "%s.result.%u", VSF_TEST_APPCFG_KEY_PREFIX, idx);
        found = __vsf_test_appcfg_read_uint32(key, result);
    }

    // 读取名字
    if (name != NULL && name_size > 0) {
        snprintf(key, sizeof(key), "%s.name.%u", VSF_TEST_APPCFG_KEY_PREFIX, idx);
        if (!__vsf_test_appcfg_read_string(key, name, name_size)) {
            strncpy(name, "unknown", name_size - 1);
            name[name_size - 1] = '\0';
        }
        // 如果读取到名字，也算找到了
        if (!found && name[0] != '\0' && strcmp(name, "unknown") != 0) {
            found = true;
        }
    }

    return found;
}

#endif

