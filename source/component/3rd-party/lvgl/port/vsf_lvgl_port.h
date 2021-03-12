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
#ifndef __VSF_LVGL_PORT_H___
#define __VSF_LVGL_PORT_H___

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED

#include "lv_conf.h"

/*============================ MACROS ========================================*/

#if !LV_USE_USER_DATA
#   error "vsf_lvgl_port require LV_USE_USER_DATA"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if USE_LV_LOG
extern void vsf_lvgl_printf(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)£»
#endif

extern void vsf_lvgl_disp_bind(vk_disp_t *disp, lv_disp_drv_t *lvgl_disp_drv);
extern void vsf_lvgl_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

#endif  // VSF_USE_UI && VSF_USE_LVGL
#endif  // __VSF_LVGL_PORT_H___
