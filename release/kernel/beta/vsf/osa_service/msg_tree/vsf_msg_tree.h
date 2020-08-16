#pragma once
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
#elif   defined(__VSF_MSG_TREE_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_MSG_TREE_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
#ifndef VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING
#   define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING         DISABLED
#endif

#define VSF_MSGT_NODE_ID_USER                           2

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

//! \name the base class for all other tree messages
//! @{
typedef struct vsf_msgt_msg_t {
    vsf_evt_t   tMSG;                                                           //!< the tree message which should larger than VSF_EVT_USER
}vsf_msgt_msg_t;
//! @}

typedef enum vsf_msgt_node_status_t {
    VSF_MSGT_NODE_VALID                 = _BV(0),                               //!< whether the node is valid/initialised or not
    VSF_MSGT_NODE_ENABLED               = _BV(1),                               //!< whether the node is enabled or not
    VSF_MSGT_NODE_VISIBLE               = _BV(2),                               //!< whether the node is visible 
    VSF_MSGT_NODE_ACTIVE                = _BV(3),
}vsf_msgt_node_status_t;

typedef enum vsf_msgt_handler_type_t {
    VSF_MSGT_NODE_HANDLER_TYPE_CALLBACK = 0,                                    //!< use normal callback
    VSF_MSGT_NODE_HANDLER_TYPE_FSM = 0,                                         //!< use fsm (same as normal callback)
    VSF_MSGT_NODE_HANDLER_TYPE_SUBCALL,                                         //!< eda sub call (pt subcall)
    VSF_MSGT_NODE_HANDLER_TYPE_EDA,                                             //!< pass msg to eda

    //VSF_MSGT_NODE_HANDLER_TYPE_DELEGATE,                                      //!< use delegate (not supported yet)
}vsf_msgt_handler_type_t;

typedef enum {
    VSF_MSGT_ERR_NONE = 0,
    VSF_MSGT_ERR_MSG_NOT_HANDLED = -1,
} vsf_msgt_err_t;

typedef struct vsf_msgt_node_t vsf_msgt_node_t;

typedef bool vsf_msgt_method_shoot_t(const vsf_msgt_node_t*, uintptr_t);
typedef vsf_msgt_node_status_t vsf_msgt_method_status_t(vsf_msgt_node_t* );
typedef fsm_rt_t vsf_msgt_method_fsm_t (vsf_msgt_node_t* ptNode, vsf_msgt_msg_t* ptMSG);
typedef struct vsf_msgt_subcall_t {
    vsf_param_eda_evthandler_t* fnSub;
    uintptr_t target_ptr;
} vsf_msgt_subcall_t;

typedef struct vsf_msgt_handler_t vsf_msgt_handler_t;
struct vsf_msgt_handler_t {
    vsf_msgt_handler_type_t         tType;                                      //!< message handler type
    union {
        vsf_msgt_method_fsm_t *fnFSM;                                            //!< message handler
        vsf_eda_t* ptEDA;                                                       //!< target eda receiver
        vsf_msgt_subcall_t* ptSubCall;                                          //!< subcall handler
    };
};

//! \name v-table for tree message node 
//! @{
def_interface(i_msg_tree_node_t)
    vsf_msgt_handler_t          tMessageHandler;
    vsf_msgt_method_status_t    *Status;                                        //!< get status of target node         
    vsf_msgt_method_shoot_t     *Shoot;                                         //!< range check
end_def_interface(i_msg_tree_node_t)
//! @}

//! \name abstract class for message tree node
//! @{
def_structure(vsf_msgt_node_t)
    
    uint8_t         u8_id;                                                       //!< node ID for lookup table
    union {
        struct {
            uint8_t                 : 6;

            /* \note when it is container, it is possible to make the container 
                     transparent. When it is not a container, bIsTransparent 
                     should be ignored. */
            uint8_t bIsTransparent  : 1;                                        //!< used together with bIsContainer
            uint8_t bIsContainer    : 1;                                        //!< whether it is a container or not
        };
        uint8_t     chAttribute;
    };
    vsf_msgt_node_t* ptParent;                                                  //!< parent node
    struct {
#if VSF_MSG_TREE_CFG_SUPPORT_DUAL_LIST == ENABLED
        vsf_msgt_node_offset_t iPrevious;
#endif
        vsf_msgt_node_offset_t iNext;
    } Offset;
#if VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING == ENABLED
    const char *pchNodeName;
#endif
end_def_structure(vsf_msgt_node_t)
//! @}

//! \name abstract class for container 
//! @{
def_structure(vsf_msgt_container_t)
    implement(vsf_msgt_node_t)

    vsf_msgt_node_t* ptNode;                                            
end_def_structure(vsf_msgt_container_t)
//! @}


typedef struct __vsf_msgt_msg_handling_fsm_t {
    uint8_t chState;
    vsf_msgt_msg_t* ptMessage;
    const vsf_msgt_node_t *ptNode;
    const vsf_msgt_handler_t* ptHandler;
    bool bIgnoreStatus;
}__vsf_msgt_msg_handling_fsm_t;

def_structure(vsf_msgt_cfg_t)
    const i_msg_tree_node_t* ptInterfaces;
    uint8_t                 chTypeNumbers;
end_def_structure(vsf_msgt_cfg_t)

declare_class(vsf_msgt_t)

def_class(vsf_msgt_t,
    private_member(
        implement_ex(vsf_msgt_cfg_t, NodeTypes)
        struct {
            uint8_t chState;
            __vsf_msgt_msg_handling_fsm_t tMSGHandling;
        } BW;
        struct {
            uint8_t chState;
            __bfs_node_fifo_t tFIFO;
            __vsf_msgt_msg_handling_fsm_t tMSGHandling;
        } FWBFS;
    )             
)
end_def_class(vsf_msgt_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern 
void vsf_msgt_init( vsf_msgt_t* obj_ptr, const vsf_msgt_cfg_t *cfg_ptr);

extern 
const vsf_msgt_node_t * vsf_msgt_shoot_top_node(  vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t *ptRoot,
                                            uintptr_t pBulletInfo);
extern 
const vsf_msgt_node_t * vsf_msgt_shoot_node(  vsf_msgt_t* obj_ptr,
                                        const vsf_msgt_node_t *ptRoot,
                                        uintptr_t pBulletInfo);

extern
fsm_rt_t vsf_msgt_backward_propagate_msg(   vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t *ptNode,
                                            vsf_msgt_msg_t *ptMessage);

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs")
extern 
void vsf_msgt_forward_propagate_msg_bfs_init(   vsf_msgt_t* obj_ptr, 
                                                uint16_t *phwFIFOBuffer, 
                                                uint_fast16_t hwBuffSize);

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs")
extern 
fsm_rt_t vsf_msgt_forward_propagate_msg_bfs(vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t* ptNode,
                                            vsf_msgt_msg_t* ptMessage);
#endif

#endif
/* EOF */
