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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf_test_i2c_eeprom_rw_fifo.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
const struct vsf_test_i2c_eeprom_rw_fifo_s {
    vsf_test_suite_t hdr;
    vsf_test_i2c_eeprom_rw_fifo_params_t          params[VSF_TEST_I2C_EEPROM_RW_FIFO_CASE_COUNT];
} vsf_test_i2c_eeprom_rw_fifo = {
    .hdr = {
        .name            = "i2c_eeprom_rw_fifo",
        .jmp_fn          = vsf_test_i2c_eeprom_rw_fifo_run,
        .case_count      = VSF_TEST_I2C_EEPROM_RW_FIFO_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_i2c_eeprom_rw_fifo_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_I2C,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_I2C_EEPROM_RW_FIFO_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

static void __i2c_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                      vsf_i2c_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(i2c_ptr);
    vsf_test_suite_t *suite = target_ptr;
    vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask |= irq_mask;
}

static bool __wait_irq(const vsf_test_suite_t *suite,
                       vsf_i2c_irq_mask_t check_mask, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask & check_mask) return true;
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }
    VSF_TEST_TRACE_ERROR("i2c_eeprom_rw_fifo:__wait_irq timeout (check_mask=0x%x) (irq_mask=0x%x)"
                         VSF_TRACE_CFG_LINEEND, (unsigned)check_mask,
                         (unsigned)vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask);
    return false;
}

static bool __fifo_write(vsf_i2c_t *i2c, uint16_t addr, vsf_i2c_cmd_t cmd,
                          uint_fast16_t count, uint8_t *buf, uint32_t timeout_ms)
{
    vsf_i2c_cmd_t cur_cmd = 0;
    uint_fast16_t offset  = 0;
    fsm_rt_t result = vsf_i2c_master_fifo_transfer(i2c, addr, cmd,
        count, buf, &cur_cmd, &offset);
    while (result == fsm_rt_on_going && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
        result = vsf_i2c_master_fifo_transfer(i2c, addr, cmd,
            count, buf, &cur_cmd, &offset);
    }
    if (result != fsm_rt_cpl) {
        VSF_TEST_TRACE_ERROR("i2c_eeprom_rw_fifo:__fifo_write failed (result=%d addr=0x%02x count=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)result, (unsigned)addr, (unsigned)count);
    }
    return result == fsm_rt_cpl;
}

static bool __eeprom_ack_poll(vsf_test_suite_t *suite,
                               vsf_i2c_t *i2c, uint8_t eeprom_addr,
                               uint8_t *dummy_buf, uint32_t max_ms)
{
    while (max_ms-- > 0) {
        vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask = 0;
        vsf_err_t err = vsf_i2c_master_request(i2c, eeprom_addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
            1, dummy_buf);
        VSF_TEST_ASSERT_ERR_NONE(err,
            "i2c_eeprom_rw_fifo:__eeprom_ack_poll:vsf_i2c_master_request failed (err=%d) (addr=0x%02x)"
            VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)eeprom_addr);
        if (__wait_irq(suite,
            VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR, VSF_TEST_I2C_ACK_POLL_SUB_TIMEOUT_MS)) {
            if (vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
                return true;
            }
        }
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }
    VSF_TEST_TRACE_ERROR("i2c_eeprom_rw_fifo:__eeprom_ack_poll timeout (addr=0x%02x)"
                         VSF_TRACE_CFG_LINEEND, (unsigned)eeprom_addr);
    return false;
}

