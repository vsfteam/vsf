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

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_IMPLEMENT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_control.h"
#include "../view/vsf_tgui_v.h"

#include "../vsf_tgui.h"


#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
#   if VSF_KERNEL_CFG_CALLBACK_TIMER != ENABLED
#       error To use TGUI TIMER, you have to enable callback timer in kernel \
by setting macro VSF_KERNEL_CFG_CALLBACK_TIMER to ENABLED
#   endif
#endif
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static const i_tgui_control_methods_t c_tVControl= {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    {
        &vsf_tgui_control_v_init,
        &vsf_tgui_control_v_depose,
        &vsf_tgui_control_v_rendering,
        &vsf_tgui_control_v_update
    },
    &vk_tgui_control_init,
    &vk_tgui_control_update,
#else
    .tView = {
        .Init =     &vsf_tgui_control_v_init,
        .Depose =   &vsf_tgui_control_v_depose,
        .Render =   &vsf_tgui_control_v_rendering,
        .Update =   &vsf_tgui_control_v_update,
    },
    .Update =   &vk_tgui_control_update,
    .Init =     &vk_tgui_control_init,
#endif
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


/*----------------------------------------------------------------------------*
 *  Timer                                                                     *
 *----------------------------------------------------------------------------*/
#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED

static void __vk_tgui_control_timer_handler(vsf_tgui_timer_t *ptTimer)
{
    vsf_protect_t tProtectStatus;
    VSF_COMPONENT_ASSERT(NULL != ptTimer);

    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(ptTimer->ptTarget);
    
    if (NULL == ptTopContainer) {
        return ;
    }

    vk_tgui_send_timer_event(ptTopContainer->ptGUI, ptTimer->ptTarget);

    tProtectStatus = vsf_protect_sched();
        if (ptTimer->bEnabled && ptTimer->bIsRepeat) {
            ptTimer->u29Interval = max(100, ptTimer->u29Interval);
            vsf_callback_timer_add_ms(  &ptTimer->use_as__vsf_callback_timer_t,
                                        ptTimer->u29Interval);
        } else {
            ptTimer->tStatus.bIsWorking = false;
        }
    vsf_unprotect_sched(tProtectStatus);
}

void vsf_tgui_timer_init(   vsf_tgui_timer_t *ptTimer, 
                            const vsf_tgui_control_t *ptControl)
{
    VSF_COMPONENT_ASSERT(NULL != ptTimer);
    VSF_COMPONENT_ASSERT(NULL != ptControl);

    ptTimer->ptTarget = ptControl;
    ptTimer->use_as__vsf_callback_timer_t.on_timer 
        = (void (*)(vsf_callback_timer_t *timer))__vk_tgui_control_timer_handler;

    if (ptTimer->bEnabled){
        vsf_tgui_timer_enable(ptTimer);
    }
}

void vsf_tgui_timer_enable(vsf_tgui_timer_t *ptTimer)
{
    vsf_protect_t tProtectStatus;
    VSF_COMPONENT_ASSERT(NULL != ptTimer);

    tProtectStatus = vsf_protect_sched();
        do {
            if (ptTimer->tStatus.bIsWorking) {
                break;
            }
            if (ptTimer->bIsRepeat) {
                ptTimer->u29Interval = max(100, ptTimer->u29Interval);
            }
            ptTimer->tStatus.bIsWorking = true;
            ptTimer->tStatus.bEnabled = true;
            vsf_callback_timer_add_ms(  &ptTimer->use_as__vsf_callback_timer_t,
                                        ptTimer->u29Interval);
        } while(0);
    vsf_unprotect_sched(tProtectStatus);
}

void vsf_tgui_timer_disable(vsf_tgui_timer_t *ptTimer)
{
    
    vsf_protect_t tProtectStatus;
    VSF_COMPONENT_ASSERT(NULL != ptTimer);

    tProtectStatus = vsf_protect_sched();
        do {
            if (!ptTimer->tStatus.bIsWorking) {
                break;
            }
            vsf_callback_timer_remove(&(ptTimer->use_as__vsf_callback_timer_t));
            ptTimer->tStatus.bIsWorking = false;
        } while(0);
        ptTimer->tStatus.bEnabled = false;
    vsf_unprotect_sched(tProtectStatus);
}

bool vsf_tgui_timer_is_working(vsf_tgui_timer_t *ptTimer)
{
    VSF_COMPONENT_ASSERT(NULL != ptTimer);
    return ptTimer->tStatus.bIsWorking;
}

#endif

/*----------------------------------------------------------------------------*
 *  Region                                                                    *
 *----------------------------------------------------------------------------*/

/*! \brief get the absolute location information base on the location information
 *!        of or derived from target control.
 *! \param ptControl    the Target Control Address
 *! \param ptLocation   the Location buffer which has already stored the location
 *!                     information of or derived from the target control
 *! \return the location buffer address passed with ptLocation
 */
vsf_tgui_location_t* __vk_tgui_calculate_absolute_location_from_control_location(
                                                const vsf_tgui_control_t *ptControl,
                                                vsf_tgui_location_t *ptLocation)
{
    const vsf_msgt_node_t *ptNode = &ptControl->use_as__vsf_msgt_node_t;
    const vsf_msgt_container_t* ptParent = NULL;
    VSF_COMPONENT_ASSERT(NULL != ptControl && NULL != ptLocation);

    do {
        ptParent = (const vsf_msgt_container_t *)ptNode->ptParent;
        if (NULL == ptParent) {
            break;
        }
        ptNode = (const vsf_msgt_node_t*)ptParent;
        if (ptNode->tAttribute._.bIsContainer) {
            vsf_tgui_container_t *ptContainer = (vsf_tgui_container_t*)ptNode;
            ptLocation->iX += ptContainer->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iX;
            ptLocation->iY += ptContainer->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iY;
        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
            ptLocation->iX += ptContainer->tConatinerPadding.chLeft;
            ptLocation->iY += ptContainer->tConatinerPadding.chTop;
        #endif
        }
    } while(1);

    return ptLocation;
}

vsf_tgui_location_t * vsf_tgui_control_get_absolute_location(
                                            const vsf_tgui_control_t* ptControl,
                                            vsf_tgui_location_t* ptOffset)
{
    //const vsf_msgt_node_t* ptNode;
    __vsf_tgui_control_core_t* ptCore;
    VSF_TGUI_ASSERT(NULL != ptControl);
    VSF_TGUI_ASSERT(NULL != ptOffset);

    ptCore = vsf_tgui_control_get_core(ptControl);
    ptOffset->iX += ptCore->tRegion.tLocation.iX;
    ptOffset->iY += ptCore->tRegion.tLocation.iY;

    return __vk_tgui_calculate_absolute_location_from_control_location(ptControl, ptOffset);
}

vsf_tgui_region_t* vsf_tgui_control_get_absolute_region(
                                                const vsf_tgui_control_t *ptControl,
                                                vsf_tgui_region_t *ptRegion)
{
    return (vsf_tgui_region_t *)
                vsf_tgui_control_get_absolute_location(
                                                ptControl,
                                                (vsf_tgui_location_t *)ptRegion);
}

vsf_tgui_region_t * vsf_tgui_get_absolute_control_region(
                                                const vsf_tgui_control_t* ptControl,
                                                vsf_tgui_region_t* ptRegionBuffer)
{
    const __vsf_tgui_control_core_t * ptCore;

    VSF_COMPONENT_ASSERT(NULL != ptControl && NULL != ptRegionBuffer);

    ptCore = vsf_tgui_control_get_core(ptControl);
    VSF_COMPONENT_ASSERT(ptCore != NULL);

    *ptRegionBuffer = ptCore->tRegion;

    __vk_tgui_calculate_absolute_location_from_control_location(ptControl, &(ptRegionBuffer->tLocation));
    return ptRegionBuffer;
}

bool vsf_tgui_control_is_in_range(      const vsf_tgui_region_t *ptRegion,
                                        const vsf_tgui_location_t *ptLocation)
{
    bool bResult = false;
    VSF_COMPONENT_ASSERT(NULL != ptRegion && NULL != ptLocation);

    do {

        if (    (ptLocation->iX < ptRegion->tLocation.iX)
            ||  (ptLocation->iY < ptRegion->tLocation.iY)) {
            break;
        }

        {/*! to mitigate C89/90 warning */
            int_fast16_t iX = ptRegion->tLocation.iX + ptRegion->tSize.iWidth;
            int_fast16_t iY = ptRegion->tLocation.iY + ptRegion->tSize.iHeight;
            if (    (ptLocation->iX >= iX)
                ||  (ptLocation->iY >= iY)) {
                break;
            }
        }

        bResult = true;
    } while(0);

    return bResult;
}

/*! \brief get the visible region (with absolute location)
 *! \param ptControl    the target control
 *! \param ptRegion     the region buffer
 *! \retval true        visible
 *! \retval false       invisible
 */
bool vsf_tgui_control_get_visible_region(   const vsf_tgui_control_t* ptControl,
                                            vsf_tgui_region_t * ptRegionBuffer)
{
    bool bHasIntersect = true;

    vsf_tgui_get_absolute_control_region(ptControl, ptRegionBuffer);

#if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
    do {
        const vsf_tgui_container_t* ptParent =
            (const vsf_tgui_container_t*)ptControl->use_as__vsf_msgt_node_t.ptParent;
        vsf_tgui_region_t tParentRegion;

        while (NULL != ptParent) {
            tParentRegion = ptParent->use_as____vsf_tgui_control_core_t.tRegion;
            __vk_tgui_calculate_absolute_location_from_control_location((vsf_tgui_control_t*)ptParent, &(tParentRegion.tLocation));
            bHasIntersect = vsf_tgui_region_intersect(ptRegionBuffer, ptRegionBuffer, &tParentRegion);
            if (!bHasIntersect) {
                break;
            }
            ptParent = (const vsf_tgui_container_t *)ptParent->use_as__vsf_msgt_container_t.use_as__vsf_msgt_node_t.ptParent;
        }
    } while (0);
#endif
    return bHasIntersect;
}


vsf_tgui_region_t* vsf_tgui_control_get_relative_region(
                                                const vsf_tgui_control_t* ptControl,
                                                vsf_tgui_region_t* ptAbsoluteRegion)
{
    vsf_tgui_region_t tRegion;
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(ptControl);
    tRegion = ptCore->tRegion;

    VSF_COMPONENT_ASSERT(NULL != ptControl && NULL != ptAbsoluteRegion);

    __vk_tgui_calculate_absolute_location_from_control_location(ptControl, &(tRegion.tLocation));

    vsf_tgui_region_get_relative_region(ptAbsoluteRegion, &tRegion, ptAbsoluteRegion);

    return ptAbsoluteRegion;
}


vsf_tgui_region_t * vsf_tgui_control_generate_dirty_region_from_parent_dirty_region(
                                    const vsf_tgui_control_t *ptParent, 
                                    const vsf_tgui_region_t *ptParentDirtyRegion,
                                    const vsf_tgui_control_t *ptPrivate,
                                    vsf_tgui_region_t *ptNewDirtyRegionBuffer)
{
    VSF_TGUI_ASSERT(NULL != ptParent && NULL != ptPrivate && NULL != ptNewDirtyRegionBuffer);
    vsf_tgui_region_t tParentDirtyRegion;
    if (NULL != ptParentDirtyRegion) {
        tParentDirtyRegion = *ptParentDirtyRegion;
    } else {
        tParentDirtyRegion = vsf_tgui_control_get_core(ptParent)->tRegion;
    }

    vsf_tgui_control_get_absolute_region(ptParent, &tParentDirtyRegion);
    vsf_tgui_get_absolute_control_region(ptPrivate, ptNewDirtyRegionBuffer);
    vsf_tgui_region_intersect(  ptNewDirtyRegionBuffer, 
                                ptNewDirtyRegionBuffer, 
                               &tParentDirtyRegion);
    vsf_tgui_control_get_relative_region(ptPrivate, ptNewDirtyRegionBuffer);

    return ptNewDirtyRegionBuffer;
}


bool vsf_tgui_control_shoot(    const vsf_tgui_control_t* ptControl,
                                const vsf_tgui_location_t *ptLocation)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(ptControl);
    vsf_tgui_region_t tRegion;
    VSF_COMPONENT_ASSERT(NULL != ptControl && NULL != ptLocation);
    uint_fast8_t chMask =   (   VSF_TGUI_CTRL_STATUS_INITIALISED
                            |   VSF_TGUI_CTRL_STATUS_VISIBLE);

    if (chMask != (ptCore->tStatus.chStatus & chMask)) {
        return false;
    }

    if (!vsf_tgui_control_get_visible_region(ptControl, &tRegion)) {
        return false;
    }

    return vsf_tgui_control_is_in_range( &tRegion, ptLocation);
}


