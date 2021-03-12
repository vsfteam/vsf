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
#ifndef __VSF_DISP_VGA_M480_H__
#define __VSF_DISP_VGA_M480_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_VGA_M480 == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(__VSF_DISP_VGA_M480_CLASS_IMPLEMENT)
#   undef __VSF_DISP_VGA_M480_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_disp_vga_m480_t)

def_simple_class(vk_disp_vga_m480_t) {
    public_member(
        implement(vk_disp_t)
    )

    private_member(
        vsf_eda_t       eda;
        vk_disp_area_t  area;
        void            *disp_buff;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_vga_m480;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_VGA_M480_H__
