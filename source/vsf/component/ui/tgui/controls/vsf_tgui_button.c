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

/*============================ INCLUDES ======================================*/
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

#define __VSF_TGUI_CONTROLS_BUTTON_CLASS_IMPLEMENT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_button.h"
#include "../view/vsf_tgui_v.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static const i_tgui_control_methods_t c_tVLabel= {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    {
        (vsf_tgui_method_t *)&vsf_tgui_button_v_init,
        (vsf_tgui_method_t *)&vsf_tgui_button_v_depose,
        (vsf_tgui_v_method_render_t *)&vsf_tgui_button_v_rendering,
        NULL,
        (vsf_tgui_method_t *)&vsf_tgui_button_v_update
    },
    (vsf_tgui_method_t *)vk_tgui_button_init,
    (vsf_tgui_method_t *)&vk_tgui_button_update
#else
    .tView = {
        .Init =     (vsf_tgui_method_t *)&vsf_tgui_button_v_init,
        .Depose =   (vsf_tgui_method_t*)&vsf_tgui_button_v_depose,
        .Render =   (vsf_tgui_v_method_render_t *)&vsf_tgui_button_v_rendering,
        .Update =   (vsf_tgui_method_t *)&vsf_tgui_button_v_update,
    },
    .Init =     (vsf_tgui_method_t *)vk_tgui_button_init,
    .Update =   (vsf_tgui_method_t*)&vk_tgui_button_update,
#endif
};



/*============================ IMPLEMENTATION ================================*/


fsm_rt_t vsf_tgui_button_msg_handler(vsf_tgui_button_t* ptButton, vsf_tgui_msg_t* ptMSG)
{
    fsm_rt_t fsm = __vsf_tgui_control_msg_handler(
                        &(ptButton->use_as__vsf_tgui_label_t.use_as__vsf_tgui_control_t),
                        ptMSG,
                        &c_tVLabel);

    //if (fsm != VSF_MSGT_ERR_MSG_NOT_HANDLED) {
        switch (ptMSG->use_as__vsf_msgt_msg_t.tMSG){
            case VSF_TGUI_EVT_POINTER_DOWN:
                if (!ptButton->_.bIsCheckButton) {
                    ptButton->_.bIsChecked = true;
                    if (fsm != VSF_TGUI_MSG_RT_REFRESH) {
                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(VSF_TRACE_WARNING, " \tRequest Refresh");
                    #endif
                        vsf_tgui_control_refresh((const vsf_tgui_control_t*)ptButton, NULL);
                    }

                }
                break;

            case VSF_TGUI_EVT_POINTER_UP:
                if (!ptButton->_.bIsCheckButton) {
                    ptButton->_.bIsChecked = false;
                    if (fsm != VSF_TGUI_MSG_RT_REFRESH) {
                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(VSF_TRACE_WARNING, " \tRequest Refresh");
                    #endif
                        vsf_tgui_control_refresh((const vsf_tgui_control_t*)ptButton, NULL);
                    }

                }
                break;

            case VSF_TGUI_EVT_POINTER_CLICK:
                if (ptButton->_.bIsCheckButton) {
                    //! toggle the bIsChecked flag
                    ptButton->_.bIsChecked = !ptButton->_.bIsChecked;
                    if (fsm != VSF_TGUI_MSG_RT_REFRESH) {
                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(VSF_TRACE_WARNING, " \tRequest Refresh");
                    #endif
                        vsf_tgui_control_refresh((const vsf_tgui_control_t*)ptButton, NULL);
                    }
                }
                break;

        #if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
            case VSF_TGUI_EVT_POINTER_ENTER:
                if (ptButton->_.bIsAllowEmphasize) {
                    ptButton->_.bIsEmphasized = true;
                    if (fsm != VSF_TGUI_MSG_RT_REFRESH) {
                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(VSF_TRACE_WARNING, " \tRequest Refresh");
                    #endif
                        vsf_tgui_control_refresh((const vsf_tgui_control_t*)ptButton, NULL);
                    }
                }
                break;

            case VSF_TGUI_EVT_POINTER_LEFT:
                if (ptButton->_.bIsAllowEmphasize) {
                    ptButton->_.bIsEmphasized = false;
                    if (fsm != VSF_TGUI_MSG_RT_REFRESH) {
                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(VSF_TRACE_WARNING, " \tRequest Refresh");
                    #endif
                        vsf_tgui_control_refresh((const vsf_tgui_control_t*)ptButton, NULL);
                    }
                }
                break;

        #endif

        }
    //}

    return fsm;
}


fsm_rt_t vk_tgui_button_update(vsf_tgui_button_t* ptButton)
{
    return vk_tgui_label_update(&(ptButton->use_as__vsf_tgui_label_t));
}

fsm_rt_t vk_tgui_button_init(vsf_tgui_button_t* ptButton)
{
    return vk_tgui_label_init(&(ptButton->use_as__vsf_tgui_label_t));
}

#endif


/* EOF */
