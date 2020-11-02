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

#ifndef __VSF_INPUT_GAMEPAD_H__
#define __VSF_INPUT_GAMEPAD_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_input_cfg.h"

#include "../vsf_input_get_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(__NAME, __BITOFFSET, __BITLEN, __IS_SIGNED, __CONFIG)\
            [__CONNECT2(GAMEPAD_ID_, __NAME)] = VSF_INPUT_ITEM_EX(              \
                __CONNECT2(GAMEPAD_ID_, __NAME),                                \
                (__BITOFFSET), (__BITLEN), (__IS_SIGNED), (__CONFIG))

#define VSF_GAMEPAD_DEF_ITEM_INFO(__NAME, __BITOFFSET, __BITLEN, __IS_SIGNED)   \
            [__CONNECT2(GAMEPAD_ID_, __NAME)] = VSF_INPUT_ITEM(                 \
                __CONNECT2(GAMEPAD_ID_, __NAME),                                \
                (__BITOFFSET), (__BITLEN), (__IS_SIGNED))

/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_GAMEPAD = VSF_INPUT_USER_TYPE,
};

//                         VIRTUAL_GAMEPAD
//          LT                                      RT
//          LB                                      RB
//                MENU_LEFT   MENU_MAIN   MENU_LEFT
//          L_UP               SPECIAL              R_UP
//  L_LEFT  LS/(LX,LY)  L_RIGHT             R_LEFT  RS/(RX,RY)  R_RIGHT
//          L_DOWN                                  R_DOWN

typedef enum vsf_gamepad_id_t {
    GAMEPAD_ID_DUMMY = 0,
    GAMEPAD_ID_L_UP,
    GAMEPAD_ID_L_DOWN,
    GAMEPAD_ID_L_LEFT,
    GAMEPAD_ID_L_RIGHT,
    GAMEPAD_ID_R_UP,
    GAMEPAD_ID_R_DOWN,
    GAMEPAD_ID_R_LEFT,
    GAMEPAD_ID_R_RIGHT,
    GAMEPAD_ID_LB,
    GAMEPAD_ID_RB,
    GAMEPAD_ID_LS,
    GAMEPAD_ID_RS,
    GAMEPAD_ID_MENU_LEFT,
    GAMEPAD_ID_MENU_RIGHT,
    GAMEPAD_ID_MENU_MAIN,
    GAMEPAD_ID_SPECIAL,
    GAMEPAD_ID_LX,
    GAMEPAD_ID_LY,
    GAMEPAD_ID_RX,
    GAMEPAD_ID_RY,
    GAMEPAD_ID_LT,
    GAMEPAD_ID_RT,
    GAMEPAD_ID_DPAD,

    GAMEPAD_ID_NUM,
    GAMEPAD_ID_USER = GAMEPAD_ID_NUM,
} vsf_gamepad_id_t;

typedef struct vk_gamepad_evt_t {
    implement(vk_input_evt_t)
    implement_ex(vk_input_item_info_t, info)
} vk_gamepad_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
