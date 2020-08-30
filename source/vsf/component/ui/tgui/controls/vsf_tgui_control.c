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
        NULL,
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

static const i_tgui_control_methods_t c_tVContainer= {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    {
        (vsf_tgui_method_t *)&vsf_tgui_container_v_init,
        (vsf_tgui_method_t *)&vsf_tgui_container_v_depose,
        (vsf_tgui_v_method_render_t *)&vsf_tgui_container_v_rendering,
        (vsf_tgui_v_method_render_t *)&vsf_tgui_container_v_post_rendering,
        (vsf_tgui_method_t *)&vsf_tgui_container_v_update
    },
    (vsf_tgui_method_t *)&vk_tgui_container_init,
    (vsf_tgui_method_t *)&vk_tgui_container_update,
#else
    .tView = {
        .Init =     (vsf_tgui_method_t *)&vsf_tgui_container_v_init,
        .Depose =   (vsf_tgui_method_t *)&vsf_tgui_container_v_depose,
        .Render =   (vsf_tgui_v_method_render_t *)&vsf_tgui_container_v_rendering,
        .ContainerPostRender = (vsf_tgui_v_method_render_t *)&vsf_tgui_container_v_post_rendering,
        .Update =   (vsf_tgui_method_t *)&vsf_tgui_container_v_update,
    },
    .Update =   (vsf_tgui_method_t *)&vk_tgui_container_update,
    .Init =     (vsf_tgui_method_t *)&vk_tgui_container_init,
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
        = vk_tgui_control_get_top(ptTimer->target_ptr);

    if (NULL == ptTopContainer) {
        return ;
    }

    vk_tgui_send_timer_event(ptTopContainer->gui_ptr, ptTimer->target_ptr);

    tProtectStatus = vsf_protect_sched();
        if (ptTimer->bEnabled && ptTimer->bIsRepeat) {
            ptTimer->u29Interval = max(100, ptTimer->u29Interval);
            vsf_callback_timer_add_ms(  &ptTimer->use_as__vsf_callback_timer_t,
                                        ptTimer->u29Interval);
        } else {
            ptTimer->Status.bIsWorking = false;
        }
    vsf_unprotect_sched(tProtectStatus);
}

void vsf_tgui_timer_init(   vsf_tgui_timer_t *ptTimer,
                            const vsf_tgui_control_t *control_ptr)
{
    VSF_COMPONENT_ASSERT(NULL != ptTimer);
    VSF_COMPONENT_ASSERT(NULL != control_ptr);

    ptTimer->target_ptr = control_ptr;
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
            if (ptTimer->Status.bIsWorking) {
                break;
            }
            if (ptTimer->bIsRepeat) {
                ptTimer->u29Interval = max(100, ptTimer->u29Interval);
            }
            ptTimer->Status.bIsWorking = true;
            ptTimer->Status.bEnabled = true;
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
            if (!ptTimer->Status.bIsWorking) {
                break;
            }
            vsf_callback_timer_remove(&(ptTimer->use_as__vsf_callback_timer_t));
            ptTimer->Status.bIsWorking = false;
        } while(0);
        ptTimer->Status.bEnabled = false;
    vsf_unprotect_sched(tProtectStatus);
}

bool vsf_tgui_timer_is_working(vsf_tgui_timer_t *ptTimer)
{
    VSF_COMPONENT_ASSERT(NULL != ptTimer);
    return ptTimer->Status.bIsWorking;
}

#endif

/*----------------------------------------------------------------------------*
 *  Region                                                                    *
 *----------------------------------------------------------------------------*/

/*! \brief get the absolute location information base on the location information
 *!        of or derived from target control.
 *! \param control_ptr    the Target Control Address
 *! \param ptLocation   the Location buffer which has already stored the location
 *!                     information of or derived from the target control
 *! \return the location buffer address passed with ptLocation
 */
vsf_tgui_location_t* __vk_tgui_calculate_absolute_location_from_control_location(
                                                const vsf_tgui_control_t *control_ptr,
                                                vsf_tgui_location_t *ptLocation)
{
    const vsf_msgt_node_t *node_ptr = &control_ptr->use_as__vsf_msgt_node_t;
    const vsf_msgt_container_t* parent_ptr = NULL;
    VSF_COMPONENT_ASSERT(NULL != control_ptr && NULL != ptLocation);

    do {
        parent_ptr = (const vsf_msgt_container_t *)node_ptr->parent_ptr;
        if (NULL == parent_ptr) {
            break;
        }
        node_ptr = (const vsf_msgt_node_t*)parent_ptr;
        if (node_ptr->Attribute._.is_container) {
            vsf_tgui_container_t *container_ptr = (vsf_tgui_container_t*)node_ptr;
            ptLocation->iX += container_ptr->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iX;
            ptLocation->iY += container_ptr->use_as____vsf_tgui_control_core_t.tRegion.tLocation.iY;
        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
            ptLocation->iX += container_ptr->tConatinerPadding.chLeft;
            ptLocation->iY += container_ptr->tConatinerPadding.chTop;
        #endif
        }
    } while(1);

    return ptLocation;
}

