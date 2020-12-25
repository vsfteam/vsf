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

#ifndef __VSF_TINY_GUI_MC_H__
#define __VSF_TINY_GUI_MC_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED
#include "./vsf_msg_tree_cfg.h"


#include "vsf_tgui_control.h"
#include "vsf_tgui_label.h"
#include "vsf_tgui_button.h"
#include "vsf_tgui_panel.h"
#include "vsf_tgui_list.h"
#include "vsf_tgui_text_list.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern bool vsf_tgui_region_intersect(  vsf_tgui_region_t* ptRegionOut,
                                        const vsf_tgui_region_t* ptRegionIn0,
                                        const vsf_tgui_region_t* ptRegionIn1);
extern
vsf_tgui_region_t * vsf_tgui_region_get_relative_region(
                                        vsf_tgui_region_t *ptOutRegion,
                                        vsf_tgui_region_t *ptReferenceRegion,
                                        vsf_tgui_region_t *ptAbsoluteRegion);
extern
vsf_tgui_location_t * vsf_tgui_region_get_relative_location(
                                        vsf_tgui_location_t *ptOutLocation,
                                        vsf_tgui_location_t *ptReferenceLocation,
                                        vsf_tgui_location_t *ptAbsoluteLocation);
#endif

#endif
/* EOF */
