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
#include "vsf_test_port_file.h"
#include <stdio.h>
#include <string.h>

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_TEST == ENABLED && VSF_TEST_CFG_USE_FILE_DATA_SYNC == ENABLED

#if VSF_USE_TRACE == ENABLED
#   include "service/trace/vsf_trace.h"
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_TRACE == ENABLED
#   define __VSF_TEST_FILE_TRACE_INFO(...)    vsf_trace_info(__VA_ARGS__)
#   define __VSF_TEST_FILE_TRACE_DEBUG(...)   vsf_trace_debug(__VA_ARGS__)
#   define __VSF_TEST_FILE_TRACE_ERROR(...)   vsf_trace_error(__VA_ARGS__)
#else
#   define __VSF_TEST_FILE_TRACE_INFO(...)
#   define __VSF_TEST_FILE_TRACE_DEBUG(...)
#   define __VSF_TEST_FILE_TRACE_ERROR(...)
#endif

//! \brief 测试数据文件结构（二进制格式）
typedef struct {
    uint32_t idx;           //!< 当前测试用例索引
    uint32_t status;         //!< 测试状态
    uint32_t req_continue;   //!< 测试请求继续标志
    // 错误信息（字符串长度限制）
    char function_name[64];
    char file_name[128];
    char condition[128];
    uint32_t line;
    // 文件末尾追加：所有测试用例的结果数组
    // vsf_test_file_case_result_t results[];
} vsf_test_file_data_t;

//! \brief 单个测试用例结果结构（追加在文件末尾）
typedef struct {
    uint32_t idx;           //!< 测试用例索引
    uint32_t result;         //!< 测试结果
    char name[64];           //!< 测试用例名称
} vsf_test_file_case_result_t;

VSF_CAL_WEAK(vsf_test_file_data_init)
void vsf_test_file_data_init(vsf_test_data_t *data)
{
    // 初始化时创建或打开文件
    // 如果文件不存在，创建一个空文件
    FILE *fp = fopen(VSF_TEST_DATA_FILE_PATH, "rb");
    if (fp == NULL) {
        // 文件不存在，创建新文件
        fp = fopen(VSF_TEST_DATA_FILE_PATH, "wb");
        if (fp != NULL) {
            vsf_test_file_data_t file_data = {0};
            fwrite(&file_data, sizeof(vsf_test_file_data_t), 1, fp);
            fclose(fp);
            __VSF_TEST_FILE_TRACE_INFO("[TEST] Data file initialized: %s (created)\r\n", VSF_TEST_DATA_FILE_PATH);
        } else {
            __VSF_TEST_FILE_TRACE_ERROR("[TEST] Failed to create data file: %s\r\n", VSF_TEST_DATA_FILE_PATH);
        }
    } else {
        fclose(fp);
        __VSF_TEST_FILE_TRACE_DEBUG("[TEST] Data file exists: %s\r\n", VSF_TEST_DATA_FILE_PATH);
    }
}

static void __vsf_test_file_read_data(vsf_test_data_t *data)
{
    FILE *fp = fopen(VSF_TEST_DATA_FILE_PATH, "rb");
    if (fp != NULL) {
        vsf_test_file_data_t file_data;
        if (fread(&file_data, sizeof(vsf_test_file_data_t), 1, fp) == 1) {
            // 只读取数值字段，字符串字段在写入时保存但不需要读回
            // 因为 data->error 中的字符串是指向常量的指针
            data->idx = file_data.idx;
            data->status = file_data.status;
            data->req_continue = file_data.req_continue;
            // 注意：error 结构中的字符串字段是指向常量的指针，不需要从文件恢复
        } else {
            __VSF_TEST_FILE_TRACE_ERROR("[TEST] Failed to read data from file\r\n");
        }
        fclose(fp);
    } else {
        __VSF_TEST_FILE_TRACE_ERROR("[TEST] Failed to open file for reading\r\n");
    }
}

//! \brief 提取测试名字（从 cfg_str 中）
static void __vsf_test_file_extract_name(const char *cfg_str, char *name, size_t name_size)
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

//! \brief 读取特定索引的测试用例结果（从文件末尾的结果数组中读取）
static bool __vsf_test_file_read_case_result(uint32_t idx, uint32_t *result, char *name, size_t name_size)
{
    FILE *fp = fopen(VSF_TEST_DATA_FILE_PATH, "rb");
    if (fp != NULL) {
        // 跳过文件头
        if (fseek(fp, sizeof(vsf_test_file_data_t), SEEK_SET) == 0) {
            // 读取文件末尾的结果数组
            vsf_test_file_case_result_t case_result;
            while (fread(&case_result, sizeof(vsf_test_file_case_result_t), 1, fp) == 1) {
                if (case_result.idx == idx) {
                    *result = case_result.result;
                    if (name != NULL && name_size > 0) {
                        strncpy(name, case_result.name, name_size - 1);
                        name[name_size - 1] = '\0';
                    }
                    fclose(fp);
                    return true;
                }
            }
        }
        fclose(fp);
    }
    return false;
}

