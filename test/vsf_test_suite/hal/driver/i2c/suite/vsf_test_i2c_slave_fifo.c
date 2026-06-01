/*============================ INCLUDES ======================================*/

#include "vsf_test_i2c_slave_fifo.h"
#include "vsf_test_suites.h"
#include "vsf_board.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
const struct vsf_test_i2c_slave_fifo_s {
    vsf_test_suite_t hdr;
    vsf_test_i2c_slave_fifo_params_t          params[VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT];
} vsf_test_i2c_slave_fifo = {
    .hdr = {
        .name            = "i2c_slave_fifo",
        .jmp_fn          = vsf_test_i2c_slave_fifo_run,
        .case_count      = VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_i2c_slave_fifo_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_NONE,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_I2C_SLAVE_FIFO_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

static void __master_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                         vsf_i2c_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(i2c_ptr);
    vsf_test_suite_t *suite = target_ptr;
    vsf_test_suite_data.i2c.i2c_slave_fifo.master_irq_mask |= irq_mask;
    if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        vsf_test_suite_data.i2c.i2c_slave_fifo.master_done = true;
    }
}

static void __slave_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                        vsf_i2c_irq_mask_t irq_mask)
{
    vsf_test_suite_t *suite = target_ptr;
    vsf_test_suite_data.i2c.i2c_slave_fifo.slave_irq_mask |= irq_mask;

    /* Slave receive via fifo_transfer: read available bytes from RX FIFO. */
    if (irq_mask & VSF_I2C_IRQ_MASK_SLAVE_RX) {
        uint_fast16_t remaining = VSF_TEST_I2C_SLAVE_FIFO_SLAVE_BUF_SIZE - vsf_test_suite_data.i2c.i2c_slave_fifo.slave_rx_offset;
        uint_fast16_t got = vsf_i2c_slave_fifo_transfer(i2c_ptr, false,
            remaining, vsf_test_suite_data.i2c.i2c_slave_fifo.slave_buf + vsf_test_suite_data.i2c.i2c_slave_fifo.slave_rx_offset);
        vsf_test_suite_data.i2c.i2c_slave_fifo.slave_rx_offset += got;
    }
    if (irq_mask & (VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT)) {
        vsf_test_suite_data.i2c.i2c_slave_fifo.slave_complete = true;
    }
}

static bool __wait_master_done(vsf_test_suite_t *suite, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (vsf_test_suite_data.i2c.i2c_slave_fifo.master_done) return true;
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }
    VSF_TEST_TRACE_ERROR("i2c_slave_fifo:__wait_master_done timeout (master_irq_mask=0x%x)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)vsf_test_suite_data.i2c.i2c_slave_fifo.master_irq_mask);
    return false;
}

static bool __wait_slave_complete(vsf_test_suite_t *suite, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (vsf_test_suite_data.i2c.i2c_slave_fifo.slave_complete) return true;
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }
    VSF_TEST_TRACE_ERROR("i2c_slave_fifo:__wait_slave_complete timeout (slave_irq_mask=0x%x)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)vsf_test_suite_data.i2c.i2c_slave_fifo.slave_irq_mask);
    return false;
}

