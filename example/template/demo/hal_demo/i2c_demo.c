/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_I2C_DEMO && VSF_HAL_USE_I2C == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_I2C_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#   undef VSF_I2C_CFG_PREFIX
#   define VSF_I2C_CFG_PREFIX                           APP_I2C_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#endif

#ifndef APP_I2C_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_I2C_DEMO_CFG_DEFAULT_INSTANCE            vsf_hw_i2c0
#endif

#ifndef APP_I2C_DEMO_CFG_DEVICES_COUNT
#   define APP_I2C_DEMO_CFG_DEVICES_COUNT               1
#endif

#ifndef APP_I2C_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_I2C_DEMO_CFG_DEVICES_ARRAY_INIT          \
        { .cnt = dimof(vsf_hw_i2c_devices), .devices = vsf_hw_i2c_devices},
#endif

#ifndef APP_I2C_DEMO_CFG_SEARCH_DEVICE_DEMO
#   define  APP_I2C_DEMO_CFG_SEARCH_DEVICE_DEMO         DISABLED
#endif

#ifndef APP_I2C_DEMO_CFG_MODE
#   define APP_I2C_DEMO_CFG_MODE                        (VSF_I2C_MODE_MASTER | VSF_I2C_SPEED_STANDARD_MODE | VSF_I2C_ADDR_7_BITS)
#endif

#ifndef APP_I2C_DEMO_CLOCK_HZ
#   define APP_I2C_DEMO_CLOCK_HZ                        1000
#endif

#ifndef APP_I2C_DEMO_SEARCH_DATA_ARRAY
#   define APP_I2C_DEMO_SEARCH_DATA_ARRAY               {0xFF, 0xFF, 0xFF}
#endif

#ifndef APP_I2C_DEMO_SEARCH_DATA_SIZE
#   define APP_I2C_DEMO_SEARCH_DATA_SIZE                1
#endif

#ifndef APP_I2C_DEMO_CFG_SEARCH_ADDRESS_START
#   define APP_I2C_DEMO_CFG_SEARCH_ADDRESS_START        0x0C
#endif

#ifndef APP_I2C_DEMO_CFG_SEARCH_ADDRESS_END
#   define APP_I2C_DEMO_CFG_SEARCH_ADDRESS_END          0x70
#endif

#ifndef APP_I2C_DEMO_CFG_SEARCH_CMD
#   define APP_I2C_DEMO_CFG_SEARCH_CMD                  (VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP | VSF_I2C_CMD_7_BITS)
#endif


#ifndef APP_I2C_DEMO_CFG_EEPROM_DEVICE_ADDRESS
#   define APP_I2C_DEMO_CFG_EEPROM_DEVICE_ADDRESS       0x50
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_ADDRESS
#   define APP_I2C_DEMO_CFG_EEPROM_ADDRESS              0x0000
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_BUFFER_SIZE
#   define APP_I2C_DEMO_CFG_EEPROM_BUFFER_SIZE          12
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_DELAY_MS
#   define APP_I2C_DEMO_CFG_EEPROM_DELAY_MS             10      // WriteCycleTiming
#endif



/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum METHOD_t {
    METHOD_SEARCH_ADDR      = 0x00 << 0,
    METHOD_EEPROM           = 0x01 << 0,
    METHOD_MASK             = 0x01 << 0,

    METHOD_EEPROM_8_BIT     = 0x00 << 1,
    METHOD_EEPROM_16_BIT    = 0x01 << 1,
    METHOD_EEPROM_BIT_MASK  = 0x01 << 1,
} METHOD_t;

typedef enum i2c_demo_evt_t {
    VSF_EVT_I2C_WAIT_ISR_ACK = __VSF_EVT_HAL_LAST,
    VSF_EVT_I2C_WAIT_ISR_NAK,

    VSF_EVT_I2C_EEPROM_DEMO,
    VSF_EVT_I2C_EEPROM_DEMO_NAK,
} i2c_demo_evt_t;

