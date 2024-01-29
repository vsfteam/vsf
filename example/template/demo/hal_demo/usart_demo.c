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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_USART_DEMO == ENABLED && VSF_HAL_USE_USART == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_USART_DEMO_CFG_USART_PREFIX
#   undef VSF_USART_CFG_PREFIX
#   define VSF_USART_CFG_PREFIX                             APP_USART_DEMO_CFG_USART_PREFIX
#endif

// If the buffer is small (64 bytes) and the baudrate is big (921600),
// then it may only need to be sent once
#ifndef APP_USART_DEMO_CFG_DEFAULT_BUFFER_SIZE
#   define APP_USART_DEMO_CFG_DEFAULT_BUFFER_SIZE           256
#endif

#ifndef APP_USART_DEMO_CFG_DEFAULT_MODE
#   define APP_USART_DEMO_CFG_DEFAULT_MODE                                      \
                VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT |                  \
                VSF_USART_NO_PARITY | VSF_USART_NO_HWCONTROL |                  \
                VSF_USART_RX_ENABLE | VSF_USART_TX_ENABLE
#endif

#ifndef APP_USART_DEMO_CFG_DEFAULT_RX_TIMEOUT
#   define APP_USART_DEMO_CFG_DEFAULT_RX_TIMEOUT            0
#endif

#ifndef APP_USART_DEMO_CFG_DEFAULT_BAUDRATE
#   define APP_USART_DEMO_CFG_DEFAULT_BAUDRATE              115200
#endif

#ifndef APP_USART_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_USART_DEMO_CFG_DEFAULT_INSTANCE               vsf_hw_usart1
#endif

#ifndef APP_USART_DEMO_CFG_DEVICES_COUNT
#   define APP_USART_DEMO_CFG_DEVICES_COUNT                 1
#endif

#ifndef APP_USART_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_USART_DEMO_CFG_DEVICES_ARRAY_INIT            \
        { .cnt = dimof(vsf_hw_usart_devices), .devices = vsf_hw_usart_devices},
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum usart_method_t {
    METHOD_RX_FIFO_POLL     = 0x00 << 0,
    METHOD_RX_FIFO_ISR      = 0x01 << 0,
    METHOD_RX_REQUEST       = 0x02 << 0,
    METHOD_RX_NONE          = 0x03 << 0,
    METHOD_RX_MASK          = 0x03 << 0,

    METHOD_TX_FIFO_POLL     = 0x00 << 2,
    METHOD_TX_FIFO_ISR      = 0x01 << 2,
    METHOD_TX_REQUEST       = 0x02 << 2,
    METHOD_TX_NONE          = 0x03 << 2,
    METHOD_TX_MASK          = 0x03 << 2,

    METHOD_TX_FIFO_GET      = 0x00 << 4,
    METHOD_TX_NO_FIFO_GET   = 0x01 << 4,
    METHOD_TX_FIFO_GET_MASK = 0x01 << 4,

    METHOD_RX_FIFO_GET      = 0x00 << 5,
    METHOD_RX_NO_FIFO_GET   = 0x01 << 5,
    METHOD_RX_FIFO_GET_MASK = 0x01 << 5,
} usart_method_t;

typedef enum usart_demo_evt_t {
    VSF_EVT_USART_FIFO_POLL = __VSF_EVT_HAL_LAST,
    VSF_EVT_USART_CHECK,
} usart_demo_evt_t;

typedef struct usart_test_t  {
    implement(hal_test_t)

    vsf_usart_cfg_t cfg;

    uint8_t bytes;
    uint32_t cnt;
    uint32_t tx_offset;
    uint32_t rx_offset;
} usart_test_t;

typedef struct usart_demo_const_t {
    implement(hal_demo_const_t)
    usart_test_t test;
} usart_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static void __usart_isr_handler(void *target, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask);
static vsf_err_t __usart_txfifo_write(usart_test_t *test);
static vsf_err_t __usart_rxfifo_read(usart_test_t *test);

#ifdef __WIN__
static void __usart_scan(hal_demo_t *demo);
#endif

