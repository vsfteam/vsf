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
#include "osa_service/vsf_osa_service_cfg.h"

#if VSF_USE_MSG_TREE == ENABLED

#define __VSF_MSG_TREE_CLASS_IMPLEMENT
#include "vsf_msg_tree.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/


static const i_msg_tree_node_t c_tRootNodes[] = {
    {0},                                                                        //!< base node
    {0}                                                                         //!< base container
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

implement_vsf_rng_buf(__bfs_node_fifo_t, uint16_t, NO_RNG_BUF_PROTECT)

void vsf_msgt_init(vsf_msgt_t* obj_ptr, const vsf_msgt_cfg_t* cfg_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr && NULL != cfg_ptr);
    VSF_OSA_SERVICE_ASSERT(     (NULL != cfg_ptr->ptInterfaces) 
                            ||  (0 != cfg_ptr->chTypeNumbers));
    
    this.NodeTypes = *cfg_ptr;
}


/*----------------------------------------------------------------------------*
 * Node shooting algorithm                                                    *
 *----------------------------------------------------------------------------*/

static bool __msgt_shoot(   vsf_msgt_t* obj_ptr,
                            const vsf_msgt_node_t *ptNode,
                            uintptr_t pBulletInfo)
{
    uint_fast8_t u8_id = ptNode->u8_id;
    bool bResult = false;
    vsf_msgt_method_shoot_t* fnShoot = NULL;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    if (u8_id < 2) {
        fnShoot = c_tRootNodes[u8_id].Shoot;
    } else {
        u8_id -= 2;
        fnShoot = this.NodeTypes.ptInterfaces[u8_id].Shoot;
    }

    if (NULL != fnShoot) {
        bResult = fnShoot(ptNode, pBulletInfo);
    }

    return bResult;
}

const vsf_msgt_node_t * vsf_msgt_shoot_top_node(  vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t *ptRoot,
                                            uintptr_t pBulletInfo)
{
    vsf_msgt_node_t* ptItem = NULL;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);

    
    do {
        if (NULL == ptRoot) {
            break;
        }

        //! shoot the root item
        if (__msgt_shoot(obj_ptr, ptRoot, pBulletInfo)) {
            ptItem = (vsf_msgt_node_t *)ptRoot;
        } else if (ptRoot->bIsContainer){
            if (!ptRoot->bIsTransparent) {
                //! not in the range
                break;
            }
            //! transparent container
        } else {
            //! not in the range
            break;
        }

        //! check nodes in this container
        do {
            vsf_msgt_container_t* ptContainer = (vsf_msgt_container_t*)ptItem;
            vsf_msgt_node_t * ptNode = ptContainer->ptNode;

            if (NULL == ptNode) {
                //! empty container
                break;
            }

            do {
                if (__msgt_shoot(obj_ptr, ptNode, pBulletInfo)) {
                    //! shoot a node
                    ptItem = ptNode;
                    break;
                }

                //! check offset of next node
                if (0 == ptNode->Offset.iNext) {
                    //!< the last node
                    break;
                }
                //! point to next node with offset 
                ptNode = (vsf_msgt_node_t*)((intptr_t)ptNode + ptNode->Offset.iNext);
            } while(true);

        } while (0);

    } while(0);
    return ptItem;
}


const vsf_msgt_node_t * vsf_msgt_shoot_node(  vsf_msgt_t* obj_ptr,
                                        const vsf_msgt_node_t *ptRoot,
                                        uintptr_t pBulletInfo)
{
    const vsf_msgt_node_t* ptItem = NULL;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);

    
    do {
        const vsf_msgt_node_t* ptNode = ptRoot;

        do {
            ptNode = vsf_msgt_shoot_top_node(obj_ptr, ptNode, pBulletInfo);
            if (NULL == ptNode) {
                break;
            }
            
            if (ptItem == ptNode) {
                //! the same container (we only hit a container)
                break;
            }
            ptItem = ptNode;
            if (!ptItem->bIsContainer) {
                //!< the leaf node
                break;
            } /* else; find a container, check the contained nodes*/
            
        } while(true);

    } while (0);

    return ptItem;
}

/*----------------------------------------------------------------------------*
 * Message Handing                                                            *
 *----------------------------------------------------------------------------*/

