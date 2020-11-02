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

#ifndef __VSF_MSG_TREE_H__
#define __VSF_MSG_TREE_H__

/*============================ INCLUDES ======================================*/
#include "osa_service/vsf_osa_service_cfg.h"

#if     VSF_USE_MSG_TREE == ENABLED

#include "kernel/vsf_kernel.h"


/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__

#if     defined(__VSF_MSG_TREE_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_MSG_TREE_CLASS_IMPLEMENT
#elif   defined(__VSF_MSG_TREE_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_MSG_TREE_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#ifndef VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING
#   define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING         DISABLED
#endif

#ifndef VSF_MSGT_NODE_OFFSET_TYPE
#   define VSF_MSGT_NODE_OFFSET_TYPE                    int16_t
#endif

#ifndef VSF_MSG_TREE_CFG_SUPPORT_DUAL_LIST
#   define VSF_MSG_TREE_CFG_SUPPORT_DUAL_LIST           DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef VSF_MSGT_NODE_OFFSET_TYPE  vsf_msgt_node_offset_t;

declare_vsf_rng_buf(__bfs_node_fifo_t)

def_vsf_rng_buf(__bfs_node_fifo_t, uint16_t)

declare_structure(vsf_msgt_cfg_t)
declare_structure(vsf_msgt_container_t)
declare_structure(vsf_msgt_node_t)

//! \name the base class for all other tree messages
//! @{
typedef struct vsf_msgt_msg_t {
    vsf_evt_t   msg;                                                           //!< the tree message which should larger than VSF_EVT_USER
}vsf_msgt_msg_t;
//! @}

typedef enum vsf_msgt_node_status_t {
    // avoid vsf_msgt_node_status_t to be optimized to 8bit
    __VSF_MSGT_NODE_STATUS_LEAST_MAX    = INT16_MAX,
    __VSF_MSGT_NODE_STATUS_LEAST_MIN    = INT16_MIN,

    VSF_MSGT_NODE_VALID                 = BIT(0),                               //!< whether the node is valid/initialised or not
    VSF_MSGT_NODE_ENABLED               = BIT(1),                               //!< whether the node is enabled or not
    VSF_MSGT_NODE_VISIBLE               = BIT(2),                               //!< whether the node is visible
    VSF_MSGT_NODE_ACTIVE                = BIT(3),
    VSF_MSGT_NODE_HIDE_CONTENT          = BIT(4),                               //!< whether hide the content inside container
}vsf_msgt_node_status_t;

typedef enum vsf_msgt_handler_type_t {
    VSF_MSGT_NODE_HANDLER_TYPE_CALLBACK = 0,                                    //!< use normal callback
    VSF_MSGT_NODE_HANDLER_TYPE_FSM = 0,                                         //!< use fsm (same as normal callback)
    VSF_MSGT_NODE_HANDLER_TYPE_SUBCALL,                                         //!< eda sub call (pt subcall)
    VSF_MSGT_NODE_HANDLER_TYPE_EDA,                                             //!< pass msg to eda

    //VSF_MSGT_NODE_HANDLER_TYPE_DELEGATE,                                      //!< use delegate (not supported yet)
}vsf_msgt_handler_type_t;

typedef enum {
    VSF_MSGT_ERR_REUQEST_VISIT_AGAIN = fsm_rt_user,
    VSF_MSGT_ERR_REQUEST_VISIT_PARENT,
    VSF_MSGT_ERR_NONE = 0,
    VSF_MSGT_ERR_MSG_NOT_HANDLED = -1,
} vsf_msgt_err_t;


typedef bool vsf_msgt_method_shoot_t(const vsf_msgt_node_t*, uintptr_t);
typedef vsf_msgt_node_status_t vsf_msgt_method_status_t(vsf_msgt_node_t* );
typedef fsm_rt_t vsf_msgt_method_fsm_t (vsf_msgt_node_t* node_ptr, vsf_msgt_msg_t* msg_ptr);
typedef struct vsf_msgt_subcall_t {
    vsf_param_eda_evthandler_t* sub_routine_fn;
    uintptr_t target_ptr;
} vsf_msgt_subcall_t;

typedef struct vsf_msgt_handler_t vsf_msgt_handler_t;
struct vsf_msgt_handler_t {
    uint16_t    u2_type      : 2;                                                //!< message handler type
    uint16_t                : 14;
    implement_ex(
        union {
            vsf_msgt_method_fsm_t* fsm_fn;                                       //!< message handler
            vsf_eda_t* eda_ptr;                                                   //!< target eda receiver
            vsf_msgt_subcall_t* sub_call_fn;                                      //!< subcall handler
        },
        fn
    )
};

//! \name v-table for tree message node
//! @{
declare_interface(i_msg_tree_node_t)
def_interface(i_msg_tree_node_t)
    vsf_msgt_handler_t          msg_handler;
    vsf_msgt_method_status_t    *Status;                                        //!< get status of target node
    vsf_msgt_method_shoot_t     *Shoot;                                         //!< range check
end_def_interface(i_msg_tree_node_t)
//! @}

