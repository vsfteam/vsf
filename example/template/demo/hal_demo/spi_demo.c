/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_SPI_DEMO == ENABLED && VSF_HAL_USE_SPI == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_SPI_DEMO_CFG_SPI_PREFIX
#   undef VSF_SPI_CFG_PREFIX
#   define VSF_SPI_CFG_PREFIX                           APP_SPI_DEMO_CFG_SPI_PREFIX
#endif

#ifndef APP_SPI_DEMO_CFG_DEVICES_COUNT
#   define APP_SPI_DEMO_CFG_DEVICES_COUNT               1
#endif

#ifndef APP_SPI_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_SPI_DEMO_CFG_DEVICES_ARRAY_INIT          \
        { .cnt = dimof(vsf_hw_spi_devices), .devices = vsf_hw_spi_devices},
#endif

#ifndef APP_SPI_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_SPI_DEMO_CFG_DEFAULT_INSTANCE            vsf_hw_spi0
#endif

#ifndef APP_SPI_DEMO_CFG_DEFAULT_MODE
#   define APP_SPI_DEMO_CFG_DEFAULT_MODE                (VSF_SPI_MASTER | VSF_SPI_MODE_3 | VSF_SPI_MSB_FIRST | VSF_SPI_DATASIZE_8 | VSF_SPI_CS_SOFTWARE_MODE)
#endif

#ifndef APP_SPI_DEMO_CFG_DEFAULT_CLOCK
#   define APP_SPI_DEMO_CFG_DEFAULT_CLOCK               (1ul * 1000ul * 1000ul)
#endif

#ifndef APP_SPI_DEMO_CFG_DEFAULT_BUFFER_CNT
#   define APP_SPI_DEMO_CFG_DEFAULT_BUFFER_CNT         (1024)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum spi_method_t {
    METHOD_FIFO            = 0x00 << 0,
    METHOD_REQUEST         = 0x01 << 0,
    METHOD_TRANSFER_MASK   = METHOD_FIFO | METHOD_REQUEST,

    METHOD_SPI_POLL        = 0x00 << 1,
    METHOD_SPI_ISR         = 0x01 << 1,
    METHOD_IRQ_MASK        = METHOD_SPI_POLL | METHOD_SPI_ISR,

    METHOD_SEND_ONLY       = 0x00 << 2,
    METHOD_RECV_ONLY       = 0x01 << 2,
    METHOD_SEND_RECV       = 0x02 << 2,
    METHOD_SEND_RECV_MASK  = 0x03 << 2,
} spi_method_t;

typedef enum spi_demo_evt_t {
    VSF_EVT_SPI_FIFO_POLL = __VSF_EVT_HAL_LAST,
    VSF_EVT_SPI_TX_FIFO_WAIT_IDLE,

    VSF_EVT_SPI_CHECK,
} spi_demo_evt_t;

typedef struct spi_test_t  {
    implement(hal_test_t)
    // for vsf_spi_init
    vsf_spi_cfg_t cfg;
    // for vsf_spi_cs_active() and vsf_spi_cs_inactive()
    uint_fast8_t cs_index;
    bool cs_less;
    bool cs_manual;
    // for vsf_spi_fifo_transfer()
    uint_fast32_t fifo_send_cnt;
    uint_fast32_t fifo_recv_cnt;
    // for vsf_spi_fifo_transfer() and vsf_spi_request_transfer()
    uint_fast32_t cnt;
} spi_test_t;

typedef struct spi_demo_const_t {
    implement(hal_demo_const_t)
    spi_test_t test;
} spi_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static void __spi_isr_handler(void *target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask);
static bool __spi_fifo_transfer(spi_test_t *test);

