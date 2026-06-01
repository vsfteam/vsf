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

#include "vsf_test_usart_mode.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_MODE_ENABLE == ENABLED
const struct vsf_test_usart_mode_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_mode_params_t          params[VSF_TEST_USART_MODE_CASE_COUNT];
} vsf_test_usart_mode = {
    .hdr = {
        .name            = "usart_mode",
        .jmp_fn          = vsf_test_usart_mode_run,
        .case_count      = VSF_TEST_USART_MODE_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_mode_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_MODE_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

/*============================ LOCAL FUNCTIONS ===============================*/

static void __usart_send_str(vsf_usart_t *usart, const char *str)
{
    while (*str) {
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, (uint8_t *)str, 1);
        str++;
    }
}

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_mode_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_mode_params_t *p = tc->params;

    VSF_TEST_TRACE_INFO("usart_mode:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_mode:init (mode=0x%x)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->mode);
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_MODE_DEFAULT_BAUDRATE,
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_mode:vsf_usart_init failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);
        VSF_TEST_TRACE_DEBUG("usart_mode:enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));
        VSF_TEST_TRACE_DEBUG("usart_mode:send_str" VSF_TRACE_CFG_LINEEND);
        __usart_send_str(inst->fixture.usart, VSF_TEST_MODE_PAYLOAD);
        vsf_test_busy_wait_ms(VSF_TEST_MODE_PAYLOAD_DRAIN_MS);
        VSF_TEST_TRACE_DEBUG("usart_mode:disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_mode:init should have failed (mode=0x%x)" VSF_TRACE_CFG_LINEEND,
                                 (unsigned)p->mode);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_mode:fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_mode:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_USART_TX_MODE_ENABLE == ENABLED */

/* EOF */