vsf_tgui_location_t *vsf_tgui_control_get_location(const vsf_tgui_control_t* control_ptr)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(control_ptr);
    return &(ptCore->tRegion.tLocation);
}

vsf_tgui_size_t *vsf_tgui_control_get_size(const vsf_tgui_control_t* control_ptr)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(control_ptr);
    return &(ptCore->tRegion.tSize);
}

vsf_tgui_location_t * vsf_tgui_control_get_absolute_location(
                                            const vsf_tgui_control_t* control_ptr,
                                            vsf_tgui_location_t* ptOffset)
{
    //const vsf_msgt_node_t* node_ptr;
    __vsf_tgui_control_core_t* ptCore;
    VSF_TGUI_ASSERT(NULL != control_ptr);
    VSF_TGUI_ASSERT(NULL != ptOffset);

    ptCore = vsf_tgui_control_get_core(control_ptr);
    ptOffset->iX += ptCore->tRegion.tLocation.iX;
    ptOffset->iY += ptCore->tRegion.tLocation.iY;

    return __vk_tgui_calculate_absolute_location_from_control_location(control_ptr, ptOffset);
}

vsf_tgui_region_t* vsf_tgui_control_get_absolute_region(
                                                const vsf_tgui_control_t *control_ptr,
                                                vsf_tgui_region_t *region_ptr)
{
    return (vsf_tgui_region_t *)
                vsf_tgui_control_get_absolute_location(
                                                control_ptr,
                                                (vsf_tgui_location_t *)region_ptr);
}

vsf_tgui_region_t * vsf_tgui_get_absolute_control_region(
                                                const vsf_tgui_control_t* control_ptr,
                                                vsf_tgui_region_t* ptRegionBuffer)
{
    const __vsf_tgui_control_core_t * ptCore;

    VSF_COMPONENT_ASSERT(NULL != control_ptr && NULL != ptRegionBuffer);

    ptCore = vsf_tgui_control_get_core(control_ptr);
    VSF_COMPONENT_ASSERT(ptCore != NULL);

    *ptRegionBuffer = ptCore->tRegion;

    __vk_tgui_calculate_absolute_location_from_control_location(control_ptr, &(ptRegionBuffer->tLocation));
    return ptRegionBuffer;
}

bool vsf_tgui_control_is_in_range(      const vsf_tgui_region_t *region_ptr,
                                        const vsf_tgui_location_t *ptLocation)
{
    bool result = false;
    VSF_COMPONENT_ASSERT(NULL != region_ptr && NULL != ptLocation);

    do {

        if (    (ptLocation->iX < region_ptr->tLocation.iX)
            ||  (ptLocation->iY < region_ptr->tLocation.iY)) {
            break;
        }

        {/*! to mitigate C89/90 warning */
            int_fast16_t iX = region_ptr->tLocation.iX + region_ptr->tSize.iWidth;
            int_fast16_t iY = region_ptr->tLocation.iY + region_ptr->tSize.iHeight;
            if (    (ptLocation->iX >= iX)
                ||  (ptLocation->iY >= iY)) {
                break;
            }
        }

        result = true;
    } while(0);

    return result;
}

/*! \brief get the visible region (with absolute location)
 *! \param control_ptr    the target control
 *! \param region_ptr     the region buffer
 *! \retval true        visible
 *! \retval false       invisible
 */
bool vsf_tgui_control_get_visible_region(   const vsf_tgui_control_t* control_ptr,
                                            vsf_tgui_region_t * ptRegionBuffer)
{
    bool bHasIntersect = true;

    vsf_tgui_get_absolute_control_region(control_ptr, ptRegionBuffer);

#if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
    do {
        const vsf_tgui_container_t* parent_ptr =
            (const vsf_tgui_container_t*)control_ptr->use_as__vsf_msgt_node_t.parent_ptr;
        vsf_tgui_region_t tParentRegion;

        while (NULL != parent_ptr) {
            tParentRegion = parent_ptr->use_as____vsf_tgui_control_core_t.tRegion;
            __vk_tgui_calculate_absolute_location_from_control_location((vsf_tgui_control_t*)parent_ptr, &(tParentRegion.tLocation));
            bHasIntersect = vsf_tgui_region_intersect(ptRegionBuffer, ptRegionBuffer, &tParentRegion);
            if (!bHasIntersect) {
                break;
            }
            parent_ptr = (const vsf_tgui_container_t *)parent_ptr->use_as__vsf_msgt_container_t.use_as__vsf_msgt_node_t.parent_ptr;
        }
    } while (0);
#endif
    return bHasIntersect;
}