void vsf_test_i2c_eeprom_rw_fifo_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_i2c_eeprom_rw_fifo_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_I2C, i2c);
    vsf_i2c_t *i2c = inst->fixture.i2c;
    uint8_t data_len = p->data_len;

    if (data_len == 0) {
        VSF_TEST_TRACE_ERROR("i2c_eeprom_rw_fifo:data_len is 0 (data_len=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)data_len);
    }
    VSF_TEST_ASSERT(data_len > 0);
    VSF_TEST_ASSERT(data_len <= VSF_TEST_I2C_CASE_MAX_COUNT);

    memset(vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf, 0, sizeof(vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf));
    memset(vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.read_buf, 0, sizeof(vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.read_buf));
    vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask = 0;

    vsf_err_t err = vsf_i2c_init(i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_TEST_I2C_EEPROM_MASTER_MODE,
        .clock_hz   = VSF_TEST_I2C_CLOCK_HZ,
        .isr        = {
            .handler_fn = __i2c_isr,
            .target_ptr = suite,
            .prio       = VSF_TEST_I2C_EEPROM_RW_FIFO_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_eeprom_rw_fifo:vsf_i2c_init failed (err=%d) (mode=0x%x clock_hz=%lu)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_EEPROM_MASTER_MODE,
        (unsigned long)VSF_TEST_I2C_CLOCK_HZ);
    while (fsm_rt_cpl != vsf_i2c_enable(i2c));
    vsf_i2c_irq_enable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);

    vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf[0] = p->mem_addr;
    for (uint8_t i = 0; i < data_len; i++) {
        vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf[1 + i] = (uint8_t)(VSF_TEST_I2C_EEPROM_RW_FIFO_PATTERN_BYTE + i);
    }

    /* Phase 1: FIFO write [mem_addr, payload] to EEPROM.
     * Uses vsf_i2c_master_fifo_transfer() in polling mode.  This is the
     * key API under test — compare with vsf_i2c_master_request() in the
     * request-based i2c_eeprom_rw suite. */
    VSF_TEST_TRACE_DEBUG("i2c_eeprom_rw_fifo:__fifo_write (addr=0x%02x count=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->eeprom_addr, (unsigned)(data_len + 1));
    bool fifo_ok = __fifo_write(i2c, p->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        data_len + 1, vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf, VSF_TEST_I2C_FIFO_TIMEOUT_MS);
    if (!fifo_ok) {
        VSF_TEST_TRACE_ERROR("i2c_eeprom_rw_fifo:__fifo_write failed at phase1 (addr=0x%02x count=%u)"
                             VSF_TRACE_CFG_LINEEND, (unsigned)p->eeprom_addr, (unsigned)(data_len + 1));
    }
    VSF_TEST_ASSERT(fifo_ok);

    /* Phase 1.5: ACK poll until EEPROM write cycle completes. */
    VSF_TEST_ASSERT(__eeprom_ack_poll(suite, i2c, p->eeprom_addr,
                                      &vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf[0], VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS));

    /* Phase 2a: Set memory address (write phase, no stop). */
    vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_NO_STOP | VSF_I2C_CMD_7_BITS,
        1, &vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf[0]);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_eeprom_rw_fifo:vsf_i2c_master_request(addr) failed (err=%d) (addr=0x%02x)"
        VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->eeprom_addr);
    VSF_TEST_ASSERT(__wait_irq(suite,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR, VSF_TEST_I2C_FIFO_ADDR_TIMEOUT_MS));
    if (!(vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE)) {
        VSF_TEST_TRACE_ERROR("i2c_eeprom_rw_fifo:addr phase no MASTER_TRANSFER_COMPLETE (irq_mask=0x%x)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    /* Phase 2b: Read data back (request API). */
    vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_RESTART | VSF_I2C_CMD_STOP | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS,
        data_len, vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.read_buf);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_eeprom_rw_fifo:vsf_i2c_master_request(read) failed (err=%d) (addr=0x%02x count=%u)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)p->eeprom_addr, (unsigned)data_len);
    VSF_TEST_ASSERT(__wait_irq(suite,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE, VSF_TEST_I2C_FIFO_TIMEOUT_MS));

    bool data_match = true;
    for (uint8_t i = 0; i < data_len; i++) {
        if (vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.read_buf[i] != vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf[1 + i]) {
            VSF_TEST_TRACE_ERROR("i2c_eeprom_rw_fifo:data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i,
                                 vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.write_buf[1 + i],
                                 vsf_test_suite_data.i2c.i2c_eeprom_rw_fifo.read_buf[i]);
            data_match = false;
            break;
        }
    }
    VSF_TEST_ASSERT(data_match);

    vsf_i2c_irq_disable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);
    while (fsm_rt_cpl != vsf_i2c_disable(i2c));
    vsf_i2c_fini(i2c);

    VSF_TEST_TRACE_INFO("i2c_eeprom_rw_fifo:pass len=%u" VSF_TRACE_CFG_LINEEND,
                   (unsigned)data_len);
}

#endif /* VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED */

/* EOF */