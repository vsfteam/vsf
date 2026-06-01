/*============================ INCLUDES ======================================*/

#include "vsf_test_i2c_slave.h"
#include "vsf_test_suites.h"
#include "vsf_board.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
const struct vsf_test_i2c_slave_s {
    vsf_test_suite_t hdr;
    vsf_test_i2c_slave_params_t          params[VSF_TEST_I2C_SLAVE_CASE_COUNT];
} vsf_test_i2c_slave = {
    .hdr = {
        .name            = "i2c_slave",
        .jmp_fn          = vsf_test_i2c_slave_run,
        .case_count      = VSF_TEST_I2C_SLAVE_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_i2c_slave_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_NONE,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_I2C_SLAVE_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED

typedef struct {
    volatile vsf_i2c_irq_mask_t master_irq_mask;
    volatile vsf_i2c_irq_mask_t slave_irq_mask;
    uint8_t master_buf[VSF_TEST_I2C_SLAVE_MASTER_BUF_SIZE];
    uint8_t slave_buf[VSF_TEST_I2C_SLAVE_SLAVE_BUF_SIZE];
} __i2c_slave_state_t;

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

static void __master_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                         vsf_i2c_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(i2c_ptr);
    __i2c_slave_state_t *st = (__i2c_slave_state_t *)target_ptr;
    st->master_irq_mask |= irq_mask;
}

static void __slave_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                        vsf_i2c_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(i2c_ptr);
    __i2c_slave_state_t *st = (__i2c_slave_state_t *)target_ptr;
    st->slave_irq_mask |= irq_mask;
}

static bool __wait_master_complete(__i2c_slave_state_t *st, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (st->master_irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
            return true;
        }
        if (st->master_irq_mask & (VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                                    | VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT
                                    | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST)) {
            VSF_TEST_TRACE_ERROR("i2c_slave:__wait_master_complete failed (master_irq_mask=0x%x)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned)st->master_irq_mask);
            return false;
        }
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }
    VSF_TEST_TRACE_ERROR("i2c_slave:__wait_master_complete timeout (master_irq_mask=0x%x)"
                         VSF_TRACE_CFG_LINEEND, (unsigned)st->master_irq_mask);
    return false;
}

static bool __wait_slave_complete(__i2c_slave_state_t *st, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (st->slave_irq_mask & VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE) {
            return true;
        }
        if (st->slave_irq_mask & VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT) {
            return true;
        }
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }
    VSF_TEST_TRACE_ERROR("i2c_slave:__wait_slave_complete timeout (slave_irq_mask=0x%x)"
                         VSF_TRACE_CFG_LINEEND, (unsigned)st->slave_irq_mask);
    return false;
}