vsf_tgui_region_t* vsf_tgui_control_get_relative_region(
                                                const vsf_tgui_control_t* control_ptr,
                                                vsf_tgui_region_t* ptAbsoluteRegion)
{
    vsf_tgui_region_t tRegion;
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(control_ptr);
    tRegion = ptCore->tRegion;

    VSF_COMPONENT_ASSERT(NULL != control_ptr && NULL != ptAbsoluteRegion);

    __vk_tgui_calculate_absolute_location_from_control_location(control_ptr, &(tRegion.tLocation));

    vsf_tgui_region_get_relative_region(ptAbsoluteRegion, &tRegion, ptAbsoluteRegion);

    return ptAbsoluteRegion;
}


vsf_tgui_region_t * vsf_tgui_control_generate_dirty_region_from_parent_dirty_region(
                                    const vsf_tgui_control_t *parent_ptr,
                                    const vsf_tgui_region_t *ptParentDirtyRegion,
                                    const vsf_tgui_control_t *ptPrivate,
                                    vsf_tgui_region_t *ptNewDirtyRegionBuffer)
{
    VSF_TGUI_ASSERT(NULL != parent_ptr && NULL != ptPrivate && NULL != ptNewDirtyRegionBuffer);
    vsf_tgui_region_t tParentDirtyRegion;
    if (NULL != ptParentDirtyRegion) {
        tParentDirtyRegion = *ptParentDirtyRegion;
    } else {
        tParentDirtyRegion = vsf_tgui_control_get_core(parent_ptr)->tRegion;
    }

    vsf_tgui_control_get_absolute_region(parent_ptr, &tParentDirtyRegion);
    vsf_tgui_get_absolute_control_region(ptPrivate, ptNewDirtyRegionBuffer);
    if (!vsf_tgui_region_intersect(  ptNewDirtyRegionBuffer,
                                ptNewDirtyRegionBuffer,
                               &tParentDirtyRegion)) {
        return NULL;
    }
    vsf_tgui_control_get_relative_region(ptPrivate, ptNewDirtyRegionBuffer);

    return ptNewDirtyRegionBuffer;
}


bool vsf_tgui_control_shoot(    const vsf_tgui_control_t* control_ptr,
                                const vsf_tgui_location_t *ptLocation)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(control_ptr);
    vsf_tgui_region_t tRegion;
    VSF_COMPONENT_ASSERT(NULL != control_ptr && NULL != ptLocation);
    uint_fast8_t chMask =   (   VSF_TGUI_CTRL_STATUS_INITIALISED
                            |   VSF_TGUI_CTRL_STATUS_VISIBLE);

    if (chMask != (ptCore->Status.chStatus & chMask)) {
        return false;
    }

    if (!vsf_tgui_control_get_visible_region(control_ptr, &tRegion)) {
        return false;
    }

    return vsf_tgui_control_is_in_range( &tRegion, ptLocation);
}


/*----------------------------------------------------------------------------*
 *  Status and Attributes                                                     *
 *----------------------------------------------------------------------------*/

 /*! \brief update is_control_transparent bit in control status
  *! \parame control_ptr target control address
  *! \retval true the original value of is_control_transparent is changed
  *! \retval false the set value is the same as the original value, no change is
  *!               made.
  */
bool vsf_tgui_control_set_is_transparent_bit(vsf_tgui_control_t* control_ptr,
                                             bool is_control_transparent)
{
    bool result = false;
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(control_ptr);
    ASSERT(NULL != control_ptr);

    result =       is_control_transparent
                !=  (ptCore->Status.Values.is_control_transparent);

    if (result) {
        ptCore->Status.Values.is_control_transparent = is_control_transparent;
    }

    return result;
}

vsf_tgui_status_t vsf_tgui_control_status_get(const vsf_tgui_control_t* control_ptr)
{
    __vsf_tgui_control_core_t *ptCore = vsf_tgui_control_get_core(control_ptr);
    VSF_COMPONENT_ASSERT(NULL != control_ptr);

    return ptCore->Status;
}

void vsf_tgui_control_status_set(vsf_tgui_control_t* control_ptr, vsf_tgui_status_t Status)
{
    __vsf_tgui_control_core_t *ptCore = vsf_tgui_control_get_core(control_ptr);
    VSF_COMPONENT_ASSERT(NULL != control_ptr);

    ptCore->Status = Status;
}

bool vsf_tgui_control_is_container(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT(NULL != control_ptr);
    return control_ptr->use_as__vsf_msgt_node_t.Attribute._.is_container;
}