static void __vsf_test_file_write_data(vsf_test_data_t *data)
{
    FILE *fp = fopen(VSF_TEST_DATA_FILE_PATH, "wb");
    if (fp != NULL) {
        vsf_test_file_data_t file_data = {0};
        file_data.idx = data->idx;
        file_data.status = data->status;
        file_data.req_continue = data->req_continue;

        // 复制字符串字段（带长度限制）
        if (data->error.function_name != NULL) {
            strncpy(file_data.function_name, data->error.function_name,
                    sizeof(file_data.function_name) - 1);
            file_data.function_name[sizeof(file_data.function_name) - 1] = '\0';
        }
        if (data->error.file_name != NULL) {
            strncpy(file_data.file_name, data->error.file_name,
                    sizeof(file_data.file_name) - 1);
            file_data.file_name[sizeof(file_data.file_name) - 1] = '\0';
        }
        if (data->error.condition != NULL) {
            strncpy(file_data.condition, data->error.condition,
                    sizeof(file_data.condition) - 1);
            file_data.condition[sizeof(file_data.condition) - 1] = '\0';
        }
        file_data.line = data->error.line;

        if (fwrite(&file_data, sizeof(vsf_test_file_data_t), 1, fp) != 1) {
            __VSF_TEST_FILE_TRACE_ERROR("[TEST] Failed to write data to file\r\n");
        }
        fclose(fp);
    } else {
        __VSF_TEST_FILE_TRACE_ERROR("[TEST] Failed to open file for writing\r\n");
    }
}

void vsf_test_file_data_sync(vsf_test_data_t *data, vsf_test_data_cmd_t index)
{
    VSF_ASSERT(data != NULL);

    switch (index) {
    case VSF_TEST_STATUS_READ:
        __vsf_test_file_read_data(data);
        break;
    case VSF_TEST_STATUS_WRITE:
        __vsf_test_file_write_data(data);
        break;

    case VSF_TEST_TESTCASE_INDEX_READ:
        __vsf_test_file_read_data(data);
        // 尝试从结果文件中读取对应索引的结果
        uint32_t case_result;
        if (__vsf_test_file_read_case_result(data->idx, &case_result, NULL, 0)) {
            data->result = case_result;
        } else {
            // 如果结果不存在，设置为默认值 PASS（0）
            data->result = VSF_TEST_RESULT_PASS;
        }
        break;
    case VSF_TEST_TESTCASE_INDEX_WRITE:
        __vsf_test_file_write_data(data);
        break;

    case VSF_TEST_TESECASE_REQUEST_WRITE:
        // 写入请求信息，然后读取响应
        if (data->request_str != NULL) {
            // 可以将请求字符串写入文件或日志
            // 这里简化处理，直接设置支持标志
            __vsf_test_file_write_data(data);
            __vsf_test_file_read_data(data);
            // 默认支持测试
            if (data->req_continue == 0) {
                data->req_continue = VSF_TEST_REQ_SUPPORT;
            }
        } else {
            data->req_continue = VSF_TEST_REQ_SUPPORT;
        }
        break;

    case VSF_TEST_TESTCASE_RESULT_WRITE:
        {
            // 注意：测试结果的输出应该在 vsf_test.c 中，而不是在 data port 中
            // 测试名字应该总是从 test case 数组获取，不应该和 data port 关联
            // 这里只负责保存测试结果到文件

            // 提取测试名字（从 request_str 中，因为 request_str 就是 cfg_str）
            char test_name[64] = "unknown";
            if (data->request_str != NULL) {
                __vsf_test_file_extract_name(data->request_str, test_name, sizeof(test_name));
            }

            // 将测试结果追加到文件末尾（更新或追加）
            FILE *fp = fopen(VSF_TEST_DATA_FILE_PATH, "rb+");
            if (fp != NULL) {
                vsf_test_file_case_result_t case_result = {
                    .idx = data->idx,
                    .result = data->result,
                };
                strncpy(case_result.name, test_name, sizeof(case_result.name) - 1);
                case_result.name[sizeof(case_result.name) - 1] = '\0';

                // 定位到文件末尾的结果数组开始位置
                fseek(fp, sizeof(vsf_test_file_data_t), SEEK_SET);

                // 尝试更新现有结果
                bool updated = false;
                vsf_test_file_case_result_t existing_result;
                long pos = ftell(fp);
                while (fread(&existing_result, sizeof(vsf_test_file_case_result_t), 1, fp) == 1) {
                    if (existing_result.idx == data->idx) {
                        fseek(fp, pos, SEEK_SET);
                        fwrite(&case_result, sizeof(vsf_test_file_case_result_t), 1, fp);
                        updated = true;
                        break;
                    }
                    pos = ftell(fp);
                }

                // 如果未找到，追加新结果
                if (!updated) {
                    fseek(fp, 0, SEEK_END);
                    fwrite(&case_result, sizeof(vsf_test_file_case_result_t), 1, fp);
                }
                fclose(fp);
            }
        }
        __vsf_test_file_write_data(data);
        break;

    case VSF_TEST_DONE:
        __VSF_TEST_FILE_TRACE_INFO("[TEST] All test cases completed\r\n");
        // 所有测试完成，可以写入完成标记或生成报告
        __vsf_test_file_write_data(data);
        break;

    default:
        break;
    }
}

//! \brief 公开函数：读取测试用例结果和名字
bool vsf_test_file_read_case_result(uint32_t idx, uint32_t *result,
                                     char *name, size_t name_size)
{
    return __vsf_test_file_read_case_result(idx, result, name, name_size);
}

#endif

