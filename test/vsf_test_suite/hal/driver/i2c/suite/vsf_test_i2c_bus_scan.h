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

#ifndef __TEST_I2C_BUS_SCAN_H__
#define __TEST_I2C_BUS_SCAN_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_CLOCK_HZ
#   define VSF_TEST_I2C_CLOCK_HZ                100000
#endif

#ifndef VSF_TEST_I2C_BUS_SCAN_MODE
#   define VSF_TEST_I2C_BUS_SCAN_MODE           0
#endif

#ifndef VSF_TEST_I2C_SCAN_ADDR_FIRST
#   define VSF_TEST_I2C_SCAN_ADDR_FIRST         0x08  /* I2C spec reserves 0x00-0x07 */
#endif
#ifndef VSF_TEST_I2C_SCAN_ADDR_LAST
#   define VSF_TEST_I2C_SCAN_ADDR_LAST          0x77
#endif

// Shared I2C EEPROM master mode (protocol standard: master, 7-bit, 100kHz)
#ifndef VSF_TEST_I2C_EEPROM_MASTER_MODE
#   define VSF_TEST_I2C_EEPROM_MASTER_MODE      (VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE)
#endif

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t irq_mask;
} vsf_test_i2c_bus_scan_data_t;
#endif

#ifndef VSF_TEST_I2C_BUS_SCAN_WRITE_CMD
#   define VSF_TEST_I2C_BUS_SCAN_WRITE_CMD                         (VSF_I2C_CMD_START|VSF_I2C_CMD_STOP|VSF_I2C_CMD_7_BITS|VSF_I2C_CMD_WRITE)
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_i2c_bus_scan_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  scl_pin;
    uint8_t  sda_pin;
} vsf_test_i2c_bus_scan_params_t;

struct vsf_test_i2c_bus_scan_s;
extern const struct vsf_test_i2c_bus_scan_s vsf_test_i2c_bus_scan;

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED */

#endif /* __TEST_I2C_BUS_SCAN_H__ */
/* EOF */
