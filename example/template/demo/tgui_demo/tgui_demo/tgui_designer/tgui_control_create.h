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
#include "vsf.h"

#ifndef __TGUI_CONTROL_CREATE_H__
#define __TGUI_CONTROL_CREATE_H__

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

extern void vsf_tgui_create_init(vsf_tgui_t* gui_ptr);
extern void vsf_tgui_control_add_child(vsf_tgui_container_t* parent, vsf_tgui_control_t* control, bool first);
extern vsf_tgui_control_t* vsf_tgui_control_create(vsf_tgui_container_t* parent, vsf_tree_msg_node_id_t id, char* control_name, bool first, uint16_t head_size);

#endif
#endif


/* EOF */
