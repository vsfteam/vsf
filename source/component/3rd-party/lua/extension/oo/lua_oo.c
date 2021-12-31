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

#if VSF_USE_LUA == ENABLED

#include "./lua_oo.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int luaO_newclass(lua_State *L, luaO_class_t *c)
{
    // metatable.__type = c->name;
    luaL_newmetatable(L, c->name);
    lua_pushstring(L, "__type");
    lua_pushstring(L, c->name);
    lua_rawset(L, -3);

    // metatable.__instsize = c->size;
    lua_pushstring(L, luaO_field_instsize);
    lua_pushinteger(L, (lua_Integer)c->size);
    lua_rawset(L, -3);

    // index_table inherit from parent
    lua_newtable(L);
    if (c->parent != NULL) {
        luaL_getmetatable(L, c->parent);
        lua_setmetatable(L, -2);
    }
    luaL_setfuncs(L, c->reg, 0);

    // metatable.__index = index_table;
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -4);

    // push constructor, which is luaO_field_new in __index
    lua_getfield(L, -1, luaO_field_new);
    VSF_ASSERT(!lua_isnoneornil(L, -1));

    // remove __index table and metatable
    lua_remove(L, -2);
    lua_remove(L, -2);
    return 1;
}

void * luaO_newinst(lua_State *L, char *c)
{
    luaL_getmetatable(L, c);
    if (lua_isnoneornil(L, -1)) {
        luaL_error(L, "%s: invalid class %d\n", __FUNCTION__, c);
    }

    lua_getfield(L, -1, luaO_field_instsize);
    lua_Integer size = lua_tointeger(L, -1);
    lua_pop(L, 1);

    void * udata = lua_newuserdata(L, size);
    lua_pushvalue(L, -2);
    lua_setmetatable(L, -2);

    lua_remove(L, -2);
    return udata;
}

void * luaO_checkinst(lua_State *L, int idx, char *c)
{
    void *udata = lua_touserdata(L, idx);

    lua_getmetatable(L, idx);
    lua_getfield(L, -1, "__type");
    const char *udata_type = luaL_checkstring(L, -1);
    lua_pop(L, 2);

    if (strcmp(udata_type, c)) {
        luaL_argerror(L, idx, "bad type");
    }
    return udata;
}

#endif
