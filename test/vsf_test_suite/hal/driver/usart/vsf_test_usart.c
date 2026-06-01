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

#include "vsf_test_usart.h"

/*============================ TODO =============================================
 *
 * Test coverage gaps (hal-peripheral-test-coverage-gaps.md):
 *
 * - [ ] USART concurrent TX+RX IRQ (new scenario: usart_concurrent_txrx) (Batch 2)
 *   - New C file: usart/suite/vsf_test_usart_concurrent_txrx.c
 *   - Uses PL011 internal loopback (LBE bit) — no external wiring change.
 *   - Start TX and RX requests simultaneously, verify both complete correctly.
 *   - Exercises ISR dispatch ordering and TX/RX FIFO interaction under
 *     dual-direction load.
 *
 *============================================================================*/

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/* EOF */