void vsf_test_i2c_slave_fifo_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_i2c_slave_fifo_params_t *p = tc->params;
    VSF_TEST_ASSERT(inst != NULL);
    VSF_TEST_ASSERT(inst->peripheral_type == VSF_PERIPHERAL_TYPE_I2C_SLAVE);
    VSF_TEST_ASSERT(inst->fixture.i2c_slave != NULL);
    VSF_TEST_ASSERT(inst->fixture.i2c_slave->master_i2c != NULL);
    VSF_TEST_ASSERT(inst->fixture.i2c_slave->slave_i2c != NULL);
    VSF_TEST_ASSERT(inst->name != NULL);
    const vsf_test_i2c_slave_ctx_t *ctx = inst->fixture.i2c_slave;
    vsf_i2c_t *master_i2c = ctx->master_i2c;
    vsf_i2c_t *slave_i2c  = ctx->slave_i2c;

    /* Zero all per-run state. */
    vsf_test_suite_data.i2c.i2c_slave_fifo.master_irq_mask = 0;
    vsf_test_suite_data.i2c.i2c_slave_fifo.slave_irq_mask  = 0;
    memset(vsf_test_suite_data.i2c.i2c_slave_fifo.master_buf, 0, sizeof(vsf_test_suite_data.i2c.i2c_slave_fifo.master_buf));
    memset(vsf_test_suite_data.i2c.i2c_slave_fifo.slave_buf, 0, sizeof(vsf_test_suite_data.i2c.i2c_slave_fifo.slave_buf));
    vsf_test_suite_data.i2c.i2c_slave_fifo.slave_rx_offset = 0;
    vsf_test_suite_data.i2c.i2c_slave_fifo.master_done     = false;
    vsf_test_suite_data.i2c.i2c_slave_fifo.slave_complete  = false;

    /* ---- Init slave (fifo-driven RX) ---- */
    vsf_err_t err = vsf_i2c_init(slave_i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_TEST_I2C_SLAVE_FIFO_MODE,
        .clock_hz   = VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ,
        .slave_addr = VSF_TEST_I2C_SLAVE_FIFO_ADDR,
        .isr        = {
            .handler_fn = __slave_isr,
            .target_ptr = suite,
            .prio       = VSF_TEST_I2C_SLAVE_FIFO_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave_fifo:vsf_i2c_init(slave) failed (err=%d) (mode=0x%x clock_hz=%lu addr=0x%02x)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_FIFO_MODE,
        (unsigned long)VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ,
        (unsigned)VSF_TEST_I2C_SLAVE_FIFO_ADDR);
    while (fsm_rt_cpl != vsf_i2c_enable(slave_i2c));
    vsf_i2c_irq_enable(slave_i2c,
        VSF_I2C_IRQ_MASK_SLAVE_RX
        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT);

    /* ---- Init master (request-driven) ---- */
    err = vsf_i2c_init(master_i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_TEST_I2C_SLAVE_FIFO_CHANNEL_MODE,
        .clock_hz   = VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ,
        .isr        = {
            .handler_fn = __master_isr,
            .target_ptr = suite,
            .prio       = VSF_TEST_I2C_SLAVE_FIFO_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave_fifo:vsf_i2c_init(master) failed (err=%d) (mode=0x%x clock_hz=%lu)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_FIFO_CHANNEL_MODE,
        (unsigned long)VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ);
    while (fsm_rt_cpl != vsf_i2c_enable(master_i2c));
    vsf_i2c_irq_enable(master_i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);

    /* ---- Slave receive via FIFO (master writes) ---- */
    for (uint8_t i = 0; i < VSF_TEST_I2C_SLAVE_FIFO_MASTER_BUF_SIZE; i++) {
        vsf_test_suite_data.i2c.i2c_slave_fifo.master_buf[i] = (uint8_t)(VSF_TEST_I2C_SLAVE_FIFO_PATTERN_BYTE + i);
        vsf_test_suite_data.i2c.i2c_slave_fifo.slave_buf[i] = 0;
    }
    vsf_test_suite_data.i2c.i2c_slave_fifo.master_done     = false;
    vsf_test_suite_data.i2c.i2c_slave_fifo.slave_complete  = false;
    vsf_test_suite_data.i2c.i2c_slave_fifo.slave_rx_offset = 0;

    err = vsf_i2c_master_request(master_i2c, VSF_TEST_I2C_SLAVE_FIFO_ADDR,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        16, vsf_test_suite_data.i2c.i2c_slave_fifo.master_buf);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave_fifo:vsf_i2c_master_request failed (err=%d) (addr=0x%02x count=%u)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_FIFO_ADDR, (unsigned)16);

    VSF_TEST_ASSERT(__wait_master_done(suite, VSF_TEST_I2C_SLAVE_FIFO_TIMEOUT_MS));
    VSF_TEST_ASSERT(__wait_slave_complete(suite, VSF_TEST_I2C_SLAVE_FIFO_TIMEOUT_MS));

    bool data_match = true;
    for (uint8_t i = 0; i < VSF_TEST_I2C_SLAVE_FIFO_MASTER_BUF_SIZE; i++) {
        if (vsf_test_suite_data.i2c.i2c_slave_fifo.slave_buf[i] != vsf_test_suite_data.i2c.i2c_slave_fifo.master_buf[i]) {
            VSF_TEST_TRACE_ERROR("i2c_slave_fifo:data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i,
                                 vsf_test_suite_data.i2c.i2c_slave_fifo.master_buf[i],
                                 vsf_test_suite_data.i2c.i2c_slave_fifo.slave_buf[i]);
            data_match = false;
            break;
        }
    }
    VSF_TEST_ASSERT(data_match);

    VSF_TEST_TRACE_INFO("i2c_slave_fifo:rx:pass" VSF_TRACE_CFG_LINEEND);

    /* ---- Cleanup ---- */
    vsf_i2c_irq_disable(master_i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);
    vsf_i2c_irq_disable(slave_i2c,
        VSF_I2C_IRQ_MASK_SLAVE_RX
        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT);

    while (fsm_rt_cpl != vsf_i2c_disable(master_i2c));
    while (fsm_rt_cpl != vsf_i2c_disable(slave_i2c));
    vsf_i2c_fini(master_i2c);
    vsf_i2c_fini(slave_i2c);
}

#endif /* VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED */

/* EOF */
