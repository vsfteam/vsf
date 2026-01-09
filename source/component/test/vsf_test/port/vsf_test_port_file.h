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

#ifndef __VSF_TEST_PORT_FILE_H__
#    define __VSF_TEST_PORT_FILE_H__

#    if VSF_USE_TEST == ENABLED && VSF_TEST_CFG_USE_FILE_DATA_SYNC == ENABLED

/*============================ MACROS ========================================*/

//! \brief 默认测试数据文件路径
#ifndef VSF_TEST_DATA_FILE_PATH
#   define VSF_TEST_DATA_FILE_PATH        "/root/test_data.bin"
#endif

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/**
 @brief Initialize the file for data synchronization
 @param[in] data: a pointer to structure @ref vsf_test_data_t

 @note Its default implementation is an empty weak function, which the user can
 re-implement as needed. After initialization, we need to make sure that file
 operations can be used properly.
 */
extern void vsf_test_file_data_init(vsf_test_data_t *data);

/**
 @brief Synchronize data via standard file interface (fopen, fread, fwrite, fclose).
 @param[in] data: a pointer to structure @ref vsf_test_data_t
 @param[in] cmd: structure @ref vsf_test_data_cmd_t

 @note This is a synchronized blocking API using file I/O.
 */
extern void vsf_test_file_data_sync(vsf_test_data_t    *data,
                                    vsf_test_data_cmd_t cmd);

/**
 @brief Read test case result and name from file.
 @param[in] idx: test case index
 @param[out] result: test result
 @param[out] name: test case name buffer (can be NULL)
 @param[in] name_size: size of name buffer

 @return true if found, false otherwise
 */
extern bool vsf_test_file_read_case_result(uint32_t idx, uint32_t *result,
                                           char *name, size_t name_size);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ===============================*/

#    endif

#endif /* __VSF_TEST_PORT_FILE_H__ */
/* EOF */

