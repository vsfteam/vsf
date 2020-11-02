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

#if APP_USE_FREETYPE_DEMO == ENABLED

#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

#if APP_USE_TGUI_DEMO == ENABLED
static FT_FILE __ft_font_dir[] = {
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "wqy-microhei.ttc",
    },
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "DejaVuSerif.ttf",
    },
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "DejaVuSans.ttf",
    },
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "DejaVuSans-Bold.ttf",
    },
};
#endif

#if APP_USE_XBOOT_XUI_DEMO == ENABLED
static FT_FILE __ft_xboot_fonts_dir[] = {
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "Roboto-Regular.ttf",
    },
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "Roboto-Italic.ttf",
    },
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "Roboto-Bold.ttf",
    },
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "Roboto-BoldItalic.ttf",
    },
    {
        .attr           = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .name           = "FontAwesome.ttf",
    },
};

static FT_FILE __ft_xboot_assets_dir[] = {
    {
        .attr           = VSF_FILE_ATTR_DIRECTORY,
        .name           = "fonts",
        .d.child        = (vk_memfs_file_t *)__ft_xboot_fonts_dir,
        .d.child_num    = dimof(__ft_xboot_fonts_dir),
        .d.child_size   = sizeof(FT_FILE),
    },
};

static FT_FILE __ft_xboot_framework_dir[] = {
    {
        .attr           = VSF_FILE_ATTR_DIRECTORY,
        .name           = "assets",
        .d.child        = (vk_memfs_file_t *)__ft_xboot_assets_dir,
        .d.child_num    = dimof(__ft_xboot_assets_dir),
        .d.child_size   = sizeof(FT_FILE),
    },
};
#endif

static FT_FILE __ft_root_dir[] = {
#if APP_USE_TGUI_DEMO == ENABLED
    {
        .attr           = VSF_FILE_ATTR_DIRECTORY,
        .name           = "font",
        .d.child        = (vk_memfs_file_t *)__ft_font_dir,
        .d.child_num    = dimof(__ft_font_dir),
        .d.child_size   = sizeof(FT_FILE),
    },
#endif
#if APP_USE_XBOOT_XUI_DEMO == ENABLED
    {
        .attr           = VSF_FILE_ATTR_DIRECTORY,
        .name           = "framework",
        .d.child        = (vk_memfs_file_t *)__ft_xboot_framework_dir,
        .d.child_num    = dimof(__ft_xboot_framework_dir),
        .d.child_size   = sizeof(FT_FILE),
    },
#endif
};

/*============================ GLOBAL VARIABLES ==============================*/

FT_FILE ft_root         = {
    .attr               = VSF_FILE_ATTR_DIRECTORY,
    .name               = "/",
    .d.child            = (vk_memfs_file_t *)__ft_root_dir,
    .d.child_num        = dimof(__ft_root_dir),
    .d.child_size       = sizeof(FT_FILE),
};

/*============================ IMPLEMENTATION ================================*/

void freetype_demo_init(void)
{
#if APP_USE_TGUI_DEMO == ENABLED || APP_USE_XBOOT_XUI_DEMO == ENABLED
    FILE *f;
    char path[256];
#endif

#if APP_USE_TGUI_DEMO == ENABLED
    for(int i = 0; i < dimof(__ft_font_dir); i++) {
        strcpy(path, APP_CFG_TGUI_RES_DIR "font/");
        strcat(path, __ft_font_dir[i].name);
        f = fopen(path, "rb");
        if (f != NULL) {
            fseek(f, 0, SEEK_END);
            __ft_font_dir[i].size = ftell(f);
            fseek(f, 0, SEEK_SET);
            __ft_font_dir[i].f.buff = malloc(__ft_font_dir[i].size);
            if (NULL == __ft_font_dir[i].f.buff) {
                __ft_font_dir[i].size = 0;
            }
            if (__ft_font_dir[i].size != fread(__ft_font_dir[i].f.buff, 1, __ft_font_dir[i].size, f)) {
                __ft_font_dir[i].size = 0;
                free(__ft_font_dir[i].f.buff);
                __ft_font_dir[i].f.buff = NULL;
            }
            fclose(f);
        } else {
            printf("fail to load font %s\r\n", path);
        }
    }
#endif

#if APP_USE_XBOOT_XUI_DEMO == ENABLED
    for(int i = 0; i < dimof(__ft_xboot_fonts_dir); i++) {
        strcpy(path, APP_CFG_XBOOT_RES_DIR "font/");
        strcat(path, __ft_xboot_fonts_dir[i].name);
        f = fopen(path, "rb");
        if (f != NULL) {
            fseek(f, 0, SEEK_END);
            __ft_xboot_fonts_dir[i].size = ftell(f);
            fseek(f, 0, SEEK_SET);
            __ft_xboot_fonts_dir[i].f.buff = malloc(__ft_xboot_fonts_dir[i].size);
            if (NULL == __ft_xboot_fonts_dir[i].f.buff) {
                __ft_xboot_fonts_dir[i].size = 0;
            }
            if (__ft_xboot_fonts_dir[i].size != fread(__ft_xboot_fonts_dir[i].f.buff, 1, __ft_xboot_fonts_dir[i].size, f)) {
                __ft_xboot_fonts_dir[i].size = 0;
                free(__ft_xboot_fonts_dir[i].f.buff);
                __ft_xboot_fonts_dir[i].f.buff = NULL;
            }
            fclose(f);
        } else {
            printf("fail to load font %s\r\n", path);
        }
    }
#endif
}

#endif
