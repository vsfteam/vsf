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

#if VSF_USE_TINY_GUI == ENABLED && VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
declare_class(vsf_tgui_t)

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
#define __VSF_TGUI_CONTROLS_LIST_CLASS_IMPLEMENT

#include "./vsf_tgui_list.h"
#include "../view/vsf_tgui_v.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
static bool __vk_tgui_list_invoke_event(vsf_tgui_list_t* ptList, vsf_evt_t tMSG);

/*============================ LOCAL VARIABLES ===============================*/
static const i_tgui_control_methods_t c_tVList = {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    {
        (vsf_tgui_method_t *)&vsf_tgui_list_v_init,
        (vsf_tgui_method_t *)&vsf_tgui_list_v_depose,
        (vsf_tgui_v_method_render_t *)&vsf_tgui_list_v_rendering,
        (vsf_tgui_v_method_render_t *)&vsf_tgui_list_v_post_rendering,
        (vsf_tgui_method_t *)&vsf_tgui_list_v_update
    },
    (vsf_tgui_method_t*)&vk_tgui_list_init,
    (vsf_tgui_method_t *)&vk_tgui_list_update
#else
    .tView = {
        .Init = (vsf_tgui_method_t *)&vsf_tgui_list_v_init,
        .Depose = (vsf_tgui_method_t *)&vsf_tgui_list_v_depose,
        .Render = (vsf_tgui_v_method_render_t *)&vsf_tgui_list_v_rendering,
        .ContainerPostRender = (vsf_tgui_v_method_render_t *)&vsf_tgui_list_v_post_rendering,
        .Update = (vsf_tgui_method_t *)&vsf_tgui_list_v_update,
    },
    .Init =     (vsf_tgui_method_t *)&vk_tgui_list_init,
    .Update =   (vsf_tgui_method_t *)&vk_tgui_list_update,
#endif
};

/*============================ IMPLEMENTATION ================================*/

static int_fast16_t __vk_tgui_calculate_offset_for_make_target_control_visible(
                        vsf_tgui_list_t* ptList,
                        const vsf_tgui_control_t *ptControl,
                        int_fast16_t iPosition,
                        bool bIsVertical)
{
    vsf_tgui_container_t *ptInnerContainer = ptList->ptList;
    int_fast16_t iListPosition = 0;
    int_fast16_t iListLength = 0;
    int_fast16_t iControlLength = 0;
    if (bIsVertical) {
        iListPosition = -ptInnerContainer->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iY;
        iListLength = vsf_tgui_control_get_core((const vsf_tgui_control_t *)ptList)->tRegion.tSize.iHeight;
    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
        iListLength -= ptInnerContainer->tConatinerPadding.chTop + ptInnerContainer->tConatinerPadding.chBottom;
    #endif

        iControlLength = vsf_tgui_control_get_core(ptControl)->tRegion.tSize.iHeight;
    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
        iControlLength += vsf_tgui_control_get_core(ptControl)->tMargin.chBottom;
    #endif
    } else {
        iListPosition = -ptInnerContainer->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iX;
        iListLength = vsf_tgui_control_get_core((const vsf_tgui_control_t *)ptList)->tRegion.tSize.iWidth;
    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
        iListLength -= ptInnerContainer->tConatinerPadding.chLeft + ptInnerContainer->tConatinerPadding.chRight;
    #endif

        iControlLength = vsf_tgui_control_get_core(ptControl)->tRegion.tSize.iWidth;
    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED

        iControlLength += vsf_tgui_control_get_core(ptControl)->tMargin.chRight;
    #endif
    }

    do {
        if (iListPosition >= iPosition) {
            break;
        } else if ((iPosition + iControlLength) > (iListPosition + iListLength)) {
            iPosition = iPosition - (iListLength - iControlLength);
            break;
        }
        /* no change */
        iPosition = iListPosition;
    } while(0);

    return iPosition;
}