void vsf_test_i2c_slave_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_i2c_slave_params_t *p = tc->params;
    VSF_TEST_ASSERT(inst != NULL);
    VSF_TEST_ASSERT(inst->peripheral_type == VSF_PERIPHERAL_TYPE_I2C_SLAVE);
    VSF_TEST_ASSERT(inst->fixture.i2c_slave != NULL);
    VSF_TEST_ASSERT(inst->fixture.i2c_slave->master_i2c != NULL);
    VSF_TEST_ASSERT(inst->fixture.i2c_slave->slave_i2c != NULL);
    VSF_TEST_ASSERT(inst->name != NULL);
    const vsf_test_i2c_slave_ctx_t *ctx = inst->fixture.i2c_slave;
    vsf_i2c_t *master_i2c = ctx->master_i2c;
    vsf_i2c_t *slave_i2c  = ctx->slave_i2c;
    __i2c_slave_state_t *st = &vsf_test_suite_data.i2c.i2c_slave.i2c_slave_state;

    memset(st, 0, sizeof(*st));

    /* ---- Init slave first ---- */
    vsf_err_t err = vsf_i2c_init(slave_i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_TEST_I2C_SLAVE_MODE,
        .clock_hz   = VSF_TEST_I2C_SLAVE_CLOCK_HZ,
        .slave_addr = VSF_TEST_I2C_SLAVE_ADDR,
        .isr        = {
            .handler_fn = __slave_isr,
            .target_ptr = st,
            .prio       = VSF_TEST_I2C_SLAVE_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave:vsf_i2c_init(slave) failed (err=%d) (mode=0x%x clock_hz=%lu addr=0x%02x)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_MODE,
        (unsigned long)VSF_TEST_I2C_SLAVE_CLOCK_HZ,
        (unsigned)VSF_TEST_I2C_SLAVE_ADDR);
    while (fsm_rt_cpl != vsf_i2c_enable(slave_i2c));
    vsf_i2c_irq_enable(slave_i2c,
        VSF_I2C_IRQ_MASK_SLAVE_RX | VSF_I2C_IRQ_MASK_SLAVE_TX
        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT);

    /* ---- Init master ---- */
    err = vsf_i2c_init(master_i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_TEST_I2C_SLAVE_CHANNEL_MODE,
        .clock_hz   = VSF_TEST_I2C_SLAVE_CLOCK_HZ,
        .isr        = {
            .handler_fn = __master_isr,
            .target_ptr = st,
            .prio       = VSF_TEST_I2C_SLAVE_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave:vsf_i2c_init(master) failed (err=%d) (mode=0x%x clock_hz=%lu)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_CHANNEL_MODE,
        (unsigned long)VSF_TEST_I2C_SLAVE_CLOCK_HZ);
    while (fsm_rt_cpl != vsf_i2c_enable(master_i2c));
    vsf_i2c_irq_enable(master_i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);

    /* ---- Case: Slave receive (master writes) ---- */
    for (uint8_t i = 0; i < VSF_TEST_I2C_SLAVE_MASTER_BUF_SIZE; i++) {
        st->master_buf[i] = (uint8_t)(VSF_TEST_I2C_SLAVE_PATTERN_BYTE + i);
        st->slave_buf[i] = 0;
    }

    st->master_irq_mask = 0;
    st->slave_irq_mask  = 0;

    /* Slave prepares to receive */
    err = vsf_i2c_slave_request(slave_i2c, false, VSF_TEST_I2C_SLAVE_SLAVE_BUF_SIZE, st->slave_buf);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave:vsf_i2c_slave_request(rx) failed (err=%d) (size=%u)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_SLAVE_BUF_SIZE);

    /* Small delay to ensure slave is ready */
    vsf_test_busy_wait_ms(VSF_TEST_I2C_SLAVE_BUSY_WAIT_MS);

    /* Master writes to slave address */
    err = vsf_i2c_master_request(master_i2c, VSF_TEST_I2C_SLAVE_ADDR,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        16, st->master_buf);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave:vsf_i2c_master_request(tx) failed (err=%d) (addr=0x%02x count=%u)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_ADDR, (unsigned)16);

    VSF_TEST_ASSERT(__wait_master_complete(st, VSF_TEST_I2C_SLAVE_TIMEOUT_MS));
    VSF_TEST_ASSERT(__wait_slave_complete(st, VSF_TEST_I2C_SLAVE_TIMEOUT_MS));

    for (uint8_t i = 0; i < VSF_TEST_I2C_SLAVE_MASTER_BUF_SIZE; i++) {
        if (st->slave_buf[i] != st->master_buf[i]) {
            VSF_TEST_TRACE_ERROR("i2c_slave:rx data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i, st->master_buf[i], st->slave_buf[i]);
            VSF_TEST_ASSERT(false);
        }
    }

    VSF_TEST_TRACE_INFO("i2c_slave:rx:pass" VSF_TRACE_CFG_LINEEND);

    /* ---- Case: Slave transmit (master reads) ---- */
    for (uint8_t i = 0; i < VSF_TEST_I2C_SLAVE_MASTER_BUF_SIZE; i++) {
        st->slave_buf[i] = (uint8_t)(VSF_TEST_I2C_SLAVE_SLAVE_PATTERN_BYTE + i);
        st->master_buf[i] = 0;
    }

    st->master_irq_mask = 0;
    st->slave_irq_mask  = 0;

    /* Slave prepares to transmit */
    err = vsf_i2c_slave_request(slave_i2c, true, VSF_TEST_I2C_SLAVE_SLAVE_BUF_SIZE, st->slave_buf);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave:vsf_i2c_slave_request(tx) failed (err=%d) (size=%u)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_SLAVE_BUF_SIZE);

    vsf_test_busy_wait_ms(VSF_TEST_I2C_SLAVE_BUSY_WAIT_MS);

    /* Master reads from slave address */
    err = vsf_i2c_master_request(master_i2c, VSF_TEST_I2C_SLAVE_ADDR,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS,
        16, st->master_buf);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "i2c_slave:vsf_i2c_master_request(rx) failed (err=%d) (addr=0x%02x count=%u)"
        VSF_TRACE_CFG_LINEEND, (int)err,
        (unsigned)VSF_TEST_I2C_SLAVE_ADDR, (unsigned)16);

    VSF_TEST_ASSERT(__wait_master_complete(st, VSF_TEST_I2C_SLAVE_TIMEOUT_MS));
    VSF_TEST_ASSERT(__wait_slave_complete(st, VSF_TEST_I2C_SLAVE_TIMEOUT_MS));

    for (uint8_t i = 0; i < VSF_TEST_I2C_SLAVE_MASTER_BUF_SIZE; i++) {
        if (st->master_buf[i] != st->slave_buf[i]) {
            VSF_TEST_TRACE_ERROR("i2c_slave:tx data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i, st->slave_buf[i], st->master_buf[i]);
            VSF_TEST_ASSERT(false);
        }
    }

    VSF_TEST_TRACE_INFO("i2c_slave:tx:pass" VSF_TRACE_CFG_LINEEND);

    /* ---- Cleanup ---- */
    vsf_i2c_irq_disable(master_i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);
    vsf_i2c_irq_disable(slave_i2c,
        VSF_I2C_IRQ_MASK_SLAVE_RX | VSF_I2C_IRQ_MASK_SLAVE_TX
        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT);

    while (fsm_rt_cpl != vsf_i2c_disable(master_i2c));
    while (fsm_rt_cpl != vsf_i2c_disable(slave_i2c));
    vsf_i2c_fini(master_i2c);
    vsf_i2c_fini(slave_i2c);
}

#endif /* VSF_TEST_I2C_SLAVE_ENABLE == ENABLED */

/* EOF */
