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

#ifndef __VSF_TEST_GPIO_H__
#define __VSF_TEST_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#define VSF_TEST_GPIO_CASE_MAX_COUNT    16

/* Phase-3 API check (usart-gpio-coverage-gaps PRD): capability() must
 * report a non-empty pin set. Catches drivers that link but never wired
 * up the capability struct. */
#define VSF_TEST_GPIO_ASSERT_CAPABILITY(__gpio)                          \
    do {                                                                 \
        vsf_gpio_capability_t __cap = vsf_gpio_capability(__gpio);       \
        VSF_TEST_ASSERT(__cap.pin_count > 0);                            \
        VSF_TEST_ASSERT(__cap.pin_mask  != 0);                           \
    } while (0)

#ifndef VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE
#   define VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE     DISABLED
#endif
#ifndef VSF_TEST_GPIO_TOGGLE_ENABLE
#   define VSF_TEST_GPIO_TOGGLE_ENABLE           DISABLED
#endif
#ifndef VSF_TEST_GPIO_DIRECTION_ENABLE
#   define VSF_TEST_GPIO_DIRECTION_ENABLE        DISABLED
#endif
#ifndef VSF_TEST_GPIO_ATOMIC_ENABLE
#   define VSF_TEST_GPIO_ATOMIC_ENABLE           DISABLED
#endif
#ifndef VSF_TEST_GPIO_PINMUX_ENABLE
#   define VSF_TEST_GPIO_PINMUX_ENABLE           DISABLED
#endif
#ifndef VSF_TEST_GPIO_MULTI_PIN_ENABLE
#   define VSF_TEST_GPIO_MULTI_PIN_ENABLE        DISABLED
#endif
#ifndef VSF_TEST_GPIO_OPEN_DRAIN_ENABLE
#   define VSF_TEST_GPIO_OPEN_DRAIN_ENABLE       DISABLED
#endif
#ifndef VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE
#   define VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE      DISABLED
#endif
#ifndef VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE
#   define VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE DISABLED
#endif
#ifndef VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE
#   define VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE    DISABLED
#endif
#ifndef VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE
#   define VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE  DISABLED
#endif
#ifndef VSF_TEST_GPIO_EXTI_ENABLE
#   define VSF_TEST_GPIO_EXTI_ENABLE             DISABLED
#endif
#ifndef VSF_TEST_GPIO_IRQ_LATENCY_ENABLE
#   define VSF_TEST_GPIO_IRQ_LATENCY_ENABLE      DISABLED
#endif
#ifndef VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE
#   define VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE    DISABLED
#endif
#ifndef VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE
#   define VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE  DISABLED
#endif
#ifndef VSF_TEST_GPIO_ANALOG_MODE_ENABLE
#   define VSF_TEST_GPIO_ANALOG_MODE_ENABLE      DISABLED
#endif
#ifndef VSF_TEST_GPIO_IO_CHECK_ENABLE
#   define VSF_TEST_GPIO_IO_CHECK_ENABLE          DISABLED
#endif

#ifndef VSF_TEST_GPIO_ENABLE
#   define VSF_TEST_GPIO_ENABLE    ENABLED
#endif

#include "suite/vsf_test_gpio_concurrent_prio.h"
#include "suite/vsf_test_gpio_exti.h"
#include "suite/vsf_test_gpio_irq_latency.h"
#include "suite/vsf_test_gpio_irq_lifecycle.h"

#include "suite/vsf_test_gpio_analog_mode.h"
#include "suite/vsf_test_gpio_atomic.h"
#include "suite/vsf_test_gpio_direction.h"
#include "suite/vsf_test_gpio_io_check.h"
#include "suite/vsf_test_gpio_multi_pin.h"
#include "suite/vsf_test_gpio_open_drain.h"
#include "suite/vsf_test_gpio_output_input.h"
#include "suite/vsf_test_gpio_pinmux.h"
#include "suite/vsf_test_gpio_systimer_health.h"
#include "suite/vsf_test_gpio_toggle.h"
#include "suite/vsf_test_gpio_toggle_freq.h"
#include "suite/vsf_test_gpio_toggle_stress.h"
#include "suite/vsf_test_gpio_write_throughput.h"
typedef union {
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    vsf_test_gpio_concurrent_prio_data_t gpio_concurrent_prio;
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    vsf_test_gpio_exti_data_t gpio_exti;
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    vsf_test_gpio_irq_latency_data_t gpio_irq_latency;
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    vsf_test_gpio_irq_lifecycle_data_t gpio_irq_lifecycle;
#endif
} vsf_test_gpio_data_t;
#ifdef __cplusplus
}
#endif
#endif  /* __VSF_TEST_GPIO_H__ */
/* EOF */