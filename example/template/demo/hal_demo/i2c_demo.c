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

/*============================ MACROS ========================================*/

#ifdef APP_I2C_DEMO_CFG_I2C_PREFIX
#   undef VSF_I2C_CFG_PREFIX
#   define VSF_I2C_CFG_PREFIX                           APP_I2C_DEMO_CFG_I2C_PREFIX
#endif

#ifndef APP_I2C_DEMO_CFG_I2C
#   define APP_I2C_DEMO_CFG_I2C                         (vsf_i2c_t *)&vsf_hw_i2c0
#endif

#ifndef APP_I2C_DEMO_CFG_SEARCH_DEVICE_DEMO
#   define  APP_I2C_DEMO_CFG_SEARCH_DEVICE_DEMO         DISABLED
#endif

#ifndef APP_I2C_DEMO_CFG_SEARCH_ADDRESS_START
#   define APP_I2C_DEMO_CFG_SEARCH_ADDRESS_START        0x0C
#endif

#ifndef APP_I2C_DEMO_CFG_SEARCH_ADDRESS_END
#   define APP_I2C_DEMO_CFG_SEARCH_ADDRESS_END          0x70
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_DEMO
#   define  APP_I2C_DEMO_CFG_EEPROM_DEMO                ENABLED
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_DEVICE_ADDRESS
#   define APP_I2C_DEMO_CFG_EEPROM_DEVICE_ADDRESS       0x50
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_ADDRESS
#   define APP_I2C_DEMO_CFG_EEPROM_ADDRESS              0x0001
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_BUFFER_SIZE
#   define APP_I2C_DEMO_CFG_EEPROM_BUFFER_SIZE          12
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_DELAY_MS
#   define APP_I2C_DEMO_CFG_EEPROM_DELAY_MS             10      // WriteCycleTiming
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_SET_WRITE_ADDR_CMD
#   define APP_I2C_DEMO_CFG_EEPROM_SET_WRITE_ADDR_CMD   (VSF_I2C_CMD_WRITE | VSF_I2C_CMD_START)
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_WRITE_DATA_CMD
#   define APP_I2C_DEMO_CFG_EEPROM_WRITE_DATA_CMD       (VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP)
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_SET_READ_ADDR_CMD
//#   define APP_I2C_DEMO_CFG_EEPROM_SET_READ_ADDR_CMD    (VSF_I2C_CMD_WRITE | VSF_I2C_CMD_START)
#   define APP_I2C_DEMO_CFG_EEPROM_SET_READ_ADDR_CMD    (VSF_I2C_CMD_WRITE | VSF_I2C_CMD_START | VSF_I2C_CMD_STOP)
#endif

#ifndef APP_I2C_DEMO_CFG_EEPROM_READ_DATA_CMD
#   define APP_I2C_DEMO_CFG_EEPROM_READ_DATA_CMD        (VSF_I2C_CMD_START | VSF_I2C_CMD_READ | VSF_I2C_CMD_STOP)
#endif

#ifndef APP_I2C_DEMO_ISR_PRIO
#   define APP_I2C_DEMO_ISR_PRIO                        vsf_arch_prio_2
#endif

#ifndef APP_I2C_DEMO_PRIO
#   define APP_I2C_DEMO_PRIO                           vsf_prio_0
#endif

#ifndef APP_I2C_DEMO_CFG_MODE
#   define APP_I2C_DEMO_CFG_MODE                        (VSF_I2C_MODE_MASTER | VSF_I2C_SPEED_STANDARD_MODE | VSF_I2C_ADDR_7_BITS)
#endif

#ifndef APP_I2C_DEMO_CLOCK_HZ
#   define APP_I2C_DEMO_CLOCK_HZ                        1000
#endif


#ifndef APP_I2C_DEMO_TEST_DATA_ARRAY
#   define APP_I2C_DEMO_TEST_DATA_ARRAY                 {0xFF}
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum i2c_demo_evt_t {
    VSF_EVT_EEPROM_WRTIE_ADDR_CPL       = VSF_EVT_USER + 0,
    VSF_EVT_EEPROM_WRTIE_DATA_CPL       = VSF_EVT_USER + 1,
    VSF_EVT_EEPROM_READ_DATA            = VSF_EVT_USER + 2,
    VSF_EVT_EEPROM_READ_DATA_CPL        = VSF_EVT_USER + 3,
    VSF_EVT_EEPROM_NAK                  = VSF_EVT_USER + 4,
} i2c_demo_evt_t;

typedef struct app_i2c_demo_t {
    uint16_t address;

#if APP_I2C_DEMO_CFG_SEARCH_DEVICE_DEMO == ENABLED
    uint16_t start_address;
    uint16_t end_address;
#endif

#if APP_I2C_DEMO_CFG_EEPROM_DEMO == ENABLED
    vsf_teda_t teda;
    i2c_demo_evt_t next_evt;
    uint8_t eeprom_address[2];
    uint8_t send_buffer[APP_I2C_DEMO_CFG_EEPROM_BUFFER_SIZE];
    uint8_t recv_buffer[APP_I2C_DEMO_CFG_EEPROM_BUFFER_SIZE];
#endif
} app_i2c_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