static void __vsf_tgui_list_adjust_inner_container_location(vsf_tgui_list_t* ptList,
                                                            uint_fast8_t chIndex)
{
    vsf_tgui_container_t *ptInnerContainer = ptList->ptList;
    const vsf_tgui_control_t *ptControl =
        (const vsf_tgui_control_t*)ptInnerContainer->use_as__vsf_msgt_container_t.node_ptr;
    int16_t nXOffset = 0;
    int16_t nYOffset = 0;

    switch (ptInnerContainer->tContainerAttribute.u5Type) {
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
            nYOffset = ptInnerContainer->tConatinerPadding.chTop;
        #endif
            while (chIndex--) {
                ptControl = __vk_tgui_control_get_next_visible_one_within_container(ptControl);
            }
            nYOffset += vsf_tgui_control_get_core(ptControl)->tRegion.tLocation.iY;

            switch (ptList->tMode.u2WorkMode) {
                case VSF_TGUI_LIST_MODE_FREE_MOVE:
                    /* do nothing */
                    return;
                case VSF_TGUI_LIST_MODE_FREE_MOVE_STICK_TO_ITEM:
                    /* no need to do anything */
                    break;
                case VSF_TGUI_LIST_MODE_ITEM_SELECTION:
                    nYOffset = __vk_tgui_calculate_offset_for_make_target_control_visible(
                                    ptList,
                                    ptControl,
                                    nYOffset,
                                    true
                                );
                    break;
                case VSF_TGUI_LIST_MODE_ITEM_SELECTION_CENTER_ALIGN:
                    nYOffset -= (   vsf_tgui_control_get_core((const vsf_tgui_control_t *)ptList)->tRegion.tSize.iHeight
                                -   vsf_tgui_control_get_core(ptControl)->tRegion.tSize.iHeight) / 2;
                    break;
                default:
                    break;
            }

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

            switch (ptList->tMode.u2WorkMode) {
                case VSF_TGUI_LIST_MODE_FREE_MOVE:
                    /* do nothing */
                    return;
                case VSF_TGUI_LIST_MODE_FREE_MOVE_STICK_TO_ITEM:
                    /* no need to do anything */
                    break;
                case VSF_TGUI_LIST_MODE_ITEM_SELECTION:
                    nXOffset = __vk_tgui_calculate_offset_for_make_target_control_visible(
                                    ptList,
                                    ptControl,
                                    nXOffset,
                                    false
                                );
                    break;
                case VSF_TGUI_LIST_MODE_ITEM_SELECTION_CENTER_ALIGN:
                    nXOffset -= (   vsf_tgui_control_get_core((const vsf_tgui_control_t *)ptList)->tRegion.tSize.iWidth
                                -   vsf_tgui_control_get_core(ptControl)->tRegion.tSize.iWidth) / 2;
                    break;
                default:
                    break;
            }

        #if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
            vk_tgui_slider_location_target_set( &(ptList->tSlider), -nXOffset);
        #else
            ptInnerContainer->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iX = -nXOffset;
        #endif
            break;
    }

#if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
    if (!ptList->tMode.bIsSliding && vk_tgui_slider_is_working(&(ptList->tSlider))) {
        ptList->tMode.bIsSliding = true;
        __vk_tgui_list_invoke_event(ptList, VSF_TGUI_EVT_LIST_SLIDING_STARTED);
    }
#endif

}
#if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
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
#endif

static bool __vk_tgui_list_invoke_event(vsf_tgui_list_t* ptList, vsf_evt_t tMSG)
{
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    vsf_tgui_evt_t tEvent = {0};
    tEvent.use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.tMSG = tMSG;
    return VSf_tgui_control_send_message(
        (const vsf_tgui_control_t *)ptList, tEvent);

#else
    return vsf_tgui_control_send_message(
        (const vsf_tgui_control_t *)ptList,
        (vsf_tgui_evt_t) {
            .tMSG = tMSG,
        });
#endif
}

static bool __vk_tgui_list_previous(vsf_tgui_list_t* ptList)
{

    if (ptList->chStartIndex == 0) {
        return false;
    }
    ptList->chStartIndex--;
    __vk_tgui_list_invoke_event(ptList, VSF_TGUI_EVT_LIST_SELECTION_CHANGED);

    __vsf_tgui_list_adjust_inner_container_location(ptList, ptList->chStartIndex);

    return true;
}

static bool __vk_tgui_list_next(vsf_tgui_list_t* ptList)
{
    if (ptList->chStartIndex == (ptList->ptList->chVisibleItemCount - 1)) {
        return false;
    } else if (ptList->chStartIndex >= ptList->ptList->chVisibleItemCount) {
        ptList->chStartIndex = ptList->ptList->chVisibleItemCount - 1;
    } else {
        ptList->chStartIndex++;
    }
    
    __vk_tgui_list_invoke_event(ptList, VSF_TGUI_EVT_LIST_SELECTION_CHANGED);
    __vsf_tgui_list_adjust_inner_container_location(ptList, ptList->chStartIndex);

    return true;
}

