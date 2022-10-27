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

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"
// for VSF_HAL_USE_I2S
#include "hal/vsf_hal.h"

#if VSF_USE_AUDIO == ENABLED && VSF_HAL_USE_I2S == ENABLED

#define __VSF_AUDIO_CLASS_INHERIT__
#define __VSF_AUDIO_I2S_CLASS_IMPLEMENT
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

#include "service/vsf_service.h"
#include "component/av/vsf_av.h"
#include "./vsf_audio_i2s.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/



#endif
