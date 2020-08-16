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

#ifndef __VSF_AV_H__
#define __VSF_AV_H__

#include "./vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED || VSF_USE_VIDEO == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_av_control_value_t {
    union {
        void *buffer;
        uint8_t uval8;
        int8_t ival8;
        uint16_t uval16;
        int16_t ival16;
        uint32_t uval32;
        int32_t ival32;
        bool enable;
    };
} vk_av_control_value_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./audio/vsf_audio.h"

#endif      // VSF_USE_AUDIO || VSF_USE_VIDEO
#endif      // __VSF_AV_H__