/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __mode_options[] = {
    HAL_DEMO_OPTION(VSF_USART_BIT_LENGTH_MASK, VSF_USART_5_BIT_LENGTH),
    HAL_DEMO_OPTION(VSF_USART_BIT_LENGTH_MASK, VSF_USART_6_BIT_LENGTH),
    HAL_DEMO_OPTION(VSF_USART_BIT_LENGTH_MASK, VSF_USART_7_BIT_LENGTH),
    HAL_DEMO_OPTION(VSF_USART_BIT_LENGTH_MASK, VSF_USART_8_BIT_LENGTH),
    HAL_DEMO_OPTION(VSF_USART_BIT_LENGTH_MASK, VSF_USART_9_BIT_LENGTH),

    HAL_DEMO_OPTION(VSF_USART_STOPBIT_MASK, VSF_USART_1_5_STOPBIT),
    HAL_DEMO_OPTION(VSF_USART_STOPBIT_MASK, VSF_USART_1_STOPBIT),
    HAL_DEMO_OPTION(VSF_USART_STOPBIT_MASK, VSF_USART_2_STOPBIT),

    HAL_DEMO_OPTION(VSF_USART_PARITY_MASK, VSF_USART_NO_PARITY),
    HAL_DEMO_OPTION(VSF_USART_PARITY_MASK, VSF_USART_EVEN_PARITY),
    HAL_DEMO_OPTION(VSF_USART_PARITY_MASK, VSF_USART_ODD_PARITY),
    HAL_DEMO_OPTION(VSF_USART_PARITY_MASK, VSF_USART_FORCE_0_PARITY),
    HAL_DEMO_OPTION(VSF_USART_PARITY_MASK, VSF_USART_FORCE_1_PARITY),

    HAL_DEMO_OPTION(VSF_USART_HWCONTROL_MASK, VSF_USART_NO_HWCONTROL),
    HAL_DEMO_OPTION(VSF_USART_HWCONTROL_MASK, VSF_USART_RTS_HWCONTROL),
    HAL_DEMO_OPTION(VSF_USART_HWCONTROL_MASK, VSF_USART_CTS_HWCONTROL),
    HAL_DEMO_OPTION(VSF_USART_HWCONTROL_MASK, VSF_USART_RTS_CTS_HWCONTROL),

    HAL_DEMO_OPTION(VSF_USART_TX_ENABLE_MASK, VSF_USART_TX_ENABLE),
    HAL_DEMO_OPTION(VSF_USART_TX_ENABLE_MASK, VSF_USART_TX_DISABLE),
    HAL_DEMO_OPTION(VSF_USART_RX_ENABLE_MASK, VSF_USART_RX_ENABLE),
    HAL_DEMO_OPTION(VSF_USART_RX_ENABLE_MASK, VSF_USART_RX_DISABLE),
};

static const hal_option_t __irq_options[] = {
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_TX_CPL),
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_RX_CPL),
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_TX),
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_RX),
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_RX_TIMEOUT),
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_FRAME_ERR),
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_PARITY_ERR),
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_BREAK_ERR),
    HAL_DEMO_OPTION(VSF_USART_IRQ_ALL_BITS_MASK, VSF_USART_IRQ_MASK_OVERFLOW_ERR),
};

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_RX_MASK, METHOD_RX_FIFO_POLL,             "rx-fifo-poll"),
    HAL_DEMO_OPTION_EX(METHOD_RX_MASK, METHOD_RX_FIFO_ISR,              "rx-fifo-irq"),
    HAL_DEMO_OPTION_EX(METHOD_RX_MASK, METHOD_RX_REQUEST,               "rx-request"),
    HAL_DEMO_OPTION_EX(METHOD_RX_MASK, METHOD_RX_NONE,                  "no-rx"),

    HAL_DEMO_OPTION_EX(METHOD_TX_MASK, METHOD_TX_FIFO_POLL,             "tx-fifo-poll"),
    HAL_DEMO_OPTION_EX(METHOD_TX_MASK, METHOD_TX_FIFO_ISR,              "tx-fifo-irq"),
    HAL_DEMO_OPTION_EX(METHOD_TX_MASK, METHOD_TX_REQUEST,               "tx-request"),
    HAL_DEMO_OPTION_EX(METHOD_TX_MASK, METHOD_TX_NONE,                  "no-tx"),

    HAL_DEMO_OPTION_EX(METHOD_TX_FIFO_GET_MASK, METHOD_TX_FIFO_GET,     "tx-get-free"),
    HAL_DEMO_OPTION_EX(METHOD_TX_FIFO_GET_MASK, METHOD_TX_NO_FIFO_GET,  "no-tx-get-free"),

    HAL_DEMO_OPTION_EX(METHOD_RX_FIFO_GET_MASK, METHOD_RX_FIFO_GET,     "rx-get-free"),
    HAL_DEMO_OPTION_EX(METHOD_RX_FIFO_GET_MASK, METHOD_RX_NO_FIFO_GET,  "no-rx-get-free"),
};