/*----------------------------------------------------------------------------*
 *  Status and Attributes                                                     *
 *----------------------------------------------------------------------------*/

 /*! \brief update bIsControlTransparent bit in control status
  *! \parame ptControl target control address
  *! \retval true the original value of bIsControlTransparent is changed
  *! \retval false the set value is the same as the original value, no change is
  *!               made.
  */
bool vsf_tgui_control_set_is_transparent_bit(vsf_tgui_control_t* ptControl,
                                             bool bIsControlTransparent)
{
    bool bResult = false;
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(ptControl);
    ASSERT(NULL != ptControl);

    bResult =       bIsControlTransparent 
                !=  (ptCore->tStatus.tValues.bIsControlTransparent);

    if (bResult) {
        ptCore->tStatus.tValues.bIsControlTransparent = bIsControlTransparent;
    }

    return bResult;
}

vsf_tgui_status_t vsf_tgui_control_status_get(const vsf_tgui_control_t* ptControl)
{
    __vsf_tgui_control_core_t *ptCore = vsf_tgui_control_get_core(ptControl);
    VSF_COMPONENT_ASSERT(NULL != ptControl);

    return ptCore->tStatus;
}

void vsf_tgui_control_status_set(vsf_tgui_control_t* ptControl, vsf_tgui_status_t tStatus)
{
    __vsf_tgui_control_core_t *ptCore = vsf_tgui_control_get_core(ptControl);
    VSF_COMPONENT_ASSERT(NULL != ptControl);

    ptCore->tStatus = tStatus;
}

