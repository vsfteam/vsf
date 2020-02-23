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
declare_class(vsf_tgui_t)

#define __VSF_TGUI_CONTROLS_CONTROLE_CLASS_INHERIT
#define __VSF_TGUI_CONTROLS_LIST_CLASS_IMPLEMENT

#include "./vsf_tgui_list.h"
#include "../view/vsf_tgui_v.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static const i_tgui_control_methods_t c_tVList = {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    {
        (vsf_tgui_method_t *)&vsf_tgui_list_v_init,
        (vsf_tgui_method_t *)&vsf_tgui_list_v_depose,
        (vsf_tgui_v_method_render_t *)&vsf_tgui_list_v_rendering,
        (vsf_tgui_method_t *)&vsf_tgui_list_v_update
    },
    (vsf_tgui_method_t*)&vk_tgui_list_init,
    (vsf_tgui_method_t *)&vk_tgui_list_update
#else
    .tView = {
        .Init = (vsf_tgui_method_t *)&vsf_tgui_list_v_init,
        .Depose = (vsf_tgui_method_t *)&vsf_tgui_list_v_depose,
        .Render = (vsf_tgui_v_method_render_t *)&vsf_tgui_list_v_rendering,
        .Update = (vsf_tgui_method_t *)&vsf_tgui_list_v_update,
    },
    .Init =     (vsf_tgui_method_t *)&vk_tgui_list_init,
    .Update =   (vsf_tgui_method_t *)&vk_tgui_list_update,
#endif
};

/*============================ IMPLEMENTATION ================================*/

static void __vsf_tgui_list_adjust_inner_container_location(vsf_tgui_list_t* ptList, 
                                                            uint_fast8_t chIndex)
{
    vsf_tgui_container_t *ptInnerContainer = ptList->ptList;
    const vsf_tgui_control_t *ptControl = 
        (const vsf_tgui_control_t*)ptInnerContainer->use_as__vsf_msgt_container_t.ptNode;
    int16_t nXOffset = 0;
    int16_t nYOffset = 0;

    switch (ptInnerContainer->tContainerAttribute.u5Type) {
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
            nYOffset = ptInnerContainer->tConatinerPadding.chTop;
            while (chIndex--) {
                ptControl = __vk_tgui_control_get_next_visible_one_within_container(ptControl);
            }
            nYOffset += vsf_tgui_control_get_core(ptControl)->tRegion.tLocation.iY;

        #if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
            vk_tgui_slider_location_target_set( &(ptList->tSlider), -nYOffset);
        #else
            ptInnerContainer->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iY = -nYOffset;
        #endif
            break;
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
            nXOffset = ptInnerContainer->tConatinerPadding.chLeft;
            while (chIndex--) {
                ptControl = __vk_tgui_control_get_next_visible_one_within_container(ptControl);
            }
            nXOffset += vsf_tgui_control_get_core(ptControl)->tRegion.tLocation.iX;
        #if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
            vk_tgui_slider_location_target_set( &(ptList->tSlider), -nXOffset);
        #else
            ptInnerContainer->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iX = -nXOffset;
        #endif
            break;
    }
}

static void __vk_tgui_list_update_inner_container_location(vsf_tgui_list_t* ptList)
{
    vsf_tgui_container_t *ptInnerContainer = ptList->ptList;

    switch (ptInnerContainer->tContainerAttribute.u5Type) {
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
            vsf_tgui_control_get_core((const vsf_tgui_control_t *)ptInnerContainer)
                ->tRegion.tLocation.iY = vk_tgui_slider_on_timer_event_handler(&(ptList->tSlider));
            break;
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
            vsf_tgui_control_get_core((const vsf_tgui_control_t *)ptInnerContainer)
                ->tRegion.tLocation.iX = vk_tgui_slider_on_timer_event_handler(&(ptList->tSlider));
            break;
    }
}

static bool __vsf_tgui_list_previous(vsf_tgui_list_t* ptList)
{

    if (ptList->chStartIndex == 0) {
        return false;
    }
    ptList->chStartIndex--;
    
    __vsf_tgui_list_adjust_inner_container_location(ptList, ptList->chStartIndex);

    return true;
}

static bool __vsf_tgui_list_next(vsf_tgui_list_t* ptList)
{
    if (ptList->chStartIndex == (ptList->ptList->chVisibleItemCount - 1)) {
        return false;
    } else if (ptList->chStartIndex >= ptList->ptList->chVisibleItemCount) {
        ptList->chStartIndex = ptList->ptList->chVisibleItemCount - 1;
    } else {
        ptList->chStartIndex++;
    }

    __vsf_tgui_list_adjust_inner_container_location(ptList, ptList->chStartIndex);

    return true;
}

