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

static void __vk_tgui_depose_top_container( vsf_tgui_t* gui_ptr, 
                                            vsf_tgui_root_container_t *top_ptr, 
                                            vsf_tgui_evt_t *evt_ptr);

static void __vk_tgui_change_actived_control(
                                        vsf_tgui_t* gui_ptr, 
                                        const vsf_tgui_control_t *control_ptr);

/*============================ IMPLEMENTATION ================================*/

implement_vsf_rng_buf(__vsf_tgui_evt_queue_t, vsf_tgui_evt_t, __vsf_interrupt_safe)


vsf_err_t vk_tgui_init(vsf_tgui_t* gui_ptr, const vsf_tgui_cfg_t* cfg_ptr)
{
    vsf_err_t tErr = VSF_ERR_NONE;
    class_internal(gui_ptr, this_ptr, vsf_tgui_t);

    VSF_TGUI_ASSERT(NULL != gui_ptr);
    VSF_TGUI_ASSERT(NULL != cfg_ptr);

    do {
        if ((NULL == cfg_ptr->evt_queue.ptr.obj_ptr)
            || (cfg_ptr->evt_queue.s32_size <= sizeof(vsf_tgui_evt_t))
#if     VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_LAYER_BY_LAYER
            || (NULL == cfg_ptr->bfs_queue.ptr.obj_ptr)
            || (cfg_ptr->bfs_queue.s32_size <= 0)
#endif
            ) {

            tErr = VSF_ERR_INVALID_PARAMETER;
            break;
        }

        //! initialise the queue
        vsf_rng_buf_prepare(__vsf_tgui_evt_queue_t,
            &this.msg_queue,
            (vsf_tgui_evt_t*)(cfg_ptr->evt_queue.ptr.obj_ptr),
            cfg_ptr->evt_queue.s32_size);

        do {
            const vsf_msgt_cfg_t cfg = {
                c_tControlInterfaces,
                UBOUND(c_tControlInterfaces),
            };
            vsf_msgt_init(&(this.use_as__vsf_msgt_t), &cfg);
        } while (0);

#if     VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_LAYER_BY_LAYER
        vsf_msgt_forward_propagate_msg_bfs_init(
            &(this.use_as__vsf_msgt_t),
            (uint16_t*)(cfg_ptr->bfs_queue.ptr.obj_ptr),
            cfg_ptr->bfs_queue.s32_size, false);
#elif   VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_Z_ORDER
        vsf_msgt_forward_propagate_msg_pre_order_traversal_init(
            &(this.use_as__vsf_msgt_t), 
            false);
#endif

        vsf_msgt_forward_propagate_msg_dfs_init(&(this.use_as__vsf_msgt_t));

        //! initialise input
        do {
            this.input.finger_idx_pre = -1; /* -1 means no valid finger index */

        } while(0);

        //! initialise consumer pt task
        do {
            this.consumer.param.queue_ptr = &this.msg_queue;
            this.consumer.param.msg_tree_ptr = &this.use_as__vsf_msgt_t;

            if (NULL != cfg_ptr->root_node_ptr) {
                vk_tgui_set_root_container(gui_ptr, (vsf_tgui_root_container_t *)cfg_ptr->root_node_ptr);
            }

            //! start pt task
            init_vsf_pt(__vsf_tgui_evt_shooter_t, &(this.consumer), cfg_ptr->priority);
        } while (0);

    } while (0);
    return tErr;
}


vsf_err_t vk_tgui_set_root_container(vsf_tgui_t* gui_ptr, 
                                    vsf_tgui_root_container_t *root_node_ptr)
{
    VSF_TGUI_ASSERT(NULL != gui_ptr);
    VSF_TGUI_ASSERT(NULL != root_node_ptr);

    if (root_node_ptr == gui_ptr->consumer.param.root_node_ptr) {
        return VSF_ERR_ALREADY_EXISTS;
    }

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    vsf_tgui_evt_t event = { { {VSF_TGUI_EVT_ON_SET_TOP_CONTAINER} } };
    event.use_as__vsf_tgui_msg_t.target_ptr = (vsf_tgui_control_t*)root_node_ptr;
    return vk_tgui_send_message(gui_ptr, event);

#else
    return vk_tgui_send_message(gui_ptr,
        (vsf_tgui_evt_t) {
        .msg = VSF_TGUI_EVT_ON_SET_TOP_CONTAINER,
            .target_ptr = (vsf_tgui_control_t*)root_node_ptr,
    });
#endif
}

