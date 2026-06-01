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

/*============================ INCLUDES ======================================*/

#include "vsf_test_flash_boundary.h"

#include <string.h>
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
const struct vsf_test_flash_boundary_s {
    vsf_test_suite_t hdr;
    vsf_test_flash_boundary_params_t          params[VSF_TEST_FLASH_BOUNDARY_CASE_COUNT];
} vsf_test_flash_boundary = {
    .hdr = {
        .name            = "flash_boundary",
        .jmp_fn          = vsf_test_flash_boundary_run,
        .case_count      = VSF_TEST_FLASH_BOUNDARY_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_flash_boundary_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_FLASH,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_FLASH_BOUNDARY_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED

/*============================ LOCAL VARIABLES ===============================*/


/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_flash_boundary_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_flash_boundary_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_FLASH, flash);
    vsf_flash_t *flash = inst->fixture.flash;
    uint32_t offset = p->offset;
    uint32_t size = p->size;

    VSF_TEST_TRACE_INFO("flash_boundary:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    if (size == 0) {
        VSF_TEST_TRACE_ERROR("flash_boundary:invalid size=%u" VSF_TRACE_CFG_LINEEND,
                              (unsigned)size);
    }
    VSF_TEST_ASSERT(size > 0);

    vsf_flash_capability_t cap = vsf_flash_capability(flash);
    if (cap.erase_sector_size == 0) {
        VSF_TEST_TRACE_ERROR("flash_boundary:erase_sector_size is 0 (value=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)cap.erase_sector_size);
    }
    VSF_TEST_ASSERT(cap.erase_sector_size > 0);
    if (cap.write_sector_size == 0) {
        VSF_TEST_TRACE_ERROR("flash_boundary:write_sector_size is 0 (value=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)cap.write_sector_size);
    }
    VSF_TEST_ASSERT(cap.write_sector_size > 0);

    /* Ensure the write crosses at least one page boundary. */
    uint32_t page_mask = cap.write_sector_size - 1;
    uint32_t start_page = offset & ~page_mask;
    uint32_t end_page   = (offset + size - 1) & ~page_mask;
    VSF_TEST_ASSERT(start_page != end_page);

    /* Align to sector for erase. */
    uint32_t erase_offset = offset & ~(cap.erase_sector_size - 1);
    uint32_t erase_end    = (offset + size + cap.erase_sector_size - 1)
                          & ~(cap.erase_sector_size - 1);
    uint32_t erase_size   = erase_end - erase_offset;
    if (erase_size == 0) {
        erase_size = cap.erase_sector_size;
    }

    VSF_TEST_TRACE_DEBUG("flash_boundary:init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_flash_init(flash, &(vsf_flash_cfg_t){
        .isr = {
            .handler_fn = NULL,
            .target_ptr = NULL,
            .prio       = VSF_TEST_FLASH_BOUNDARY_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "flash_boundary:vsf_flash_init failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);
    while (fsm_rt_cpl != vsf_flash_enable(flash));

    /* Prepare test pattern. */
    VSF_TEST_TRACE_DEBUG("flash_boundary:prepare pattern offset=%lu size=%lu"
                          VSF_TRACE_CFG_LINEEND,
                          (unsigned long)offset, (unsigned long)size);
    for (uint32_t i = 0; i < size; i++) {
        vsf_test_suite_data.flash.flash_boundary.write_buf[i] = (uint8_t)(VSF_TEST_FLASH_BOUNDARY_PATTERN_BYTE + i);
    }

    /* Phase 1: Erase. */
    VSF_TEST_TRACE_DEBUG("flash_boundary:erase offset=%lu size=%lu"
                          VSF_TRACE_CFG_LINEEND,
                          (unsigned long)erase_offset, (unsigned long)erase_size);
    err = vsf_flash_erase_multi_sector(flash, erase_offset, erase_size);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "flash_boundary:vsf_flash_erase_multi_sector failed (err=%d) (offset=%lu size=%lu)"
                              VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)erase_offset, (unsigned long)erase_size);

    /* Phase 2: Program across page boundary. */
    VSF_TEST_TRACE_DEBUG("flash_boundary:program offset=%lu size=%lu"
                          VSF_TRACE_CFG_LINEEND,
                          (unsigned long)offset, (unsigned long)size);
    err = vsf_flash_write_multi_sector(flash, offset, vsf_test_suite_data.flash.flash_boundary.write_buf, size);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "flash_boundary:vsf_flash_write_multi_sector failed (err=%d) (offset=%lu size=%lu)"
                              VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)offset, (unsigned long)size);

    /* Phase 3: Read back and verify. */
    VSF_TEST_TRACE_DEBUG("flash_boundary:read offset=%lu size=%lu"
                          VSF_TRACE_CFG_LINEEND,
                          (unsigned long)offset, (unsigned long)size);
    err = vsf_flash_read_multi_sector(flash, offset, vsf_test_suite_data.flash.flash_boundary.read_buf, size);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "flash_boundary:vsf_flash_read_multi_sector failed (err=%d) (offset=%lu size=%lu)"
                              VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)offset, (unsigned long)size);
    for (uint32_t i = 0; i < size; i++) {
        if (vsf_test_suite_data.flash.flash_boundary.read_buf[i] != vsf_test_suite_data.flash.flash_boundary.write_buf[i]) {
            VSF_TEST_TRACE_ERROR("flash_boundary:data mismatch at offset=%lu (expected=0x%02x actual=0x%02x)"
                                  VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)i,
                                  vsf_test_suite_data.flash.flash_boundary.write_buf[i],
                                  vsf_test_suite_data.flash.flash_boundary.read_buf[i]);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.flash.flash_boundary.read_buf[i] == vsf_test_suite_data.flash.flash_boundary.write_buf[i]);
    }

    VSF_TEST_TRACE_INFO("flash_boundary:pass offset=%lu size=%lu pages=%lu-%lu"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned long)offset, (unsigned long)size,
                        (unsigned long)(start_page / cap.write_sector_size),
                        (unsigned long)(end_page / cap.write_sector_size));

    while (fsm_rt_cpl != vsf_flash_disable(flash));
    vsf_flash_fini(flash);
}

#endif /* VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED */

/* EOF */