/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __mode_options[] = {
    HAL_DEMO_OPTION(VSF_SPI_MASTER, VSF_SPI_MASTER),
    HAL_DEMO_OPTION(VSF_SPI_MASTER, VSF_SPI_SLAVE),

    HAL_DEMO_OPTION(VSF_SPI_BIT_ORDER_MASK, VSF_SPI_MSB_FIRST),
    HAL_DEMO_OPTION(VSF_SPI_BIT_ORDER_MASK, VSF_SPI_LSB_FIRST),

    HAL_DEMO_OPTION(VSF_SPI_MODE_MASK, VSF_SPI_MODE_0),
    HAL_DEMO_OPTION(VSF_SPI_MODE_MASK, VSF_SPI_MODE_1),
    HAL_DEMO_OPTION(VSF_SPI_MODE_MASK, VSF_SPI_MODE_2),
    HAL_DEMO_OPTION(VSF_SPI_MODE_MASK, VSF_SPI_MODE_3),

    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_4),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_4),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_5),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_6),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_7),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_8),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_9),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_10),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_11),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_12),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_13),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_14),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_15),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_16),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_17),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_18),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_19),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_20),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_21),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_22),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_23),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_24),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_25),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_26),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_27),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_28),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_29),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_30),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_31),
    HAL_DEMO_OPTION(VSF_SPI_DATASIZE_MASK, VSF_SPI_DATASIZE_32),

    HAL_DEMO_OPTION(VSF_SPI_CS_MODE_MASK, VSF_SPI_CS_SOFTWARE_MODE),
    HAL_DEMO_OPTION(VSF_SPI_CS_MODE_MASK, VSF_SPI_CS_HARDWARE_MODE),
};

static const hal_option_t __irq_options[] = {
    HAL_DEMO_OPTION(VSF_SPI_IRQ_MASK_TX, VSF_SPI_IRQ_MASK_TX),
    HAL_DEMO_OPTION(VSF_SPI_IRQ_MASK_RX, VSF_SPI_IRQ_MASK_RX),
    HAL_DEMO_OPTION(VSF_SPI_IRQ_MASK_TX_CPL, VSF_SPI_IRQ_MASK_TX_CPL),
    HAL_DEMO_OPTION(VSF_SPI_IRQ_MASK_CPL, VSF_SPI_IRQ_MASK_CPL),
    HAL_DEMO_OPTION(VSF_SPI_IRQ_MASK_ERROR, VSF_SPI_IRQ_MASK_ERROR),
};

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_TRANSFER_MASK, METHOD_FIFO, "fifo"),
    HAL_DEMO_OPTION_EX(METHOD_TRANSFER_MASK, METHOD_REQUEST, "req"),
    HAL_DEMO_OPTION_EX(METHOD_IRQ_MASK, METHOD_SPI_POLL, "poll"),
    HAL_DEMO_OPTION_EX(METHOD_IRQ_MASK, METHOD_SPI_ISR, "isr"),

    HAL_DEMO_OPTION_EX(METHOD_SEND_RECV_MASK, METHOD_SEND_ONLY, "send-only"),
    HAL_DEMO_OPTION_EX(METHOD_SEND_RECV_MASK, METHOD_RECV_ONLY, "recv-only"),
    HAL_DEMO_OPTION_EX(METHOD_SEND_RECV_MASK, METHOD_SEND_RECV, "send-recv"),
};