typedef struct i2c_request_item_t {
    uint8_t *buffer;
    uint16_t count;
    vsf_i2c_cmd_t cmd;
    uint32_t delay_ms;
} i2c_request_item_t;

typedef struct i2c_test_t  {
    implement(hal_test_t)

    vsf_i2c_cfg_t cfg;

    uint16_t address;
    i2c_request_item_t request;

    struct {
        vsf_i2c_cmd_t cmd;
        uint16_t start_address;
        uint16_t end_address;
        uint8_t  request_size;
        uint8_t  buffer[3];
    } search;

    struct {
        uint8_t command_delay;
        uint8_t write_delay;
        uint8_t device_address;
        uint8_t seq_index;
        uint16_t eeprom_address;

        union {
            struct {
                i2c_request_item_t write_addr;
                i2c_request_item_t write_data;
                i2c_request_item_t read_addr;
                i2c_request_item_t read_data;
            };
            i2c_request_item_t seq[4];
        };
    } eeprom;
} i2c_test_t;

typedef struct i2c_demo_const_t {
    implement(hal_demo_const_t)
    i2c_test_t test;
} i2c_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static void __i2c_irq_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask);

/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __mode_options[] = {
    HAL_DEMO_OPTION(VSF_I2C_MODE_MASK, VSF_I2C_MODE_MASTER),
    HAL_DEMO_OPTION(VSF_I2C_MODE_MASK, VSF_I2C_MODE_SLAVE),

    HAL_DEMO_OPTION(VSF_I2C_SPEED_MASK, VSF_I2C_SPEED_STANDARD_MODE),
    HAL_DEMO_OPTION(VSF_I2C_SPEED_MASK, VSF_I2C_SPEED_FAST_MODE),
    HAL_DEMO_OPTION(VSF_I2C_SPEED_MASK, VSF_I2C_SPEED_FAST_MODE_PLUS),
    HAL_DEMO_OPTION(VSF_I2C_SPEED_MASK, VSF_I2C_SPEED_HIGH_SPEED_MODE),

    HAL_DEMO_OPTION(VSF_I2C_ADDR_MASK, VSF_I2C_ADDR_7_BITS),
    HAL_DEMO_OPTION(VSF_I2C_ADDR_MASK, VSF_I2C_ADDR_10_BITS),
};

static const hal_option_t __cmd_options[] = {
    HAL_DEMO_OPTION(VSF_I2C_CMD_START_MASK, VSF_I2C_CMD_START),
    HAL_DEMO_OPTION(VSF_I2C_CMD_START_MASK, VSF_I2C_CMD_NO_START),

    HAL_DEMO_OPTION(VSF_I2C_CMD_STOP_RESTART_MASK, VSF_I2C_CMD_RESTART),
    HAL_DEMO_OPTION(VSF_I2C_CMD_STOP_RESTART_MASK, VSF_I2C_CMD_STOP),
    HAL_DEMO_OPTION(VSF_I2C_CMD_STOP_RESTART_MASK, VSF_I2C_CMD_NO_STOP_RESTART),

    HAL_DEMO_OPTION(VSF_I2C_CMD_RW_MASK, VSF_I2C_CMD_WRITE),
    HAL_DEMO_OPTION(VSF_I2C_CMD_RW_MASK, VSF_I2C_CMD_READ),

    HAL_DEMO_OPTION(VSF_I2C_CMD_BITS_MASK, VSF_I2C_CMD_7_BITS),
    HAL_DEMO_OPTION(VSF_I2C_CMD_BITS_MASK, VSF_I2C_CMD_10_BITS),
};

