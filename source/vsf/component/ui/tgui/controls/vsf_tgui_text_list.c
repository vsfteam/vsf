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
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED && VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED

#define __VSF_TGUI_CONTROLS_TEXT_LIST_CLASS_IMPLEMENT
#define __VK_TGUI_CONTROLS_SLIDER_CLASS_INHERIT

declare_class(vsf_tgui_t)
#include "./vsf_tgui_text_list.h"
#include "../view/vsf_tgui_v.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
int_fast16_t __vk_tgui_label_get_line_height( const vsf_tgui_label_t* ptLabel);

/*============================ LOCAL VARIABLES ===============================*/

static const i_tgui_control_methods_t c_tVTextList= {

    .tView = {
        .Init =     (vsf_tgui_method_t *)&vsf_tgui_text_list_v_init,
        .Depose =   (vsf_tgui_method_t *)&vsf_tgui_text_list_v_depose,
        .Render =   (vsf_tgui_v_method_render_t *)&vsf_tgui_text_list_v_rendering,
        .ContainerPostRender = (vsf_tgui_v_method_render_t *)&vsf_tgui_text_list_v_post_rendering,
        .Update =   (vsf_tgui_method_t *)&vsf_tgui_text_list_v_update,
    },
    .Init =     (vsf_tgui_method_t *)&vk_tgui_text_list_init,
    .Update =   (vsf_tgui_method_t *)&vk_tgui_text_list_update,

};



/*============================ IMPLEMENTATION ================================*/

static int_fast16_t __vk_tgui_text_list_get_real_container_y(
                                                const vsf_tgui_text_list_t* ptTextList,
                                                int_fast16_t iTargetLineY)
{
    int_fast16_t iContentHeight = ptTextList->tList.tContent.tRegion.tSize.iHeight;

    while (iTargetLineY < 0) {
        iTargetLineY += iContentHeight;

        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
            iTargetLineY += ptTextList->tList.tBuffer.tMargin.chTop;
        #endif
    }

    while (iTargetLineY >= iContentHeight) {
        iTargetLineY -= iContentHeight;

        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
            iTargetLineY -= ptTextList->tList.tBuffer.tMargin.chTop;
        #endif
    }

    return iTargetLineY;
}


static int_fast16_t __vk_tgui_text_list_calculate_container_y_adjust(
                                                    int_fast16_t iLineHeight,
                                                    int_fast16_t iListHeight
                                                )
{
    return  (iListHeight - iLineHeight) / 2;
}

static int_fast16_t __vk_tgui_text_list_calculate_container_y_height(
                                                    int_fast16_t iLineHeight,
                                                    int_fast16_t iLineSelect,
                                                    int_fast8_t chLineSpace
                                                )
{
    return  (iLineHeight + chLineSpace) * iLineSelect;
}



static int_fast16_t __vk_tgui_text_list_calculate_container_y(
                                                    int_fast16_t iLineHeight,
                                                    int_fast16_t iLineSelect,
                                                    int_fast16_t iListHeight,
                                                    int_fast8_t chLineSpace
                                                )
{
    int_fast16_t iResult = 0;

    iResult = __vk_tgui_text_list_calculate_container_y_height (iLineHeight, iLineSelect, chLineSpace);
    iResult -= __vk_tgui_text_list_calculate_container_y_adjust(iLineHeight, iListHeight);

    return iResult;
}

static int16_t __vk_tgui_text_list_get_safe_line_selection(vsf_tgui_text_list_t* ptTextList,
                                                        int_fast16_t iLineSelect)
{
    uint_fast16_t hwLineCount = ptTextList->hwLineCount;

    if (0 == hwLineCount) {
        return 0;
    }

    while(iLineSelect >= (int_fast16_t)hwLineCount) {
        iLineSelect -= hwLineCount;
    }

    while(iLineSelect < 0) {
        iLineSelect += hwLineCount;
    }

    return iLineSelect;
}

