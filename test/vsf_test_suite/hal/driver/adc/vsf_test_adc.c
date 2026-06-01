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

#include "vsf_test_adc.h"

/*============================ TODO =============================================
 *
 * Test coverage gaps (hal-peripheral-test-coverage-gaps.md):
 *
 * - [ ] ADC multi-channel round-robin (new scenario: adc_round_robin) (Batch 2)
 *   - New C file: adc/suite/vsf_test_adc_round_robin.c
 *   - Sample temperature sensor (channel 4) and GPIO ADC channel alternately.
 *   - Verify each sample is within its expected range.
 *   - Exercises channel-mux sequencing and internal-vs-external channel paths.
 *
 *============================================================================*/

/* EOF */