/*! \brief tgui msg queue producer */

bool vk_tgui_send_message(vsf_tgui_t* gui_ptr, vsf_tgui_evt_t event)
{
    class_internal(gui_ptr, this_ptr, vsf_tgui_t);
    if (NULL == gui_ptr) {
        return false;
    }

    if (!vsf_rng_buf_send_one(__vsf_tgui_evt_queue_t,
        this.consumer.param.queue_ptr,
        event)) {
        return false;
    }

    __vsf_interrupt_safe(
        if (this.consumer.param.Attribute.is_queue_drain) {
            this.consumer.param.Attribute.is_queue_drain = false;
            //! wake pt task up
            vsf_eda_post_evt(&(this.consumer.use_as__vsf_pt_t.use_as__vsf_eda_t),
                VSF_TGUI_MSG_AVAILABLE);
        }
    )

    return true;
}

bool vk_tgui_update(vsf_tgui_t* gui_ptr,
                    const vsf_tgui_control_t* target_ptr)
{

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    vsf_tgui_evt_t event = { { {VSF_TGUI_EVT_UPDATE} } };
    event.use_as__vsf_tgui_msg_t.target_ptr = (vsf_tgui_control_t*)target_ptr;
    return vk_tgui_send_message(gui_ptr, event);

#else
    return vk_tgui_send_message(gui_ptr,
        (vsf_tgui_evt_t) {
        .msg = VSF_TGUI_EVT_UPDATE,
            .target_ptr = (vsf_tgui_control_t*)target_ptr,
    });
#endif
}

bool vk_tgui_send_timer_event(  vsf_tgui_t* gui_ptr,
                                const vsf_tgui_control_t* target_ptr)
{

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    vsf_tgui_evt_t event = { { {VSF_TGUI_EVT_ON_TIME} } };
    event.use_as__vsf_tgui_msg_t.target_ptr = (vsf_tgui_control_t*)target_ptr;
    return vk_tgui_send_message(gui_ptr, event);

#else
    return vk_tgui_send_message(gui_ptr,
        (vsf_tgui_evt_t) {
        .msg = VSF_TGUI_EVT_ON_TIME,
            .target_ptr = (vsf_tgui_control_t*)target_ptr,
    });
#endif
}

bool vk_tgui_update_tree(vsf_tgui_t* gui_ptr,
                        const vsf_tgui_control_t* target_ptr)
{

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    vsf_tgui_evt_t event = { { {VSF_TGUI_EVT_UPDATE_TREE} } };
    event.use_as__vsf_tgui_msg_t.target_ptr = (vsf_tgui_control_t*)target_ptr;
    return vk_tgui_send_message(gui_ptr, event);

#else
    return vk_tgui_send_message(gui_ptr,
        (vsf_tgui_evt_t) {
        .msg = VSF_TGUI_EVT_UPDATE_TREE,
            .target_ptr = (vsf_tgui_control_t*)target_ptr,
    });
#endif
}

#if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE

bool vk_tgui_refresh_ex(vsf_tgui_t* gui_ptr,
                        const vsf_tgui_control_t* target_ptr,
                        const vsf_tgui_region_t* region_ptr)
{

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    vsf_tgui_evt_t event = { { {VSF_TGUI_EVT_REFRESH} } };
    event.use_as__vsf_tgui_msg_t.target_ptr = (vsf_tgui_control_t*)target_ptr;
    event.RefreshEvt.region_ptr = region_ptr;
    return vk_tgui_send_message(gui_ptr, event);

#else
    return vk_tgui_send_message(gui_ptr,
        (vsf_tgui_evt_t) {
        .RefreshEvt = {
            .msg = VSF_TGUI_EVT_REFRESH,
            .target_ptr = (vsf_tgui_control_t*)target_ptr,
            .region_ptr = (vsf_tgui_region_t*)region_ptr,
        },
    });
#endif
}