HAL_DEMO_INIT(usart, APP_USART,
    "usart-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [rx-fifo-poll|rx-fifo-irq|rx-request|no-rx]" VSF_TRACE_CFG_LINEEND
    "               [tx-fifo-poll|tx-fifo-irq|tx-request|no-tx]" VSF_TRACE_CFG_LINEEND
    "               [rx-get-free|no-rx-get-free]" VSF_TRACE_CFG_LINEEND
    "               [tx-get-free|no-tx-get-free]" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE            i.e. vsf_hw_usart0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT            repeat count" VSF_TRACE_CFG_LINEEND
    "  -c, --mode MODE                @ref vsf_usart_mode_t" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT          test timeout" VSF_TRACE_CFG_LINEEND
    "  -b, --boudrate <boudrate>" VSF_TRACE_CFG_LINEEND
    "  -c, --data_cnt <data_cnt>      tx and rx buffer size" VSF_TRACE_CFG_LINEEND,

#ifdef __WIN__
    .scan = &__usart_scan,
#endif

    .test.cfg.mode          = APP_USART_DEMO_CFG_DEFAULT_MODE,
    .test.cfg.baudrate      = APP_USART_DEMO_CFG_DEFAULT_BAUDRATE,
    .test.cfg.rx_timeout    = APP_USART_DEMO_CFG_DEFAULT_RX_TIMEOUT,
    .test.cnt               = APP_USART_DEMO_CFG_DEFAULT_BUFFER_SIZE,

    .init_has_cfg           = true,
    .device_init            = (hal_init_fn_t       )vsf_usart_init,
    .device_fini            = (hal_fini_fn_t       )vsf_usart_fini,
    .device_enable          = (hal_enable_fn_t     )vsf_usart_enable,
    .device_disable         = (hal_disable_fn_t    )vsf_usart_disable,
    .device_irq_enable      = (hal_irq_enable_fn_t )vsf_usart_irq_enable,
    .device_irq_disable     = (hal_irq_disable_fn_t)vsf_usart_irq_disable,
    .mode.options           = __mode_options,
    .mode.cnt               = dimof(__mode_options),
    .irq.options            = __irq_options,
    .irq.cnt                = dimof(__irq_options),
    .method.options         = __method_options,
    .method.cnt             = dimof(__method_options),
);

/*============================ IMPLEMENTATION ================================*/

#ifdef __WIN__
static void __usart_scan(hal_demo_t *demo)
{
    static vsf_hal_device_t __vsf_hw_usart_devices[VSF_HW_USART_COUNT];

    usart_demo_t *usart_demo = vsf_container_of(demo, usart_demo_t, use_as__hal_demo_t);
    VSF_ASSERT(usart_demo->array_cnt == 1);

    vsf_usart_win_device_t com_vsf_usart[VSF_HW_USART_COUNT] = {0};
    uint8_t cnt = vsf_win_usart_scan_devices((vsf_usart_win_device_t *)com_vsf_usart, dimof(com_vsf_usart));

    memset(__vsf_hw_usart_devices, 0, sizeof(__vsf_hw_usart_devices));
    for (int i = 0; i < cnt; i++) {
        uint8_t port = com_vsf_usart[i].port;
        __vsf_hw_usart_devices[i] = vsf_hw_usart_devices[port];
    }

    demo->devices_array[0].cnt = cnt;
    demo->devices_array[0].devices = __vsf_hw_usart_devices;
}
#endif