fsm_rt_t vsf_tgui_mc_list_msg_handler(vsf_tgui_list_t* ptList, vsf_tgui_msg_t* ptMSG)
{
    fsm_rt_t fsm;

    //! replace UPDATE with TREE_UPDATE
    if (VSF_TGUI_EVT_UPDATE == ptMSG->use_as__vsf_msgt_msg_t.tMSG) {
        if (vsf_tgui_control_update_tree((const vsf_tgui_control_t *)ptList)) {
            return fsm_rt_cpl;
        }
    }
#if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
    else if (VSF_TGUI_EVT_ON_TIME == ptMSG->use_as__vsf_msgt_msg_t.tMSG) {
        __vk_tgui_list_update_inner_container_location(ptList);
        return (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
    }
#endif
    fsm = __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t *)ptList, 
                                            ptMSG, 
                                            &c_tVList);

    bool bStatusChanged = false;

    if (VSF_TGUI_EVT_KEY_PRESSED == ptMSG->use_as__vsf_msgt_msg_t.tMSG){
        vsf_tgui_key_evt_t* ptEvt = (vsf_tgui_key_evt_t*)ptMSG;
        switch (ptList->ptList->tContainerAttribute.u5Type) {
            case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
                if          (VSF_TGUI_KEY_LEFT == ptEvt->hwKeyValue) {
                    bStatusChanged = __vsf_tgui_list_previous(ptList);
                } else if   (VSF_TGUI_KEY_RIGHT == ptEvt->hwKeyValue) {
                    bStatusChanged = __vsf_tgui_list_next(ptList);
                } else {
                    break;
                }
                fsm = (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
                break;
            case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
                if (VSF_TGUI_KEY_UP == ptEvt->hwKeyValue) {
                    bStatusChanged = __vsf_tgui_list_previous(ptList);
                }
                else if (VSF_TGUI_KEY_DOWN == ptEvt->hwKeyValue) {
                    bStatusChanged = __vsf_tgui_list_next(ptList);
                } else {
                    break;
                }
                fsm = (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
                break;
            default:
                /* should not happen, do nothing */
                break;
        }
    
        
    }

#if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
    else if (VSF_TGUI_EVT_GESTURE_SLIDE == ptMSG->use_as__vsf_msgt_msg_t.tMSG) {
        vsf_tgui_gesture_evt_t* ptEvt = (vsf_tgui_gesture_evt_t*)ptMSG;
        switch (ptList->ptList->tContainerAttribute.u5Type) {
            case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
                if (ptEvt->tDelta.use_as__vsf_tgui_location_t.iX > 0) {
                    bStatusChanged = __vsf_tgui_list_previous(ptList);
                    break;
                } else if (ptEvt->tDelta.use_as__vsf_tgui_location_t.iX < 0) {
                    bStatusChanged = __vsf_tgui_list_next(ptList);
                    break;
                } 
                //! fall-through
            case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
                if (ptEvt->tDelta.use_as__vsf_tgui_location_t.iY > 0) {
                    bStatusChanged = __vsf_tgui_list_previous(ptList);
                } else if (ptEvt->tDelta.use_as__vsf_tgui_location_t.iY < 0) {
                    bStatusChanged = __vsf_tgui_list_next(ptList);
                } 
                break;
        }
    }
#endif
    
    if (bStatusChanged) {
        fsm = VSF_TGUI_MSG_RT_REFRESH;
    }

    return fsm;
}

static void __vk_tgui_list_start_index_validate(vsf_tgui_list_t* ptList)
{
    uint8_t chVisibleItemCount = ptList->ptList->chVisibleItemCount; 
    if (ptList->chStartIndex >= chVisibleItemCount) {
        ptList->chStartIndex = chVisibleItemCount - 1;
    }
}

static void __vk_tgui_list_select_index_validate(vsf_tgui_list_t* ptList)
{
    uint8_t chVisibleItemCount = ptList->ptList->chVisibleItemCount; 
    if (ptList->chSelectIndex >= chVisibleItemCount) {
        ptList->chSelectIndex = chVisibleItemCount - 1;
    }
}


fsm_rt_t vk_tgui_list_update(vsf_tgui_list_t* ptList)
{
    /*! \note before this function is called, all controls in the container are
              updated with TREE_UPDATE message
     */

    __vk_tgui_list_start_index_validate(ptList);
    __vk_tgui_list_select_index_validate(ptList);
        
    __vsf_tgui_list_adjust_inner_container_location(ptList, ptList->chStartIndex);

    return vk_tgui_container_update(
                &(ptList->use_as__vsf_tgui_container_t));
}

fsm_rt_t vk_tgui_list_init(vsf_tgui_list_t* ptList)
{
#if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
    vk_tgui_slider_init(&(ptList->tSlider),                         /*!< slider */
                        (const vsf_tgui_control_t *)ptList,         /*!< host control*/
                        0);                                         /*!< use default fps */
#endif

    return vk_tgui_container_init(&(ptList->use_as__vsf_tgui_container_t));
}

uint_fast8_t vk_tgui_list_display_item_start_get(vsf_tgui_list_t* ptList)
{
    VSF_TGUI_ASSERT(NULL != ptList);
    return ptList->chStartIndex;
}

uint_fast8_t vk_tgui_list_display_item_start_set(vsf_tgui_list_t* ptList, 
                                                uint_fast8_t chStartIndex)
{
    VSF_TGUI_ASSERT(NULL != ptList);
    ptList->chStartIndex = chStartIndex;
    //__vk_tgui_list_start_index_validate(ptList);

    return ptList->chStartIndex;
}


#endif


/* EOF */