__vsf_tgui_control_core_t* vsf_tgui_control_get_core(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT(NULL != control_ptr);
    if (vsf_tgui_control_is_container(control_ptr)) {
        const vsf_msgt_node_t* node_ptr = &control_ptr->use_as__vsf_msgt_node_t;
        const vsf_tgui_container_t* container_ptr = (const vsf_tgui_container_t*)node_ptr;
        return (__vsf_tgui_control_core_t *)&container_ptr->use_as____vsf_tgui_control_core_t;
    }

    return (__vsf_tgui_control_core_t*)&control_ptr->use_as____vsf_tgui_control_core_t;
}

vsf_tgui_control_t* vsf_tgui_control_get_parent(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT(NULL != control_ptr);
    return (vsf_tgui_control_t *)control_ptr->use_as__vsf_msgt_node_t.parent_ptr;
}


uint_fast8_t vk_tgui_container_visible_item_get(const vsf_tgui_container_t *container_ptr)
{
    VSF_TGUI_ASSERT(NULL != container_ptr);
    return container_ptr->chVisibleItemCount;
}

/*----------------------------------------------------------------------------*
 *  Methods and Others                                                        *
 *----------------------------------------------------------------------------*/

const vsf_tgui_top_container_t *
vk_tgui_control_get_top(const vsf_tgui_control_t* control_ptr)
{
    //const vsf_tgui_top_container_t* ptTopContainer = NULL;
    VSF_COMPONENT_ASSERT(NULL != control_ptr);

    while (NULL != control_ptr->use_as__vsf_msgt_node_t.parent_ptr) {
        control_ptr = (const vsf_tgui_control_t*)
            control_ptr->use_as__vsf_msgt_node_t.parent_ptr;
    }
    VSF_COMPONENT_ASSERT(control_ptr->use_as__vsf_msgt_node_t.Attribute._.is_top);

    return (const vsf_tgui_top_container_t*)control_ptr;
}

bool vsf_tgui_control_send_message( const vsf_tgui_control_t* control_ptr, 
                                    vsf_tgui_evt_t event)
{
    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);

    if (NULL == ptTopContainer) {
        return false;
    }

    if (NULL == event.use_as__vsf_tgui_msg_t.target_ptr) {
        event.use_as__vsf_tgui_msg_t.target_ptr = (vsf_tgui_control_t *)control_ptr;
    }

    return vk_tgui_send_message(ptTopContainer->gui_ptr, event);
}

bool vsf_tgui_control_update(const vsf_tgui_control_t* control_ptr)
{
    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_update(ptTopContainer->gui_ptr, control_ptr);
}

bool vsf_tgui_control_update_tree(const vsf_tgui_control_t* control_ptr)
{
    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_update_tree(ptTopContainer->gui_ptr, control_ptr);
}


#if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
bool vsf_tgui_control_refresh(  const vsf_tgui_control_t *control_ptr,
                                const vsf_tgui_region_t *region_ptr)
{
    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_refresh_ex(ptTopContainer->gui_ptr, control_ptr, region_ptr);
}
#endif

bool vsf_tgui_control_set_active(const vsf_tgui_control_t* control_ptr)
{
    const vsf_tgui_top_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);
    vsf_tgui_evt_t temp_event = {
            .msg = VSF_TGUI_EVT_GET_ACTIVE,
            .target_ptr = (vsf_tgui_control_t*)control_ptr,
        };

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_send_message(ptTopContainer->gui_ptr, temp_event);
}


#undef THIS_FSM_STATE
#define THIS_FSM_STATE  ptCore->tMSGMap.chState

#define RESET_TGUI_USER_MSG_HANDLING_FSM()                          \
        do { THIS_FSM_STATE = 0; } while(0)

fsm_rt_t __vk_tgui_control_user_message_handling(   vsf_tgui_control_t* control_ptr,
                                                    const vsf_tgui_evt_t* event_ptr)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(control_ptr);
    uint_fast8_t chCount = ptCore->tMSGMap.chCount;
    const vsf_tgui_user_evt_handler *ptItems = ptCore->tMSGMap.ptItems;
    uint_fast16_t hwMSG = event_ptr->use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg;


    VSF_COMPONENT_ASSERT(NULL != control_ptr && NULL != event_ptr);

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
                uint_fast16_t hwMask = ptItems->use_as__vsf_tgui_control_handler_t.u10EvtMask;

                //! check message
                if (    (ptItems->use_as__vsf_msgt_msg_t.msg & hwMask)
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
                                        (void *)&(event_ptr->use_as__vsf_tgui_msg_t));
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
            fsm_rt_t fsm_rt = ptItems[ptCore->tMSGMap.chIndex]
                                .use_as__vsf_tgui_control_handler_t
                                    .fn.FSM(control_ptr,
                                            (vsf_tgui_msg_t *)&(event_ptr->use_as__vsf_tgui_msg_t));
            if (    fsm_rt_cpl == fsm_rt
                ||  VSF_TGUI_MSG_RT_REFRESH == fsm_rt
                || VSF_TGUI_MSG_RT_REFRESH_PARENT == fsm_rt) {
                //! message has been handled
                RESET_TGUI_USER_MSG_HANDLING_FSM();
                return fsm_rt;
            } else if (fsm_rt < 0) {
                //! message is not handled
                RESET_TGUI_USER_MSG_HANDLING_FSM();
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            } else {
                return fsm_rt;
            }
            break;
        }

    }

    return fsm_rt_on_going;
}