static bool __usart_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    usart_test_t *test = vsf_container_of(hal_test, usart_test_t, use_as__hal_test_t);

    VSF_ASSERT(test != NULL);
    VSF_ASSERT(test->device != NULL);

    if (test->verbose >= 2) {
        hal_options_trace(VSF_TRACE_DEBUG, "vsf_usart_cfg_t cfg = { .mode = ", __mode_options, dimof(__mode_options), test->cfg.mode);
        vsf_trace_debug(", .baudrate = %d, .rx_timeout = %d, isr = { .prio = vsf_arch_prio_%u }};" VSF_TRACE_CFG_LINEEND,
                        test->cfg.baudrate, test->cfg.rx_timeout, hal_demo_arch_prio_to_num(test->cfg.isr.prio));
    }

    vsf_usart_capability_t cap = vsf_usart_capability(test->device);
    if ((test->cfg.baudrate > cap.max_baudrate) || (test->cfg.baudrate < cap.min_baudrate)) {
        vsf_trace_error("baudrate(%u) out of range(%u, %u)" VSF_TRACE_CFG_LINEEND, test->cfg.baudrate, cap.min_baudrate, cap.max_baudrate);
        return false;
    }
    uint8_t data_bits = vsf_usart_mode_to_data_bits(test->cfg.mode);
    if ((data_bits > cap.max_data_bits) || (data_bits < cap.min_data_bits)) {
        vsf_trace_error("data bits（%u) out of range(%u, %u)!" VSF_TRACE_CFG_LINEEND, data_bits, cap.min_data_bits, cap.max_data_bits);
        return false;
    }

    test->bytes = data_bits > 8 ? 2 : 1;

    vsf_usart_mode_t tx_mode;
    vsf_usart_mode_t rx_mode;
    if ((test->method & METHOD_TX_MASK) != METHOD_TX_NONE) {
        test->send.size = test->bytes * test->cnt;
        tx_mode = VSF_USART_TX_ENABLE;
    } else {
        tx_mode = VSF_USART_TX_DISABLE;
    }
    if ((test->method & METHOD_RX_MASK) != METHOD_RX_NONE) {
        test->recv.size = test->bytes * test->cnt;
        rx_mode = VSF_USART_RX_ENABLE;
    } else {
        rx_mode = VSF_USART_RX_DISABLE;
    }
    test->cfg.mode = (test->cfg.mode & ~(VSF_USART_TX_ENABLE_MASK | VSF_USART_RX_ENABLE_MASK)) | tx_mode | rx_mode;

    vsf_usart_irq_mask_t rx_irq_mask = 0;
    if ((test->method & METHOD_RX_MASK) == METHOD_RX_FIFO_ISR) {
        rx_irq_mask = VSF_USART_IRQ_MASK_RX;
    } else if ((test->method & METHOD_RX_MASK) == METHOD_RX_REQUEST) {
        rx_irq_mask = VSF_USART_IRQ_MASK_RX_CPL;
    }
    vsf_usart_irq_mask_t tx_irq_mask = 0;
    if ((test->method & METHOD_TX_MASK) == METHOD_TX_REQUEST) {
        tx_irq_mask = VSF_USART_IRQ_MASK_TX_CPL;
    }
    test->expected_irq_mask = tx_irq_mask | rx_irq_mask;

    vsf_usart_irq_mask_t irq_mask = test->expected_irq_mask & ~cap.irq_mask;
    if (irq_mask) {
        vsf_trace_error("unsupported interrupt(0x%08x)!" VSF_TRACE_CFG_LINEEND, irq_mask);
        hal_options_trace(VSF_TRACE_DEBUG, "", __irq_options, dimof(__irq_options), irq_mask);
        return false;
    }
    test->irq_mask = test->expected_irq_mask | (VSF_USART_IRQ_MASK_ERR & cap.irq_mask);

    if (test->irq_mask) {
        test->cfg.isr.target_ptr = test;
        test->cfg.isr.handler_fn = __usart_isr_handler;
        test->cfg.isr.prio = test->isr_arch_prio;
    }

    test->hal_cfg = (void *)&test->cfg;

    return true;
}

