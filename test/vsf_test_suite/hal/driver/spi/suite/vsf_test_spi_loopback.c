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

#include "vsf_test_spi_loopback.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
const struct vsf_test_spi_loopback_s {
    vsf_test_suite_t hdr;
    vsf_test_spi_loopback_params_t          params[VSF_TEST_SPI_LOOPBACK_CASE_COUNT];
} vsf_test_spi_loopback = {
    .hdr = {
        .name            = "spi_loopback",
        .jmp_fn          = vsf_test_spi_loopback_run,
        .case_count      = VSF_TEST_SPI_LOOPBACK_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_spi_loopback_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_SPI,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_SPI_LOOPBACK_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#define VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN              256

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_spi_loopback_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_spi_loopback_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_SPI, spi);
    vsf_spi_t *spi = inst->fixture.spi;

    uint16_t data_len = p->data_len;
    if (data_len == 0 || data_len > VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN) {
        data_len = VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN;
    }

    vsf_err_t err = vsf_spi_init(spi, &(vsf_spi_cfg_t){
        .mode      = VSF_SPI_MASTER | p->mode | VSF_SPI_DATASIZE_8,
        .clock_hz  = p->clock_hz,
        .isr       = { NULL, NULL, VSF_TEST_SPI_LOOPBACK_PRIO },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "spi_loopback:vsf_spi_init failed (err=%d) (mode=0x%x clock_hz=%lu)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)(VSF_SPI_MASTER | p->mode | VSF_SPI_DATASIZE_8),
        (unsigned long)p->clock_hz);

    while (fsm_rt_cpl != vsf_spi_enable(spi));

    uint8_t tx_buf[VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN];
    uint8_t rx_buf[VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN] = {0};

    for (uint16_t i = 0; i < data_len; i++) {
        tx_buf[i] = (uint8_t)(VSF_TEST_SPI_LOOPBACK_PATTERN_BYTE + i);
    }

    /* Activate CS (software) */
    vsf_spi_cs_active(spi, 0);

    uint_fast32_t tx_offset = 0, rx_offset = 0;
    vsf_spi_fifo_transfer(spi, tx_buf, &tx_offset,
                          rx_buf, &rx_offset,
                          data_len);

    vsf_spi_cs_inactive(spi, 0);

    /* With MOSI-MISO loopback jumper, rx should match tx */
    bool match = true;
    for (uint16_t i = 0; i < data_len; i++) {
        if (rx_buf[i] != tx_buf[i]) {
            VSF_TEST_TRACE_ERROR("spi_loopback:data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i, tx_buf[i], rx_buf[i]);
            match = false;
            break;
        }
    }

    if (match) {
        VSF_TEST_TRACE_INFO("spi_loopback:pass" VSF_TRACE_CFG_LINEEND);
    }

    while (fsm_rt_cpl != vsf_spi_disable(spi));
    vsf_spi_fini(spi);
}

#endif /* VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED */
/* EOF */
