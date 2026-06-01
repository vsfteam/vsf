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

#include "vsf_test_i2c_eeprom_page.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
const struct vsf_test_i2c_eeprom_page_s {
    vsf_test_suite_t hdr;
    vsf_test_i2c_eeprom_page_params_t          params[VSF_TEST_I2C_EEPROM_PAGE_CASE_COUNT];
} vsf_test_i2c_eeprom_page = {
    .hdr = {
        .name            = "i2c_eeprom_page",
        .jmp_fn          = vsf_test_i2c_eeprom_page_run,
        .case_count      = VSF_TEST_I2C_EEPROM_PAGE_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_i2c_eeprom_page_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_I2C,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_I2C_EEPROM_PAGE_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED

typedef enum {
    I2C_POLL_COMPLETE,
    I2C_POLL_ERR,
    I2C_POLL_TIMEOUT,
} __i2c_poll_result_t;

static void __i2c_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                      vsf_i2c_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(i2c_ptr);
    vsf_test_suite_t *suite = target_ptr;
    vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask |= irq_mask;
}

static __i2c_poll_result_t __i2c_wait_result(const vsf_test_suite_t *suite,
                                              uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
            return I2C_POLL_COMPLETE;
        }
        if (vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask & VSF_I2C_IRQ_MASK_MASTER_ERR) {
            return I2C_POLL_ERR;
        }
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }
    return I2C_POLL_TIMEOUT;
}

static bool __i2c_wait_complete(const vsf_test_suite_t *suite, uint32_t timeout_ms)
{
    __i2c_poll_result_t result = __i2c_wait_result(suite, timeout_ms);
    if (result != I2C_POLL_COMPLETE) {
        VSF_TEST_TRACE_ERROR("i2c_eeprom_page:__i2c_wait_complete failed (result=%d) (irq_mask=0x%x)"
                             VSF_TRACE_CFG_LINEEND, (int)result,
                             (unsigned)vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask);
        return false;
    }
    return true;
}