static void __usart_isr_handler(void *target, vsf_usart_t *usart_ptr,
                                vsf_usart_irq_mask_t irq_mask)
{
    usart_test_t *test = (usart_test_t *)target;
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(usart_ptr != NULL);
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    if (!hal_test_irq_check(&test->use_as__hal_test_t, irq_mask)) {
        return;
    }

    if (irq_mask & VSF_USART_IRQ_MASK_TX) {
        if (VSF_ERR_NONE == __usart_txfifo_write(test)) {
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_usart_irq_disable(&%s, VSF_USART_IRQ_MASK_TX)" VSF_TRACE_CFG_LINEEND, test->device_name);
            }
            vsf_usart_irq_disable(usart_ptr, VSF_USART_IRQ_MASK_TX);
            test->irq_mask &= ~VSF_USART_IRQ_MASK_TX;
            test->expected_irq_mask &= ~VSF_USART_IRQ_MASK_TX;

            vsf_eda_post_evt(eda, VSF_EVT_USART_CHECK);
        }
    }

    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        if (VSF_ERR_NONE == __usart_rxfifo_read(test)) {
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_usart_irq_disable(&%s, VSF_USART_IRQ_MASK_RX)" VSF_TRACE_CFG_LINEEND, test->device_name);
            }
            vsf_usart_irq_disable(usart_ptr, VSF_USART_IRQ_MASK_RX);
            test->irq_mask &= ~VSF_USART_IRQ_MASK_RX;
            test->expected_irq_mask &= ~VSF_USART_IRQ_MASK_RX;

            vsf_eda_post_evt(eda, VSF_EVT_USART_CHECK);
        }
    }

    if (irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
        test->tx_offset = test->cnt;
        vsf_eda_post_evt(eda, VSF_EVT_USART_CHECK);
    }

    if (irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
        test->rx_offset = test->cnt;
        vsf_eda_post_evt(eda, VSF_EVT_USART_CHECK);
    }
}

static bool __usart_is_tx_busy(usart_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_usart_t *usart_ptr = test->device;
    VSF_ASSERT(usart_ptr != NULL);

    vsf_usart_status_t status = vsf_usart_status(usart_ptr);

    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_usart_status(&%s); // is_busy: %s, is_tx_busy: %s, is_rx_busy: %s" VSF_TRACE_CFG_LINEEND,
                        test->device_name, status.is_busy ? "busy" : "idle", status.is_tx_busy ? "busy" : "idle",
                        status.is_rx_busy ? "busy" : "idle");
    }

    return status.is_tx_busy;
}

static vsf_err_t __usart_txfifo_write(usart_test_t *test)
{
    vsf_usart_t *usart_ptr = test->device;
    VSF_ASSERT(usart_ptr != NULL);

    if (test->tx_offset < test->cnt) {
        uint8_t *buffer = test->send.buffer + test->tx_offset * test->bytes;
        uint_fast16_t remain_count = test->cnt - test->tx_offset;
        uint_fast16_t return_count = 0;
        if ((test->method & METHOD_TX_FIFO_GET_MASK) == METHOD_TX_FIFO_GET) {
            uint_fast16_t free_count = vsf_usart_txfifo_get_free_count(usart_ptr);
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_usart_txfifo_get_free_count(&%s) = %d;" VSF_TRACE_CFG_LINEEND,
                                test->device_name, free_count);
            }
            uint_fast16_t write_count = vsf_min(remain_count, free_count);
            if (write_count != 0) {
                return_count = vsf_usart_txfifo_write(usart_ptr, (void *)buffer, write_count);
                if (test->verbose >= 2) {
                    vsf_trace_debug("vsf_usart_txfifo_write(&%s, %p/*buffer*/, %u) = %d" VSF_TRACE_CFG_LINEEND,
                                    test->device_name, buffer, write_count, return_count);
                }
                if (return_count != write_count) {
                    vsf_trace_error("vsf_usart_txfifo_write error, write %u, return %u", write_count, return_count);
                    return VSF_ERR_FAIL;
                }
            }
        } else {
            return_count = vsf_usart_txfifo_write(usart_ptr, (void *)buffer, remain_count);

            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_usart_txfifo_write(&%s, %p/*buffer*/, %u) = %d" VSF_TRACE_CFG_LINEEND,
                                test->device_name, buffer, remain_count, return_count);
            }

            if (return_count > remain_count) {
                vsf_trace_error("vsf_usart_txfifo_write error, write %u, return %u", remain_count, return_count);
                return VSF_ERR_FAIL;
            }
        }
        test->tx_offset += return_count;
    }

    return (test->tx_offset >= test->cnt) ? VSF_ERR_NONE : VSF_ERR_NOT_READY;;
}

