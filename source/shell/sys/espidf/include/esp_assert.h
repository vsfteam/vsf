/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

/*
 * Clean-room re-implementation of ESP-IDF public API "esp_assert.h".
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_ASSERT_H__
#define __VSF_ESPIDF_ESP_ASSERT_H__

#include <assert.h>

/* C17 / C++17 standard static_assert */
#define ESP_STATIC_ASSERT static_assert

/* Compile-time assert when condition is constant, runtime assert otherwise */
#ifndef __cplusplus
#define TRY_STATIC_ASSERT(CONDITION, MSG) do {                                          \
        ESP_STATIC_ASSERT(__builtin_choose_expr(__builtin_constant_p(CONDITION),        \
                                                (CONDITION), 1), #MSG);                 \
        assert(#MSG && (CONDITION));                                                    \
    } while(0)
#else
#define TRY_STATIC_ASSERT(CONDITION, MSG) do {                                          \
        if (__builtin_constant_p(CONDITION) && !(CONDITION)) {                          \
            extern __attribute__((error(#MSG))) void failed_compile_time_assert(void);   \
            failed_compile_time_assert();                                               \
        }                                                                               \
        assert(#MSG && (CONDITION));                                                    \
    } while(0)
#endif

#endif      // __VSF_ESPIDF_ESP_ASSERT_H__