static const hal_option_t __irq_options[] = {
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_STARTED, VSF_I2C_IRQ_MASK_MASTER_STARTED),
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_STOPPED, VSF_I2C_IRQ_MASK_MASTER_STOPPED),
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT, VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT),
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT, VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT),
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST, VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST),
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY, VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY),
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_ERROR, VSF_I2C_IRQ_MASK_MASTER_ERROR),
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE),
    HAL_DEMO_OPTION(VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK, VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK),
};

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_MASK, METHOD_SEARCH_ADDR,  "search"),
    HAL_DEMO_OPTION_EX(METHOD_MASK, METHOD_EEPROM,  "eeprom"),
    HAL_DEMO_OPTION_EX(METHOD_EEPROM_BIT_MASK, METHOD_EEPROM_8_BIT,  "8bits"),
    HAL_DEMO_OPTION_EX(METHOD_EEPROM_BIT_MASK, METHOD_EEPROM_16_BIT,  "16bits"),
};

HAL_DEMO_INIT(i2c, APP_I2C,
    "i2c-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [search|eeprom [8bits|16bits]" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE            i.e. vsf_hw_i2c0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT            repeat count" VSF_TRACE_CFG_LINEEND
    "  -c, --mode MODE                @ref vsf_i2c_mode_t" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT          test timeout" VSF_TRACE_CFG_LINEEND
    "  -f, --freq                     @ref clock_hz in vsf_i2c_cfg_t" VSF_TRACE_CFG_LINEEND
    "  -s, --search SIZE VALUE START-ADDR END-ADDR" VSF_TRACE_CFG_LINEEND
    "  -e, --eeprom ADDR SIZE DEVICE-ADDR CMD-DELAY WRITE-DELAY" VSF_TRACE_CFG_LINEEND,

    .test.method = METHOD_SEARCH_ADDR,

    .test.cfg.mode = APP_I2C_DEMO_CFG_MODE,
    .test.cfg.clock_hz = APP_I2C_DEMO_CLOCK_HZ,

    .test.eeprom.device_address = APP_I2C_DEMO_CFG_EEPROM_DEVICE_ADDRESS,
    .test.eeprom.eeprom_address = APP_I2C_DEMO_CFG_EEPROM_ADDRESS,
    .test.eeprom.write_addr.cmd = VSF_I2C_CMD_START    | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_NO_STOP_RESTART | VSF_I2C_CMD_7_BITS,
    .test.eeprom.write_data.cmd = VSF_I2C_CMD_NO_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP            | VSF_I2C_CMD_7_BITS,
    .test.eeprom.read_addr.cmd  = VSF_I2C_CMD_START    | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_RESTART         | VSF_I2C_CMD_7_BITS,
    .test.eeprom.read_data.cmd  = VSF_I2C_CMD_START    | VSF_I2C_CMD_READ  | VSF_I2C_CMD_STOP            | VSF_I2C_CMD_7_BITS,

    .test.search.start_address  = APP_I2C_DEMO_CFG_SEARCH_ADDRESS_START,
    .test.search.end_address    = APP_I2C_DEMO_CFG_SEARCH_ADDRESS_END,
    .test.search.buffer         = APP_I2C_DEMO_SEARCH_DATA_ARRAY,
    .test.search.request_size   = APP_I2C_DEMO_SEARCH_DATA_SIZE,
    .test.search.cmd            = APP_I2C_DEMO_CFG_SEARCH_CMD,

    .init_has_cfg               = true,
    .device_init                = (hal_init_fn_t       )vsf_i2c_init,
    .device_fini                = (hal_fini_fn_t       )vsf_i2c_fini,
    .device_enable              = (hal_enable_fn_t     )vsf_i2c_enable,
    .device_disable             = (hal_disable_fn_t    )vsf_i2c_disable,
    .device_irq_enable          = (hal_irq_enable_fn_t )vsf_i2c_irq_enable,
    .device_irq_disable         = (hal_irq_disable_fn_t)vsf_i2c_irq_disable,
    .mode.options               = __mode_options,
    .mode.cnt                   = dimof(__mode_options),
    .irq.options                = __irq_options,
    .irq.cnt                    = dimof(__irq_options),
    .method.options             = __method_options,
    .method.cnt                 = dimof(__method_options),
);

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static bool __i2c_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    i2c_test_t *test = vsf_container_of(hal_test, i2c_test_t, use_as__hal_test_t);
    i2c_demo_t *demo = (i2c_demo_t *)test->demo;
    VSF_ASSERT(demo != NULL);
    const i2c_demo_const_t *demo_const_ptr = (const i2c_demo_const_t *)demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    VSF_ASSERT(test->device != NULL);

    if (test->verbose >= 2) {
        hal_options_trace(VSF_TRACE_DEBUG, "vsf_i2c_cfg_t cfg = { .mode = ", __mode_options, dimof(__mode_options), test->cfg.mode);
        vsf_trace_debug(", .clock_hz = %d, .isr = { .prio = vsf_arch_prio_%u }};" VSF_TRACE_CFG_LINEEND, test->cfg.clock_hz, hal_demo_arch_prio_to_num(test->cfg.isr.prio));
    }

    vsf_i2c_capability_t cap = vsf_i2c_capability(test->device);

    if ((test->method & METHOD_MASK) == METHOD_SEARCH_ADDR) {
        if (((test->search.cmd & VSF_I2C_CMD_STOP_RESTART_MASK) == VSF_I2C_CMD_RESTART) && !cap.support_restart) {
            vsf_trace_error("the i2c device does not support restart!" VSF_TRACE_CFG_LINEEND);
            return false;
        }
        if (((test->search.cmd & VSF_I2C_CMD_STOP_RESTART_MASK) == VSF_I2C_CMD_NO_STOP_RESTART) && !cap.support_no_stop_restart) {
            vsf_trace_error("the i2c device does not support not stop and no restart!" VSF_TRACE_CFG_LINEEND);
            return false;
        }
        if (((test->search.cmd & VSF_I2C_CMD_STOP_RESTART_MASK) == VSF_I2C_CMD_NO_STOP_RESTART) && !cap.support_no_stop_restart) {
            vsf_trace_error("the i2c device does not support not stop and no restart!" VSF_TRACE_CFG_LINEEND);
            return false;
        }

        test->address = test->search.start_address;
        test->request.count = test->search.request_size;
        test->request.cmd = test->search.cmd;
        test->request.buffer = test->search.buffer;
    } else {
        uint16_t addr_size = ((test->method & METHOD_EEPROM_BIT_MASK) == METHOD_EEPROM_8_BIT) ? 1 : 2;

        test->eeprom.write_addr.count = addr_size;
        test->eeprom.write_addr.buffer = (uint8_t *) & test->eeprom.eeprom_address;
        test->eeprom.write_addr.delay_ms = test->eeprom.command_delay;

        test->eeprom.write_data.count = test->send.size;
        test->eeprom.write_data.buffer = test->send.buffer;
        test->eeprom.write_data.delay_ms = test->eeprom.write_delay;

        test->eeprom.read_addr.count = addr_size;
        test->eeprom.read_addr.buffer = (uint8_t *)&test->eeprom.eeprom_address;
        test->eeprom.read_addr.delay_ms = test->eeprom.command_delay;

        test->eeprom.read_data.count = test->recv.size;
        test->eeprom.read_data.buffer = test->recv.buffer;
        test->eeprom.read_data.delay_ms = test->eeprom.command_delay;

        test->request = test->eeprom.seq[0];
        test->address = test->eeprom.device_address;
    }

    test->expected_irq_mask =   VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                              | VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                              | VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT;
    test->irq_mask = test->expected_irq_mask;
    test->cfg.isr.handler_fn = __i2c_irq_handler;
    test->cfg.isr.target_ptr = test;

    test->hal_cfg = (void *)&test->cfg;

    return true;
}

