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

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_LUA == ENABLED && VSF_LUA_USE_LOVE == ENABLED

#include "../lovevsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int __l_font_new(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    int ptsize = (int)luaL_optnumber(L, 2, 8);

    love_font_t *font = luaO_newinst(L, LOVE_CLASS(font));

    font->ttf_font = TTF_OpenFont(filename, ptsize);
    if (NULL == font->ttf_font) {
        luaL_error(L, "Fail to open font %s with size %d", filename, ptsize);
    }
    return 1;
}

static int __l_font_delete(lua_State *L)
{
    love_font_t *font = luaO_checkinst(L, 1, LOVE_CLASS(font));
    TTF_CloseFont(font->ttf_font);
    return 0;
}

static int __l_font_getWidth(lua_State *L)
{
    love_font_t *font = luaO_checkinst(L, 1, LOVE_CLASS(font));
    const char *str = luaL_checkstring(L, 2);
    int w = 0;

    TTF_SizeText(font->ttf_font, str, &w, NULL);
    lua_pushinteger(L, w);
    return 1;
}

static int __l_font_getHeight(lua_State *L)
{
    love_font_t *font = luaO_checkinst(L, 1, LOVE_CLASS(font));
    const char *str = luaL_checkstring(L, 2);
    int h = 0;

    TTF_SizeText(font->ttf_font, str, NULL, &h);
    lua_pushinteger(L, h);
    return 1;
}

int LOVE_MODULE_INIT(font)(lua_State *L)
{
    luaL_Reg oo_reg[] = {
        {luaO_field_new,        __l_font_new        },
        {luaO_field_delete,     __l_font_delete     },
        {"getWidth",            __l_font_getWidth   },
        {"getHeight",           __l_font_getHeight  },
        {0,                     0                   },
    };
    luaO_class_t oo_class = {
        .name   = LOVE_CLASS(font),
        .parent = NULL,
        .size   = sizeof(love_font_t),
        .reg    = oo_reg,
    };
    return luaO_newclass(L, &oo_class);
}

#endif