HAL_DEMO_INIT(spi, APP_SPI,
    "spi-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [[fifo|req] [poll|isr] [send-only|recv-only|send-recv]]" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE            i.e. vsf_hw_i2c0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT            repeat count" VSF_TRACE_CFG_LINEEND
    "  -c, --mode MODE                @ref vsf_spi_mode_t" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT          test timeout" VSF_TRACE_CFG_LINEEND
    "  -f, --freq                     spi sample frequency" VSF_TRACE_CFG_LINEEND
    "  -s, --cs-index                 chip select index" VSF_TRACE_CFG_LINEEND
    "  -e, --cs-less                  only start/end change chip select" VSF_TRACE_CFG_LINEEND
    "  -n, --buffer-len               buffer length in one direction" VSF_TRACE_CFG_LINEEND,

    .test.cfg.mode          = APP_SPI_DEMO_CFG_DEFAULT_MODE,
    .test.cfg.clock_hz      = APP_SPI_DEMO_CFG_DEFAULT_CLOCK,
    .test.cnt               = APP_SPI_DEMO_CFG_DEFAULT_BUFFER_CNT,

    .init_has_cfg           = true,
    .device_init            = (hal_init_fn_t       )vsf_spi_init,
    .device_fini            = (hal_fini_fn_t       )vsf_spi_fini,
    .device_enable          = (hal_enable_fn_t     )vsf_spi_enable,
    .device_disable         = (hal_disable_fn_t    )vsf_spi_disable,
    .device_irq_enable      = (hal_irq_enable_fn_t )vsf_spi_irq_enable,
    .device_irq_disable     = (hal_irq_disable_fn_t)vsf_spi_irq_disable,
    .mode.options           = __mode_options,
    .mode.cnt               = dimof(__mode_options),
    .irq.options            = __irq_options,
    .irq.cnt                = dimof(__irq_options),
    .method.options         = __method_options,
    .method.cnt             = dimof(__method_options),
);

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static bool __spi_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    spi_test_t *test = vsf_container_of(hal_test, spi_test_t, use_as__hal_test_t);

    VSF_ASSERT(test->device != NULL);

    if (test->verbose >= 2) {
        hal_options_trace(VSF_TRACE_DEBUG, "vsf_spi_cfg_t cfg = { .mode = ", __mode_options, dimof(__mode_options), test->cfg.mode);
        vsf_trace_debug(", .clock_hz = %d, .isr = { .prio = vsf_arch_prio_%u }};" VSF_TRACE_CFG_LINEEND, test->cfg.clock_hz, hal_demo_arch_prio_to_num(test->cfg.isr.prio));
    }

    vsf_spi_capability_t cap = vsf_spi_capability(test->device);

    if (((test->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_HARDWARE_MODE) && !cap.support_hardware_cs) {
        vsf_trace_error("use VSF_SPI_CS_HARDWARE_MODE but not support!" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    if (((test->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) && !cap.support_software_cs) {
        vsf_trace_error("use VSF_SPI_CS_SOFTWARE_MODE but not support!" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    if (test->cs_index >= cap.cs_count) {
        vsf_trace_error("use cs %u , over the range [0, %u) supported!" VSF_TRACE_CFG_LINEEND, test->cs_index, cap.cs_count);
        return false;
    }
    if (test->cfg.clock_hz > cap.max_clock_hz) {
        vsf_trace_error("clock freq(%uHz) cannot be grater then max_clock_hz(%uHz)" VSF_TRACE_CFG_LINEEND, test->cfg.clock_hz, cap.max_clock_hz);
        return false;
    }
    if (test->cfg.clock_hz < cap.min_clock_hz) {
        vsf_trace_error("clock freq(%uHz) cannot be less then min_clock_hz(%uHz)" VSF_TRACE_CFG_LINEEND, test->cfg.clock_hz, cap.min_clock_hz);
        return false;
    }

    bool is_req = (test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST;
    bool is_isr = (test->method & METHOD_IRQ_MASK) == METHOD_SPI_ISR;

    size_t buf_size = vsf_spi_mode_to_data_bytes(test->cfg.mode) * test->cnt;
    uint32_t method = (test->method & METHOD_SEND_RECV_MASK);
    switch (method) {
    case METHOD_SEND_RECV:
        test->send.size = buf_size;
        test->recv.size = buf_size;
        if (is_req) {
            test->expected_irq_mask = VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_CPL;
            test->irq_mask          = VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_CPL | VSF_SPI_IRQ_MASK_ERROR;
        } else {
            test->expected_irq_mask = VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX;
            test->irq_mask          = VSF_SPI_IRQ_MASK_RX | VSF_SPI_IRQ_MASK_ERROR;      // VSF_SPI_IRQ_MASK_TX is turned on after sending
        }
        break;
    case METHOD_SEND_ONLY:
        test->send.size = buf_size;
        test->recv.size = 0;
        if (is_req) {
            test->expected_irq_mask = VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_CPL;
            test->irq_mask          = VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_CPL | VSF_SPI_IRQ_MASK_ERROR;
        } else {
            test->expected_irq_mask = VSF_SPI_IRQ_MASK_TX;
            test->irq_mask          = VSF_SPI_IRQ_MASK_ERROR;
        }
        break;
    case METHOD_RECV_ONLY:
        test->send.size = 0;
        test->recv.size = buf_size;
        if (is_req) {
            test->expected_irq_mask = VSF_SPI_IRQ_MASK_CPL;
            test->irq_mask          = VSF_SPI_IRQ_MASK_CPL | VSF_SPI_IRQ_MASK_ERROR;
        } else {
            test->expected_irq_mask = VSF_SPI_IRQ_MASK_RX;
            test->irq_mask          = VSF_SPI_IRQ_MASK_RX | VSF_SPI_IRQ_MASK_ERROR;
        }
        break;
    }

    if (test->irq_mask) {
        test->cfg.isr.handler_fn = __spi_isr_handler;
        test->cfg.isr.target_ptr = test;
        test->cfg.isr.prio = test->isr_arch_prio;
    }

    test->hal_cfg = (void *)&test->cfg;

    return true;
}


static void __spi_isr_handler(void *target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    spi_test_t *test = (spi_test_t *)target_ptr;
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(spi_ptr != NULL);
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    if (!hal_test_irq_check(&test->use_as__hal_test_t, irq_mask)) {
        return;
    }

    if (irq_mask & (VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX)) {
        if (__spi_fifo_transfer(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_SPI_CHECK);
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_TX_CPL) {
        test->expected_irq_mask &= ~VSF_SPI_IRQ_MASK_TX_CPL;
        test->fifo_send_cnt = test->cnt;
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_CPL) {
        test->expected_irq_mask &= ~VSF_SPI_IRQ_MASK_CPL;
        test->fifo_recv_cnt = test->cnt;
        vsf_eda_post_evt(eda, VSF_EVT_SPI_CHECK);
    }
}


static bool __spi_fifo_is_cpl(spi_test_t *test)
{
    if (((test->send.buffer == NULL) || (test->fifo_send_cnt == test->cnt))        // not send or send complete
        && ((test->recv.buffer == NULL) || (test->fifo_recv_cnt == test->cnt))) {     // not receive or receive complete
        return true;
    } else {
        return false;
    }
}

static void __spi_cs_active(spi_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_spi_t *spi_ptr = test->device;
    VSF_ASSERT(spi_ptr != NULL);

    if ((test->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        if (test->verbose >= 2) {
            vsf_trace_debug("vsf_spi_cs_active(&%s, %u)" VSF_TRACE_CFG_LINEEND, test->device_name, test->cs_index);
        }
        vsf_spi_cs_active(spi_ptr, test->cs_index);
    }
}

static void __spi_cs_inactive(spi_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_spi_t *spi_ptr = test->device;
    VSF_ASSERT(spi_ptr != NULL);

    if ((test->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        if (test->verbose >= 2) {
            vsf_trace_debug("vsf_spi_cs_inactive(&%s, %u)" VSF_TRACE_CFG_LINEEND, test->device_name, test->cs_index);
        }
        vsf_spi_cs_inactive(spi_ptr, test->cs_index);
    }
}

static bool __spi_fifo_transfer(spi_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_spi_t *spi_ptr = test->device;
    VSF_ASSERT(spi_ptr != NULL);

    vsf_spi_fifo_transfer(spi_ptr,
                          test->send.buffer, &test->fifo_send_cnt,
                          test->recv.buffer, &test->fifo_recv_cnt,
                          test->cnt);

    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_spi_fifo_transfer(&%s, %p/*sbuf*/, %p/*%u*/, %p/*rbuf*/, %p/*%u*/, %u)" VSF_TRACE_CFG_LINEEND, test->device_name,
                        test->send.buffer, &test->fifo_send_cnt, test->fifo_send_cnt,
                        test->recv.buffer, &test->fifo_recv_cnt, test->fifo_recv_cnt,
                        test->cnt);
    }

    if (((test->send.buffer == NULL) || (test->fifo_send_cnt == test->cnt))        // not send or send complete
        && ((test->recv.buffer == NULL) || (test->fifo_recv_cnt == test->cnt))) {     // not receive or receive complete
        return true;
    } else {
        return false;
    }
}

static bool __spi_is_idle(spi_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_spi_t *spi_ptr = test->device;
    VSF_ASSERT(spi_ptr != NULL);

    vsf_spi_status_t status = vsf_spi_status(spi_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_spi_status(&%s) = %s" VSF_TRACE_CFG_LINEEND, test->device_name, status.is_busy ? "busy" : "idle");
    }

    return !status.is_busy;
}

static vsf_err_t __spi_request_transfer(spi_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_spi_t *spi_ptr = test->device;
    VSF_ASSERT(spi_ptr != NULL);

    vsf_err_t err = vsf_spi_request_transfer(spi_ptr, test->send.buffer,
                                             test->recv.buffer, test->cnt);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_spi_request_transfer(&%s, %p/*sbuf*/, %p/*rbuf*/, %u/*cnt*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->send.buffer, test->recv.buffer,
                        test->cnt, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("spi request faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static void __spi_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    spi_test_t *test = vsf_container_of(eda, spi_test_t, teda);
    vsf_spi_t * spi_ptr = test->device;
    VSF_ASSERT(spi_ptr != NULL);
    bool is_send_done;
    bool is_recv_done;

    switch (evt) {
    case VSF_EVT_HAL_TEST_END:
        if (test->cs_less) {
            __spi_cs_inactive(test);
        }
        vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_IRQ_DISABLE);
        break;

    case VSF_EVT_HAL_TEST_START:
        if (test->cs_less) {
            __spi_cs_active(test);
        }

    case VSF_EVT_HAL_TEST_RUN:
        VSF_ASSERT((test->send.buffer != NULL) || (test->recv.buffer != NULL));

        if (!test->cs_less) {
            __spi_cs_active(test);
        }

        test->fifo_recv_cnt = 0;
        test->fifo_send_cnt = 0;

        if ((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST) {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
            VSF_ASSERT((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST);
            if (VSF_ERR_NONE != __spi_request_transfer(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
            }
            break;
        } else /*if ((test->method & METHOD_TRANSFER_MASK) == METHOD_FIFO)*/ {
            if ((test->method & METHOD_IRQ_MASK) == METHOD_IRQ_MASK) {
                if ((test->method & METHOD_SEND_RECV_MASK) != VSF_SPI_IRQ_MASK_RX) {
                    test->irq_mask |= VSF_SPI_IRQ_MASK_TX;
                }
                __spi_fifo_transfer(test);
                hal_device_irq_enable(&test->use_as__hal_test_t);
                break;
            } else {

            }
        }

    case VSF_EVT_SPI_FIFO_POLL:
        VSF_ASSERT((test->method & METHOD_TRANSFER_MASK) == METHOD_FIFO);
        if (!__spi_fifo_transfer(test)) {
            if (hal_test_is_timeout(&test->use_as__hal_test_t, test->timeout_ms.test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
            vsf_eda_post_evt(eda, VSF_EVT_SPI_FIFO_POLL);
        } else {
            if (test->recv.buffer == NULL) {
                // The data is in the send fifo, wait for all data to be transferred
                vsf_eda_post_evt(eda, VSF_EVT_SPI_TX_FIFO_WAIT_IDLE); // wait tx cpl
            } else {
                // receiving completed, assuming it is idle
                if (!test->cs_less) {
                    __spi_cs_inactive(test);
                }
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
            }
        }
        break;

    case VSF_EVT_SPI_TX_FIFO_WAIT_IDLE:
        VSF_ASSERT((test->method & METHOD_TRANSFER_MASK) == METHOD_FIFO);
        if (!__spi_is_idle(test)) {
            if (hal_test_is_timeout(&test->use_as__hal_test_t, test->timeout_ms.test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
            }
            break;
        }

        if (!test->cs_less) {
            __spi_cs_inactive(test);
        }
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        break;

    case VSF_EVT_SPI_CHECK:
        VSF_ASSERT((test->method & METHOD_TRANSFER_MASK) == METHOD_REQUEST);
        is_recv_done = true;
        is_send_done = true;
        switch (test->method & METHOD_SEND_RECV_MASK) {
        case METHOD_SEND_ONLY:
            is_send_done = test->fifo_send_cnt == test->cnt;
            break;
        case METHOD_RECV_ONLY:
            is_recv_done = test->fifo_recv_cnt == test->cnt;
            break;
        case METHOD_SEND_RECV:
            is_recv_done = test->fifo_recv_cnt == test->cnt;
            is_send_done = test->fifo_send_cnt == test->cnt;
            break;
        }

        if (!is_send_done || !is_recv_done) {
            break;
        }

        if (!test->cs_less) {
            __spi_cs_inactive(test);
        }
        vsf_teda_cancel_timer();
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        break;

    case VSF_EVT_TIMER:
        if (test->verbose >= 1) {
            vsf_trace_error("reqeust test timeout" VSF_TRACE_CFG_LINEEND);
        }
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __spi_parser_args(hal_test_t * hal_test, int argc, char *argv[])
{
    spi_test_t *test = (spi_test_t *)hal_test;
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    VSF_ASSERT(demo != NULL);

    vsf_err_t err = VSF_ERR_NONE;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::m:r:d:c:p:i:t:f:s:e:n:z:";
    static const struct option __long_options[] = {
        { "help",       no_argument,       NULL, 'h'  },
        { "list-device",no_argument,       NULL, 'l'  },
        { "verbose",    optional_argument, NULL, 'v'  },
        { "method",     required_argument, NULL, 'm'  },
        { "repeat",     required_argument, NULL, 'r'  },
        { "device",     required_argument, NULL, 'd'  },
        { "config",     required_argument, NULL, 'c'  },
        { "prio",       required_argument, NULL, 'p'  },
        { "isr_prio",   required_argument, NULL, 'i'  },
        { "timeout",    required_argument, NULL, 't'  },
        { "freq",       required_argument, NULL, 'f'  },
        { "cs-index",   required_argument, NULL, 's' },
        { "cs-less",    no_argument,       NULL, 'e' },
        { "buffer-len", required_argument, NULL, 'n' },
        { "clock-hz",   required_argument, NULL, 'z' },
        { NULL,         0,                 NULL, '\0' },
    };

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch(c) {
        case 's':
            test->cs_index = strtol(optarg, NULL, 0);
            break;
        case 'e':
            test->cs_less = true;
            break;
        case 'n':
            test->cnt = strtol(optarg, NULL, 0);
            if (test->cnt == 0) {
                vsf_trace_error("spi buffer length cannot be 0" VSF_TRACE_CFG_LINEEND);
                return VSF_ERR_FAIL;
            }
            break;
        case 'z':
            test->cfg.clock_hz = strtol(optarg, NULL, 0);
            if (test->cnt == 0) {
                vsf_trace_error("spi clock cannot be 0" VSF_TRACE_CFG_LINEEND);
                return VSF_ERR_FAIL;
            }
            break;
        default:
            err = hal_test_evthandler(hal_test, argc, argv, c);
            if (err != VSF_ERR_NONE) {
                return err;
            }
            break;
        }
    }

    test->cfg.mode = (test->cfg.mode & ~test->mode_mask) | test->mode_value;

    return err;
}

int spi_main(int argc, char *argv[])
{
    return hal_main(&__spi_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__spi_demo.use_as__hal_demo_t);
}
#endif

#endif
