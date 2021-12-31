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

int __l_image_new(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
//    love_image_t *image = luaO_newinst(L, LOVE_CLASS(image));

    // TODO: setup image with filename
    printf("TODO: setup image %s\n", filename);
    return 1;
}

static int __l_image_delete(lua_State *L)
{
    // TODO:
    return 0;
}

int LOVE_MODULE_INIT(image)(lua_State *L)
{
    luaL_Reg oo_reg[] = {
        {luaO_field_new,    __l_image_new       },
        {luaO_field_delete, __l_image_delete    },
        {0,                 0                   },
    };
    luaO_class_t oo_class = {
        .name   = LOVE_CLASS(image),
        .parent = NULL,
        .size   = sizeof(love_image_t),
        .reg    = oo_reg,
    };
    return luaO_newclass(L, &oo_class);
}

#endif