static bool __vk_tgui_text_list_invoke_event(vsf_tgui_text_list_t* ptTextList, vsf_evt_t msg)
{

    return vsf_tgui_control_send_message(
        (const vsf_tgui_control_t *)ptTextList,
        (vsf_tgui_evt_t) {
            .msg = msg,
        });
}

static void __vk_tgui_text_list_update_line_selection(
                                                    vsf_tgui_text_list_t* ptTextList,
                                                    int_fast16_t iLineSelect)
{
    uint_fast16_t hwLineCount = ptTextList->hwLineCount;
    int_fast16_t iYOffset = 0;


    if (0 == hwLineCount) {
        return;
    }

    /*! \NOTE IMPORTANT  Please do NOT set ptTextList->iLineSelect with iLineSelect
     *!                  in this function. Its value is deliberately allowed to be 
     *!                  out of the legal range, i.e. to be negative or larger than
     *!                  the  ptTextList->hwLineCount.
     *!                  The range validation work is done in other place.
     */

#if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABLED

    do {
        int_fast16_t iLineDif = ptTextList->iLineSelect - ptTextList->iOldLineSelect;
        ptTextList->iOldLineSelect = ptTextList->iLineSelect;
        //if (iLineDif != 0) {
            iYOffset = __vk_tgui_text_list_calculate_container_y_height(
                    __vk_tgui_label_get_line_height(&(ptTextList->tList.tContent)),
                    iLineDif,
                    ptTextList->tList.tContent.tLabel.chInterLineSpace);
            vk_tgui_slider_location_target_increase( &(ptTextList->tSlider), iYOffset);
            
        //}
    } while(0);

#else
    iLineSelect = __vk_tgui_text_list_get_safe_line_selection(ptTextList, iLineSelect);

    int_fast16_t iListHeight = ptTextList->tRegion.tSize.iHeight;

    iYOffset = __vk_tgui_text_list_calculate_container_y(
                    __vk_tgui_label_get_line_height(&(ptTextList->tList.tContent)),
                    iLineSelect,
                    iListHeight,
                    ptTextList->tList.tContent.tLabel.chInterLineSpace);


     iYOffset = __vk_tgui_text_list_get_real_container_y(
                        (const vsf_tgui_text_list_t* )ptTextList, iYOffset);


    ptTextList->tList.tRegion.tLocation.iY = -iYOffset;
    vsf_tgui_control_refresh(ptTextList, NULL);
#endif

}

static void __vk_tgui_text_list_internal_update(vsf_tgui_text_list_t* ptTextList)
{
    ptTextList->tList.tBuffer = ptTextList->tList.tContent;
    ptTextList->tList.tContent.Offset.next =
        (intptr_t)&ptTextList->tList.tBuffer - (intptr_t)&ptTextList->tList.tContent;
    ptTextList->tList.tBuffer.Offset.next = 0;
#if VSF_MSG_TREE_CFG_SUPPORT_DUAL_LIST == ENABLED
    ptTextList->tList.tBuffer.Offset.previous =
        (intptr_t)&ptTextList->tList.tBuffer - (intptr_t)&ptTextList->tList.tContent;
#endif


    ptTextList->tList.tBuffer.iY = ptTextList->tList.tContent.iHeight 
                                 + ptTextList->tList.tContent.tLabel.chInterLineSpace;

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
    ptTextList->tList.tBuffer.tMargin.chTop = ptTextList->tList.tContent.tLabel.chInterLineSpace;
#endif 
#   if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    ptTextList->hwLineCount = ptTextList->tList.tContent.tLabel.tInfoCache.hwLines;
#else
    __vk_tgui_label_v_text_get_size(&(ptTextList->tList.tContent), &ptTextList->hwLineCount, NULL);
#endif
}