static vsf_err_t __usart_request_tx(usart_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_usart_t *usart_ptr = test->device;
    VSF_ASSERT(usart_ptr != NULL);
    VSF_ASSERT(test->send.buffer != NULL);

    vsf_err_t err = vsf_usart_request_tx(usart_ptr, test->send.buffer, test->cnt);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_usart_request_tx(&%s, %p/*buffer*/, %u) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->send.buffer, test->cnt, err);
    }
    if (err != VSF_ERR_NONE) {
        vsf_trace_error("call vsf_usart_request_tx() faild, return: %d", err);
    }

    return err;
}

static vsf_err_t __usart_tx_cancel_and_get(usart_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_usart_t *usart_ptr = test->device;
    VSF_ASSERT(usart_ptr != NULL);

    vsf_err_t err = vsf_usart_cancel_tx(usart_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_usart_cancel_tx(&%s) = %d" VSF_TRACE_CFG_LINEEND, test->device_name, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("vsf_usart_cancel_tx() faild, return: %d", err);
        return err;
    }
    test->tx_offset = vsf_usart_get_tx_count(usart_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_usart_get_tx_count(&%s) = %d" VSF_TRACE_CFG_LINEEND, test->device_name, test->tx_offset);
    }

    return err;
}

static vsf_err_t __usart_rxfifo_read(usart_test_t *test)
{
    vsf_usart_t *usart_ptr = test->device;
    VSF_ASSERT(usart_ptr != NULL);
    VSF_ASSERT(test->recv.buffer != NULL);

    if (test->rx_offset < test->cnt) {
        uint8_t *buffer = test->send.buffer + test->rx_offset * test->bytes;
        uint_fast16_t remain_count = test->cnt - test->rx_offset;

        uint_fast16_t return_count = 0;
        if ((test->method & METHOD_RX_FIFO_GET_MASK) == METHOD_RX_FIFO_GET) {
            uint_fast16_t free_count = vsf_usart_rxfifo_get_data_count(usart_ptr);
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_usart_rxfifo_get_data_count(&%s) = %d;" VSF_TRACE_CFG_LINEEND,
                                test->device_name, free_count);
            }
            if (free_count != 0) {
                uint_fast16_t read_count = vsf_min(remain_count, free_count);
                return_count = vsf_usart_rxfifo_read(usart_ptr, (void *)buffer, read_count);
                if (test->verbose >= 2) {
                    vsf_trace_debug("vsf_usart_rxfifo_read(&%s, %p/*buffer*/, %u) = %d" VSF_TRACE_CFG_LINEEND,
                                    test->device_name, buffer, read_count, return_count);
                }
                if (return_count != read_count) {
                    vsf_trace_error("vsf_usart_rxfifo_read error, want to read %u, return %u", read_count, return_count);
                    return VSF_ERR_FAIL;
                }
            }
        } else {
            return_count = vsf_usart_rxfifo_read(usart_ptr, (void *)buffer, remain_count);
            if (test->verbose >= 2) {
                vsf_trace_debug("vsf_usart_rxfifo_read(&%s, %p/*buffer*/, %u) = %d" VSF_TRACE_CFG_LINEEND,
                                test->device_name, buffer, remain_count, return_count);
            }
            if (return_count > remain_count) {
                vsf_trace_error("vsf_usart_rxfifo_read error, want to read less than %u, return %u", remain_count, return_count);
                return VSF_ERR_FAIL;
            }
        }
        test->rx_offset += return_count;
    }

    return (test->rx_offset >= test->cnt) ? VSF_ERR_NONE : VSF_ERR_NOT_READY;
}