fsm_rt_t __vsf_tgui_control_msg_handler(vsf_tgui_control_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG,
                                        const i_tgui_control_methods_t*ptMethods)
{
    __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(control_ptr);
    vsf_evt_t msg = ptMSG->use_as__vsf_msgt_msg_t.msg;
    fsm_rt_t result = (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;

    VSF_COMPONENT_ASSERT(NULL != control_ptr && NULL != ptMSG);

    if (
        (msg != VSF_TGUI_EVT_REFRESH) &&
#if VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG != ENABLED
        (msg != VSF_TGUI_EVT_ON_TIME) &&
#endif
#if VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG != ENABLED
        ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_POINTER_EVT & VSF_TGUI_MSG_MSK)) &&
#endif
#if VSF_TGUI_CFG_SHOW_KEY_EVT_LOG != ENABLED
        ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_KEY_EVT & VSF_TGUI_MSG_MSK)) &&
#endif
#if VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG != ENABLED
        ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_GESTURE_EVT & VSF_TGUI_MSG_MSK)) &&
#endif
#if VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG != ENABLED
        ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT & VSF_TGUI_MSG_MSK)) &&
#endif
    true) {
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                    VSF_TRACE_CFG_LINEEND
                    "[Control Event]%s" VSF_TRACE_CFG_LINEEND "\t",
                    control_ptr->use_as__vsf_msgt_node_t.node_name_ptr);
#else
    VSF_TGUI_LOG(   VSF_TRACE_WARNING, 
                    VSF_TRACE_CFG_LINEEND
                    "[Control Event]");