app_i2c_demo_t __app_i2c_demo = {
#if APP_I2C_DEMO_CFG_EEPROM_DEMO == ENABLED
    .eeprom_address = {
        APP_I2C_DEMO_CFG_EEPROM_ADDRESS >> 0x08,
        APP_I2C_DEMO_CFG_EEPROM_ADDRESS & 0xFF
    },
#else
    0
#endif
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __i2c_demo_init(vsf_i2c_t *i2c_ptr,
                                 vsf_i2c_isr_t *irs_ptr,
                                 vsf_i2c_irq_mask_t mask)
{
    VSF_ASSERT(i2c_ptr != NULL);
    VSF_ASSERT(irs_ptr != NULL);
    VSF_ASSERT((mask & ~VSF_I2C_IRQ_MASK_MASTER_ALL) == 0);

    vsf_i2c_cfg_t i2c_cfg = {
        .mode     = APP_I2C_DEMO_CFG_MODE,
        .clock_hz = APP_I2C_DEMO_CLOCK_HZ,
        .isr      = *irs_ptr,
    };

    vsf_err_t init_result = vsf_i2c_init(i2c_ptr, &i2c_cfg);
    if (init_result != VSF_ERR_NONE) {
        return init_result;
    }

    while (fsm_rt_cpl != vsf_i2c_enable(i2c_ptr));

    if (mask & VSF_I2C_IRQ_MASK_MASTER_ALL) {
        vsf_i2c_irq_enable(i2c_ptr, mask);
    }

    return VSF_ERR_NONE;
}

static void __i2c_demo_deinit(vsf_i2c_t *i2c_ptr)
{
    VSF_ASSERT(i2c_ptr != NULL);
    vsf_i2c_irq_disable(i2c_ptr, VSF_I2C_IRQ_MASK_MASTER_ALL);
    while (fsm_rt_cpl != vsf_i2c_disable(i2c_ptr));
}

#if APP_I2C_DEMO_CFG_SEARCH_DEVICE_DEMO == ENABLED
static void __i2c_search_next(app_i2c_demo_t *i2c_demo_ptr, vsf_i2c_t *i2c_ptr)
{
#ifdef APP_I2C_DEMO_TEST_DATA_ARRAY
    static uint8_t __reqeust_data[] = APP_I2C_DEMO_TEST_DATA_ARRAY;
    uint16_t request_data_len = dimof(__reqeust_data);
#else
    uint8_t *__reqeust_data = NULL;
    uint16_t request_data_len = 0;
#endif

    if (i2c_demo_ptr->start_address <= i2c_demo_ptr->end_address) {
        vsf_i2c_master_request(i2c_ptr, i2c_demo_ptr->start_address,
                               VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP,
                               request_data_len, __reqeust_data);
    } else {
        __i2c_demo_deinit(i2c_ptr);
    }
}

static void __i2c_search_irq_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    app_i2c_demo_t *i2c_demo_ptr = (app_i2c_demo_t *)target_ptr;

    if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        vsf_trace_debug("i2c address :%d transfer complete interrutp" VSF_TRACE_CFG_LINEEND, i2c_demo_ptr->start_address);
    }

    i2c_demo_ptr->start_address++;
    __i2c_search_next(i2c_demo_ptr, i2c_ptr);
}

static void __i2c_address_search(app_i2c_demo_t *i2c_demo_ptr, vsf_i2c_t *i2c_ptr)
{
    VSF_ASSERT(i2c_demo_ptr != NULL);
    VSF_ASSERT(i2c_ptr != NULL);

    vsf_trace_debug("i2c search address, range: 0x%02x, 0x%02x" VSF_TRACE_CFG_LINEEND, i2c_demo_ptr->start_address, i2c_demo_ptr->end_address);

    vsf_i2c_isr_t isr = {
        .handler_fn = __i2c_search_irq_handler,
        .target_ptr = i2c_demo_ptr,
        .prio = APP_I2C_DEMO_ISR_PRIO,
    };

    vsf_i2c_irq_mask_t mask =  VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                             | VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT
                             | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;

    vsf_err_t result = __i2c_demo_init(i2c_ptr, &isr, mask);
    VSF_ASSERT(result == VSF_ERR_NONE);

    __i2c_search_next(i2c_demo_ptr, i2c_ptr);
}
#endif

volatile bool is_activity;

#if APP_I2C_DEMO_CFG_EEPROM_DEMO == ENABLED
static void __eeprom_irq_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    app_i2c_demo_t *i2c_demo_ptr = (app_i2c_demo_t *)target_ptr;

    if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        vsf_eda_post_evt(&i2c_demo_ptr->teda.use_as__vsf_eda_t, i2c_demo_ptr->next_evt);
    } else {
        vsf_eda_post_evt(&i2c_demo_ptr->teda.use_as__vsf_eda_t, VSF_EVT_EEPROM_NAK);
    }
}

    vsf_err_t result;
