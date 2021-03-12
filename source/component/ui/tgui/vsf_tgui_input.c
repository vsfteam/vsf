/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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
#include "./vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
#define __VSF_TGUI_CLASS_IMPLEMENT
#include "./vsf_tgui.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/



bool __vk_tgui_send_touch_evt(  vsf_tgui_t *tgui_ptr,
                                uint_fast8_t finger_idx,
                                vsf_evt_t   msg,
                                vsf_tgui_location_t position )
{
    vsf_tgui_evt_t event = {
        .PointerEvt = {
            .msg = msg,
            .use_as__vsf_tgui_location_t = position,
            .idx = finger_idx,
        },
    };

    return vk_tgui_send_message(tgui_ptr, event);
}

bool __vk_tgui_send_gesture_evt(vsf_tgui_t *tgui_ptr,
                                uint_fast8_t finger_idx,
                                vsf_evt_t   msg,
                                vsf_tgui_location_t delta,
                                int32_t duration )
{
    vsf_tgui_evt_t event = {
        .tGestureEvt = {
            .msg = msg,
            .delta = {
                .use_as__vsf_tgui_location_t = delta,
                .ms = duration,
            },
            .idx = finger_idx,
        },
    };

    return vk_tgui_send_message(tgui_ptr, event);
}

vsf_err_t vsf_tgui_send_touch_evt(  vsf_tgui_t *tgui_ptr,
                                    uint_fast8_t finger_idx,
                                    vsf_tgui_location_t position, 
                                    bool is_down,
                                    int32_t duration)
{
    bool result = true;
    VSF_TGUI_ASSERT(finger_idx < 2);

    if (is_down) {
        if (tgui_ptr->input.finger.status & BIT(finger_idx)) {

            result &= __vk_tgui_send_gesture_evt(tgui_ptr, 
                                                 finger_idx,
                                                 VSF_TGUI_EVT_GESTURE_SLIDE,
                                                 (vsf_tgui_location_t) {
                                                     .iX = position.iX - tgui_ptr->input.finger.location_pre[finger_idx].iX,
                                                     .iY = position.iY - tgui_ptr->input.finger.location_pre[finger_idx].iY,
                                                 },
                                                 duration);

            /* finger already down, treat this as pointer move and a sliding guesture */
            result &= __vk_tgui_send_touch_evt( tgui_ptr, 
                                                finger_idx,
                                                VSF_TGUI_EVT_POINTER_MOVE,
                                                position);

        } else {

            tgui_ptr->input.finger.status |= BIT(finger_idx);
            /* first time */
            result &=  __vk_tgui_send_touch_evt(
                            tgui_ptr, 
                            finger_idx,
                            VSF_TGUI_EVT_POINTER_DOWN,
                            position);
        }
        
        //! update postion
        tgui_ptr->input.finger.location_pre[finger_idx] = position;

    } else {

        if (!(tgui_ptr->input.finger.status & BIT(finger_idx))) {
            /* finger is already up, treat this as pointer move */
             result &= __vk_tgui_send_touch_evt( tgui_ptr, 
                                                finger_idx,
                                                VSF_TGUI_EVT_POINTER_MOVE,
                                                position);
        } else {
            result &= __vk_tgui_send_touch_evt(tgui_ptr, finger_idx, VSF_TGUI_EVT_POINTER_UP, position);
            result &= __vk_tgui_send_touch_evt(tgui_ptr, finger_idx, VSF_TGUI_EVT_POINTER_CLICK, position);
            tgui_ptr->input.finger.status &= ~BIT(finger_idx);
        }
        
    }

    return result ? VSF_ERR_NONE : VSF_ERR_NOT_ENOUGH_RESOURCES;
}

#endif /* VSF_USE_TINY_GUI */