#endif
    }

    switch(msg & VSF_TGUI_MSG_MSK) {

        case VSF_TGUI_MSG_CONTROL_EVT & VSF_TGUI_MSG_MSK:
            switch(msg & VSF_TGUI_EVT_MSK) {

            #if VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME == ENABLED
                case VSF_TGUI_EVT_ON_LOAD & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_ON_LOAD");
                    ptMethods->Init(control_ptr);
                    ptMethods->tView.Init(control_ptr);
                    result = fsm_rt_cpl;
                    break;
            #endif

            #if VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME == ENABLED
                case VSF_TGUI_EVT_ON_DEPOSE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_ON_DEPOSE");
                    ptMethods->tView.Depose(control_ptr);
                    result = fsm_rt_cpl;
                    break;
            #endif

                case VSF_TGUI_EVT_UPDATE_TREE & VSF_TGUI_EVT_MSK:
                case VSF_TGUI_EVT_UPDATE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_UPDATE");
                    ptMethods->Update(control_ptr);
                    ptMethods->tView.Update(control_ptr);
                    result = fsm_rt_cpl;
                    break;

            #if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
                case VSF_TGUI_EVT_REFRESH & VSF_TGUI_EVT_MSK: {
                    
                    vsf_tgui_control_refresh_mode_t tMode = VSF_TGUI_CONTROL_REFRESHED_BY_PARENT;
                    vsf_tgui_refresh_evt_t *event_ptr = (vsf_tgui_refresh_evt_t*)ptMSG;
                    vsf_tgui_region_t *temp_ptr = event_ptr->region_ptr;
                    
                    
                #if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
                    vsf_tgui_region_t tRegion = { 0 };
                    if (!vsf_tgui_control_get_visible_region(control_ptr, &tRegion)) {
                        return fsm_rt_cpl;
                    }
                    if (NULL != event_ptr->region_ptr) {
                        if (!vsf_tgui_region_intersect(&tRegion, &tRegion, event_ptr->region_ptr)) {
                            return fsm_rt_cpl;
                        }
                    }
                    vsf_tgui_control_get_relative_region(control_ptr, &tRegion);
                #else
                #   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
                    vsf_tgui_region_t tRegion = { 0 };
                    tRegion.tSize = ptCore->tRegion.tSize;
                #   else
                    vsf_tgui_region_t tRegion = { .tSize = ptCore->tSize };
                #   endif
                #endif
                    event_ptr->region_ptr = &tRegion;
                    
                    if (ptCore->Status.Values.__is_the_first_node_for_refresh) {
                        ptCore->Status.Values.__is_the_first_node_for_refresh = false;
                        tMode = VSF_TGUI_CONTROL_REFRESHED_DIRECTLY_BY_USER;
                    }
                
                    if (control_ptr->use_as__vsf_msgt_node_t.Attribute._.is_visited) {
                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                                        VSF_TRACE_CFG_LINEEND
                                        "[Control Event]%s" VSF_TRACE_CFG_LINEEND "\t",
                                        control_ptr->use_as__vsf_msgt_node_t.node_name_ptr);
                        VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_POST_REFRESH");
                     #endif

                        ptMSG->use_as__vsf_msgt_msg_t.msg = VSF_TGUI_EVT_POST_REFRESH;
                        result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG);

                        if (    (VSF_MSGT_ERR_MSG_NOT_HANDLED == result) 
                            &&  (NULL != ptMethods->tView.ContainerPostRender)) {
                            result = ptMethods->tView.ContainerPostRender(control_ptr, &tRegion, tMode);
                        }
                        ptMSG->use_as__vsf_msgt_msg_t.msg = VSF_TGUI_EVT_REFRESH;
                    } else {

                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                                        VSF_TRACE_CFG_LINEEND
                                        "[Control Event]%s" VSF_TRACE_CFG_LINEEND "\t",
                                        control_ptr->use_as__vsf_msgt_node_t.node_name_ptr);
                        VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_REFRESH");
                     #endif

                        result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG);
                        if (VSF_MSGT_ERR_MSG_NOT_HANDLED == result) {
                            result = ptMethods->tView.Render(control_ptr, &tRegion, tMode);
                        }   
                    }
                    
                    event_ptr->region_ptr = temp_ptr;

                    return result;
                }
            #endif

            #if VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG == ENABLED
                case VSF_TGUI_EVT_ON_TIME & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_ON_TIME");
                    break;
            #endif

                case VSF_TGUI_EVT_GET_ACTIVE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_GET_ACTIVE");
                    result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG);
                    if (result < 0) {
            #if VSF_TGUI_CFG_REFRESH_CONTROL_ON_ACTIVE_STATE_CHANGE == ENABLED
                        result = (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
            #else   
                        result = fsm_rt_cpl;
            #endif
                    }
                    return result;

                case VSF_TGUI_EVT_LOST_ACTIVE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_LOST_ACTIVE");
                    result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG);
                    if (result < 0) {
            #if VSF_TGUI_CFG_REFRESH_CONTROL_ON_ACTIVE_STATE_CHANGE == ENABLED
                        result = (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
            #else   
                        result = fsm_rt_cpl;
            #endif
                    }
                    return result;
            }

            while(fsm_rt_on_going == __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG));
            break;

        //! message contains data
        case VSF_TGUI_MSG_POINTER_EVT & VSF_TGUI_MSG_MSK: {
            vsf_tgui_pointer_evt_t *event_ptr = (vsf_tgui_pointer_evt_t *)ptMSG;
            UNUSED_PARAM(event_ptr);

        #if VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_POINTER_EVT\t" );

            switch(msg & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_POINTER_DOWN & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer Down" );

                    VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d",
                        event_ptr->use_as__vsf_tgui_location_t.iX,
                        event_ptr->use_as__vsf_tgui_location_t.iY);

                    break;
                case VSF_TGUI_EVT_POINTER_UP & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer Up" );

                    VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d",
                        event_ptr->use_as__vsf_tgui_location_t.iX,
                        event_ptr->use_as__vsf_tgui_location_t.iY);

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
                        event_ptr->use_as__vsf_tgui_location_t.iX,
                        event_ptr->use_as__vsf_tgui_location_t.iY);
                    break;
            #   endif
            #endif

                default:
                break;
            }
        #endif

            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);

        #if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
            switch(msg & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_POINTER_ENTER & VSF_TGUI_EVT_MSK:
                case VSF_TGUI_EVT_POINTER_LEFT & VSF_TGUI_EVT_MSK:
                    if (result < 0) {
                        result = (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
                    }
                    break;
            }
        #endif

            break;
        }

        case VSF_TGUI_MSG_KEY_EVT & VSF_TGUI_MSG_MSK:{
            vsf_tgui_key_evt_t* event_ptr = (vsf_tgui_key_evt_t *)ptMSG;
            UNUSED_PARAM(event_ptr);

        #if VSF_TGUI_CFG_SHOW_KEY_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_KEY_EVT\t");
            switch (msg & VSF_TGUI_EVT_MSK) {
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

            switch(event_ptr->hwKeyValue) {
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
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[0x%04x]", event_ptr->hwKeyValue);
                    break;
            }
        #endif
            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);
            break;
        }

        case VSF_TGUI_MSG_GESTURE_EVT & VSF_TGUI_MSG_MSK: {
            vsf_tgui_gesture_evt_t* event_ptr = (vsf_tgui_gesture_evt_t *)ptMSG;
            UNUSED_PARAM(event_ptr);

        #if VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_GESTURE_EVT\t");
            switch (msg & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_GESTURE_SLIDE:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\tSlide");
                    break;
            }

            VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d in %d ms",
                            event_ptr->tDelta.use_as__vsf_tgui_location_t.iX,
                            event_ptr->tDelta.use_as__vsf_tgui_location_t.iY,
                            event_ptr->tDelta.hwMillisecond
                        );
        #endif
            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);
            break;
        }

        case VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT & VSF_TGUI_MSG_MSK: {

        #if VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_INFO, 
                        "VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT : [0x%04x]\t", 
                        ptMSG->use_as__vsf_msgt_msg_t.msg);
        #endif
            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);
            break;
        }

        default:
            //return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);
            break;
    }

    return result;
}