bool vsf_tgui_control_is_container(const vsf_tgui_control_t* ptControl)
{
    VSF_TGUI_ASSERT(NULL != ptControl);
    return ptControl->use_as__vsf_msgt_node_t.tAttribute._.bIsContainer;
}

__vsf_tgui_control_core_t* vsf_tgui_control_get_core(const vsf_tgui_control_t* ptControl)
{
    VSF_TGUI_ASSERT(NULL != ptControl);
    if (vsf_tgui_control_is_container(ptControl)) {
        const vsf_msgt_node_t* ptNode = &ptControl->use_as__vsf_msgt_node_t;
        const vsf_tgui_container_t* ptContainer = (const vsf_tgui_container_t*)ptNode;
        return (__vsf_tgui_control_core_t *)&ptContainer->use_as____vsf_tgui_control_core_t;
    }

    return (__vsf_tgui_control_core_t*)&ptControl->use_as____vsf_tgui_control_core_t;
}

vsf_tgui_control_t* vsf_tgui_control_get_parent(const vsf_tgui_control_t* ptControl)
{
    VSF_TGUI_ASSERT(NULL != ptControl);
    return (vsf_tgui_control_t *)ptControl->use_as__vsf_msgt_node_t.ptParent;
}


uint_fast8_t vk_tgui_container_visible_item_get(const vsf_tgui_container_t *ptContainer)
{
    VSF_TGUI_ASSERT(NULL != ptContainer);
    return ptContainer->chVisibleItemCount;
}