void __vsf_msg_handling_init(   __vsf_msgt_msg_handling_fsm_t* ptFSM,
                                vsf_msgt_t* obj_ptr,
                                vsf_msgt_msg_t* ptMessage,
                                bool bIgnoreStatus)
{
    ptFSM->chState = 0;
    ptFSM->ptMessage = ptMessage;
    ptFSM->bIgnoreStatus = bIgnoreStatus;
}

#undef THIS_FSM_STATE
#define THIS_FSM_STATE  ptFSM->chState

#define RESET_MSGT_HANDLING_FSM()         \
        do { THIS_FSM_STATE = 0; } while(0)


fsm_rt_t __vsf_msg_handling(__vsf_msgt_msg_handling_fsm_t *ptFSM, 
                            vsf_msgt_t* obj_ptr,
                            const vsf_msgt_node_t* ptNode)
{
    enum {
        START = 0,
        CHECK_STATUS,
        GET_HANDLER,
        HANDLE_FSM,
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        HANDLE_SUBCALL,
#endif
    };
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    switch (THIS_FSM_STATE) {
        case START:
            if (ptFSM->bIgnoreStatus) {
                THIS_FSM_STATE = GET_HANDLER;
                break;
            } else {
                THIS_FSM_STATE ++;
            }
            //break;
        case CHECK_STATUS:
            if (ptNode->u8_id - 2 >= this.NodeTypes.chTypeNumbers) {
                //! illegal Node ID detected, ignore this
                RESET_MSGT_HANDLING_FSM();
                return fsm_rt_err;
            } else {
                vsf_msgt_method_status_t* fnStatus = NULL;

                uint_fast8_t u8_id = ptNode->u8_id;
                if (u8_id < 2) {
                    fnStatus = c_tRootNodes[u8_id].Status;
                } else {
                    u8_id -= 2;
                    fnStatus = this.NodeTypes.ptInterfaces[u8_id].Status;
                }

                if (NULL != fnStatus) {
                    //!< get status
                    vsf_msgt_node_status_t tStatus = (*fnStatus)((vsf_msgt_node_t *)ptNode);
                    if (0 == (tStatus & (VSF_MSGT_NODE_VALID | VSF_MSGT_NODE_ENABLED))) {
                        //! not valid and enabled
                        RESET_MSGT_HANDLING_FSM();
                        return fsm_rt_err;
                    }
                }
                THIS_FSM_STATE = GET_HANDLER;
            }
            //break;
        case GET_HANDLER: {
            const vsf_msgt_handler_t* ptHandler = NULL;
            uint_fast8_t u8_id = ptNode->u8_id;
            if (u8_id < 2) {
                ptHandler = &c_tRootNodes[u8_id].tMessageHandler;
            } else {
                u8_id -= 2;
                ptHandler = &this.NodeTypes.ptInterfaces[u8_id].tMessageHandler;
            }
            if (NULL == ptHandler->fnFSM) {
                //! empty handler
                RESET_MSGT_HANDLING_FSM();
                return fsm_rt_err;
            }
            ptFSM->ptHandler = ptHandler;

            switch (ptHandler->tType) {
                case VSF_MSGT_NODE_HANDLER_TYPE_FSM:
                    THIS_FSM_STATE = HANDLE_FSM;
                    break;
    #if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
                case VSF_MSGT_NODE_HANDLER_TYPE_SUBCALL:
                    if (NULL == vsf_eda_get_cur()) {
                        //! it's not possible to call sub eda
                        RESET_MSGT_HANDLING_FSM();
                        return (fsm_rt_t)VSF_ERR_NOT_SUPPORT;
                    }
                    THIS_FSM_STATE = HANDLE_SUBCALL;
                    break;
    #endif
                case VSF_MSGT_NODE_HANDLER_TYPE_EDA: {
                    vsf_err_t tErr = vsf_eda_post_msg(ptHandler->ptEDA, ptFSM->ptMessage);
                    VSF_OSA_SERVICE_ASSERT(tErr == VSF_ERR_NONE);
                    UNUSED_PARAM(tErr);
                    RESET_MSGT_HANDLING_FSM();
                    return fsm_rt_cpl;
                }
                default:
                    //!< unknown type
                    RESET_MSGT_HANDLING_FSM();
                    return fsm_rt_err;
            }
            break;
        }

        case HANDLE_FSM: {
            fsm_rt_t tfsm = ptFSM->ptHandler->fnFSM((vsf_msgt_node_t *)ptNode, ptFSM->ptMessage);
            if (fsm_rt_cpl == tfsm) {
                //! message has been handled
                RESET_MSGT_HANDLING_FSM();
                return fsm_rt_cpl;
            } else if (tfsm < 0) {
                //! message is not handled
                RESET_MSGT_HANDLING_FSM();
                return fsm_rt_err;
            } else {
                return tfsm;
            }
            break;
        }
        
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        case HANDLE_SUBCALL:
            //! todo add support for subcall
            RESET_MSGT_HANDLING_FSM();
            return fsm_rt_err;
#endif
    }

    return fsm_rt_on_going;
}


