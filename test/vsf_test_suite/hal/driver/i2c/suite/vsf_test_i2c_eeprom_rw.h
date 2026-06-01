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

#ifndef __TEST_I2C_EEPROM_RW_H__
#define __TEST_I2C_EEPROM_RW_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"
/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_EEPROM_RW_WRITE_BUF_SIZE
#   define VSF_TEST_I2C_EEPROM_RW_WRITE_BUF_SIZE        17
#endif
#ifndef VSF_TEST_I2C_EEPROM_RW_PATTERN_BYTE
#   define VSF_TEST_I2C_EEPROM_RW_PATTERN_BYTE                0xA0
#endif

#ifndef VSF_TEST_I2C_EEPROM_RW_PRIO
#   define VSF_TEST_I2C_EEPROM_RW_PRIO              vsf_arch_prio_1
#endif

#ifndef VSF_TEST_I2C_EEPROM_RW_READ_BUF_SIZE
#   define VSF_TEST_I2C_EEPROM_RW_READ_BUF_SIZE        16
#endif

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t irq_mask;
    uint8_t write_buf[VSF_TEST_I2C_EEPROM_RW_WRITE_BUF_SIZE];
    uint8_t read_buf[VSF_TEST_I2C_EEPROM_RW_READ_BUF_SIZE];
} vsf_test_i2c_eeprom_rw_data_t;
#endif

#ifndef VSF_TEST_I2C_EEPROM_RW_CMD_2
#   define VSF_TEST_I2C_EEPROM_RW_CMD_2                            (VSF_I2C_CMD_RESTART|VSF_I2C_CMD_STOP|VSF_I2C_CMD_READ|VSF_I2C_CMD_7_BITS)
#endif

#ifndef VSF_TEST_I2C_EEPROM_RW_CMD_1
#   define VSF_TEST_I2C_EEPROM_RW_CMD_1                            (VSF_I2C_CMD_START|VSF_I2C_CMD_WRITE|VSF_I2C_CMD_NO_STOP|VSF_I2C_CMD_7_BITS)
#endif

#ifndef VSF_TEST_I2C_EEPROM_RW_WRITE_CMD
#   define VSF_TEST_I2C_EEPROM_RW_WRITE_CMD                        (VSF_I2C_CMD_START|VSF_I2C_CMD_STOP|VSF_I2C_CMD_WRITE|VSF_I2C_CMD_7_BITS)
#endif

#ifndef VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS
#   define VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS                     100
#endif

#ifndef VSF_TEST_I2C_CLOCK_HZ
#   define VSF_TEST_I2C_CLOCK_HZ                                   100000
#endif

#ifndef VSF_TEST_I2C_TIMEOUT_MS
#   define VSF_TEST_I2C_TIMEOUT_MS                                 1000
#endif

/*============================ PROTOTYPES ====================================*/

#ifndef VSF_TEST_I2C_ACK_POLL_SUB_TIMEOUT_MS
#   define VSF_TEST_I2C_ACK_POLL_SUB_TIMEOUT_MS          10
#endif

void vsf_test_i2c_eeprom_rw_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  i2c_idx;
    uint8_t  eeprom_addr;
    uint8_t  mem_addr;
    uint8_t  data_len;
} vsf_test_i2c_eeprom_rw_params_t;

struct vsf_test_i2c_eeprom_rw_s;
extern const struct vsf_test_i2c_eeprom_rw_s vsf_test_i2c_eeprom_rw;

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED */

#endif /* __TEST_I2C_EEPROM_RW_H__ */
/* EOF */
