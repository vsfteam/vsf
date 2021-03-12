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



#ifndef __VSF_INPUT_TOUCHSCREEN_H__
#define __VSF_INPUT_TOUCHSCREEN_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_input_cfg.h"

#include "../vsf_input_get_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_input_touchscreen_set(__evt, __id, __is_down, __pressure, __x, __y) \
            do {                                                                \
                (__evt)->id = ((__id) | ((__is_down) << 8));                    \
                (__evt)->cur.valu64 = (__x) | ((__y) << 16) | ((uint64_t)(__pressure) << 32);\
            } while (0)

#define vsf_input_touchscreen_get_id(__evt)                                     \
            ((uint8_t)(((__evt)->id >> 0) & 0xFF))
#define vsf_input_touchscreen_is_down(__evt)                                    \
            (!!(((__evt)->id >> 8) & 0xFF))
#define vsf_input_touchscreen_get_x(__evt)                                      \
            ((uint16_t)(((__evt)->cur.valu64 >> 0) & 0xFFFF))
#define vsf_input_touchscreen_get_y(__evt)                                      \
            ((uint16_t)(((__evt)->cur.valu64 >> 16) & 0xFFFF))
#define vsf_input_touchscreen_get_pressure(__evt)                               \
            ((uint16_t)(((__evt)->cur.valu64 >> 32) & 0xFFFF))

/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_TOUCHSCREEN = VSF_INPUT_USER_TYPE,
};

typedef struct vk_touchscreen_info_t {
    uint16_t width;
    uint16_t height;
} vk_touchscreen_info_t;

typedef struct vk_touchscreen_evt_t {
    implement(vk_input_evt_t)
    vk_touchscreen_info_t info;
} vk_touchscreen_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */