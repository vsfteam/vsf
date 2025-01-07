/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless requir by applicable law or agreed to in writing, software        *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_TINY_GUI_PORT_H__
#define __VSF_TINY_GUI_PORT_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

#if VSF_USE_INPUT == ENABLED
#   include "component/input/vsf_input.h"
#endif

#include "component/ui/disp/vsf_disp.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __TGUI_FONT_DEF(__NAME, __HEIGHT)           __NAME
#define TGUI_FONT_DEF(__NAME, __HEIGHT)                                         \
    __TGUI_FONT_DEF(__NAME, __HEIGHT)

#if VSF_TGUI_CFG_FONT_USE_FREETYPE == ENABLED
#   define TGUI_FT2_FONT_DEF(__NAME, __PATH, __SIZE)                            \
    __TGUI_FONT_DEF(__NAME, 0)
#endif

/*============================ TYPES =========================================*/

typedef enum vsf_tgui_font_type_t {
#if VSF_TGUI_CFG_FONT_USE_FREETYPE == ENABLED
    VSF_TGUI_FONT_FT2,
#endif
} vsf_tgui_font_type_t;

typedef struct vsf_tgui_font_t {
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    const char *name_ptr;
#endif
    vsf_tgui_font_type_t type;
    uint8_t height;

    union {
#if VSF_TGUI_CFG_FONT_USE_FREETYPE == ENABLED
        struct {
            uint8_t font_size;
            const char *font_path_ptr;
            void *data_ptr;
        } ft2;
#endif
    };
} vsf_tgui_font_t;

#ifdef VSF_TGUI_FONTS
enum {
    VSF_TGUI_FONTS
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_USE_INPUT == ENABLED
extern void vsf_tgui_input_init(vsf_tgui_t *gui_ptr, vk_input_notifier_t *notifier);
#endif

extern const vsf_tgui_font_t * vsf_tgui_font_get(uint8_t font_index);
extern uint8_t vsf_tgui_font_number(void);
extern bool vsf_tgui_fonts_init(vsf_tgui_font_t *font_ptr, size_t font_num, const char *font_dir);

// common APIs for view

extern void vsf_tgui_v_bind_disp(vsf_tgui_t *gui_ptr, vk_disp_t *disp, void *pfb, size_t pfb_size);

#endif
#endif
/* EOF */