bool vk_tgui_refresh(vsf_tgui_t* gui_ptr)
{
    return vk_tgui_refresh_ex(gui_ptr, NULL, NULL);
}
#endif



static fsm_rt_t __vsf_tgui_send_msg(vsf_tgui_t* gui_ptr,
                                    const vsf_tgui_control_t* control_ptr,
                                    vsf_tgui_evt_t* event_ptr)
{
    fsm_rt_t fsm_rt;
    vsf_msgt_t* msg_tree_ptr = &(gui_ptr->use_as__vsf_msgt_t);
    do {
        fsm_rt = vsf_msgt_backward_propagate_msg(
            msg_tree_ptr,
            (const vsf_msgt_node_t*)control_ptr,
            &event_ptr->use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t);
        if (fsm_rt_cpl == fsm_rt) {
            break;
        } else if (fsm_rt < 0) {
            //! msg is not handled
            break;
        } else if (fsm_rt >= fsm_rt_user) {
            break;
        }
    } while (1);

#if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
    if (fsm_rt >= 0) {
        //! update reference to the control which handles the message
        control_ptr = (const vsf_tgui_control_t* )
        vsf_msgt_backward_propagate_msg_get_last_node(msg_tree_ptr);

        if (VSF_TGUI_MSG_RT_REFRESH == fsm_rt) {
        #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_WARNING, " \tRequest Refresh\r\n");
        #endif
            vk_tgui_refresh_ex(gui_ptr, control_ptr, NULL);
        } else if (VSF_TGUI_MSG_RT_REFRESH_PARENT == fsm_rt) {
        #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_WARNING, " \tRequest Refresh Parent\r\n");
        #endif
            if (NULL == control_ptr->use_as__vsf_msgt_node_t.parent_ptr) {
                vk_tgui_refresh_ex(gui_ptr, control_ptr, NULL);
            } else {
                vk_tgui_refresh_ex( 
                    gui_ptr, 
                    (const vsf_tgui_control_t *)control_ptr->
                        use_as__vsf_msgt_node_t.parent_ptr, 
                    NULL);
            }
        }
    }
#endif
    
    return fsm_rt;
}


static void __vk_tgui_send_dfs_msg( vsf_msgt_t* msg_tree_ptr,
                                    const vsf_msgt_node_t* node_ptr,
                                    vsf_tgui_evt_t* event_ptr)
{
    do {
        fsm_rt_t fsm_rt = vsf_msgt_forward_propagate_msg_dfs(
            msg_tree_ptr,
            node_ptr,
            &(event_ptr->use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t));
        if (fsm_rt_cpl == fsm_rt) {
            break;
        } else if (fsm_rt < 0) {
            //! msg is not handled
            break;
        }
    } while (1);
}

static void __vk_tgui_send_bfs_msg( vsf_msgt_t* msg_tree_ptr,
                                    const vsf_msgt_node_t* node_ptr,
                                    vsf_tgui_evt_t* event_ptr,
                                    uint_fast8_t u8_status_mask)
{
    do {
        fsm_rt_t fsm_rt = vsf_msgt_forward_propagate_msg_bfs(
            msg_tree_ptr,
            node_ptr,
            &(event_ptr->use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t),
            u8_status_mask);
        if (fsm_rt_cpl == fsm_rt) {
            break;
        } else if (fsm_rt < 0) {
            //! msg is not handled
            break;
        }
    } while (1);
}

static void __vk_tgui_send_pot_msg( vsf_msgt_t* msg_tree_ptr,
                                    const vsf_msgt_node_t* node_ptr,
                                    vsf_tgui_evt_t* event_ptr,
                                    uint_fast8_t u8_status_mask)
{
    do {
        fsm_rt_t fsm_rt = vsf_msgt_forward_propagate_msg_pre_order_traversal(
            msg_tree_ptr,
            node_ptr,
            &(event_ptr->use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t),
            u8_status_mask);
        if (fsm_rt_cpl == fsm_rt) {
            break;
        } else if (fsm_rt < 0) {
            //! msg is not handled
            break;
        }
    } while (1);
}