//! \name abstract class for message tree node
//! @{
def_structure(vsf_msgt_node_t)

    uint8_t         id;                                                       //!< node ID for lookup table

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    union {
        union {
            struct {
                uint8_t is_visited        : 1;
                uint8_t                 : 4;

                uint8_t is_top          : 1;

                /* \note when it is container, it is possible to make the container
                         transparent. When it is not a container, is_transparent
                         should be ignored. */
                uint8_t is_transparent  : 1;                                    //!< used together with is_container
                uint8_t is_container    :  1;                                   //!< whether it is a container or not
            };
            uint8_t     chAttribute;
        };
#endif
        union {
            struct {
                uint8_t is_visited        : 1;
                uint8_t                 : 4;

                uint8_t is_top          : 1;
                /* \note when it is container, it is possible to make the container
                         transparent. When it is not a container, is_transparent
                         should be ignored. */
                uint8_t is_transparent  : 1;                                    //!< used together with is_container
                uint8_t is_container    : 1;                                    //!< whether it is a container or not
            }_;
            uint8_t     value;
        } Attribute;

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    };
#endif


    vsf_msgt_container_t* parent_ptr;                                             //!< parent node
    struct {
#if VSF_MSG_TREE_CFG_SUPPORT_DUAL_LIST == ENABLED
        vsf_msgt_node_offset_t previous;
#endif
        vsf_msgt_node_offset_t next;
    } Offset;
#if VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING == ENABLED
    const char *node_name_ptr;
#endif
end_def_structure(vsf_msgt_node_t)
//! @}

//! \name abstract class for container
//! @{
def_structure(vsf_msgt_container_t)
    implement(vsf_msgt_node_t)

    vsf_msgt_node_t* node_ptr;
end_def_structure(vsf_msgt_container_t)
//! @}


typedef struct __vsf_msgt_msg_handling_fsm_t {
    uint8_t                     state;
    vsf_msgt_msg_t             *msg_ptr;
    const vsf_msgt_node_t      *node_ptr;
    const vsf_msgt_handler_t   *handler_fn;
    uint8_t                    status_msk;
}__vsf_msgt_msg_handling_fsm_t;

def_structure(vsf_msgt_cfg_t)
    const i_msg_tree_node_t    *interface_ptr;
    uint8_t                     type_num;
end_def_structure(vsf_msgt_cfg_t)

declare_class(vsf_msgt_t)

def_class(vsf_msgt_t,
    private_member(
        implement_ex(vsf_msgt_cfg_t, NodeTypes)
        struct {
            uint8_t state;
            __vsf_msgt_msg_handling_fsm_t msg_handling;
        } BW;
        struct {
            uint8_t state;
            __vsf_msgt_msg_handling_fsm_t msg_handling;
            bool is_support_container_post_handling;
        } FWPOT;
        struct {
            uint8_t state;
            __bfs_node_fifo_t fifo;
            __vsf_msgt_msg_handling_fsm_t msg_handling;
            bool is_support_container_post_handling;
        } FWBFS;
        struct {
            uint8_t state;
            __vsf_msgt_msg_handling_fsm_t msg_handling;
        } FWDFS;
    )
)
end_def_class(vsf_msgt_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern
void vsf_msgt_init( vsf_msgt_t* obj_ptr, const vsf_msgt_cfg_t *cfg_ptr);

extern
const vsf_msgt_node_t* vsf_msgt_get_next_node_within_container(
                                            const vsf_msgt_node_t* node_ptr);

extern
const vsf_msgt_node_t * vsf_msgt_shoot_top_node(  vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t *root_ptr,
                                            uintptr_t bullet_info_ptr);
extern
const vsf_msgt_node_t * vsf_msgt_shoot_node(  vsf_msgt_t* obj_ptr,
                                        const vsf_msgt_node_t *root_ptr,
                                        uintptr_t bullet_info_ptr);

extern
fsm_rt_t vsf_msgt_backward_propagate_msg(   vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t *node_ptr,
                                            vsf_msgt_msg_t *msg_ptr);
extern
const vsf_msgt_node_t *vsf_msgt_backward_propagate_msg_get_last_node(
                                                            vsf_msgt_t* obj_ptr);


SECTION(".text.vsf.osa_service.msg_tree"
        ".vsf_msgt_forward_propagate_msg_pre_order_traversal_init")
extern
void vsf_msgt_forward_propagate_msg_pre_order_traversal_init(vsf_msgt_t* obj_ptr,
                                             bool is_support_container_post_handling);

SECTION(".text.vsf.osa_service.msg_tree"
        ".vsf_msgt_forward_propagate_msg_pre_order_traversal_setting")
extern
void vsf_msgt_forward_propagate_msg_pre_order_traversal_setting(vsf_msgt_t* obj_ptr,
                                                bool is_support_container_post_handling);

SECTION(".text.vsf.osa_service.msg_tree"
        ".vsf_msgt_forward_propagate_msg_pre_order_traversal")
extern
fsm_rt_t vsf_msgt_forward_propagate_msg_pre_order_traversal(
                                            vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t* root_ptr,
                                            vsf_msgt_msg_t* msg_ptr,
                                            uint_fast8_t status_msk);

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs")
extern
void vsf_msgt_forward_propagate_msg_bfs_init(   vsf_msgt_t* obj_ptr,
                                                uint16_t *fifo_buffer_ptr,
                                                uint_fast16_t buff_size,
                                                bool is_support_container_post_handling);

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs_setting")
extern
void vsf_msgt_forward_propagate_msg_bfs_setting(vsf_msgt_t* obj_ptr,
                                                bool is_support_container_post_handling);

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs")
extern
fsm_rt_t vsf_msgt_forward_propagate_msg_bfs(vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t* node_ptr,
                                            vsf_msgt_msg_t* msg_ptr,
                                            uint_fast8_t status_msk);

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_dfs")
extern
void vsf_msgt_forward_propagate_msg_dfs_init(vsf_msgt_t* obj_ptr);

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_dfs")
extern
fsm_rt_t vsf_msgt_forward_propagate_msg_dfs(vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t* node_ptr,
                                            vsf_msgt_msg_t* msg_ptr);

#ifdef __cplusplus
}
#endif

#endif

#endif
/* EOF */
