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

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_TEST == ENABLED && VSF_TEST_CFG_USE_STDIO_DATA_SYNC == ENABLED


void vsf_test_stdio_wdt_init(vsf_test_wdt_t *wdt, uint32_t timeout_ms)
{
    printf("external_watch_init %u\r\n", timeout_ms);
}

void vsf_test_stdio_wdt_feed(vsf_test_wdt_t *wdt)
{
    printf("external_watch_feed\r\n");
}

void vsf_test_stdio_reboot(void)
{
    printf("external_reboot\r\n");
}

VSF_CAL_WEAK(vsf_test_stdio_data_init)
void vsf_test_stdio_data_init(vsf_test_data_t *data)
{
    printf("external_data_init\r\n");
}

void vsf_test_stdio_data_sync(vsf_test_data_t *data, vsf_test_data_cmd_t index)
{
    VSF_ASSERT(data != NULL);

    switch (index) {
    case VSF_TEST_STATUS_READ:
        printf("testcase_status read\r\n");
        scanf("%u", &data->status);
        break;
    case VSF_TEST_STATUS_WRITE:
        printf("testcase_status write %u\r\n", data->status);
        break;

    case VSF_TEST_TESTCASE_INDEX_READ:
        printf("testcase_index read\r\n");
        scanf("%u", &data->idx);
        break;
    case VSF_TEST_TESTCASE_INDEX_WRITE:
        printf("testcase_index write %u\r\n", data->idx);
        break;

    case VSF_TEST_TESECASE_REQUEST_WRITE:
        if (data->request_str != NULL) {
            printf("testcase_request %s\r\n", data->request_str);
            scanf("%u", &data->req_continue);
        } else {
            data->req_continue = VSF_TEST_REQ_SUPPORT;
        }
        break;

    case VSF_TEST_TESTCASE_RESULT_WRITE:
        if ((data->error.function_name != NULL) &&
            (data->error.file_name != NULL)) {
            if (data->error.condition == NULL) {
                data->error.condition = "";
            }
            printf("testcase_result write %u %u %s:%u %s %s\r\n", data->idx,
                   data->result, data->error.file_name, data->error.line,
                   data->error.function_name, data->error.condition);
        } else {
            printf("testcase_result write %u %u\r\n", data->idx, data->result);
        }
        break;

    case VSF_TEST_DONE:
        printf("testcase_all_done\r\n");
        break;

    default:
        break;
    }
}

#endif
