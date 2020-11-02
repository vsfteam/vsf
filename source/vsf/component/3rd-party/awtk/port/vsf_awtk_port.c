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

// TODO: add support to other color_format and lcd_mode

/*============================ INCLUDES ======================================*/

#define __VSF_DISP_CLASS_INHERIT__

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_AWTK == ENABLED

#include "awtk.h"
#include "./platforms/vsf/main_loop_vsf.h"
#include "awtk_vsf_port.h"
// for standard c headers
#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/

#ifndef VSF_AWTK_USE_LCD_MEM_FRAGMENT
#   define VSF_AWTK_USE_LCD_MEM_FRAGMENT        ENABLED
#endif

/*============================ INCLUDES ======================================*/

#if VSF_AWTK_USE_LCD_MEM_FRAGMENT == ENABLED
#   include "lcd/lcd_mem_fragment.h"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern ret_t platform_prepare(void);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ret_t vsf_awtk_init(wh_t w, wh_t h, app_type_t app_type, const char* app_name, const char* app_root)
{
    vsf_awtk_op_t op = {
        .malloc_impl = malloc,
        .realloc_impl = realloc,
        .free_impl = free,
        .printf_impl = printf,
        .strcasecmp_impl = strcasecmp,
        .vsnprintf_impl = vsnprintf,
        .vsscanf_impl = vsscanf,

        .sleep_ms_impl = sleep_ms,

        .platform_prepare_impl = platform_prepare,
        .main_loop_init_impl = main_loop_init,
        .get_time_ms64_impl = get_time_ms64,
        .os_fs_impl = os_fs,

        .vsf_eda_get_cur_impl = vsf_eda_get_cur,
        .vsf_eda_post_evt_impl = vsf_eda_post_evt,
        .vk_disp_refresh_impl = vk_disp_refresh,
        .vsf_thread_wfe_impl = vsf_thread_wfe,
        .vk_disp_init_impl = vk_disp_init,
    };
    awtk_vsf_init(&op);

    return tk_init(w, h, app_type, app_name, app_root);
}

#endif
