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

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// buffer returned is allocated from heap, should be freed by user
static int __l_filesystem_read_file(lua_State *L, const char *filename, void ** ptr)
{
    FILE *f = fopen(filename, "");
    if (NULL == f) {
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    char *buffer = malloc(size);
    if (NULL == buffer) {
        return -1;
    }

    fseek(f, 0, SEEK_SET);
    if (size != fread(buffer, 1, size, f)) {
        free(buffer);
        fclose(f);
        return -1;
    }
    fclose(f);

    *ptr = buffer;
    return size;
}

static int __l_filesystem_do_nothing(lua_State *L)
{
    return 0;
}

static int __l_filesystem_exists(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    struct stat stat_buf;
    int err = stat(filename, &stat_buf);
    lua_pushboolean(L, !err);
    return 1;
}

static int __l_filesystem_isFile(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    struct stat stat_buf;
    int err = stat(filename, &stat_buf);
    lua_pushboolean(L, !err && S_ISREG(stat_buf.st_mode));
    return 1;
}

static int __l_filesystem_read(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    void *buffer;
    int size = __l_filesystem_read_file(L, filename, &buffer);
    if (size <= 0) {
        luaL_error(L, "cound not read file %s", filename);
    }
    lua_pushlstring(L, buffer, size);
    free(buffer);
    return 1;
}

static int __l_filesystem_isDirectory(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    struct stat stat_buf;
    int err = stat(filename, &stat_buf);
    lua_pushboolean(L, !err && S_ISDIR(stat_buf.st_mode));
    return 1;
}

static int __l_filesystem_getWorkingDirectory(lua_State *L)
{
    char path[MAX_PATH];
    getcwd(path, sizeof(path));
    lua_pushstring(L, path);
    return 1;
}

static int __l_filesystem_getDirectoryItems(lua_State *L)
{
    DIR *dir = opendir(luaL_checkstring(L, 1));
    struct dirent *entry;
    int childnum = 0;

    lua_createtable(L, 0, 0);
    do {
        entry = readdir(dir);
        if (entry != NULL) {
            lua_pushstring(L, entry->d_name);
            lua_rawseti(L, -2, childnum + 1);
            childnum++;
        }
    } while (entry != NULL);
    return 1;
}

static int __l_filesystem_load(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    void *buffer;
    int size = __l_filesystem_read_file(L, filename, &buffer);
    if (size <= 0) {
        luaL_error(L, "cound not read file %s", filename);
    }

    int status = luaL_loadbuffer(L, buffer, size, filename);
    free(buffer);
    switch (status) {
    case LUA_ERRMEM:
        return luaL_error(L, "%s: Memory allocation error: %s\n", __FUNCTION__, lua_tostring(L, -1));
	case LUA_ERRSYNTAX:
        return luaL_error(L, "%s: Syntax error: %s\n", __FUNCTION__, lua_tostring(L, -1));
	default:
        return 1;
	}
}

int LOVE_MODULE_INIT(filesystem)(lua_State *L)
{
    luaL_Reg lib_reg[] = {
        {"init",                __l_filesystem_do_nothing           },
        {"exists",              __l_filesystem_exists               },
        {"isFile",              __l_filesystem_isFile               },
        {"isDirectory",         __l_filesystem_isDirectory          },
        {"read",                __l_filesystem_read                 },
        {"getWorkingDirectory", __l_filesystem_getWorkingDirectory  },
        {"getDirectoryItems",   __l_filesystem_getDirectoryItems    },
        {"load",                __l_filesystem_load                 },
        {0,                     0                                   },
    };
    luaL_newlib(L, lib_reg);
    return 1;
}

#endif