static void __i2c_irq_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    i2c_test_t *test = (i2c_test_t *)target_ptr;
    VSF_ASSERT(test != NULL);
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    if (!hal_test_irq_check(&test->use_as__hal_test_t, irq_mask)) {
        return ;
    }

    if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        vsf_eda_post_evt(eda, VSF_EVT_I2C_WAIT_ISR_ACK);
    }

    if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT) {
        vsf_eda_post_evt(eda, VSF_EVT_I2C_WAIT_ISR_NAK);
    }
}

static vsf_err_t __i2c_demo_request(i2c_test_t *test)
{
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(test->address <= 0x7F);

    vsf_err_t err = vsf_i2c_master_request(test->device,
                                           test->address,
                                           test->request.cmd,
                                           test->request.count,
                                           test->request.buffer);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_i2c_master_request(&%s, 0x%08x/*addr*/, ", test->device_name, test->address);
        hal_options_trace(VSF_TRACE_DEBUG, "", __cmd_options, dimof(__cmd_options), test->request.cmd);
        vsf_trace_debug(", 0x%04x/*count*/, %p/*buff*/);" VSF_TRACE_CFG_LINEEND, test->request.count, test->request.buffer);
    }
    if (err != VSF_ERR_NONE) {
        vsf_trace_error("i2c: vsf_i2c_master_request faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }

    return err;
}

static void __i2c_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    i2c_test_t *test = vsf_container_of(eda, i2c_test_t, teda);
    vsf_i2c_t * i2c_ptr = test->device;
    VSF_ASSERT(i2c_ptr != NULL);
    i2c_demo_t *demo = (i2c_demo_t *)test->demo;
    VSF_ASSERT(demo != NULL);
    const i2c_demo_const_t *demo_const_ptr = (const i2c_demo_const_t *)demo->demo_const_ptr;
    VSF_ASSERT(demo_const_ptr != NULL);

    switch (evt) {
    case VSF_EVT_HAL_TEST_START:
        if ((test->method & METHOD_MASK) == METHOD_SEARCH_ADDR) {
            test->address = test->search.start_address;
        } else {
            test->eeprom.seq_index = 0;
        }

    case VSF_EVT_HAL_TEST_RUN:
        if (VSF_ERR_NONE != __i2c_demo_request(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
            break;
        }
        vsf_teda_set_timer_ms(test->timeout_ms.test);
        break;

    case VSF_EVT_I2C_WAIT_ISR_ACK:
        vsf_trace_info("i2c find device in 0x%02x" VSF_TRACE_CFG_LINEEND, test->address);
        // fall through

    case VSF_EVT_I2C_WAIT_ISR_NAK:
        vsf_teda_cancel_timer();
        if ((test->method & METHOD_MASK) == METHOD_SEARCH_ADDR) {
            if (++test->address <= test->search.end_address) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_RUN);
            } else {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_END);
            }
        } else {
            if (++test->eeprom.seq_index < dimof(test->eeprom.seq)) {
                test->request = test->eeprom.seq[test->eeprom.seq_index];
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_RUN);
            } else {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_END);
            }
        }
        break;

    case VSF_EVT_TIMER:
        vsf_trace_error("i2c request timeout" VSF_TRACE_CFG_LINEEND);
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __i2c_parser_args(hal_test_t * hal_test, int argc, char *argv[])
{
    i2c_test_t *test = (i2c_test_t *)hal_test;

    vsf_err_t err = VSF_ERR_NONE;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::m:r:d:c:p:i:t:f:s:e:";
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
        { "search",     required_argument, NULL, 's' },
        { "eeprom",     required_argument, NULL, 'e' },
        { NULL,     0,                  NULL, '\0' },
    };

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch(c) {
        case 's':
            if ((optind + 2 < argc) && (argv[optind][0] != '-') && (argv[optind + 1][0] != '-') && (argv[optind + 2][0] != '-')) {
                test->search.start_address = strtol(optarg, NULL, 0);
                test->search.end_address = strtol(argv[optind++], NULL, 0);
                if (test->search.start_address > test->search.end_address) {
                    vsf_trace_error("search start address(%u) is greater than the end address(%u)" VSF_TRACE_CFG_LINEEND,
                                    test->search.start_address, test->search.end_address);
                    return VSF_ERR_FAIL;
                }
                if (test->search.end_address > 0x7F) {
                    vsf_trace_error("search end address(%u) is too large" VSF_TRACE_CFG_LINEEND, test->search.end_address);
                    return VSF_ERR_FAIL;
                }

                int data = strtol(argv[optind++], NULL, 0);
                test->search.request_size = strtol(argv[optind++], NULL, 0);
                if (test->search.request_size > dimof(test->search.buffer)) {
                    vsf_trace_error("search request size(%u) > %u" VSF_TRACE_CFG_LINEEND, test->search.request_size, dimof(test->search.buffer));
                    return VSF_ERR_FAIL;
                }
                memset(test->search.buffer, data, test->search.request_size);

                if ((optind < argc) && (argv[optind][0] != '-')) {
                    uint32_t cmd_value = 0;
                    uint32_t cmd_mask = 0;
                    if (!hal_options_get_value(argc, argv, __cmd_options, dimof(__cmd_options), &cmd_value, &cmd_mask)) {
                        vsf_trace_error("cmd config error!" VSF_TRACE_CFG_LINEEND);
                        return VSF_ERR_FAIL;
                    }
                    test->search.cmd = (test->search.cmd & ~cmd_mask) | cmd_value;
                }
            } else {
                vsf_trace_error("search arg error: start-addr end-addr data size" VSF_TRACE_CFG_LINEEND);
                return VSF_ERR_FAIL;
            }
            break;
        case 'e':
            if ((optind + 4 < argc) && (argv[optind][0] != '-') && (argv[optind + 1][0] != '-') &&
                    (argv[optind + 2][0] != '-') && (argv[optind + 3][0] != '-') && (argv[optind + 4][0] != '-')) {
                uint16_t address = strtol(optarg, NULL, 0);
                test->eeprom.eeprom_address = cpu_to_be16(address);

                size_t size = strtol(argv[optind++], NULL, 0);
                if (size <= 0) {
                    vsf_trace_error("buffer size cannot be 0" VSF_TRACE_CFG_LINEEND);
                    return VSF_ERR_FAIL;
                }
                test->send.size = size;
                test->recv.size = size;

                test->eeprom.device_address = strtol(argv[optind++], NULL, 0);
                if (test->eeprom.device_address > 0x7F) {
                    vsf_trace_error("eeprom device address(%u) is too large" VSF_TRACE_CFG_LINEEND, test->eeprom.device_address);
                    return VSF_ERR_FAIL;
                }

                test->eeprom.eeprom_address = strtol(argv[optind++], NULL, 0);
                if (test->eeprom.eeprom_address > 0x7F) {
                    vsf_trace_error("eeprom device address(%u) is too large" VSF_TRACE_CFG_LINEEND, test->eeprom.device_address);
                    return VSF_ERR_FAIL;
                }

                test->eeprom.command_delay = strtol(argv[optind++], NULL, 0);
                test->eeprom.write_delay = strtol(argv[optind++], NULL, 0);
            } else {
                vsf_trace_error("eeprom args error" VSF_TRACE_CFG_LINEEND);
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

    if ((test->method & METHOD_EEPROM_BIT_MASK) == METHOD_EEPROM_8_BIT) {
        *((uint8_t *)&test->eeprom.eeprom_address) = test->eeprom.eeprom_address;
    }

    return err;
}

int i2c_main(int argc, char *argv[])
{
    return hal_main(&__i2c_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__i2c_demo.use_as__hal_demo_t);
}
#endif

#endif