#if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABLED
static void __vk_tgui_text_list_use_minimal_position(vsf_tgui_text_list_t* ptTextList)
{
    int_fast16_t iRealPosition = __vk_tgui_text_list_get_real_container_y(
                ptTextList,
                vk_tgui_slider_location_target_get(&(ptTextList->tSlider)));

    /*iRealPosition -= __vk_tgui_text_list_calculate_container_y_adjust(
                    __vk_tgui_label_get_line_height(&(ptTextList->tList.tContent)),
                    vsf_tgui_control_get_core(
                        (const vsf_tgui_control_t *)ptTextList)
                            ->tRegion.tSize.iHeight);
     */
    vk_tgui_slider_location_set(&(ptTextList->tSlider), iRealPosition, iRealPosition);

}


static void __vk_tui_text_list_update_container_position(vsf_tgui_text_list_t* ptTextList)
{
    ptTextList->tList.tRegion.tLocation.iY =
            - __vk_tgui_text_list_get_real_container_y(
                ptTextList,

                    vk_tgui_slider_on_timer_event_handler(&(ptTextList->tSlider))
                -   __vk_tgui_text_list_calculate_container_y_adjust(
                        __vk_tgui_label_get_line_height(&(ptTextList->tList.tContent)),
                        ptTextList->tRegion.tSize.iHeight)
            );
}
#endif