/*----------------------------------------------------------------------------*
 * Backward Message Propagation                                               *
 *----------------------------------------------------------------------------*/
#undef THIS_FSM_STATE
#define THIS_FSM_STATE  this.BW.chState

#define RESET_MSGT_BACKWARD_PROPAGATE_MSG_FSM()                                 \
        do { THIS_FSM_STATE = 0; } while(0)

fsm_rt_t vsf_msgt_backward_propagate_msg(   vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t *ptNode,
                                            vsf_msgt_msg_t *ptMessage)
{
    enum {
        START = 0,
        MSG_HANDLING,
        GET_PARENT,
    };
    fsm_rt_t tfsm = fsm_rt_on_going;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    

    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    VSF_OSA_SERVICE_ASSERT(NULL != ptMessage);

    switch (THIS_FSM_STATE) {
        case START:
            if (NULL == ptNode) {
                return (fsm_rt_t)VSF_ERR_INVALID_PTR;
            } 
            this.BW.tMSGHandling.ptNode = ptNode;
            if (this.NodeTypes.chTypeNumbers > 0 && NULL == this.NodeTypes.ptInterfaces) {
                return (fsm_rt_t)VSF_ERR_INVALID_PARAMETER;
            }
            __vsf_msg_handling_init(&this.BW.tMSGHandling, obj_ptr, ptMessage, false);
            THIS_FSM_STATE++;
            //break;

        case MSG_HANDLING:
            tfsm = __vsf_msg_handling(&this.BW.tMSGHandling, obj_ptr, this.BW.tMSGHandling.ptNode);
            if (tfsm < 0) {
                THIS_FSM_STATE = GET_PARENT;
                break;
            } else if (fsm_rt_cpl == tfsm) {
                RESET_MSGT_BACKWARD_PROPAGATE_MSG_FSM();
                //return fsm_rt_cpl;
            } else if (tfsm >= fsm_rt_user) {
                RESET_MSGT_BACKWARD_PROPAGATE_MSG_FSM();
            }
            return tfsm;

        case GET_PARENT:
            //! backward propagate
            if (NULL == this.BW.tMSGHandling.ptNode->ptParent) {
                //! no parent
                RESET_MSGT_BACKWARD_PROPAGATE_MSG_FSM();
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            }
            this.BW.tMSGHandling.ptNode = this.BW.tMSGHandling.ptNode->ptParent;
            THIS_FSM_STATE = MSG_HANDLING;
            break;
    }
    

    return fsm_rt_on_going;
}

/*----------------------------------------------------------------------------*
 * Forward Message Propagation using DFS algorithm                            *
 *----------------------------------------------------------------------------*/

fsm_rt_t vsf_msgt_forward_propagate_msg_dfs(vsf_msgt_t* obj_ptr,
                                        vsf_msgt_node_t* ptNode,
                                        vsf_msgt_msg_t* ptMessage)
{
    class_internal(&obj_ptr, this_ptr, vsf_msgt_t);

    

    return fsm_rt_on_going;
}

