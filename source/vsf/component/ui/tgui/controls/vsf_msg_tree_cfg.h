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

#ifndef __VSF_MSG_TREE_USER_CFG_H__
#define __VSF_MSG_TREE_USER_CFG_H__

/*============================ INCLUDES ======================================*/
#include "osa_service/vsf_osa_service.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum vsf_msgt_node_id_t {
/*-------------Add your own gui element below-------------*/
    VSF_TGUI_COMPONENT_ID_CONTROL   = VSF_MSGT_NODE_ID_USER,
    VSF_TGUI_COMPONENT_ID_CONTAINER,
    VSF_TGUI_COMPONENT_ID_LABEL,
    VSF_TGUI_COMPONENT_ID_BUTTON,
    VSF_TGUI_COMPONENT_ID_PANEL,
    VSF_TGUI_COMPONENT_ID_LIST,
    VSF_TGUI_COMPONENT_ID_TEXT_LIST,
/*-------------Add your own gui element above-------------*/
    __VSF_TGUI_COMPONENT_TYPE_NUM,
}vsf_tree_msg_NODE_id_t;


/*============================ GLOBAL VARIABLES ==============================*/
extern 
const i_msg_tree_node_t 
c_tControlInterfaces[__VSF_TGUI_COMPONENT_TYPE_NUM - VSF_MSGT_NODE_ID_USER];

/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
