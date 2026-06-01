/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
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

#include "vsf_test_i2c.h"

/*============================ TODO =============================================
 *
 * Test coverage gaps (hal-peripheral-test-coverage-gaps.md):
 *
 * - [ ] I2C NACK detection (new scenario: i2c_nack) (Batch 2)
 *   - New C file: i2c/suite/vsf_test_i2c_nack.c
 *   - Scan a known-unused device address, assert driver returns proper NACK
 *     error code (not timeout or hang).
 *   - Verifies TX_ABRT source detection and error recovery path.
 *   - No on-bus device needed.
 *
 *============================================================================*/

/* EOF */