vsf_tgui_size_t vsf_tgui_control_get_size(const vsf_tgui_control_t* ptControl)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(ptControl);
    return ptCore->tRegion.tSize;
}

/*----------------------------------------------------------------------------*
 *  Methods and Others                                                        *
 *----------------------------------------------------------------------------*/

const vsf_tgui_top_container_t *
vk_tgui_control_get_top(const vsf_tgui_control_t* ptControl)
{
    //const vsf_tgui_top_container_t* ptTopContainer = NULL;
    VSF_COMPONENT_ASSERT(NULL != ptControl);

    while (NULL != ptControl->use_as__vsf_msgt_node_t.ptParent) {
        ptControl = (const vsf_tgui_control_t*)
            ptControl->use_as__vsf_msgt_node_t.ptParent;
    }
    VSF_COMPONENT_ASSERT(ptControl->use_as__vsf_msgt_node_t.tAttribute._.bIsTop);

    return (const vsf_tgui_top_container_t*)ptControl;
}

bool vsf_tgui_control_update(const vsf_tgui_control_t* ptControl)
{
    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(ptControl);

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_update(ptTopContainer->ptGUI, ptControl);
}

bool vsf_tgui_control_update_tree(const vsf_tgui_control_t* ptControl)
{
    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(ptControl);

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_update_tree(ptTopContainer->ptGUI, ptControl);
}


#if VSF_TGUI_CFG_SUPPORT_REFRESH_SCHEME == ENABLED
bool vsf_tgui_control_refresh(  const vsf_tgui_control_t *ptControl,
                                const vsf_tgui_region_t *ptRegion)
{
    const vsf_tgui_top_container_t* ptTopContainer 
        = vk_tgui_control_get_top(ptControl);
    
    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_refresh_ex(ptTopContainer->ptGUI, ptControl, ptRegion);
}
#endif

bool vsf_tgui_control_set_active(const vsf_tgui_control_t* ptControl)
{
    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(ptControl);
    vsf_tgui_evt_t tTempEvent = { 
            .tMSG = VSF_TGUI_EVT_GET_ACTIVE,
            .ptTarget = (vsf_tgui_control_t*)ptControl,
        };

    if (NULL == ptTopContainer) {
        return false;
    }

    return vsf_tgui_send_message(ptTopContainer->ptGUI, tTempEvent);
}


#undef THIS_FSM_STATE
#define THIS_FSM_STATE  ptCore->tMSGMap.chState

#define RESET_TGUI_USER_MSG_HANDLING_FSM()                          \
        do { THIS_FSM_STATE = 0; } while(0)

fsm_rt_t __vk_tgui_control_user_message_handling(   vsf_tgui_control_t* ptControl, 
                                                    const vsf_tgui_evt_t* ptEvent)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(ptControl);
    uint_fast8_t chCount = ptCore->tMSGMap.chCount;
    const vsf_tgui_user_evt_handler *ptItems = ptCore->tMSGMap.ptItems;
    uint_fast16_t hwMSG = ptEvent->use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.tMSG;
    

    VSF_COMPONENT_ASSERT(NULL != ptControl && NULL != ptEvent);

    enum {
        START = 0,
        SEARCH_MSG_MAP,
        CHECK_USER_HANDLER_TYPE,
        CALL_FSM,
    };
    
    switch (THIS_FSM_STATE) {
        case START:
            if ((0 == chCount) || (NULL == ptItems) || (chCount > 254)) {
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            }
            ptCore->tMSGMap.chIndex = 255;
            THIS_FSM_STATE++;
            //break;
        case SEARCH_MSG_MAP: {
            uint_fast8_t chIndex = 0;
            do {
                //! get mask
                #if 0
                uint_fast16_t hwMask = (uint_fast16_t)(-1);
                if (0 != ptItems->use_as__vsf_tgui_control_handler_t.u10EvtMask) {
                    hwMask = ptItems->use_as__vsf_tgui_control_handler_t.u10EvtMask;
                }
                #else   
                uint_fast16_t hwMask = ptItems->use_as__vsf_tgui_control_handler_t.u10EvtMask;
                #endif
        
                //! check message
                if (    (ptItems->use_as__vsf_msgt_msg_t.tMSG & hwMask) 
                    ==  (hwMSG & hwMask)) {
                    ptCore->tMSGMap.chIndex = chIndex;
                    THIS_FSM_STATE = CHECK_USER_HANDLER_TYPE;
                    break;
                }
                chIndex++;
                ptItems++;
            } while(--chCount);

            if (ptCore->tMSGMap.chIndex == 255) {
                RESET_TGUI_USER_MSG_HANDLING_FSM();
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            }
            //break;
        }

        case CHECK_USER_HANDLER_TYPE:
            switch (ptItems->use_as__vsf_tgui_control_handler_t.u2Type) {
                case VSF_MSGT_NODE_HANDLER_TYPE_FSM:
                    THIS_FSM_STATE = CALL_FSM;
                    break;
                case VSF_MSGT_NODE_HANDLER_TYPE_EDA: {
                    vsf_err_t tErr = vsf_eda_post_msg(
                                        ptItems->use_as__vsf_tgui_control_handler_t.fn.ptEDA, 
                                        (void *)&(ptEvent->use_as__vsf_tgui_msg_t));
                    VSF_OSA_SERVICE_ASSERT(tErr == VSF_ERR_NONE);
                    UNUSED_PARAM(tErr);
                    RESET_TGUI_USER_MSG_HANDLING_FSM();
                    return fsm_rt_cpl;
                }

                case VSF_MSGT_NODE_HANDLER_TYPE_SUBCALL:
                default:
                    //! haven't support yet
                    RESET_TGUI_USER_MSG_HANDLING_FSM();
                    return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            }
            break;

        case CALL_FSM: {
            fsm_rt_t tfsm = ptItems[ptCore->tMSGMap.chIndex]
                                .use_as__vsf_tgui_control_handler_t
                                    .fn.FSM(ptControl, 
                                            (vsf_msgt_msg_t *)&(ptEvent->use_as__vsf_tgui_msg_t));
            if (    fsm_rt_cpl == tfsm 
                ||  VSF_TGUI_MSG_RT_REFRESH == tfsm 
                || VSF_TGUI_MSG_RT_REFRESH_PARENT == tfsm) {
                //! message has been handled
                RESET_TGUI_USER_MSG_HANDLING_FSM();
                return tfsm;
            } else if (tfsm < 0) {
                //! message is not handled
                RESET_TGUI_USER_MSG_HANDLING_FSM();
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            } else {
                return tfsm;
            }
            break;
        }
            
    } 

    return fsm_rt_on_going;
}

