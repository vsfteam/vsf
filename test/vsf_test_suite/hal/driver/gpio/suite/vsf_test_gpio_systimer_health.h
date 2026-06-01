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

#ifndef __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__
#define __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT
#   define VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT     3
#endif
#ifndef VSF_TEST_GPIO_SYSTIMER_HEALTH_MODE
#   define VSF_TEST_GPIO_SYSTIMER_HEALTH_MODE       (VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN)
#endif


/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

void vsf_test_gpio_systimer_health_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  pin;
    uint32_t interval_ms;
    uint32_t toggle_count;
} vsf_test_gpio_systimer_health_params_t;

struct vsf_test_gpio_systimer_health_s;
extern const struct vsf_test_gpio_systimer_health_s vsf_test_gpio_systimer_health;

#ifdef __cplusplus
}
#endif

#endif

#endif /* __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__ */
/* EOF */
