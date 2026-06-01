/******************************************************************************
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
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef __VSF_TEST_SPI_H__
#define __VSF_TEST_SPI_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_SPI_LOOPBACK_ENABLE
#   define VSF_TEST_SPI_LOOPBACK_ENABLE         ENABLED
#endif

#ifndef VSF_TEST_SPI_ASYNC_ENABLE
#   define VSF_TEST_SPI_ASYNC_ENABLE            ENABLED
#endif

#ifndef VSF_TEST_SPI_ENABLE
#   define VSF_TEST_SPI_ENABLE    ENABLED
#endif

#include "suite/vsf_test_spi_async.h"

#include "suite/vsf_test_spi_loopback.h"
typedef union {
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
    vsf_test_spi_async_data_t spi_async;
#endif
} vsf_test_spi_data_t;
#ifdef __cplusplus
}
#endif
#endif /* __VSF_TEST_SPI_H__ */
/* EOF */