fsm_rt_t __vsf_tgui_control_msg_handler(vsf_tgui_control_t* ptControl,
                                        vsf_tgui_msg_t* ptMSG,
                                        const i_tgui_control_methods_t*ptMethods)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(ptControl);
    vsf_evt_t tMSG = ptMSG->use_as__vsf_msgt_msg_t.tMSG;
    fsm_rt_t tResult = (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;

    VSF_COMPONENT_ASSERT(NULL != ptControl && NULL != ptMSG);

    if (
#if VSF_TGUI_CFG_SHOW_REFRESH_LOG != ENABLED
        (tMSG != VSF_TGUI_EVT_REFRESH) && 
#endif
#if VSF_TGUI_CFG_SHOW_ON_TIME_LOG != ENABLED
        (tMSG != VSF_TGUI_EVT_ON_TIME) && 
#endif
    true) {
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                    VSF_TRACE_CFG_LINEEND 
                    "[Control Event]%s" VSF_TRACE_CFG_LINEEND "\t",
                    ptControl->use_as__vsf_msgt_node_t.pchNodeName);
#else
    VSF_TGUI_LOG(VSF_TRACE_DEBUG, "[Control Event]");
#endif
    }

    switch(tMSG & VSF_TGUI_MSG_MSK) {

        case VSF_TGUI_MSG_CONTROL_EVT & VSF_TGUI_MSG_MSK:
            switch(tMSG & VSF_TGUI_EVT_MSK) {
            
            #if VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME == ENABLED
                case VSF_TGUI_EVT_ON_LOAD & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_ON_LOAD");
                    ptMethods->Init(ptControl);
                    ptMethods->tView.Init(ptControl);
                    tResult = fsm_rt_cpl;
                    break;
            #endif

            #if VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME == ENABLED
                case VSF_TGUI_EVT_ON_DEPOSE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_ON_DEPOSE");
                    ptMethods->tView.Depose(ptControl);
                    tResult = fsm_rt_cpl;
                    break;
            #endif
            
                case VSF_TGUI_EVT_UPDATE_TREE & VSF_TGUI_EVT_MSK:
                case VSF_TGUI_EVT_UPDATE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_UPDATE");
                    ptMethods->Update(ptControl);
                    ptMethods->tView.Update(ptControl);
                    tResult = fsm_rt_cpl;
                    break;

            #if VSF_TGUI_CFG_SUPPORT_REFRESH_SCHEME == ENABLED
                case VSF_TGUI_EVT_REFRESH & VSF_TGUI_EVT_MSK: {
                    vsf_tgui_control_refresh_mode_t tMode = VSF_TGUI_CONTROL_REFRESHED_BY_PARENT;
                    
                #if VSF_TGUI_CFG_SHOW_REFRESH_LOG == ENABLED
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_REFRESH");
                #endif

                #if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
                    vsf_tgui_refresh_evt_t *ptEvent = (vsf_tgui_refresh_evt_t*)ptMSG;
                    vsf_tgui_region_t tRegion = { 0 };
                    if (!vsf_tgui_control_get_visible_region(ptControl, &tRegion)) {
                        break;
                    }
                    if (NULL != ptEvent->ptRegion) {
                        if (!vsf_tgui_region_intersect(&tRegion, &tRegion, ptEvent->ptRegion)) {
                            break;
                        }
                    }
                    vsf_tgui_control_get_relative_region(ptControl, &tRegion);
                #else
                #   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
                    vsf_tgui_region_t tRegion = { 0 };
                    tRegion.tSize = ptCore->tRegion.tSize;
                #   else
                    vsf_tgui_region_t tRegion = { .tSize = ptCore->tSize };
                #   endif
                #endif
                    if (ptCore->tStatus.tValues.__bIsTheFirstRefreshNode) {
                        ptCore->tStatus.tValues.__bIsTheFirstRefreshNode = false;
                        tMode = VSF_TGUI_CONTROL_REFRESHED_DIRECTLY_BY_USER;
                    }
                    tResult = ptMethods->tView.Render(ptControl, &tRegion, tMode);
                    break;
                }
            #endif
            
            #if VSF_TGUI_CFG_SHOW_ON_TIME_LOG == ENABLED
                case VSF_TGUI_EVT_ON_TIME & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_ON_TIME");
                    break;
            #endif
            
                case VSF_TGUI_EVT_GET_ACTIVE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_GET_ACTIVE");
                    tResult = (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
                    break;

                case VSF_TGUI_EVT_LOST_ACTIVE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_LOST_ACTIVE");
                    tResult = (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
                    break;
            }

            while(fsm_rt_on_going == __vk_tgui_control_user_message_handling(ptControl, (const vsf_tgui_evt_t *)ptMSG));
            break;

        //! message contains data
        case VSF_TGUI_MSG_POINTER_EVT & VSF_TGUI_MSG_MSK: {
            vsf_tgui_pointer_evt_t *ptEvent = (vsf_tgui_pointer_evt_t *)ptMSG;
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_POINTER_EVT\t" );

            switch(tMSG & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_POINTER_DOWN & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer Down" );

                    VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d",
                        ptEvent->use_as__vsf_tgui_location_t.iX,
                        ptEvent->use_as__vsf_tgui_location_t.iY);

                    break;
                case VSF_TGUI_EVT_POINTER_UP & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer Up" );

                    VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d",
                        ptEvent->use_as__vsf_tgui_location_t.iX,
                        ptEvent->use_as__vsf_tgui_location_t.iY);

                    break;
                case VSF_TGUI_EVT_POINTER_CLICK & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer Click");
                    break;

            #if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
            
                case VSF_TGUI_EVT_POINTER_ENTER & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer Enter");
                    break;

                case VSF_TGUI_EVT_POINTER_LEFT & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer Left");
                    break;
            #   if VSF_TGUI_CFG_SUPPORT_MOUSE_MOVE_HANDLING == ENABLED
                case VSF_TGUI_EVT_POINTER_MOVE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer Move" );

                    VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d",
                        ptEvent->use_as__vsf_tgui_location_t.iX,
                        ptEvent->use_as__vsf_tgui_location_t.iY);
                    break;
            #   endif
            #endif

                default:
                break;
            }

            do {
                tResult = __vk_tgui_control_user_message_handling(ptControl, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == tResult);

        #if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
            switch(tMSG & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_POINTER_ENTER & VSF_TGUI_EVT_MSK:
                case VSF_TGUI_EVT_POINTER_LEFT & VSF_TGUI_EVT_MSK:
                    if (tResult < 0) {
                        tResult = (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
                    }
                    break;
            }
        #endif

            break;
        }

        case VSF_TGUI_MSG_KEY_EVT & VSF_TGUI_MSG_MSK:{
            vsf_tgui_key_evt_t* ptEvent = (vsf_tgui_key_evt_t *)ptMSG;
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_KEY_EVT\t");
            switch (tMSG & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_KEY_DOWN & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Key Down   ");
                    break;
                case VSF_TGUI_EVT_KEY_UP & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Key Up     ");
                    break;
                case VSF_TGUI_EVT_KEY_PRESSED & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Key Pressed");
                    break;
                default:
                    break;
            }

            switch(ptEvent->hwKeyValue) {
                case VSF_TGUI_KEY_UP:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_UP]");
                    break;
                case VSF_TGUI_KEY_DOWN:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_DOWN]");
                    break;
                case VSF_TGUI_KEY_LEFT:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_LEFT]");
                    break;
                case VSF_TGUI_KEY_RIGHT:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_RIGHT]");
                    break;
                case VSF_TGUI_KEY_OK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_OK]");
                    break;
                case VSF_TGUI_KEY_CANCEL:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_CANCEL]");
                    break;
                default:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[0x%04x]", ptEvent->hwKeyValue);
                    break;
            }
            
            do {
                tResult = __vk_tgui_control_user_message_handling(ptControl, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == tResult);
            break;
        }
            
        case VSF_TGUI_MSG_GESTURE_EVT & VSF_TGUI_MSG_MSK: {
            vsf_tgui_gesture_evt_t* ptEvent = (vsf_tgui_gesture_evt_t *)ptMSG;
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_GESTURE_EVT\t");
            switch (tMSG & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_GESTURE_SLIDE:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\tSlide");
                    break;
            }

            VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d in %d ms",
                            ptEvent->tDelta.use_as__vsf_tgui_location_t.iX,
                            ptEvent->tDelta.use_as__vsf_tgui_location_t.iY,
                            ptEvent->tDelta.hwMillisecond
                        );
            
            do {
                tResult = __vk_tgui_control_user_message_handling(ptControl, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == tResult);
            break;
        }
        default:
            //return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            do {
                tResult = __vk_tgui_control_user_message_handling(ptControl, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == tResult);
            break;
    }

    return tResult;
}