static vsf_err_t __usart_request_rx(usart_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_usart_t *usart_ptr = test->device;
    VSF_ASSERT(usart_ptr != NULL);
    VSF_ASSERT(test->send.buffer != NULL);

    vsf_err_t err = vsf_usart_request_rx(usart_ptr, test->send.buffer, test->cnt);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_usart_request_rx(%s, %p/*buffer*/, %u) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->send.buffer, test->cnt, err);
    }
    if (err != VSF_ERR_NONE) {
        vsf_trace_error("call vsf_usart_request_rx() faild, return: %d", err);
    }

    return err;
}

static vsf_err_t __usart_rx_cancel_and_get(usart_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_usart_t *usart_ptr = test->device;
    VSF_ASSERT(usart_ptr != NULL);

    vsf_err_t err = vsf_usart_cancel_rx(usart_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_usart_cancel_rx(&%s) = %d" VSF_TRACE_CFG_LINEEND, test->device_name, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("vsf_usart_cancel_rx() faild, return: %d", err);
        return err;
    }
    test->rx_offset = vsf_usart_get_rx_count(usart_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_usart_get_tx_count(&%s) = %d" VSF_TRACE_CFG_LINEEND, test->device_name, test->rx_offset);
    }
    return err;
}


static void __usart_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    usart_test_t *test = vsf_container_of(eda, usart_test_t , teda);
    vsf_usart_t * usart_ptr = test->device;
    VSF_ASSERT(usart_ptr != NULL);

    bool is_tx_poll = false;
    bool is_rx_poll = false;
    vsf_err_t tx_err;
    vsf_err_t rx_err;
    bool is_tx_cpl = false;
    bool is_rx_cpl = false;

    switch (evt) {
    case VSF_EVT_HAL_TEST_RUN:
        VSF_ASSERT((test->send.buffer != NULL) || (test->recv.buffer != NULL));

        if ((test->method & METHOD_RX_MASK) == METHOD_RX_FIFO_ISR) {
            // VSF_USART_IRQ_MASK_RX_CPL has been enabled
            __usart_request_rx(test);
        } else if ((test->method & METHOD_RX_MASK) == METHOD_RX_REQUEST) {
            if (VSF_ERR_NONE != __usart_request_rx(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else if ((test->method & METHOD_RX_MASK) == METHOD_RX_FIFO_POLL) {
            is_rx_poll = true;
        }

        if ((test->method & METHOD_TX_MASK) == METHOD_TX_FIFO_ISR) {
            /**
             \~chinese
              @note VSF_USART_IRQ_MASK_TX 中断应该在 vsf_usart_txfifo_write() 调用之后再使能。
                    否则就需要关闭全局中断才能确保数据是正确的。
                    当 vsf_usart_txfifo_write() 里发送了部分数据的时候，可能被更高优先级的任务或者中断抢占，
                    从而使得 TX FIFO 的数据已经发送出去一部分，进而达到门限值，提前触发了 VSF_USART_IRQ_MASK_TX 中断，
                    最后导致数据是乱序的。
             */
            test->irq_mask |= VSF_USART_IRQ_MASK_TX;
            if (__usart_txfifo_write(test) < 0) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
            hal_device_irq_enable(&test->use_as__hal_test_t);
        } else if ((test->method & METHOD_TX_MASK) == METHOD_TX_REQUEST) {
            if (VSF_ERR_NONE != __usart_request_tx(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else if ((test->method & METHOD_TX_MASK) == METHOD_TX_FIFO_POLL) {
            is_tx_poll = true;
        }

        if (!is_rx_poll && !is_tx_poll) {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
            break;
        }
        //break;

    case VSF_EVT_USART_FIFO_POLL:
        if ((test->method & METHOD_TX_MASK) == METHOD_TX_FIFO_POLL) {
            tx_err = __usart_txfifo_write(test);
        } else {
            tx_err = VSF_ERR_NONE;
        }
        if ((test->method & METHOD_RX_MASK) == METHOD_RX_FIFO_POLL) {
            rx_err = __usart_rxfifo_read(test);
        } else {
            rx_err = VSF_ERR_NONE;
        }
        if ((tx_err < 0) || (rx_err < 0)) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
            break;
        }
        if ((tx_err == VSF_ERR_NONE) && (rx_err == VSF_ERR_NONE)) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        } else if (hal_test_is_timeout(&test->use_as__hal_test_t, test->timeout_ms.test)) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        } else {
            vsf_eda_post_evt(eda, VSF_EVT_USART_FIFO_POLL);
        }
        break;

    case VSF_EVT_USART_CHECK:
        is_tx_cpl = (test->tx_offset == test->cnt) || ((test->method & METHOD_TX_MASK) == METHOD_TX_NONE);
        is_rx_cpl = (test->rx_offset == test->cnt) || ((test->method & METHOD_RX_MASK) == METHOD_RX_NONE);
        if (is_tx_cpl && is_rx_cpl) {
            vsf_teda_cancel_timer();
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        }
        break;

    case VSF_EVT_HAL_TEST_END:
        if (__usart_is_tx_busy(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_END);
        } else {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_CALL_IRQ_DISABLE);
        }
        break;

    case VSF_EVT_TIMER:
        if ((test->method & METHOD_TX_MASK) != METHOD_TX_NONE) {
            if ((test->method & METHOD_TX_MASK) == METHOD_TX_REQUEST) {
                __usart_tx_cancel_and_get(test);
            }
            if (test->tx_offset != test->cnt) {
                vsf_trace_error("tx send timeout, cur: %d, total: %d" VSF_TRACE_CFG_LINEEND, test->tx_offset, test->cnt);
            }
        }
        if ((test->method & METHOD_RX_MASK) != METHOD_RX_NONE) {
            if ((test->method & METHOD_RX_MASK) == METHOD_RX_REQUEST) {
                __usart_rx_cancel_and_get(test);
            }
            if (test->rx_offset != test->cnt) {
                vsf_trace_error("rx send timeout, cur: %d, total: %d" VSF_TRACE_CFG_LINEEND, test->rx_offset, test->cnt);
            }
        }
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

/******************************************************************************/

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __usart_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    usart_test_t *test = (usart_test_t *)hal_test;

    int c = 0;
    int option_index = 0;
    vsf_err_t err = VSF_ERR_NONE;

    static const char *__short_options = "hlv::r:d:c:m:p:i:t:b:n:";
    static const struct option __long_options[] = {
        { "help",        no_argument,       NULL, 'h' },
        { "list-device", no_argument,       NULL, 'l' },
        { "verbose",     optional_argument, NULL, 'v' },
        { "repeat",      required_argument, NULL, 'r' },
        { "device",      required_argument, NULL, 'd' },
        { "config",      required_argument, NULL, 'c' },
        { "method",      required_argument, NULL, 'm' },
        { "prio",        required_argument, NULL, 'p' },
        { "isr_prio",    required_argument, NULL, 'i' },
        { "timeout",     required_argument, NULL, 't' },
        { "boudrate",    required_argument, NULL, 'b' },
        { "count",       required_argument, NULL, 'n' },
        { NULL,          0                , NULL, '\0' },
    };
    optind = 1;
    while (EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch (c) {
        case 'b':
            test->cfg.baudrate = strtoul(optarg, NULL, 0);
            break;
        case 'c':
            test->cnt = strtoul(optarg, NULL, 0);
            break;

        default:
            err = hal_test_evthandler(hal_test, argc, argv, c);
            if (err != VSF_ERR_NONE) {
                return err;
            }
            break;
        }
    }

    return err;
}

int usart_main(int argc, char *argv[])
{
    return hal_main(&__usart_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__usart_demo.use_as__hal_demo_t);
}
#endif

#endif