static void __i2c_eeprom_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_i2c_t * i2c_ptr = APP_I2C_DEMO_CFG_I2C;
    uint16_t device_address = __app_i2c_demo.address;

    switch (evt) {
    case VSF_EVT_INIT: {
            vsf_i2c_isr_t isr = {
                .handler_fn = __eeprom_irq_handler,
                .target_ptr = &__app_i2c_demo,
                .prio = APP_I2C_DEMO_ISR_PRIO,
            };
            result = __i2c_demo_init(i2c_ptr, &isr, VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK |
                                                    VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT |
                                                    VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);
            VSF_ASSERT(result == VSF_ERR_NONE);

            for (int i = 0; i < dimof(__app_i2c_demo.send_buffer); i++) {
                __app_i2c_demo.send_buffer[i] = dimof(__app_i2c_demo.send_buffer) - i;
            }
            memset(__app_i2c_demo.recv_buffer, 0, sizeof(__app_i2c_demo.recv_buffer));
            __app_i2c_demo.next_evt = VSF_EVT_EEPROM_WRTIE_ADDR_CPL;
            result = vsf_i2c_master_request(i2c_ptr, device_address, APP_I2C_DEMO_CFG_EEPROM_SET_WRITE_ADDR_CMD,
                                            dimof(__app_i2c_demo.eeprom_address), __app_i2c_demo.eeprom_address);
            VSF_ASSERT(result == VSF_ERR_NONE);
        }
        break;

    case VSF_EVT_EEPROM_WRTIE_ADDR_CPL:
        __app_i2c_demo.next_evt = VSF_EVT_EEPROM_WRTIE_DATA_CPL;
        result = vsf_i2c_master_request(i2c_ptr, device_address, APP_I2C_DEMO_CFG_EEPROM_WRITE_DATA_CMD,
                               dimof(__app_i2c_demo.send_buffer), __app_i2c_demo.send_buffer);
        VSF_ASSERT(result == VSF_ERR_NONE);
        break;

    case VSF_EVT_EEPROM_WRTIE_DATA_CPL:
        vsf_teda_set_timer_ms(APP_I2C_DEMO_CFG_EEPROM_DELAY_MS);
        break;

    case VSF_EVT_TIMER:
        __app_i2c_demo.next_evt = VSF_EVT_EEPROM_READ_DATA;
        result = vsf_i2c_master_request(i2c_ptr, device_address, APP_I2C_DEMO_CFG_EEPROM_SET_READ_ADDR_CMD, 2, __app_i2c_demo.eeprom_address);
        VSF_ASSERT(result == VSF_ERR_NONE);
        break;

    case VSF_EVT_EEPROM_READ_DATA:
        __app_i2c_demo.next_evt = VSF_EVT_EEPROM_READ_DATA_CPL;
        result = vsf_i2c_master_request(i2c_ptr, device_address, APP_I2C_DEMO_CFG_EEPROM_READ_DATA_CMD, dimof(__app_i2c_demo.recv_buffer), __app_i2c_demo.recv_buffer);
        VSF_ASSERT(result == VSF_ERR_NONE);
        break;

    case VSF_EVT_EEPROM_READ_DATA_CPL:
        if(memcmp(__app_i2c_demo.send_buffer, __app_i2c_demo.recv_buffer, dimof(__app_i2c_demo.recv_buffer)) != 0) {
            vsf_trace_debug("read data error\r\n");
            for(int i = 0; i < dimof(__app_i2c_demo.recv_buffer); i++) {
                vsf_trace_debug("send/recv[%d]:0x%02x/0x%02x\r\n", i, __app_i2c_demo.send_buffer[i], __app_i2c_demo.recv_buffer[i]);
            }
        } else {
            vsf_trace_debug("read data completed\r\n");
        }
        __i2c_demo_deinit(i2c_ptr);
        break;

    case VSF_EVT_EEPROM_NAK:
        __i2c_demo_deinit(i2c_ptr);
        vsf_trace_debug("eeprom read or write nak\r\n");
        break;
    }
}
#endif


#if APP_USE_LINUX_DEMO == ENABLED
int i2c_main(int argc, char *argv[])
{
    if (argc >= 2) {
        __app_i2c_demo.address = atoi(argv[1]);
    } else {
        __app_i2c_demo.address = APP_I2C_DEMO_CFG_EEPROM_DEVICE_ADDRESS;
    }
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

#if APP_I2C_DEMO_CFG_SEARCH_DEVICE_DEMO == ENABLED
    __app_i2c_demo.start_address = APP_I2C_DEMO_CFG_SEARCH_ADDRESS_START;
    __app_i2c_demo.end_address = APP_I2C_DEMO_CFG_SEARCH_ADDRESS_END;
    __i2c_address_search(&__app_i2c_demo, APP_I2C_DEMO_CFG_I2C);
#endif

#if APP_I2C_DEMO_CFG_EEPROM_DEMO == ENABLED
    const vsf_eda_cfg_t cfg = {
        .fn.evthandler  = __i2c_eeprom_evthandler,
        .priority       = APP_I2C_DEMO_PRIO,
    };
    return vsf_teda_start(&__app_i2c_demo.teda, (vsf_eda_cfg_t*)&cfg);
#endif

    return 0;
}

#endif
