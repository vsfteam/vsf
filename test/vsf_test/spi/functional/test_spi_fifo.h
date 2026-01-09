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
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                          *
 ****************************************************************************/

#ifndef __TEST_SPI_FIFO_H__
#define __TEST_SPI_FIFO_H__

/*============================ INCLUDES ======================================*/

#include "../test_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

// FIFO 传输测试
extern void vsf_test_spi_fifo_transfer_null_ptr(void);
extern void vsf_test_spi_fifo_transfer_basic(void);
extern void vsf_test_spi_fifo_transfer_offset(void);
extern void vsf_test_spi_fifo_polling_transfer(void);
extern void vsf_test_spi_fifo_irq_transfer(void);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_SPI_FIFO_H__ */
/* EOF */