fsm_rt_t vsf_tgui_text_list_msg_handler( vsf_tgui_text_list_t* ptTextList,
                                            vsf_tgui_msg_t* ptMSG)
{
    fsm_rt_t fsm;
    bool bStatusChanged = false;

#if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABLED

    if (VSF_TGUI_EVT_ON_TIME == ptMSG->use_as__vsf_msgt_msg_t.msg) {

        //vk_tgui_slider_on_timer_event_handler(&(ptTextList->tSlider));
        __vk_tui_text_list_update_container_position(ptTextList);

        if (!vk_tgui_slider_is_working(&(ptTextList->tSlider))) {
            //! adjust the line select to the minimal positive range
            ptTextList->iLineSelect = __vk_tgui_text_list_get_safe_line_selection(ptTextList, ptTextList->iLineSelect);
            ptTextList->iOldLineSelect = ptTextList->iLineSelect;

            __vk_tgui_text_list_use_minimal_position(ptTextList);
            
        }

        return (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
    }
#endif

    fsm = __vsf_tgui_control_msg_handler(   (vsf_tgui_control_t *)ptTextList,
                                            ptMSG,
                                            &c_tVTextList);

    if (VSF_TGUI_EVT_KEY_PRESSED == ptMSG->use_as__vsf_msgt_msg_t.msg){
        vsf_tgui_key_evt_t* ptEvt = (vsf_tgui_key_evt_t*)ptMSG;

        switch (ptEvt->hwKeyValue) {
            case VSF_TGUI_KEY_UP:
                ptTextList->iLineSelect--;
                bStatusChanged = true;
                break;
            case VSF_TGUI_KEY_DOWN:
                ptTextList->iLineSelect++;
                bStatusChanged = true;
                break;
            default:
                break;
        }
    }
#if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
    else if (VSF_TGUI_EVT_GESTURE_WHEEL == ptMSG->use_as__vsf_msgt_msg_t.msg) {
        vsf_tgui_gesture_evt_t* ptEvt = (vsf_tgui_gesture_evt_t*)ptMSG;
        if (ptEvt->delta.use_as__vsf_tgui_location_t.iY > 0) {
            ptTextList->iLineSelect++;
        } else {
            ptTextList->iLineSelect--;
        }
        bStatusChanged = true;
    }
#endif

    if (bStatusChanged) {
        __vk_tgui_text_list_update_line_selection(ptTextList, ptTextList->iLineSelect);
    #if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABLED
        ptTextList->iOldLineSelect = ptTextList->iLineSelect;
    #endif

        __vk_tgui_text_list_invoke_event(ptTextList, VSF_TGUI_EVT_LIST_SELECTION_CHANGED);

        fsm = VSF_TGUI_MSG_RT_REFRESH;
    }

    return fsm;
}


fsm_rt_t vk_tgui_text_list_update(vsf_tgui_text_list_t* ptTextList)
{
    bool __is_auto_size = ptTextList->ContainerAttribute.bIsAutoSize;
    fsm_rt_t result;
    /*! \note before this function is called, all controls in the container are
              updated with TREE_UPDATE message
     */
    __vk_tgui_text_list_internal_update(ptTextList);
    
    
    if (__is_auto_size) {
        /*! text list doesn't support autosize, let's fix this */
        ptTextList->ContainerAttribute.bIsAutoSize = false;
        ptTextList->iHeight = max(ptTextList->tList.tContent.iHeight, ptTextList->iHeight);
        ptTextList->iWidth = max(ptTextList->tList.iWidth, ptTextList->iWidth);
    } else {
        if (0 == ptTextList->iHeight) {
            
        #if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
            int16_t __char_height = ptTextList->tList.tContent.tLabel.tInfoCache.chCharHeight;
        #else
            int16_t __char_height = 0; 
            uint8_t __temp;
            __vk_tgui_label_v_text_get_size(&(ptTextList->tList.tContent.tLabel), NULL, &__temp);
            __char_height = __temp;
        #endif
            int16_t __line_height = __char_height + ptTextList->tList.tContent.tLabel.chInterLineSpace;

            ptTextList->iHeight = max(__char_height, __line_height);
        }  
        if (0 == ptTextList->iWidth) {
            ptTextList->iWidth = max(ptTextList->tList.iWidth, ptTextList->iWidth);
        }
    }

    __vk_tgui_text_list_update_line_selection(ptTextList, ptTextList->iLineSelect);

#if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABLED
    ptTextList->iOldLineSelect = ptTextList->iLineSelect;
    __vk_tgui_text_list_use_minimal_position(ptTextList);
    __vk_tui_text_list_update_container_position(ptTextList);
#endif

    result = vk_tgui_container_update( &(ptTextList->use_as__vsf_tgui_container_t));

    ptTextList->ContainerAttribute.bIsAutoSize = __is_auto_size;

    return result;
}

static fsm_rt_t vk_tgui_text_list_init(vsf_tgui_text_list_t* ptTextList)
{
#if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABLED
    vk_tgui_slider_init(&(ptTextList->tSlider),                     /*!< slider */
                        (const vsf_tgui_control_t *)ptTextList,     /*!< host control*/
                        0);                                         /*!< use default fps */
#endif

    __vk_tgui_text_list_internal_update(ptTextList);

    vk_tgui_container_init(
                &(ptTextList->use_as__vsf_tgui_container_t));

    do {
        vsf_tgui_status_t Status = vsf_tgui_control_status_get((vsf_tgui_control_t*)ptTextList);
        Status.Values.__does_contain_builtin_structure = true;

        vsf_tgui_control_status_set((vsf_tgui_control_t*)ptTextList, Status);
    } while(0);

    return fsm_rt_cpl;
}

int_fast16_t vsf_tgui_text_list_select_get(vsf_tgui_text_list_t* ptTextList)
{
    VSF_TGUI_ASSERT(NULL != ptTextList);

    return __vk_tgui_text_list_get_safe_line_selection(ptTextList, ptTextList->iLineSelect);
}

/*! \brief set the index of selection
 *! 
 *! \note if the input number is out of the valid range, the text list will automatically
 *!       correct it to the valid range.
 *! 
 *! \param ptTextList the target text list object
 *! \param iSelect user specified selection number
 *! \return none
 */
void vsf_tgui_text_list_select_set( vsf_tgui_text_list_t* ptTextList,
                                    int_fast16_t iSelect)
{
    VSF_TGUI_ASSERT(NULL != ptTextList);
    /*if (0 == ptTextList->hwLineCount) {
        return;
    }*/
#if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABLED
    //ptTextList->iOldLineSelect = iSelect;
#endif
    ptTextList->iLineSelect = iSelect;
    __vk_tgui_text_list_update_line_selection(ptTextList, iSelect);

}

#endif


/* EOF */
