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

#ifndef __VSF_TEST_DMA_MEM2MEM_H__
#define __VSF_TEST_DMA_MEM2MEM_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_DMA_MEM2MEM_CASE_COUNT
#   define VSF_TEST_DMA_MEM2MEM_CASE_COUNT     1
#endif
#ifndef VSF_TEST_DMA_MEM2MEM_MODE
#   define VSF_TEST_DMA_MEM2MEM_MODE                          (VSF_DMA_MEMORY_TO_MEMORY | VSF_DMA_SRC_ADDR_INCREMENT | VSF_DMA_DST_ADDR_INCREMENT)
#endif

#ifndef VSF_TEST_DMA_MEM2MEM_PATTERN_BYTE
#   define VSF_TEST_DMA_MEM2MEM_PATTERN_BYTE                  0xA5
#endif

#ifndef VSF_TEST_DMA_MEM2MEM_SPIN_COUNT
#   define VSF_TEST_DMA_MEM2MEM_SPIN_COUNT          10000
#endif

#ifndef VSF_TEST_DMA_MEM2MEM_PRIO
#   define VSF_TEST_DMA_MEM2MEM_PRIO                vsf_arch_prio_invalid
#endif


#ifndef VSF_TEST_DMA_MEM2MEM_BUF_SIZE
#   define VSF_TEST_DMA_MEM2MEM_BUF_SIZE       32
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_dma_mem2mem_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    bool     expect_pass;
} vsf_test_dma_mem2mem_params_t;

struct vsf_test_dma_mem2mem_s;
extern const struct vsf_test_dma_mem2mem_s vsf_test_dma_mem2mem;

#endif /* __VSF_TEST_DMA_MEM2MEM_H__ */
/* EOF */
