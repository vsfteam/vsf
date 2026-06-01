/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef __VSF_TEST_I2C_H__
#define __VSF_TEST_I2C_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#define VSF_TEST_I2C_CASE_MAX_COUNT     16

#ifndef VSF_TEST_I2C_EEPROM_RW_ENABLE
#   define VSF_TEST_I2C_EEPROM_RW_ENABLE        DISABLED
#endif

#ifndef VSF_TEST_I2C_BUS_SCAN_ENABLE
#   define VSF_TEST_I2C_BUS_SCAN_ENABLE         DISABLED
#endif

#ifndef VSF_TEST_I2C_EEPROM_PAGE_ENABLE
#   define VSF_TEST_I2C_EEPROM_PAGE_ENABLE      ENABLED
#endif

#ifndef VSF_TEST_I2C_SLAVE_ENABLE
#   define VSF_TEST_I2C_SLAVE_ENABLE            ENABLED
#endif

#ifndef VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE
#   define VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE   ENABLED
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_ENABLE
#   define VSF_TEST_I2C_SLAVE_FIFO_ENABLE       ENABLED
#endif

#ifndef VSF_TEST_I2C_ENABLE
#   define VSF_TEST_I2C_ENABLE    ENABLED
#endif

#include "suite/vsf_test_i2c_bus_scan.h"
#include "suite/vsf_test_i2c_eeprom_page.h"
#include "suite/vsf_test_i2c_eeprom_rw.h"
#include "suite/vsf_test_i2c_eeprom_rw_fifo.h"
#include "suite/vsf_test_i2c_slave.h"
#include "suite/vsf_test_i2c_slave_fifo.h"

typedef union {
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
    vsf_test_i2c_bus_scan_data_t i2c_bus_scan;
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
    vsf_test_i2c_eeprom_page_data_t i2c_eeprom_page;
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
    vsf_test_i2c_eeprom_rw_data_t i2c_eeprom_rw;
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
    vsf_test_i2c_eeprom_rw_fifo_data_t i2c_eeprom_rw_fifo;
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
    vsf_test_i2c_slave_data_t i2c_slave;
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
    vsf_test_i2c_slave_fifo_data_t i2c_slave_fifo;
#endif
} vsf_test_i2c_data_t;
#ifdef __cplusplus
}
#endif
#endif /* __VSF_TEST_I2C_H__ */
/* EOF */