/*----------------------------------------------------------------------------*
 * Forward Message Propagation using BFS algorithm                            *
 *----------------------------------------------------------------------------*/

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs")
void vsf_msgt_forward_propagate_msg_bfs_init(   vsf_msgt_t* obj_ptr, 
                                                uint16_t *phwFIFOBuffer, 
                                                uint_fast16_t hwBuffSize)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    VSF_OSA_SERVICE_ASSERT(NULL != phwFIFOBuffer);
    VSF_OSA_SERVICE_ASSERT(hwBuffSize > 2);

    this.FWBFS.chState = 0;
    vsf_rng_buf_prepare(__bfs_node_fifo_t, 
                        &(this.FWBFS.tFIFO), 
                        phwFIFOBuffer, 
                        hwBuffSize);
}

#undef THIS_FSM_STATE
#define THIS_FSM_STATE  this.FWBFS.chState

#define RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM()                                   \
        do { THIS_FSM_STATE = 0; } while(0)

SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs")
fsm_rt_t vsf_msgt_forward_propagate_msg_bfs(vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t* ptNode,
                                            vsf_msgt_msg_t* ptMessage)
{
    enum {
        START = 0,
        FETCH_ITEM,
        VISIT_ITEM,        
    };
    fsm_rt_t tfsm = fsm_rt_on_going;
    uint16_t hwOffset = 0;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    
    //vsf_msgt_node_t *ptItem = NULL;

    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    VSF_OSA_SERVICE_ASSERT(NULL != ptMessage);

    switch (THIS_FSM_STATE) {
        case START:
            if (NULL == ptNode) {
                return (fsm_rt_t)VSF_ERR_INVALID_PTR;
            }
            if (    (this.NodeTypes.chTypeNumbers > 0) 
                &&  (NULL == this.NodeTypes.ptInterfaces)) {
                return (fsm_rt_t)VSF_ERR_INVALID_PARAMETER;
            }
            VSF_OSA_SERVICE_ASSERT (NULL != this.FWBFS.tFIFO.buffer_ptr);

            if (!vsf_rng_buf_send_one(  __bfs_node_fifo_t, 
                                        &(this.FWBFS.tFIFO), 
                                        0)) {
                return (fsm_rt_t)VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
            }

            __vsf_msg_handling_init(&this.FWBFS.tMSGHandling, 
                                    obj_ptr, 
                                    ptMessage, 
                                    true);
            THIS_FSM_STATE ++;
            // break;

        case FETCH_ITEM:
            if (!vsf_rng_buf_get_one(   __bfs_node_fifo_t, 
                                        &(this.FWBFS.tFIFO), 
                                        &hwOffset)) {
                //! search complete
                RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM();
                return fsm_rt_cpl;
            }
            //! calculate target item address
            this.FWBFS.tMSGHandling.ptNode = 
                (vsf_msgt_node_t *)((uintptr_t)ptNode + hwOffset);
            THIS_FSM_STATE = VISIT_ITEM;
            //break;

        case VISIT_ITEM:
            tfsm = __vsf_msg_handling(  &this.FWBFS.tMSGHandling, 
                                        obj_ptr, 
                                        this.FWBFS.tMSGHandling.ptNode);
            if (tfsm < 0 || tfsm == fsm_rt_cpl) {

                THIS_FSM_STATE = FETCH_ITEM;
                //! add nodes to fifo
                if (this.FWBFS.tMSGHandling.ptNode->bIsContainer) {
                    vsf_msgt_container_t* ptContainer = 
                        (vsf_msgt_container_t*)this.FWBFS.tMSGHandling.ptNode;
                    vsf_msgt_node_t *ptTemp = ptContainer->ptNode;
                    if (NULL == ptTemp) {
                        break;
                    }

                    //! put all nodes into fifo
                    do {
                        hwOffset = 
                            (uint16_t)((uintptr_t)ptTemp 
                                     - (uintptr_t)this.FWBFS.tMSGHandling.ptNode);
                        if (!vsf_rng_buf_send_one(  __bfs_node_fifo_t, 
                                                    &(this.FWBFS.tFIFO), 
                                                    hwOffset)) {
                            RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM();
                            return (fsm_rt_t)
                                VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
                        }
                        if (0 == ptTemp->Offset.iNext) {
                            break;
                        }
                        ptTemp = (vsf_msgt_node_t*)
                                    ((intptr_t)ptTemp + ptTemp->Offset.iNext);
                    } while(1);
                } 
                break;
            } 
            return tfsm;
    }

    return fsm_rt_on_going;
}

#endif
/* EOF */
