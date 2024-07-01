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
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#ifndef __VSF_TEST_PORT_STDIO_H__
#    define __VSF_TEST_PORT_STDIO_H__

#    if VSF_USE_TEST == ENABLED && VSF_TEST_CFG_USE_STDIO_DATA_SYNC == ENABLED

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/**
 @brief Initialize external watchdog via stdio (requires assist device support)
 @param[in] wdt: a pointer to structure @ref vsf_test_wdt_t
 @param[in] timeout_ms: timeout (in milliseconds)
 */
extern void vsf_test_stdio_wdt_init(vsf_test_wdt_t *wdt, uint32_t timeout_ms);

/**
 @brief Reset watchdog timeout (requires assist device support)
 @param[in] wdt: a pointer to structure @ref vsf_test_wdt_t
 */
extern void vsf_test_stdio_wdt_feed(vsf_test_wdt_t *wdt);

/**
 @brief Reboot watchdog timeout (requires assist device support)
 */
extern void vsf_test_stdio_reboot(void);

/**
 @brief initialize the peripherals needed to synchronize the data
 @param[in] data: a pointer to structure @ref vsf_test_data_t

 @note Its default implementation is an empty weak function, which the user can
 re-implement as needed. After initialization, we need to make sure that printf
 and scanf can be used properly.
 */
extern void vsf_test_stdio_data_init(vsf_test_data_t *data);

/**
 @brief Synchronize data via stdio's printf and scanf.
 @param[in] data: a pointer to structure @ref vsf_test_data_t
 @param[in] cmd: structure @ref vsf_test_data_cmd_t

 @note This is a synchronized blocking API.
 */
extern void vsf_test_stdio_data_sync(vsf_test_data_t    *data,
                                     vsf_test_data_cmd_t cmd);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

#    endif

#endif /* __VSF_TEST_PORT_STDIO_H__ */
       /* EOF */