static void __vk_tgui_set_focus(vsf_tgui_t* gui_ptr,
                                const vsf_tgui_control_t* previous_ptr,
                                const vsf_tgui_control_t* current_ptr,
                                vsf_evt_t msg)
{
    vsf_tgui_evt_t temp_event = {0};// = { { {VSF_TGUI_EVT_LOST_ACTIVE}} };
    temp_event.use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg = msg + 1;

    //! send deactivated message first to previous node
    if (NULL != previous_ptr) {
        __vsf_tgui_send_msg(gui_ptr, previous_ptr, &temp_event);
    }

    if (NULL != current_ptr) {
        //! send activated message to current node
        temp_event.use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg = msg;

        __vsf_tgui_send_msg(gui_ptr, current_ptr, &temp_event);
    }
}

static void __vk_tgui_change_focus_control( vsf_tgui_t* gui_ptr, 
                                            const vsf_tgui_control_t *control_ptr,
                                            __vk_tgui_focus_t *focus_ptr,
                                            vsf_evt_t msg)
{
    //class_internal(&(gui_ptr->consumer.param), this_ptr, vsf_pt(__vsf_tgui_evt_shooter_t));

    if (focus_ptr->current_ptr != control_ptr) {
        focus_ptr->previous_ptr = focus_ptr->current_ptr;
        focus_ptr->current_ptr = control_ptr;

        __vk_tgui_set_focus(gui_ptr,
            focus_ptr->previous_ptr,
            focus_ptr->current_ptr,
            msg);
    }
}


const vsf_tgui_control_t *vsf_tgui_actived_control_get(vsf_tgui_t *gui_ptr)
{
    VSF_TGUI_ASSERT(NULL != gui_ptr);
    return gui_ptr->consumer.param.Activated.current_ptr;
}

#if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
const vsf_tgui_control_t *vsf_tgui_pointed_control_get(vsf_tgui_t *gui_ptr)
{
    VSF_TGUI_ASSERT(NULL != gui_ptr);
    return gui_ptr->consumer.param.pointer_above.current_ptr;
}
#endif

bool vsf_tgui_set_pointer_location(  vsf_tgui_t *gui_ptr, 
                                        int_fast8_t idx, 
                                        vsf_tgui_location_t *location_ptr)
{
    VSF_TGUI_ASSERT(NULL != gui_ptr);
    VSF_TGUI_ASSERT(idx < dimof(gui_ptr->input.current));

    if (idx < 0) {
        idx = 0;
    }

    if (NULL == location_ptr) {
        location_ptr = gui_ptr->input.current + idx;
    }

    return __vk_tgui_send_touch_evt(gui_ptr, 
                                    idx,
                                    VSF_TGUI_EVT_POINTER_MOVE,
                                    *location_ptr);
}

const vsf_tgui_location_t *vsf_tgui_get_pointer_location(vsf_tgui_t *gui_ptr, int_fast8_t idx)
{
    VSF_TGUI_ASSERT(NULL != gui_ptr);

    if (idx < 0) {
        return (const vsf_tgui_location_t *)gui_ptr->input.current;
    } else if (idx < dimof(gui_ptr->input.current)) {
        return (const vsf_tgui_location_t *)gui_ptr->input.current + idx;
    }

    return NULL;
}


void vsf_tgui_low_level_refresh_ready(vsf_tgui_t *gui_ptr)
{
#if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
    VSF_TGUI_ASSERT(NULL != gui_ptr);

    vsf_eda_post_evt(   &(gui_ptr->consumer.use_as__vsf_pt_t.use_as__vsf_eda_t),
                        VSF_TGUI_MSG_LOW_LEVEL_READY_TO_REFRESH);
#endif
}

static bool __vk_tgui_decide_refresh_region(vsf_pt(__vsf_tgui_evt_shooter_t) *this_ptr,
                                            const vsf_tgui_control_t *control_ptr)
{
    bool result = true;
    this.region_ptr = NULL;

    if (!vsf_tgui_control_get_visible_region(control_ptr, &this.temp_region)) {
        return false;
    }

    if (NULL != this.event.RefreshEvt.region_ptr) {
        result = vsf_tgui_region_intersect(&this.temp_region, &this.temp_region, this.event.RefreshEvt.region_ptr);
    }

    this.region_ptr = &this.temp_region;
    return result;
}


