/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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
 ****************************************************************************/

#ifndef __HAL_DRIVER_RP2040_PWM_H__
#define __HAL_DRIVER_RP2040_PWM_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_PWM == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/

/* RP2040 PWM: 8 slices, each with 2 channels (A/B).
 * VSF models each slice as one PWM instance with 2 channels.
 *
 * Hardware reference:
 *   - pwm_hw->slice[0..7].csr  : control (EN, PH_CORRECT, A_INV, B_INV)
 *   - pwm_hw->slice[0..7].div  : clock divider (INT[11:4] + FRAC[3:0])
 *   - pwm_hw->slice[0..7].ctr  : counter
 *   - pwm_hw->slice[0..7].cc   : compare (A[15:0], B[31:16])
 *   - pwm_hw->slice[0..7].top  : wrap value
 *   - pwm_hw->en               : alias of all CSR.EN bits
 *
 * Clock: runs from clk_sys (125 MHz default).
 */

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif      /* VSF_HAL_USE_PWM */
#endif      /* __HAL_DRIVER_RP2040_PWM_H__ */
/* EOF */