fsm_rt_t vsf_tgui_control_msg_handler(  vsf_tgui_control_t* ptControl, 
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t*)ptControl, 
                                            ptMSG, 
                                            &c_tVControl);
}

fsm_rt_t vsf_tgui_container_msg_handler(vsf_tgui_container_t* ptControl,
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t*)ptControl,
                                            ptMSG,
                                            &c_tVControl);
}



/*----------------------------------------------------------------------------*
 *  Update Scheme                                                             *
 *----------------------------------------------------------------------------*/

static void __vsf_tgui_container_update_size(vsf_tgui_container_t *ptContainer, 
                                             vsf_tgui_size_t* ptSize)
{
    vsf_tgui_size_t *ptControlSize = NULL;
    
    ASSERT(NULL != ptContainer && NULL != ptSize);
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    do {
        vsf_tgui_margin_t tMargin = ptContainer->tConatinerPadding;
        ptSize->iHeight += tMargin.chTop + tMargin.chBottom;
        ptSize->iWidth += tMargin.chLeft + tMargin.chRight;
    } while(0);
#endif
    ptControlSize = 
        &(ptContainer->use_as____vsf_tgui_control_core_t.tRegion.tSize);

    switch(ptContainer->tContainerAttribute.u5Type) {
        case VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL:
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
            ptControlSize->iHeight = max(ptSize->iHeight, ptControlSize->iHeight);
            ptControlSize->iWidth = ptSize->iWidth;
            break;
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
        case VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL:
            ptControlSize->iHeight = ptSize->iHeight;
            ptControlSize->iWidth = max(ptSize->iWidth, ptControlSize->iWidth);
            break;

        default:
        case VSF_TGUI_CONTAINER_TYPE_PLANE:
            *ptControlSize = *ptSize;
            break;
    }
}

