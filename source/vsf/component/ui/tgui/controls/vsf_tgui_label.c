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

#define __VSF_TGUI_CONTROLS_LABEL_CLASS_IMPLEMENT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_label.h"
#include "../view/vsf_tgui_v.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
vsf_tgui_size_t __vk_tgui_label_v_text_get_size(vsf_tgui_label_t* ptLabel,
                                                uint16_t *phwLineCount);

extern 
vsf_tgui_size_t __vk_tgui_label_v_get_minimal_rendering_size(vsf_tgui_label_t* ptLabel);
/*============================ LOCAL VARIABLES ===============================*/

static const i_tgui_control_methods_t c_tVLabel= {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    {
        (vsf_tgui_method_t *)&vsf_tgui_label_v_init,
        (vsf_tgui_method_t *)&vsf_tgui_label_v_depose,
        (vsf_tgui_v_method_render_t *)&vsf_tgui_label_v_rendering,
        (vsf_tgui_method_t *)&vsf_tgui_label_v_update
    },
    (vsf_tgui_method_t*)&vk_tgui_label_init,
    (vsf_tgui_method_t *)&vk_tgui_label_update
#else
    .tView = {
        .Init =     (vsf_tgui_method_t *)&vsf_tgui_label_v_init,
        .Depose =   (vsf_tgui_method_t *)&vsf_tgui_label_v_depose,
        .Render =   (vsf_tgui_v_method_render_t *)&vsf_tgui_label_v_rendering,
        .Update =   (vsf_tgui_method_t *)&vsf_tgui_label_v_update,
    },
    .Init =     (vsf_tgui_method_t *)&vk_tgui_label_init,
    .Update =   (vsf_tgui_method_t *)&vk_tgui_label_update,
#endif
};

/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_mc_label_msg_handler( vsf_tgui_label_t* ptControl, 
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t *)ptControl, 
                                            ptMSG, 
                                            &c_tVLabel);
}

fsm_rt_t vk_tgui_label_update(vsf_tgui_label_t* ptLabel)
{
    if (fsm_rt_cpl == vk_tgui_control_update((vsf_tgui_control_t *)ptLabel)) {
    #if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
        if (ptLabel->tLabel.bIsChanged) {
            ptLabel->tLabel.bIsChanged = false;
            ptLabel->tLabel.tInfoCache.tStringSize = 
                __vk_tgui_label_v_text_get_size(ptLabel, &(ptLabel->tLabel.tInfoCache.hwLines));
        }
    #endif

        if (ptLabel->bIsAutoSize) {

            vsf_tgui_size_t tNewSize = 
                __vk_tgui_label_v_get_minimal_rendering_size(ptLabel);
            vsf_tgui_size_t *ptControlSize = 
                    &(ptLabel->use_as__vsf_tgui_control_t.
                        use_as____vsf_tgui_control_core_t.tRegion.tSize);

            ptControlSize->iHeight = tNewSize.iHeight;
            ptControlSize->iWidth = max(tNewSize.iWidth, ptControlSize->iWidth);

        }

        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}

static fsm_rt_t vk_tgui_label_init(vsf_tgui_label_t* ptLabel)
{
    return vk_tgui_control_init((vsf_tgui_control_t*)ptLabel);
}


#endif


/* EOF */
