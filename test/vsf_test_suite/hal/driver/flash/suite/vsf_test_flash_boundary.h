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
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef __TEST_FLASH_BOUNDARY_H__
#define __TEST_FLASH_BOUNDARY_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"
/*============================ MACROS ========================================*/

#ifndef VSF_TEST_FLASH_BOUNDARY_BUF_SIZE
#   define VSF_TEST_FLASH_BOUNDARY_BUF_SIZE        512
#endif
#ifndef VSF_TEST_FLASH_BOUNDARY_PATTERN_BYTE
#   define VSF_TEST_FLASH_BOUNDARY_PATTERN_BYTE               0xA5
#endif

#ifndef VSF_TEST_FLASH_BOUNDARY_PRIO
#   define VSF_TEST_FLASH_BOUNDARY_PRIO             vsf_arch_prio_1
#endif


#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
typedef struct {
    uint8_t write_buf[VSF_TEST_FLASH_BOUNDARY_BUF_SIZE];
    uint8_t read_buf[VSF_TEST_FLASH_BOUNDARY_BUF_SIZE];
} vsf_test_flash_boundary_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_flash_boundary_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint32_t offset;
    uint32_t size;
} vsf_test_flash_boundary_params_t;

struct vsf_test_flash_boundary_s;
extern const struct vsf_test_flash_boundary_s vsf_test_flash_boundary;

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED */

#endif /* __TEST_FLASH_BOUNDARY_H__ */
/* EOF */