fsm_rt_t vsf_tgui_control_msg_handler(  vsf_tgui_control_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t*)control_ptr,
                                            ptMSG,
                                            &c_tVControl);
}

fsm_rt_t vsf_tgui_container_msg_handler(vsf_tgui_container_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t*)control_ptr,
                                            ptMSG,
                                            &c_tVContainer);
}



/*----------------------------------------------------------------------------*
 *  Update Scheme                                                             *
 *----------------------------------------------------------------------------*/

static void __vsf_tgui_container_update_size(vsf_tgui_container_t *container_ptr,
                                             vsf_tgui_size_t* ptSize)
{
    vsf_tgui_size_t *ptControlSize = NULL;

    ASSERT(NULL != container_ptr && NULL != ptSize);
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    do {
        vsf_tgui_margin_t tMargin = container_ptr->tConatinerPadding;
        ptSize->iHeight += tMargin.chTop + tMargin.chBottom;
        ptSize->iWidth += tMargin.chLeft + tMargin.chRight;
    } while(0);
#endif
    ptControlSize =
        &(container_ptr->use_as____vsf_tgui_control_core_t.tRegion.tSize);

    switch(container_ptr->ContainerAttribute.u5Type) {

#if VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER == ENABLED ||                    \
    VSF_TGUI_CFG_SUPPORT_STREAM_CONTAINER == ENABLED

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

#endif

        default:
        case VSF_TGUI_CONTAINER_TYPE_PLANE:
            *ptControlSize = *ptSize;
            break;
    }
}

const vsf_tgui_control_t* __vk_tgui_control_get_next_visible_one_within_container(
                                            const vsf_tgui_control_t* item_ptr)
{
    if (NULL == item_ptr) {
        return NULL;
    }
    do {
        item_ptr = (vsf_tgui_control_t*)
            vsf_msgt_get_next_node_within_container(&(item_ptr->use_as__vsf_msgt_node_t));
        if (NULL == item_ptr) {
            break;
        }

        if (vsf_tgui_control_status_get(item_ptr).Values.bIsVisible) {
            break;
        }
    } while (true);

    return item_ptr;
}


static vsf_tgui_size_t* __vsf_tgui_plane_container_update(
                                                vsf_tgui_container_t* container_ptr,
                                                vsf_tgui_size_t* ptSize)
{
    vsf_tgui_size_t tSize = { 0 };
    vsf_tgui_control_t* item_ptr =
        (vsf_tgui_control_t*)container_ptr->use_as__vsf_msgt_container_t.node_ptr;
    container_ptr->chVisibleItemCount = 0;

    while (NULL != item_ptr) {
        __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core (item_ptr);
        VSF_TGUI_ASSERT(container_ptr->chVisibleItemCount < 255);
        container_ptr->chVisibleItemCount++;

        int16_t iWidth = ptCore->tRegion.tSize.iWidth + ptCore->tRegion.tLocation.iX;
        int16_t iHeight = ptCore->tRegion.tSize.iHeight + ptCore->tRegion.tLocation.iY;
        tSize.iWidth =  max(tSize.iWidth, iWidth);
        tSize.iHeight = max(tSize.iHeight, iHeight);
        item_ptr = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(item_ptr);
    }

    *ptSize = tSize;

    return ptSize;
}

#if VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER == ENABLED
static vsf_tgui_size_t* __vsf_tgui_line_stream_update_vertical( vsf_tgui_container_t* container_ptr,
                                                                vsf_tgui_size_t* ptSize)
{
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    //vsf_tgui_margin_t tMargin = container_ptr->tConatinerPadding;
#endif
    vsf_tgui_location_t tLocation = { 0 };
    vsf_tgui_size_t tSize = {0};
    int16_t iHeight = 0, iWidth = 0;
    vsf_tgui_control_t *item_ptr =
        (vsf_tgui_control_t *)container_ptr->use_as__vsf_msgt_container_t.node_ptr;
    container_ptr->chVisibleItemCount = 0;

    while(NULL != item_ptr) {
        __vsf_tgui_control_core_t *ptCore = vsf_tgui_control_get_core(item_ptr);
        VSF_TGUI_ASSERT(container_ptr->chVisibleItemCount < 255);
        container_ptr->chVisibleItemCount++;

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
        item_ptr = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(item_ptr);
    }
    *ptSize = tSize;

    return ptSize;
}

