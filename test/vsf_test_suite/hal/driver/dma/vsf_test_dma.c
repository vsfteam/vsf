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

#include "vsf_test_dma.h"

/*============================ TODO =============================================
 *
 * Test coverage gaps (hal-peripheral-test-coverage-gaps.md):
 *
 * - [ ] DMA peripheral DREQ pacing (new scenario: dma_peripheral_dreq) (Batch 3)
 *   - New C file: dma/suite/vsf_test_dma_peripheral_dreq.c
 *   - Use UART TX DREQ to pace a memory-to-peripheral transfer.
 *   - Verify the DREQ handshake and transfer-count accounting.
 *   - Requires UART configured with DMA DREQ signal enabled.
 *
 *============================================================================*/

/*============================ IMPLEMENTATION ================================*/

/* Static-init macros in vsf_test_dma.h define suite + case arrays.
 * No runtime init or registration needed here. */

/* EOF */
