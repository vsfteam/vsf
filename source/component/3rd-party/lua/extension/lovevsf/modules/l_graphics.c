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

typedef struct __l_graphics_t {
    SDL_Color color;
    SDL_Color bg_color;

    struct {
        SDL_Window *window;
        SDL_Surface *surface;
    } sdl;
} __l_graphics_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __l_graphics_t __l_graphics;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static SDL_Color __l_graphics_getColorArg(lua_State *L, SDL_Color def)
{
    int argc;
    if (lua_isnoneornil(L, 1)) {
        return def;
    }

    if (lua_istable(L, 1)) {
        for (int i = 1; i <= 4; i++) {
            lua_rawgeti(L, 1, i);
        }

        def.r = (uint8_t)luaL_checknumber(L, -4);
        def.g = (uint8_t)luaL_checknumber(L, -3);
        def.b = (uint8_t)luaL_checknumber(L, -2);
        def.a = (uint8_t)luaL_checknumber(L, -1);

        lua_pop(L, 4);
        return def;
    }

    argc = lua_gettop(L);
    def.r = (uint8_t)luaL_checknumber(L, 1);
    def.g = (uint8_t)luaL_checknumber(L, 2);
    def.b = (uint8_t)luaL_checknumber(L, 3);
    if (argc >= 4) {
        def.a = (uint8_t)luaL_checknumber(L, 4);
    } else {
        def.a = 0xFF;
    }
    return def;
}

static int __l_graphics_pushColor(lua_State *L, SDL_Color color)
{
    lua_pushinteger(L, color.r);
    lua_pushinteger(L, color.g);
    lua_pushinteger(L, color.b);
    lua_pushinteger(L, color.a);
    return 4;
}

static int __l_graphics_getDimensions(lua_State *L)
{
    int w, h;
    SDL_GetWindowSize(__l_graphics.sdl.window, &w, &h);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
}

static int __l_graphics_getWidth(lua_State *L)
{
    int w;
    SDL_GetWindowSize(__l_graphics.sdl.window, &w, NULL);
    lua_pushinteger(L, w);
    return 1;
}

static int __l_graphics_getHeight(lua_State *L)
{
    int h;
    SDL_GetWindowSize(__l_graphics.sdl.window, NULL, &h);
    lua_pushinteger(L, h);
    return 1;
}

static int __l_graphics_getBackgroundColor(lua_State *L)
{
    return __l_graphics_pushColor(L, __l_graphics.bg_color);
}

static int __l_graphics_setBackgroundColor(lua_State *L)
{
    SDL_Color def = { .a = 0xFF, .r = 0, .g = 0, .b = 0 };
    __l_graphics.bg_color = __l_graphics_getColorArg(L, def);
    return 0;
}

static int __l_graphics_getColor(lua_State *L)
{
    return __l_graphics_pushColor(L, __l_graphics.color);
}

static int __l_graphics_setColor(lua_State *L)
{
    SDL_Color def = { .a = 0xFF, .r = 0xFF, .g = 0xFF, .b = 0xFF };
    __l_graphics.color = __l_graphics_getColorArg(L, def);
    return 0;
}

static int __l_graphics_clear(lua_State *L)
{
    SDL_Color bg_color = __l_graphics_getColorArg(L, __l_graphics.bg_color);
    uint32_t actual_color = SDL_MapRGBA(__l_graphics.sdl.surface->format,
                            bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_Rect rect = { 0 };
    SDL_GetWindowSize(__l_graphics.sdl.window, &rect.w, &rect.h);
    SDL_FillRect(__l_graphics.sdl.surface, &rect, actual_color);
    return 0;
}

static int __l_graphics_present(lua_State *L)
{
    SDL_UpdateWindowSurface(__l_graphics.sdl.window);
    return 0;
}

static int __l_graphics_print(lua_State *L)
{
    const char *str = luaL_tolstring(L, 1, NULL);
    int x = (int)luaL_checknumber(L, 2);
    int y = (int)luaL_checknumber(L, 3);

    lua_getglobal(L, "love");
    lua_getfield(L, -1, "graphics");
    lua_getfield(L, -1, "font");
    love_font_t *font = luaO_checkinst(L, -1, LOVE_CLASS(font));
    lua_pop(L, 3);

    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font->ttf_font, str, __l_graphics.color);
    if (text_surface != NULL) {
        int w, h;
        SDL_GetWindowSize(__l_graphics.sdl.window, &w, &h);
        SDL_Rect rect = {
            .x      = x,
            .y      = y,
            .w      = w - x,
            .h      = h - y,
        };
        SDL_BlitSurface(text_surface, NULL, __l_graphics.sdl.surface, &rect);
        SDL_FreeSurface(text_surface);
    }
    return 0;
}

int LOVE_MODULE_INIT(graphics)(lua_State *L)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    __l_graphics.sdl.window = SDL_CreateWindow("lovevsf", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dm.w, dm.h, SDL_WINDOW_SHOWN);
    VSF_ASSERT(__l_graphics.sdl.window != NULL);
    __l_graphics.sdl.surface = SDL_GetWindowSurface(__l_graphics.sdl.window);
    VSF_ASSERT(__l_graphics.sdl.surface != NULL);

    {
        int result;
        result = TTF_Init();
        if (result) {
            luaL_error(L, "Fail to initialize TTF\n");
        }

        int flags = IMG_INIT_PNG;
        result = IMG_Init(flags);
        if ((result & flags) != flags) {
            luaL_error(L, "Fail to initialize IMG\n");
        }
    }

    extern int __l_font_new(lua_State *L);
    extern int __l_image_new(lua_State *L);
    luaL_Reg lib_reg[] = {
        {"getDimensions",       __l_graphics_getDimensions      },
        {"getWidth",            __l_graphics_getWidth           },
        {"getHeight",           __l_graphics_getHeight          },
        {"getBackgroundColor",  __l_graphics_getBackgroundColor },
        {"setBackgroundColor",  __l_graphics_setBackgroundColor },
        {"getColor",            __l_graphics_getColor           },
        {"setColor",            __l_graphics_setColor           },
        {"print",               __l_graphics_print              },
        {"clear",               __l_graphics_clear              },
        {"present",             __l_graphics_present            },
        {"newFont",             __l_font_new                    },
        {"newImge",             __l_image_new                   },
        {0,                     0                               },
    };
    luaL_newlib(L, lib_reg);
    return 1;
}

#endif