/*! \brief tgui msg queue consumer */
implement_vsf_pt(__vsf_tgui_evt_shooter_t)
{
    vsf_tgui_t* gui_ptr = container_of(this_ptr, vsf_tgui_t, consumer.param);
    bool result = false;
    vsf_evt_t   msg;
    vsf_pt_begin();

    while (1) {

loop_start:
        this.node_ptr = NULL;
        result = false;

        do {
            __vsf_interrupt_safe(
                result = vsf_rng_buf_get_one(__vsf_tgui_evt_queue_t, this.queue_ptr, &this.event);
                if (!result) {
                    this.Attribute.is_queue_drain = true;
                }
            )
            if (result) {
                break;
            }

            //! wait for new event arrival
            vsf_pt_wait_for_evt(VSF_TGUI_MSG_AVAILABLE);
        } while (1);

        msg = this.event.use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg;

        /*! \note Use user specific target_ptr, otherwise use RootNode */
        if (NULL != this.event.use_as__vsf_tgui_msg_t.target_ptr) {
            /*! use user specified target */
            this.node_ptr = (const vsf_msgt_node_t*)this.event.use_as__vsf_tgui_msg_t.target_ptr;
        } else if (NULL == this.root_node_ptr) {
            continue;
        } else {
            this.node_ptr = (const vsf_msgt_node_t*)this.root_node_ptr;
        }

        switch (msg & VSF_TGUI_MSG_MSK) {
            case VSF_TGUI_MSG_CONTROL_EVT& VSF_TGUI_MSG_MSK:

                switch (msg & VSF_TGUI_EVT_MSK) {

                    case VSF_TGUI_EVT_ON_SET_TOP_CONTAINER & VSF_TGUI_EVT_MSK:
                #if VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME == ENABLED
                        if (NULL != this.root_node_ptr) {
                             __vk_tgui_depose_top_container(gui_ptr, 
                                                            (vsf_tgui_root_container_t *)this.root_node_ptr, 
                                                            NULL);
                        }
                #endif
                        this.root_node_ptr = NULL;
                        this.Activated.current_ptr = NULL;
                        this.Activated.previous_ptr = NULL;

                        do {
                            if (NULL == this.event.use_as__vsf_tgui_msg_t.target_ptr) {
                                break;
                            } else if (!this.node_ptr->Attribute._.is_top) {
                                break;
                            }

                            //! set new top container
                            this.Activated.current_ptr = (const vsf_tgui_control_t *)this.node_ptr;
                            this.root_node_ptr = (const vsf_tgui_root_container_t *)this.node_ptr;
                            ((vsf_tgui_root_container_t *)this.root_node_ptr)->gui_ptr = gui_ptr;

                    #if VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME == ENABLED
                            //! send on load message 
                            this.event.use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg = VSF_TGUI_EVT_ON_LOAD;

                    #   if VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL
                            vsf_msgt_forward_propagate_msg_bfs_setting(this.msg_tree_ptr, false);
                            /*! constructor message */
                            __vk_tgui_send_bfs_msg( this.msg_tree_ptr, 
                                                    this.node_ptr, 
                                                    &this.event,
                                                    0);
                    #   elif VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_PRE_ORDER_TRAVERSAL
                            vsf_msgt_forward_propagate_msg_pre_order_traversal_setting(this.msg_tree_ptr, false);
                            /*! constructor message */
                            __vk_tgui_send_pot_msg( this.msg_tree_ptr, 
                                                    this.node_ptr, 
                                                    &this.event,
                                                    0);
                    #   endif
                    #endif

                            this.event.use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg = VSF_TGUI_EVT_UPDATE_TREE;
                            __vk_tgui_send_dfs_msg(this.msg_tree_ptr, this.node_ptr, &this.event);

                        } while(0);
                        goto loop_start;

                #if VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME == ENABLED
                    case VSF_TGUI_EVT_ON_LOAD & VSF_TGUI_EVT_MSK:
                #   if VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL
                        vsf_msgt_forward_propagate_msg_bfs_setting(this.msg_tree_ptr, false);
                        /*! constructor message */
                        __vk_tgui_send_bfs_msg( this.msg_tree_ptr, 
                                                this.node_ptr, 
                                                &this.event,
                                                0);
                #   elif VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_PRE_ORDER_TRAVERSAL
                        vsf_msgt_forward_propagate_msg_pre_order_traversal_setting(this.msg_tree_ptr, false);
                        /*! constructor message */
                        __vk_tgui_send_pot_msg( this.msg_tree_ptr, 
                                                this.node_ptr, 
                                                &this.event,
                                                0);
                #   endif
                        goto loop_start;
                #endif 


                #if VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME == ENABLED
                    case VSF_TGUI_EVT_ON_DEPOSE & VSF_TGUI_EVT_MSK: {
                        __vk_tgui_depose_top_container(gui_ptr, (vsf_tgui_root_container_t *)this.node_ptr, &this.event);

                        goto loop_start;
                    }
                #endif
                    case VSF_TGUI_EVT_ON_TIME & VSF_TGUI_EVT_MSK:
                        __vsf_tgui_send_msg(gui_ptr, (const vsf_tgui_control_t*)this.node_ptr, &this.event);
                        break;

                    case VSF_TGUI_EVT_UPDATE & VSF_TGUI_EVT_MSK:

                        if (!vsf_tgui_control_status_get((vsf_tgui_control_t*)this.node_ptr).Values.__does_contain_builtin_structure) {
                            //! send message
                            __vsf_tgui_send_msg(gui_ptr, (const vsf_tgui_control_t*)this.node_ptr, &this.event);
                            //goto loop_start;
                            break;
                        }
                        //this.event.use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg = VSF_TGUI_EVT_UPDATE_TREE;
                        // fall through

                    case VSF_TGUI_EVT_UPDATE_TREE & VSF_TGUI_EVT_MSK:
                        //! send message
                        __vk_tgui_send_dfs_msg(this.msg_tree_ptr, this.node_ptr, &this.event);
                        goto loop_start;

        #if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
                    case VSF_TGUI_EVT_REFRESH & VSF_TGUI_EVT_MSK:
                        if (NULL != this.event.use_as__vsf_tgui_msg_t.target_ptr) {
                            /*! use user specified target */
                            /*! do not refresh it if the control doesn't belong to current top container*/
                            if (this.root_node_ptr != vk_tgui_control_get_top((const vsf_tgui_control_t*)this.node_ptr)) {
                                goto loop_start;
                            }
                        }

                        if (!__vk_tgui_decide_refresh_region(this_ptr, (const vsf_tgui_control_t *)this.node_ptr)) {
                            goto loop_start;
                        }

        #   if   VSF_TGUI_CFG_SUPPORT_TRANSPARENT_CONTROL == ENABLED
                        bool bIsRequestRefreshParent = false;

                        //! if the target control is transparent, refresh its parent
                        do {
                            __vsf_tgui_control_core_t* ptCore = vsf_tgui_control_get_core((vsf_tgui_control_t*)this.node_ptr);
                            if (ptCore->Status.Values.is_control_transparent) {
                                //! try to fetch its parent
                                if (NULL != this.node_ptr->parent_ptr) {
                                    this.node_ptr = (const vsf_msgt_node_t*)this.node_ptr->parent_ptr;
                                    bIsRequestRefreshParent = true;
                                    continue;
                                } 
                            }
                            break;
                        } while (true);

                        if (bIsRequestRefreshParent) {
                            const vsf_tgui_container_t *container_ptr = (const vsf_tgui_container_t *)this.node_ptr;
                            if (container_ptr->ContainerAttribute.is_forced_to_refresh_whole_background) {
                                if (!__vk_tgui_decide_refresh_region(this_ptr, (const vsf_tgui_control_t *)container_ptr)) {
                                    goto loop_start;
                                }
                            }
                        }
        #   endif
                        
                        goto refresh_loop;
        #endif

                    case VSF_TGUI_EVT_GET_ACTIVE& VSF_TGUI_EVT_MSK: 
                        __vk_tgui_change_focus_control( gui_ptr, 
                                                        (const vsf_tgui_control_t *)this.node_ptr, 
                                                        &this.Activated, 
                                                        VSF_TGUI_EVT_GET_ACTIVE);
                        goto loop_start;

                    default:
                    case VSF_TGUI_EVT_LOST_ACTIVE& VSF_TGUI_EVT_MSK:
                        //case VSF_TGUI_EVT_ON_LOAD & VSF_TGUI_EVT_MSK:
                            //! message we don't need to handle
                        goto loop_start;
                }
                break;

            case VSF_TGUI_MSG_POINTER_EVT & VSF_TGUI_MSG_MSK:

                /*! \note specified target will not change current activated control*/
                if (NULL != this.event.use_as__vsf_tgui_msg_t.target_ptr) {
                    break;
                }

                //! send message to currently activated control
                this.node_ptr = &(this.Activated.current_ptr->use_as__vsf_msgt_node_t);
                VSF_TGUI_ASSERT( NULL != this.node_ptr );
                

                //! pointer up event could only be sent to active node
                if (VSF_TGUI_EVT_POINTER_DOWN == msg) {

                    //! check new target of activation
                    this.node_ptr = vsf_msgt_shoot_node(
                        this.msg_tree_ptr,
                        (const vsf_msgt_node_t*)this.root_node_ptr,
                        (uintptr_t) & (this.event.PointerEvt.use_as__vsf_tgui_location_t));
                

                    if (NULL == this.node_ptr) {
                        //! missed all node (control)
                        goto loop_start;
                    }

                    
                #if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
                    __vk_tgui_change_focus_control( gui_ptr, 
                                    (const vsf_tgui_control_t *)this.node_ptr, 
                                    &this.pointer_above, 
                                    VSF_TGUI_EVT_POINTER_ENTER);
                #endif

                    /*! \IMPORTANT only pointer/finger 0 can active controls. 
                     *             tGUI is mainly designed to support touch screen
                     *             rather than a device with a mouse. So when you
                     *             use PC to simulate a touchscreen, a right click
                     *             won't change the actived controls. It is a *KNOWN
                     *             ISSUE* and we will *NOT* fix this, because designing  
                     *             an application with normal mouse support is out of 
                     *             original scope. 
                     */
                    if (0 == this.event.PointerEvt.idx) {
                        __vk_tgui_change_focus_control( gui_ptr, 
                                        (const vsf_tgui_control_t *)this.node_ptr, 
                                        &this.Activated, 
                                        VSF_TGUI_EVT_GET_ACTIVE);
                    }
                }
            #if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
                else if (VSF_TGUI_EVT_POINTER_MOVE == msg) {
                    //! check new target of activation
                    this.node_ptr = vsf_msgt_shoot_node(
                        this.msg_tree_ptr,
                        (const vsf_msgt_node_t*)this.root_node_ptr,
                        (uintptr_t) & (this.event.PointerEvt.use_as__vsf_tgui_location_t));

                    __vk_tgui_change_focus_control( gui_ptr, 
                                    (const vsf_tgui_control_t *)this.node_ptr, 
                                    &this.pointer_above, 
                                    VSF_TGUI_EVT_POINTER_ENTER);
                    if (this.event.PointerEvt.idx < dimof(gui_ptr->input.current)) {
                        gui_ptr->input.current[this.event.PointerEvt.idx] = this.event.PointerEvt.use_as__vsf_tgui_location_t;
                    }
                #if VSF_TGUI_CFG_SUPPORT_MOUSE_MOVE_HANDLING == ENABLED
                    if (NULL == this.node_ptr) {
                        //! missed all node (control)
                        goto loop_start;
                    }
                #else
                    goto loop_start;
                #endif
                }
            #endif
                break;

            case VSF_TGUI_MSG_KEY_EVT & VSF_TGUI_MSG_MSK:
            case VSF_TGUI_MSG_GESTURE_EVT& VSF_TGUI_MSG_MSK:
            case VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT & VSF_TGUI_MSG_MSK:
                if (NULL == this.event.use_as__vsf_tgui_msg_t.target_ptr) {
                    //! send message to currently activated control
                    this.node_ptr = &(this.Activated.current_ptr->use_as__vsf_msgt_node_t);
                    VSF_TGUI_ASSERT( NULL != this.node_ptr );
                }
                break;

            default:
                //! message we don't need to handle
                goto loop_start;
        }

        //! default message handling
        __vsf_tgui_send_msg(gui_ptr, (const vsf_tgui_control_t*)this.node_ptr, &this.event);

        continue;

    #if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
refresh_loop :
        do {
            vsf_tgui_region_t *region_ptr = NULL;

            do {
                region_ptr = vsf_tgui_v_refresh_loop_begin(gui_ptr, this.region_ptr);
                if (NULL == region_ptr) {
                    /*! wait until it is ready to refresh */
                    vsf_pt_wait_for_evt(VSF_TGUI_MSG_LOW_LEVEL_READY_TO_REFRESH);
                    region_ptr = NULL;
                }
            } while(NULL == region_ptr);
            this.region_ptr = region_ptr;

            /* this statement is very important, do not remove */
            this.event.RefreshEvt.region_ptr = (vsf_tgui_region_t*)this.region_ptr;

            //! set first refresh node bit
            do {
                __vsf_tgui_control_core_t* ptCore =
                    vsf_tgui_control_get_core((const vsf_tgui_control_t*)this.node_ptr);
                ptCore->Status.Values.__is_the_first_node_for_refresh = true;
            } while (0);

        #if VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL
            vsf_msgt_forward_propagate_msg_bfs_setting(this.msg_tree_ptr, true);
            __vk_tgui_send_bfs_msg( this.msg_tree_ptr, 
                                    this.node_ptr, 
                                    &this.event,
                                    (   VSF_TGUI_CTRL_STATUS_INITIALISED
                                    |   VSF_TGUI_CTRL_STATUS_ENABLED
                                    |   VSF_TGUI_CTRL_STATUS_VISIBLE
                                    ));
        #elif VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_PRE_ORDER_TRAVERSAL
            vsf_msgt_forward_propagate_msg_pre_order_traversal_setting(this.msg_tree_ptr, true);
            __vk_tgui_send_pot_msg( this.msg_tree_ptr, 
                                    this.node_ptr, 
                                    &this.event,
                                    (   VSF_TGUI_CTRL_STATUS_INITIALISED
                                    |   VSF_TGUI_CTRL_STATUS_ENABLED
                                    |   VSF_TGUI_CTRL_STATUS_VISIBLE
                                    ));
        #endif
        } while (vsf_tgui_v_refresh_loop_end(gui_ptr));
    #endif
    }

    vsf_pt_end();
}


