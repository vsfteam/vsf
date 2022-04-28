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

#ifndef APP_I2C_DEMO_CFG_ADDRESS_START
#   define APP_I2C_DEMO_CFG_ADDRESS_START               0x0C
#endif

#ifndef APP_I2C_DEMO_CFG_ADDRESS_END
#   define APP_I2C_DEMO_CFG_ADDRESS_END                 0x77
#endif

#ifndef APP_I2C_DEMO_PRIO
#   define APP_I2C_DEMO_PRIO                            vsf_arch_prio_2
#endif

#ifndef APP_I2C_DEMO_CFG_MODE
#   define APP_I2C_DEMO_CFG_MODE                        (I2C_MODE_MASTER | I2C_SPEED_STANDARD_MODE | I2C_ADDR_7_BITS)
#endif

#ifndef APP_I2C_DEMO_CLOCK_HZ
#   define APP_I2C_DEMO_CLOCK_HZ                        1000
#endif

#ifndef APP_I2C_DEMO_TEST_DATA_CNT
#   define APP_I2C_DEMO_TEST_DATA_CNT                   1
#endif

#ifndef APP_I2C_DEMO_TEST_DATA_ARRAY
#   define APP_I2C_DEMO_TEST_DATA_ARRAY                 {0xFF}
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct app_i2c_demo_t {
    uint16_t start_address;
    uint16_t end_address;
} app_i2c_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

app_i2c_demo_t __app_i2c_demo = {
    .start_address = APP_I2C_DEMO_CFG_ADDRESS_START,
    .end_address = APP_I2C_DEMO_CFG_ADDRESS_END,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __i2c_demo_init(vsf_i2c_t *i2c_ptr,
                                 vsf_i2c_isr_t *irs_ptr,
                                 em_i2c_irq_mask_t mask)
{
    VSF_ASSERT(i2c_ptr != NULL);
    VSF_ASSERT(irs_ptr != NULL);
    VSF_ASSERT((mask & ~I2C_IRQ_MASK_MASTER_ALL) == 0);

    i2c_cfg_t i2c_cfg = {
        .mode           = APP_I2C_DEMO_CFG_MODE,
        .clock_hz       = APP_I2C_DEMO_CLOCK_HZ,
        .isr            = *irs_ptr,
    };

    vsf_err_t init_result = VSF_I2C_INIT(i2c_ptr, &i2c_cfg);
    if (init_result != VSF_ERR_NONE) {
        return init_result;
    }

    while (fsm_rt_cpl != VSF_I2C_ENABLE(i2c_ptr));

    if (mask & I2C_IRQ_MASK_MASTER_ALL) {
        VSF_I2C_IRQ_ENABLE(i2c_ptr, mask);
    }

    return VSF_ERR_NONE;
}

static void __i2c_demo_deinit(vsf_i2c_t *i2c_ptr)
{
    VSF_ASSERT(i2c_ptr != NULL);
    VSF_I2C_IRQ_DISABLE(i2c_ptr, I2C_IRQ_MASK_MASTER_ALL);
    while (fsm_rt_cpl != VSF_I2C_DISABLE(i2c_ptr));
}

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
        VSF_I2C_MASTER_REQUEST(i2c_ptr, i2c_demo_ptr->start_address,
                               I2C_CMD_START | I2C_CMD_WRITE | I2C_CMD_STOP,
                               request_data_len, __reqeust_data);
    } else {
        __i2c_demo_deinit(i2c_ptr);
    }
}

static void __i2c_irq_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    app_i2c_demo_t *i2c_demo_ptr = (app_i2c_demo_t *)target_ptr;

    if (irq_mask & I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        vsf_trace_debug("i2c address :%d transfer complete interrutp" VSF_TRACE_CFG_LINEEND, i2c_demo_ptr->start_address);
    }

    i2c_demo_ptr->start_address++;
    __i2c_search_next(i2c_demo_ptr, i2c_ptr);
}

static void __i2c_address_search(app_i2c_demo_t *i2c_demo_ptr, vsf_i2c_t *i2c_ptr)
{
    VSF_ASSERT(i2c_demo_ptr != NULL);
    VSF_ASSERT(i2c_ptr != NULL);

    vsf_trace_debug("i2c search address, range: 0x%02x, 0x%02x", i2c_demo_ptr->start_address, i2c_demo_ptr->end_address);

    vsf_i2c_isr_t isr = {
        .handler_fn = __i2c_irq_handler,
        .target_ptr = i2c_demo_ptr,
        .prio = APP_I2C_DEMO_PRIO,
    };

    em_i2c_irq_mask_t mask =  I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                            | I2C_IRQ_MASK_MASTER_NACK_DETECT
                            | I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;

    vsf_err_t result = __i2c_demo_init(i2c_ptr, &isr, mask);
    VSF_ASSERT(result == VSF_ERR_NONE);

    __i2c_search_next(i2c_demo_ptr, i2c_ptr);
}


#if APP_USE_LINUX_DEMO == ENABLED
int i2c_main(int argc, char *argv[])
{
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

    __i2c_address_search(&__app_i2c_demo, APP_I2C_DEMO_CFG_I2C);

    return 0;
}

#endif