const vsf_tgui_control_t* __vk_tgui_control_get_next_visible_one_within_container(
                                            const vsf_tgui_control_t* ptItem)
{
    if (NULL == ptItem) {
        return NULL;
    }
    do {
        ptItem = (vsf_tgui_control_t*)
            vsf_msgt_get_next_node_within_container(&(ptItem->use_as__vsf_msgt_node_t));
        if (NULL == ptItem) {
            break;
        }
        
        if (vsf_tgui_control_status_get(ptItem).tValues.bIsVisible) {
            break;
        }
    } while (true);

    return ptItem;
}


static vsf_tgui_size_t* __vsf_tgui_plane_container_update(   
                                                vsf_tgui_container_t* ptContainer,
                                                vsf_tgui_size_t* ptSize)
{
    vsf_tgui_size_t tSize = { 0 };
    vsf_tgui_control_t* ptItem =
        (vsf_tgui_control_t*)ptContainer->use_as__vsf_msgt_container_t.ptNode;
    ptContainer->chVisibleItemCount = 0;

    while (NULL != ptItem) {
        __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core (ptItem);
        VSF_TGUI_ASSERT(ptContainer->chVisibleItemCount < 255);
        ptContainer->chVisibleItemCount++;

        int16_t iWidth = ptCore->tRegion.tSize.iWidth + ptCore->tRegion.tLocation.iX;
        int16_t iHeight = ptCore->tRegion.tSize.iHeight + ptCore->tRegion.tLocation.iY;
        tSize.iWidth =  max(tSize.iWidth, iWidth);
        tSize.iHeight = max(tSize.iHeight, iHeight);
        ptItem = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(ptItem);
    }

    *ptSize = tSize;

    return ptSize;
}

static vsf_tgui_size_t* __vsf_tgui_line_stream_update_vertical( vsf_tgui_container_t* ptContainer,
                                                                vsf_tgui_size_t* ptSize)
{
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    //vsf_tgui_margin_t tMargin = ptContainer->tConatinerPadding;
#endif
    vsf_tgui_location_t tLocation = { 0 };
    vsf_tgui_size_t tSize = {0};
    int16_t iHeight = 0, iWidth = 0;
    vsf_tgui_control_t *ptItem = 
        (vsf_tgui_control_t *)ptContainer->use_as__vsf_msgt_container_t.ptNode;
    ptContainer->chVisibleItemCount = 0;

    while(NULL != ptItem) {
        __vsf_tgui_control_core_t *ptCore = vsf_tgui_control_get_core(ptItem);
        VSF_TGUI_ASSERT(ptContainer->chVisibleItemCount < 255);
        ptContainer->chVisibleItemCount++;

        ptCore->tRegion.tLocation = tLocation;
        iWidth = ptCore->tRegion.tSize.iWidth;
        iHeight = ptCore->tRegion.tSize.iHeight;

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
        ptCore->tRegion.tLocation.iX += ptCore->tMargin.chLeft;
        ptCore->tRegion.tLocation.iY += ptCore->tMargin.chTop;
        iWidth += ptCore->tRegion.tLocation.iX + ptCore->tMargin.chRight;
        iHeight += ptCore->tMargin.chTop + ptCore->tMargin.chBottom;
#endif

        tLocation.iY += iHeight;
        tSize.iHeight += iHeight;
        tSize.iWidth = max(tSize.iWidth, iWidth);
        ptItem = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(ptItem);
    }
    *ptSize = tSize;
    
    return ptSize;
}