static void __vk_tgui_depose_top_container( vsf_tgui_t* gui_ptr, 
                                            vsf_tgui_root_container_t *top_ptr, 
                                            vsf_tgui_evt_t *evt_ptr)
{
    vsf_tgui_evt_t event;
    class_internal(&(gui_ptr->consumer.param), this_ptr, vsf_pt(__vsf_tgui_evt_shooter_t));

    if (!top_ptr->use_as__vsf_tgui_container_t.
            use_as__vsf_msgt_container_t.
                use_as__vsf_msgt_node_t.Attribute._.is_top) {
        //! we only deal with top node
        return ;
    }

    if (NULL == evt_ptr) {
        evt_ptr = &event;
        event.use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg = VSF_TGUI_EVT_ON_DEPOSE;
        event.use_as__vsf_tgui_msg_t.target_ptr = (vsf_tgui_control_t *)top_ptr;  
    }

    //! send message
    __vk_tgui_send_dfs_msg(this.msg_tree_ptr, (const vsf_msgt_node_t *)top_ptr, evt_ptr);

    //! remove reference to tGUI obj
    do {
        top_ptr->gui_ptr = NULL;
        if (top_ptr == this.root_node_ptr) {
            this.root_node_ptr = NULL;
        }
    } while(0);
}



#endif


/* EOF */
