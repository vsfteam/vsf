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

/*============================ INCLUDES ======================================*/

#include "vsf_test_gpio.h"

/*============================ TODO =============================================
 *
 * Test coverage gaps (hal-peripheral-test-coverage-gaps.md):
 *
 * - [ ] GPIO EXTI rising-edge & both-edges trigger modes (Batch 1)
 *   - The existing gpio_exti run() already handles RISING_FALLING; purely a
 *     YAML addition — add cases with trigger_mode=RISING and BOTH_EDGES to
 *     params/suite/gpio.yml.
 *
 * - [ ] GPIO ctrl API test (new scenario: gpio_ctrl) (Batch 1)
 *   - New C file: gpio/suite/vsf_test_gpio_ctrl.c
 *   - Exercises vsf_gpio_ctrl() with slew-rate and drive-strength readback.
 *   - Verify the gpio_ctrl() dispatch path does not silently return NOT_SUPPORT
 *     for valid operations.
 *
 *============================================================================*/

/*============================ IMPLEMENTATION ================================*/

/* GPIO test suites are now statically initialised via VSF_TEST_GPIO_*_STATIC
 * macros defined in vsf_test_gpio.h. No runtime init or binding table needed.
 */