static bool __eeprom_ack_poll(vsf_test_suite_t *suite,
                               vsf_i2c_t *i2c, uint8_t eeprom_addr,
                               uint8_t *dummy_buf, uint32_t max_ms)
{
    while (max_ms-- > 0) {
        vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask = 0;
        vsf_err_t err = vsf_i2c_master_request(i2c, eeprom_addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
            1, dummy_buf);
        VSF_TEST_ASSERT_ERR_NONE(err,
            "i2c_eeprom_page:__eeprom_ack_poll:vsf_i2c_master_request failed (err=%d) (addr=0x%02x)"
            VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)eeprom_addr);

        __i2c_poll_result_t result = __i2c_wait_result(suite, VSF_TEST_I2C_ACK_POLL_SUB_TIMEOUT_MS);
        if (result == I2C_POLL_COMPLETE) {
            return true;
        }
        if (result == I2C_POLL_TIMEOUT) {
            VSF_TEST_TRACE_ERROR("i2c_eeprom_page:__eeprom_ack_poll timeout (addr=0x%02x) (irq_mask=0x%x)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned)eeprom_addr,
                                 (unsigned)vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask);
            return false;
        }
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }
    return false;
}

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_i2c_eeprom_page_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_i2c_eeprom_page_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_I2C, i2c);
    vsf_i2c_t *i2c = inst->fixture.i2c;
    uint8_t data_len = p->data_len;
    uint8_t mem_addr = p->mem_addr;

    if (data_len == 0) {
        VSF_TEST_TRACE_ERROR("i2c_eeprom_page:data_len is 0 (data_len=%u mem_addr=0x%x)" VSF_TRACE_CFG_LINEEND, (unsigned)data_len, (unsigned)mem_addr);
    }
    VSF_TEST_ASSERT(data_len > 0);
    VSF_TEST_ASSERT(data_len <= VSF_TEST_I2C_CASE_MAX_COUNT);

    memset(vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf, 0, sizeof(vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf));
    memset(vsf_test_suite_data.i2c.i2c_eeprom_page.read_buf, 0, sizeof(vsf_test_suite_data.i2c.i2c_eeprom_page.read_buf));
    vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask = 0;

    /* Init i2c master at standard 100kHz. */
    vsf_err_t err = vsf_i2c_init(i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_TEST_I2C_EEPROM_MASTER_MODE,
        .clock_hz   = VSF_TEST_I2C_CLOCK_HZ,
        .isr        = {
            .handler_fn = __i2c_isr,
            .target_ptr = suite,
            .prio       = VSF_TEST_I2C_EEPROM_PAGE_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_eeprom_page:vsf_i2c_init failed (err=%d) (mode=0x%x clock_hz=%lu)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_EEPROM_MASTER_MODE,
        (unsigned long)VSF_TEST_I2C_CLOCK_HZ);
    while (fsm_rt_cpl != vsf_i2c_enable(i2c));
    vsf_i2c_irq_enable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_ERR | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    /* Build write buffer: [mem_addr, payload...]. */
    vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf[0] = mem_addr;
    for (uint8_t i = 0; i < data_len; i++) {
        vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf[1 + i] = (uint8_t)(VSF_TEST_I2C_EEPROM_PAGE_PATTERN_BYTE + i);
    }

    /* Phase 1: Write [mem_addr, payload] to EEPROM. */
    vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        data_len + 1, vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_eeprom_page:vsf_i2c_master_request(write) failed (err=%d) (addr=0x%02x count=%u)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)p->eeprom_addr, (unsigned)(data_len + 1));
    VSF_TEST_ASSERT(__i2c_wait_complete(suite, VSF_TEST_I2C_TIMEOUT_MS));

    /* Phase 1.5: ACK poll until EEPROM write cycle completes. */
    VSF_TEST_ASSERT(__eeprom_ack_poll(suite, i2c, p->eeprom_addr,
                                      &vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf[0], VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS));

    /* Phase 2a: Set memory address (write phase, no stop). */
    vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_NO_STOP | VSF_I2C_CMD_7_BITS,
        1, &vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf[0]);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_eeprom_page:vsf_i2c_master_request(addr) failed (err=%d) (addr=0x%02x)"
        VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->eeprom_addr);
    VSF_TEST_ASSERT(__i2c_wait_complete(suite, VSF_TEST_I2C_TIMEOUT_MS));

    /* Phase 2b: Restart, then read data_len bytes. */
    vsf_test_suite_data.i2c.i2c_eeprom_page.irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_RESTART | VSF_I2C_CMD_STOP | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS,
        data_len, vsf_test_suite_data.i2c.i2c_eeprom_page.read_buf);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_eeprom_page:vsf_i2c_master_request(read) failed (err=%d) (addr=0x%02x count=%u)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)p->eeprom_addr, (unsigned)data_len);
    VSF_TEST_ASSERT(__i2c_wait_complete(suite, VSF_TEST_I2C_TIMEOUT_MS));

    bool data_match = true;
    for (uint8_t i = 0; i < data_len; i++) {
        if (vsf_test_suite_data.i2c.i2c_eeprom_page.read_buf[i] != vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf[1 + i]) {
            VSF_TEST_TRACE_ERROR("i2c_eeprom_page:data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i,
                                 vsf_test_suite_data.i2c.i2c_eeprom_page.write_buf[1 + i],
                                 vsf_test_suite_data.i2c.i2c_eeprom_page.read_buf[i]);
            data_match = false;
            break;
        }
    }
    VSF_TEST_ASSERT(data_match);

    vsf_i2c_irq_disable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_ERR | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);
    while (fsm_rt_cpl != vsf_i2c_disable(i2c));
    vsf_i2c_fini(i2c);

    VSF_TEST_TRACE_INFO("i2c_eeprom_page:pass addr=0x%02x len=%u"
                   VSF_TRACE_CFG_LINEEND,
                   (unsigned)mem_addr, (unsigned)data_len);
}

#endif /* VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED */

/* EOF */