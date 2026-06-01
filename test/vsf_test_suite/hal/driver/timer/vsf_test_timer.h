/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  You may not use this file except in compliance with the License.         *
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

#ifndef __VSF_TEST_TIMER_H__
#define __VSF_TEST_TIMER_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_ONESHOT_ENABLE
#   define VSF_TEST_TIMER_ONESHOT_ENABLE         ENABLED
#endif

#ifndef VSF_TEST_TIMER_PERIODIC_ENABLE
#   define VSF_TEST_TIMER_PERIODIC_ENABLE        ENABLED
#endif

#ifndef VSF_TEST_TIMER_ASYNC_ENABLE
#   define VSF_TEST_TIMER_ASYNC_ENABLE           ENABLED
#endif

#ifndef VSF_TEST_TIMER_ENABLE
#   define VSF_TEST_TIMER_ENABLE    ENABLED
#endif

#include "suite/vsf_test_timer_async.h"
#include "suite/vsf_test_timer_oneshot.h"
#include "suite/vsf_test_timer_periodic.h"

typedef union {
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    vsf_test_timer_async_data_t timer_async;
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    vsf_test_timer_oneshot_data_t timer_oneshot;
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    vsf_test_timer_periodic_data_t timer_periodic;
#endif
} vsf_test_timer_data_t;
#ifdef __cplusplus
}
#endif
#endif /* __VSF_TEST_TIMER_H__ */
/* EOF */