fsm_rt_t vsf_tgui_list_msg_handler(vsf_tgui_list_t* ptList, vsf_tgui_msg_t* ptMSG)
{
    fsm_rt_t fsm;

#if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
    if (VSF_TGUI_EVT_ON_TIME == ptMSG->use_as__vsf_msgt_msg_t.tMSG) {
        __vk_tgui_list_update_inner_container_location(ptList);
        if (!vk_tgui_slider_is_working(&(ptList->tSlider)) && ptList->tMode.bIsSliding) {
            ptList->tMode.bIsSliding = false;
            __vk_tgui_list_invoke_event(ptList, VSF_TGUI_EVT_LIST_SLIDING_STOPPED);
        }
        return (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
    }
#endif
    fsm = __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t *)ptList,
                                            ptMSG,
                                            &c_tVList);

    bool bStatusChanged = false;

    if (VSF_TGUI_EVT_LIST_SELECTION_CHANGED == ptMSG->use_as__vsf_msgt_msg_t.tMSG) {
        //! block backward propagation
        if (VSF_TGUI_MSG_RT_UNHANDLED == fsm) {
            fsm = (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
        }
    } else if (VSF_TGUI_EVT_KEY_PRESSED == ptMSG->use_as__vsf_msgt_msg_t.tMSG){
        vsf_tgui_key_evt_t* ptEvt = (vsf_tgui_key_evt_t*)ptMSG;
        switch (ptList->ptList->tContainerAttribute.u5Type) {
            case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
                if          (VSF_TGUI_KEY_LEFT == ptEvt->hwKeyValue) {
                    bStatusChanged = __vk_tgui_list_previous(ptList);
                } else if   (VSF_TGUI_KEY_RIGHT == ptEvt->hwKeyValue) {
                    bStatusChanged = __vk_tgui_list_next(ptList);
                } else {
                    break;
                }
                fsm = (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
                break;
            case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
                if (VSF_TGUI_KEY_UP == ptEvt->hwKeyValue) {
                    bStatusChanged = __vk_tgui_list_previous(ptList);
                }
                else if (VSF_TGUI_KEY_DOWN == ptEvt->hwKeyValue) {
                    bStatusChanged = __vk_tgui_list_next(ptList);
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
                    bStatusChanged = __vk_tgui_list_previous(ptList);
                    break;
                } else if (ptEvt->tDelta.use_as__vsf_tgui_location_t.iX < 0) {
                    bStatusChanged = __vk_tgui_list_next(ptList);
                    break;
                }
                //! fall-through
            case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
                if (ptEvt->tDelta.use_as__vsf_tgui_location_t.iY > 0) {
                    bStatusChanged = __vk_tgui_list_previous(ptList);
                } else if (ptEvt->tDelta.use_as__vsf_tgui_location_t.iY < 0) {
                    bStatusChanged = __vk_tgui_list_next(ptList);
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

/*
static void __vk_tgui_list_start_index_validate(vsf_tgui_list_t* ptList)
{
    uint8_t chVisibleItemCount = ptList->ptList->chVisibleItemCount;
    if (ptList->chStartIndex >= chVisibleItemCount) {
        ptList->chStartIndex = chVisibleItemCount - 1;
        __vk_tgui_list_invoke_selection_changed_event(ptList);
    }
}
*/
static void __vk_tgui_list_select_index_validate(vsf_tgui_list_t* ptList)
{
    uint8_t chVisibleItemCount = ptList->ptList->chVisibleItemCount;
    if (ptList->chSelectIndex >= chVisibleItemCount) {
        ptList->chSelectIndex = chVisibleItemCount - 1;
        __vk_tgui_list_invoke_event(ptList, VSF_TGUI_EVT_LIST_SELECTION_CHANGED);
    }
}


fsm_rt_t vk_tgui_list_update(vsf_tgui_list_t* ptList)
{
    /*! \note before this function is called, all controls in the container are
              updated with TREE_UPDATE message
     */

    //__vk_tgui_list_start_index_validate(ptList);
    __vk_tgui_list_select_index_validate(ptList);

    __vsf_tgui_list_adjust_inner_container_location(ptList, ptList->chStartIndex);

#if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
    vk_tgui_slider_reset(&(ptList->tSlider));
#endif

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

    vk_tgui_container_init(&(ptList->use_as__vsf_tgui_container_t));

    do {
        vsf_tgui_status_t tStatus = vsf_tgui_control_status_get((vsf_tgui_control_t*)ptList);
        tStatus.tValues.__bContainBuiltInStructure = true;

        vsf_tgui_control_status_set((vsf_tgui_control_t*)ptList, tStatus);
    } while(0);

    return fsm_rt_cpl;
}

int_fast16_t vk_tgui_list_display_item_start_get(vsf_tgui_list_t* ptList)
{
    VSF_TGUI_ASSERT(NULL != ptList);

    if (ptList->u2WorkMode != VSF_TGUI_LIST_MODE_FREE_MOVE_STICK_TO_ITEM) {
        return -1;
    }

    return ptList->chStartIndex;
}

int_fast16_t vk_tgui_list_display_item_start_set(vsf_tgui_list_t* ptList,
                                                uint_fast8_t chStartIndex)
{
    VSF_TGUI_ASSERT(NULL != ptList);
    if (ptList->u2WorkMode != VSF_TGUI_LIST_MODE_FREE_MOVE_STICK_TO_ITEM) {
        return -1;
    }
    if (chStartIndex != ptList->chStartIndex) {
        ptList->chStartIndex = chStartIndex;
        __vk_tgui_list_invoke_event(ptList, VSF_TGUI_EVT_LIST_SELECTION_CHANGED);
    }
    return ptList->chStartIndex;
}


int_fast16_t vk_tgui_list_selected_item_get(vsf_tgui_list_t* ptList)
{
    VSF_TGUI_ASSERT(NULL != ptList);

    if (ptList->u2WorkMode < VSF_TGUI_LIST_MODE_ITEM_SELECTION) {
        return -1;
    }

    return ptList->chSelectIndex;
}

int_fast16_t vk_tgui_list_selected_item_set(vsf_tgui_list_t* ptList,
                                            uint_fast8_t chSelectIndex)
{
    VSF_TGUI_ASSERT(NULL != ptList);
    if (ptList->u2WorkMode < VSF_TGUI_LIST_MODE_ITEM_SELECTION) {
        return -1;
    }
    ptList->chSelectIndex = chSelectIndex;

    if (chSelectIndex != ptList->chSelectIndex) {
        ptList->chSelectIndex = chSelectIndex;
        __vk_tgui_list_invoke_event(ptList, VSF_TGUI_EVT_LIST_SELECTION_CHANGED);
    }

    return ptList->chSelectIndex;
}

#if VSF_TGUI_CFG_LIST_SUPPORT_SCROOLBAR == ENABLED
SECTION(".text.vsf.component.tgui.vsf_tgui_list_scrollbar_regions_generate");
vsf_tgui_list_scrollbar_region_t * vsf_tgui_list_scrollbar_regions_generate(
                            const vsf_tgui_control_t* ptControl,
                            const vsf_tgui_list_t* ptList, 
                            uint_fast8_t chScalingRatio, 
                            vsf_tgui_list_scrollbar_region_t *ptOutputBuffer)
{
    VSF_TGUI_ASSERT(    NULL != ptControl 
                    &&  NULL != ptList 
                    &&  chScalingRatio > 0 
                    &&  NULL != ptOutputBuffer);

    const vsf_tgui_container_t* ptInnerContainer = ptList->ptList;
    const vsf_tgui_size_t *ptPanelSize = 
        vsf_tgui_control_get_size((const vsf_tgui_control_t *)ptControl);
    const vsf_tgui_size_t *ptListSize = 
        vsf_tgui_control_get_size((const vsf_tgui_control_t *)ptList);
    const vsf_tgui_size_t *ptListInnerContainerSize = 
        vsf_tgui_control_get_size((const vsf_tgui_control_t *)ptInnerContainer);
    const vsf_tgui_location_t *ptListInnerContainerLocation = 
        vsf_tgui_control_get_location((const vsf_tgui_control_t *)ptInnerContainer);

    int16_t iScrollbarTrackPieceSize;
    int16_t iScrollbarSize;
    int16_t iInnerContainerRawSize;
    int16_t iListRawSize;
    int16_t iOriginOffset;
    int16_t iLineHeight = 4;
    int16_t iLineWidth = 4;

    switch (ptInnerContainer->tContainerAttribute.u5Type) {
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
            iListRawSize = ptListSize->iHeight;
            iInnerContainerRawSize =    ptListInnerContainerSize->iHeight 
                                    -   ptInnerContainer->tConatinerPadding.chTop
                                    -   ptInnerContainer->tConatinerPadding.chBottom;
            iScrollbarSize = iListRawSize/chScalingRatio;
            iScrollbarTrackPieceSize = iInnerContainerRawSize/chScalingRatio;

            iOriginOffset = (ptPanelSize->iHeight/2)-(iScrollbarTrackPieceSize/2);

            do {
                vsf_tgui_list_scrollbar_region_t tRectRegionsY = {
                    {
                        .tLocation = { .iX = ptPanelSize->iWidth - iLineWidth, .iY = iOriginOffset },
                        .tSize = { .iWidth = iLineWidth, .iHeight = iScrollbarTrackPieceSize },
                    }, // right
                    {
                        .tLocation = { .iX = ptPanelSize->iWidth - iLineWidth, .iY = iOriginOffset },
                        .tSize = { .iWidth = iLineWidth, .iHeight = iScrollbarSize },
                    }, // right
                };
 
                tRectRegionsY.tBar.tLocation.iY +=iScrollbarTrackPieceSize*(-ptListInnerContainerLocation->iY)/iInnerContainerRawSize;    // 滚动条为定值时必须用这条
                if ((tRectRegionsY.tBar.tLocation.iY+iScrollbarSize)>(tRectRegionsY.tTrack.tLocation.iY+iScrollbarTrackPieceSize)) {
                     tRectRegionsY.tBar.tSize.iHeight-=tRectRegionsY.tBar.tLocation.iY+iScrollbarSize-tRectRegionsY.tTrack.tLocation.iY-iScrollbarTrackPieceSize;
                }
                if ((tRectRegionsY.tBar.tLocation.iY)<(tRectRegionsY.tTrack.tLocation.iY)) {
                     tRectRegionsY.tBar.tSize.iHeight-=(tRectRegionsY.tTrack.tLocation.iY-tRectRegionsY.tBar.tLocation.iY);//截短
                     tRectRegionsY.tBar.tLocation.iY=tRectRegionsY.tTrack.tLocation.iY;
                }
                *ptOutputBuffer = tRectRegionsY;
            } while(0);

            break;

        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
            iListRawSize = ptListSize->iWidth;
            iInnerContainerRawSize =    ptListInnerContainerSize->iWidth 
                                    -   ptInnerContainer->tConatinerPadding.chLeft
                                    -   ptInnerContainer->tConatinerPadding.chRight;
            iScrollbarSize = iListRawSize/chScalingRatio;
            iScrollbarTrackPieceSize = iInnerContainerRawSize/chScalingRatio;
            iOriginOffset = (ptPanelSize->iWidth / 2) - (iScrollbarTrackPieceSize/2); // 居中

            do {
                vsf_tgui_list_scrollbar_region_t tRectRegionsX = {
                    {
                        .tLocation = { .iX = iOriginOffset, .iY = ptPanelSize->iHeight - iLineHeight },
                        .tSize = { .iWidth = iScrollbarTrackPieceSize, .iHeight = iLineHeight },
                    }, // bottom 滑动条槽
                    {
                        .tLocation = { .iX = iOriginOffset, .iY = ptPanelSize->iHeight - iLineHeight },
                        .tSize = { .iWidth = iScrollbarSize, .iHeight = iLineHeight },
                    }, // bottom
                };
            
                tRectRegionsX.tBar.tLocation.iX +=iScrollbarTrackPieceSize*(-ptListInnerContainerLocation->iX)/iInnerContainerRawSize;
                if ((tRectRegionsX.tBar.tLocation.iX+iScrollbarSize)>(tRectRegionsX.tTrack.tLocation.iX+iScrollbarTrackPieceSize)) {
                     tRectRegionsX.tBar.tSize.iWidth-=tRectRegionsX.tBar.tLocation.iX+iScrollbarSize-tRectRegionsX.tTrack.tLocation.iX-iScrollbarTrackPieceSize;
                }

                if ((tRectRegionsX.tBar.tLocation.iX)<(tRectRegionsX.tTrack.tLocation.iX)) {
                     tRectRegionsX.tBar.tSize.iWidth-=(tRectRegionsX.tTrack.tLocation.iX-tRectRegionsX.tBar.tLocation.iX);//截短
                     tRectRegionsX.tBar.tLocation.iX=tRectRegionsX.tTrack.tLocation.iX;
                }

                *ptOutputBuffer = tRectRegionsX;
            } while(0);

            break;

        default:
            return NULL;
    }

    return ptOutputBuffer;
}

#endif

#endif


/* EOF */
