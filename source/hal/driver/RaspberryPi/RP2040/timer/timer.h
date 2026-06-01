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

#ifndef __HAL_DRIVER_RASPBERRYPI_RP2040_TIMER_H__
#define __HAL_DRIVER_RASPBERRYPI_RP2040_TIMER_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_TIMER == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/

#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK         ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE     ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_timer_irq_mask_t {
    VSF_TIMER_IRQ_MASK_OVERFLOW = (0x01 << 0),
} vsf_timer_irq_mask_t;

typedef enum vsf_timer_channel_mode_t {
    VSF_TIMER_CHANNEL_MODE_BASE = (0x00 << 0),
    VSF_TIMER_BASE_ONESHOT      = (0x00 << 5),
    VSF_TIMER_BASE_CONTINUES    = (0x01 << 5),
} vsf_timer_channel_mode_t;

/*============================ INCLUDES ======================================*/
#include "hal/driver/common/template/vsf_template_timer.h"
/*============================ PROTOTYPES ====================================*/

#endif      // VSF_HAL_USE_TIMER
#endif      // __HAL_DRIVER_RASPBERRYPI_RP2040_TIMER_H__
/* EOF */
