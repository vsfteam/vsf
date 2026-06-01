/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
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

#include "vsf_test_i2c_bus_scan.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
const struct vsf_test_i2c_bus_scan_s {
    vsf_test_suite_t hdr;
    vsf_test_i2c_bus_scan_params_t          params[VSF_TEST_I2C_BUS_SCAN_CASE_COUNT];
} vsf_test_i2c_bus_scan = {
    .hdr = {
        .name            = "i2c_bus_scan",
        .jmp_fn          = vsf_test_i2c_bus_scan_run,
        .case_count      = VSF_TEST_I2C_BUS_SCAN_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_i2c_bus_scan_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_I2C,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_I2C_BUS_SCAN_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED

/*============================ LOCAL FUNCTIONS ===============================*/

static void __bus_scan_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                            vsf_i2c_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(i2c_ptr);
    vsf_test_suite_data.i2c.i2c_bus_scan.irq_mask = irq_mask;
}

static int __bus_scan_once(vsf_test_suite_t *suite,
                            vsf_gpio_i2c_t *gpio_i2c, uint8_t scl, uint8_t sda)
{
    gpio_i2c->scl_pin = scl;
    gpio_i2c->sda_pin = sda;

    vsf_i2c_t *i2c = (vsf_i2c_t *)gpio_i2c;
    vsf_i2c_init(i2c, &(vsf_i2c_cfg_t){
        .mode = VSF_TEST_I2C_BUS_SCAN_MODE, .clock_hz = VSF_TEST_I2C_CLOCK_HZ,
        .isr  = {.handler_fn = __bus_scan_isr, .target_ptr = suite},
    });
    vsf_i2c_enable(i2c);
    vsf_i2c_irq_enable(i2c, VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                             | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    int found = 0;
    for (uint8_t addr = VSF_TEST_I2C_SCAN_ADDR_FIRST; addr <= VSF_TEST_I2C_SCAN_ADDR_LAST; addr++) {
        vsf_test_suite_data.i2c.i2c_bus_scan.irq_mask = 0;
        vsf_i2c_master_request(i2c, addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_STOP
            | VSF_I2C_CMD_7_BITS | VSF_I2C_CMD_WRITE,
            0, NULL);
        if (!(vsf_test_suite_data.i2c.i2c_bus_scan.irq_mask & VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK)) {
            found++;
        }
    }

    vsf_i2c_irq_disable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
        | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);
    while (fsm_rt_cpl != vsf_i2c_disable(i2c));
    vsf_i2c_fini(i2c);

    return found;
}



/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_i2c_bus_scan_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_i2c_bus_scan_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_I2C, i2c);
    vsf_gpio_i2c_t **gpio_i2c_arr = (vsf_gpio_i2c_t **)fixture;
    vsf_gpio_i2c_t *gpio_i2c = gpio_i2c_arr[p->idx];

    uint8_t scl = p->scl_pin;
    uint8_t sda = p->sda_pin;

    VSF_TEST_TRACE_INFO("i2c_bus_scan:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);
    VSF_TEST_TRACE_DEBUG("i2c_bus_scan:scl=GP%u sda=GP%u" VSF_TRACE_CFG_LINEEND,
                          scl, sda);
    int found = __bus_scan_once(suite, gpio_i2c, scl, sda);
    VSF_TEST_TRACE_DEBUG("i2c_bus_scan:devices found=%d" VSF_TRACE_CFG_LINEEND, found);

    bool swapped = false;
    if (found == 0) {
        VSF_TEST_TRACE_DEBUG("i2c_bus_scan:no response, trying swapped SCL/SDA"
                       VSF_TRACE_CFG_LINEEND);
        found = __bus_scan_once(suite, gpio_i2c, sda, scl);
        VSF_TEST_TRACE_DEBUG("i2c_bus_scan:devices found (swapped)=%d" VSF_TRACE_CFG_LINEEND,
                       found);
        swapped = (found > 0);
    }

    if (swapped) {
        VSF_TEST_TRACE_WARNING("i2c_bus_scan:warn:SCL/SDA appear swapped (SCL=GP%u SDA=GP%u)"
                       VSF_TRACE_CFG_LINEEND, scl, sda);
    } else if (found == 0) {
        VSF_TEST_TRACE_WARNING("i2c_bus_scan:warn:no device found on either pin order"
                       VSF_TRACE_CFG_LINEEND);
    }

    VSF_TEST_ASSERT(found > 0);
    VSF_TEST_TRACE_INFO("i2c_bus_scan:pass devices=%d" VSF_TRACE_CFG_LINEEND, found);
}

#endif /* VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED */

/* EOF */