static vsf_tgui_size_t* __vsf_tgui_line_stream_update_horizontal( vsf_tgui_container_t* ptContainer,
                                                                    vsf_tgui_size_t* ptSize)
{
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    //vsf_tgui_margin_t tMargin = ptContainer->tConatinerPadding;
#endif
    vsf_tgui_location_t tLocation = { 0 };
    vsf_tgui_size_t tSize = { 0 };
    int16_t iHeight = 0, iWidth = 0;
    vsf_tgui_control_t* ptItem =
        (vsf_tgui_control_t*)ptContainer->use_as__vsf_msgt_container_t.ptNode;
    ptContainer->chVisibleItemCount = 0;

    while (NULL != ptItem) {
        __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(ptItem);
        VSF_TGUI_ASSERT(ptContainer->chVisibleItemCount < 255);
        ptContainer->chVisibleItemCount++;

        ptCore->tRegion.tLocation = tLocation;
        iWidth = ptCore->tRegion.tSize.iWidth;
        iHeight = ptCore->tRegion.tSize.iHeight;

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
        ptCore->tRegion.tLocation.iX += ptCore->tMargin.chLeft;
        ptCore->tRegion.tLocation.iY += ptCore->tMargin.chTop;
        iWidth += ptCore->tMargin.chLeft + ptCore->tMargin.chRight;
        iHeight += ptCore->tRegion.tLocation.iY + ptCore->tMargin.chBottom;
#endif

        tLocation.iX += iWidth;
        tSize.iWidth += iWidth;
        tSize.iHeight = max(tSize.iHeight, iHeight);
        ptItem = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(ptItem);
    }
    *ptSize = tSize;

    return ptSize;
}

static vsf_tgui_size_t* __vsf_tgui_stream_update_vertical(  vsf_tgui_container_t* ptContainer,
                                                            vsf_tgui_size_t* ptSize)
{
    //! todo
    return ptSize;
}

static vsf_tgui_size_t* __vsf_tgui_stream_update_horizontal(vsf_tgui_container_t* ptContainer,
                                                            vsf_tgui_size_t* ptSize)
{
    //! todo
    return ptSize;
}

static fsm_rt_t __vsf_tgui_container_update(vsf_tgui_container_t* ptContainer)
{
    
    vsf_tgui_size_t tSize;
    ASSERT(NULL != ptContainer);

    switch(ptContainer->tContainerAttribute.u5Type) {
        case VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL:
            /* stream container, with horizontal orientation*/
            /* horizontal orientation: the width is fixed, there could be multiple rows*/
            __vsf_tgui_stream_update_horizontal(ptContainer, &tSize);
            break;

        case VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL:
            /* stream container, with orientation either vertical or horizontal */
            /* vertical orientation: the height is fixed, there could be multiple columns*/
            __vsf_tgui_stream_update_vertical(ptContainer, &tSize);
            break;

        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
            /* line stream container, with orientation either vertical or horizontal */
            /* horizontal orientation: only one row*/
            __vsf_tgui_line_stream_update_horizontal(ptContainer, &tSize);
            break;

        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
            /* line stream container, with orientation either vertical or horizontal */
            /* horizontal orientation: only one row*/
            __vsf_tgui_line_stream_update_vertical(ptContainer, &tSize);
            break;

        default:
            /* unknown stream type is treated as plan stream*/
        case VSF_TGUI_CONTAINER_TYPE_PLANE:
            /* controls are placed on a plane with their relative locations */
            if (ptContainer->tContainerAttribute.bIsAutoSize) {
                __vsf_tgui_plane_container_update(ptContainer, &tSize);
            }
            break;
    }

    if (ptContainer->tContainerAttribute.bIsAutoSize) {
        __vsf_tgui_container_update_size(ptContainer, &tSize);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_control_update(vsf_tgui_control_t *ptControl)
{
    ASSERT(NULL != ptControl);
    if (vsf_tgui_control_is_container((const vsf_tgui_control_t *)ptControl)) {
        return __vsf_tgui_container_update((vsf_tgui_container_t *)ptControl);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_container_update(vsf_tgui_container_t* ptContainer)
{
    return vk_tgui_control_update((vsf_tgui_control_t *) ptContainer);
}

fsm_rt_t vk_tgui_control_init(vsf_tgui_control_t* ptControl)
{
    vsf_tgui_status_t tStatus = vsf_tgui_control_status_get((vsf_tgui_control_t*)ptControl);
    tStatus.chStatus |= VSF_TGUI_CTRL_STATUS_INITIALISED;

    vsf_tgui_control_status_set((vsf_tgui_control_t*)ptControl, tStatus);

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_container_init(vsf_tgui_container_t *ptContainer)
{
    return vk_tgui_control_init((vsf_tgui_control_t*)ptContainer);
}

#endif


/* EOF */
