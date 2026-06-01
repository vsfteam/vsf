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

#ifndef __TEST_GPIO_EXTI_H__
#define __TEST_GPIO_EXTI_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
typedef struct {
    vsf_gpio_pin_mask_t expected_pin;
    volatile uint32_t count;
    bool disable_on_fire;
} vsf_test_gpio_exti_data_t;
#endif

#ifndef VSF_TEST_GPIO_EXTI_PRIO
#   define VSF_TEST_GPIO_EXTI_PRIO                  vsf_arch_prio_highest
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_gpio_exti_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  out_pin;
    uint8_t  in_pin;
    uint32_t trigger_mode;
} vsf_test_gpio_exti_params_t;

struct vsf_test_gpio_exti_s;
extern const struct vsf_test_gpio_exti_s vsf_test_gpio_exti;

#ifdef __cplusplus
}
#endif

#endif

#endif /* __TEST_GPIO_EXTI_H__ */
/* EOF */