static vsf_tgui_size_t* __vsf_tgui_line_stream_update_horizontal( vsf_tgui_container_t* container_ptr,
                                                                    vsf_tgui_size_t* ptSize)
{
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    //vsf_tgui_margin_t tMargin = container_ptr->tConatinerPadding;
#endif
    vsf_tgui_location_t tLocation = { 0 };
    vsf_tgui_size_t tSize = { 0 };
    int16_t iHeight = 0, iWidth = 0;
    vsf_tgui_control_t* item_ptr =
        (vsf_tgui_control_t*)container_ptr->use_as__vsf_msgt_container_t.node_ptr;
    container_ptr->chVisibleItemCount = 0;

    while (NULL != item_ptr) {
        __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core(item_ptr);
        VSF_TGUI_ASSERT(container_ptr->chVisibleItemCount < 255);
        container_ptr->chVisibleItemCount++;

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
        item_ptr = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(item_ptr);
    }
    *ptSize = tSize;

    return ptSize;
}
#endif

#if VSF_TGUI_CFG_SUPPORT_STREAM_CONTAINER == ENABLED
static vsf_tgui_size_t* __vsf_tgui_stream_update_vertical(  vsf_tgui_container_t* container_ptr,
                                                            vsf_tgui_size_t* ptSize)
{
    //! todo
    return ptSize;
}

static vsf_tgui_size_t* __vsf_tgui_stream_update_horizontal(vsf_tgui_container_t* container_ptr,
                                                            vsf_tgui_size_t* ptSize)
{
    //! todo
    return ptSize;
}
#endif

static fsm_rt_t __vsf_tgui_container_update(vsf_tgui_container_t* container_ptr)
{

    vsf_tgui_size_t tSize;
    ASSERT(NULL != container_ptr);

    switch(container_ptr->ContainerAttribute.u5Type) {

    #if VSF_TGUI_CFG_SUPPORT_STREAM_CONTAINER == ENABLED
        case VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL:
            /* stream container, with horizontal orientation*/
            /* horizontal orientation: the width is fixed, there could be multiple rows*/
            __vsf_tgui_stream_update_horizontal(container_ptr, &tSize);
            break;

        case VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL:
            /* stream container, with orientation either vertical or horizontal */
            /* vertical orientation: the height is fixed, there could be multiple columns*/
            __vsf_tgui_stream_update_vertical(container_ptr, &tSize);
            break;
    #endif
    #if VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER == ENABLED
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
            /* line stream container, with orientation either vertical or horizontal */
            /* horizontal orientation: only one row*/
            __vsf_tgui_line_stream_update_horizontal(container_ptr, &tSize);
            break;

        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
            /* line stream container, with orientation either vertical or horizontal */
            /* horizontal orientation: only one row*/
            __vsf_tgui_line_stream_update_vertical(container_ptr, &tSize);
            break;
    #endif

        default:
            /* unknown stream type is treated as plan stream*/
        case VSF_TGUI_CONTAINER_TYPE_PLANE:
            /* controls are placed on a plane with their relative locations */
            if (container_ptr->ContainerAttribute.bIsAutoSize) {
                __vsf_tgui_plane_container_update(container_ptr, &tSize);
            }
            break;
    }

    if (container_ptr->ContainerAttribute.bIsAutoSize) {
        __vsf_tgui_container_update_size(container_ptr, &tSize);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_control_update(vsf_tgui_control_t *control_ptr)
{
    ASSERT(NULL != control_ptr);
    if (vsf_tgui_control_is_container((const vsf_tgui_control_t *)control_ptr)) {
        return __vsf_tgui_container_update((vsf_tgui_container_t *)control_ptr);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_container_update(vsf_tgui_container_t* container_ptr)
{
    return vk_tgui_control_update((vsf_tgui_control_t *) container_ptr);
}

fsm_rt_t vk_tgui_control_init(vsf_tgui_control_t* control_ptr)
{
    vsf_tgui_status_t Status = vsf_tgui_control_status_get((vsf_tgui_control_t*)control_ptr);
    Status.chStatus |= VSF_TGUI_CTRL_STATUS_INITIALISED;
    Status.Values.__does_contain_builtin_structure = false;

    vsf_tgui_control_status_set((vsf_tgui_control_t*)control_ptr, Status);

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_container_init(vsf_tgui_container_t *container_ptr)
{
    return vk_tgui_control_init((vsf_tgui_control_t *)container_ptr);
}

#endif


/* EOF */
