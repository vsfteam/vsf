/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __USRAPP_AUDIO_COMMON_H__
#define __USRAPP_AUDIO_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_AUDIO == ENABLED

#if VSF_USE_WINSOUND == ENABLED
#   include "component/av/audio/driver/winsound/vsf_winsound.h"
#endif

/*============================ MACROS ========================================*/

#ifndef APP_CFG_WINSOUND_PRIO
#   define APP_CFG_WINSOUND_PRIO            vsf_arch_prio_0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usrapp_audio_common_t {
    vk_audio_dev_t *default_dev;
#if VSF_USE_WINSOUND == ENABLED
    struct {
        vk_winsound_dev_t dev;
    } winsound;
#endif
} usrapp_audio_common_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern usrapp_audio_common_t usrapp_audio_common;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_AUDIO
#endif      // __USRAPP_AUDIO_COMMON_H__
/* EOF */