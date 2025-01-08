/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#ifndef __VSF_TINY_GUI_V_PORT_H__
#define __VSF_TINY_GUI_V_PORT_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_EXAMPLE

#define __VSF_DISP_CLASS_INHERIT__
#include "component/ui/disp/vsf_disp.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TGUI_HOR_MAX
#   define VSF_TGUI_HOR_MAX                         800
#endif

#ifndef VSF_TGUI_VER_MAX
#   define VSF_TGUI_VER_MAX                         600
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_tgui_vport_init(__vsf_tgui_v_port_ptr)

/*============================ TYPES =========================================*/

def_structure(vsf_tgui_v_port_t)
#if __IS_COMPILER_IAR__
    //please remove this if your structure is not emplty
    uint8_t             : 8;
#endif
end_def_structure(vsf_tgui_v_port_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif

#endif
/* EOF */
