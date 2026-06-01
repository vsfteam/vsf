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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf_test_spi.h"
#include "suite/vsf_test_spi_loopback.h"
#include "suite/vsf_test_spi_async.h"

/*============================ TODO =============================================
 *
 * Test coverage gaps (hal-peripheral-test-coverage-gaps.md):
 *
 * - [ ] LSB-first bit order in spi_loopback (Batch 1)
 *   - Add `bit_order` field to case struct, pass through to spi_init.
 *   - Add LSB-first cases to params/suite/spi.yml.
 *
 * - [ ] Data-width sweep 4–15 bits in spi_loopback (Batch 1)
 *   - Existing cases use DATASIZE_8. Add cases covering 4–15 bits.
 *   - Verify PL022 DSS field mapping for all hardware values.
 *
 * - [ ] SPI slave-mode test (new scenario: spi_slave) (Batch 3)
 *   - New C file: spi/suite/vsf_test_spi_slave.c
 *   - RP2040 acts as SPI slave, host-driven master sends/receives.
 *   - Requires SPI master device or second Pico.
 *
 *============================================================================*/

/* EOF */
