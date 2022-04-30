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

#ifndef __LOVEVSF__
#define __LOVEVSF__

/*============================ INCLUDES ======================================*/

#include "lua.h"
#include "lauxlib.h"

#include "lua_oo.h"

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

/*============================ MACROS ========================================*/

#define LOVE_MODULE_INIT(__name)    VSF_MCONNECT2(luaopen_love_, __name)
#define LOVE_CLASS(__name)          "love_class_" __VSF_STR(__name)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct love_font_t {
    TTF_Font *ttf_font;
} love_font_t;

typedef struct love_image_t {
    int i;
} love_image_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int luaopen_love(lua_State *L);

#endif
