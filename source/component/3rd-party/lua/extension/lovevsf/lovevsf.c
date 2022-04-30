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

/* LoveVSF is inspired by LoveDOS(github.com/rxi/lovedos), and re-write
*   according to vsf coding standard. LoveVSF depends on linux and sdl2 shell
*   in vsf, which provide same APIs of linux and sdl2.
*/

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_LUA == ENABLED && VSF_LUA_USE_LOVE == ENABLED

#include "vsf.h"

#include "./lovevsf.h"
#include "lauxlib.h"

/*============================ MACROS ========================================*/

#define LOVE_VERSION                "0.2.1"
#define dcl_love_module(__name)     extern int LOVE_MODULE_INIT(__name)(lua_State *L);
#define def_love_module(__name)     {__VSF_STR(__name), LOVE_MODULE_INIT(__name)},

#define dcl_love_modules(...)       VSF_MFOREACH(dcl_love_module, __VA_ARGS__)
#define def_love_modules(...)       VSF_MFOREACH(def_love_module, __VA_ARGS__)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

dcl_love_modules(audio, event, filesystem, font, graphics, image, joystick, keyboard, mouse, system, timer, touch, video)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static int __l_love_getVersion(lua_State *L)
{
    lua_pushstring(L, LOVE_VERSION);
    return 1;
}

int luaopen_love(lua_State *L)
{
    luaL_Reg reg[] = {
        {"getVersion",      __l_love_getVersion },
        {0,                 0                   }
    };
    luaL_newlib(L, reg);

    struct {
        char *name;
        int (*fn)(lua_State *L);
    } modules[] = {
        def_love_modules(image, filesystem, timer, graphics, font)
    };
    for (int i = 0; i < dimof(modules); i++) {
        int ret = modules[i].fn(L);
        VSF_ASSERT(1 == ret);

        int type = lua_type(L, -1);
        switch (type) {
        case LUA_TFUNCTION:
            // class, returns constructor function
            lua_pop(L, 1);
            break;
        case LUA_TTABLE:
            // library, returns table
            lua_setfield(L, -2, modules[i].name);
            break;
        }
    }

    return 1;
}

#endif
