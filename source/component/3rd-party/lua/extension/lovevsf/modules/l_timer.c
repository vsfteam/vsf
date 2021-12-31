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
#include <time.h>
#include <sys/time.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __l_timer_t {
    clock_t                 last_clock;
    double                  last_delta;
    struct {
        double              accumulate;
        double              timer;
        double              last_delta;
        int                 count;
    }                       average;
} __l_timer_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __l_timer_t __l_timer = {
    .average.accumulate     = 1,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static int __l_timer_step(lua_State *L)
{
    clock_t now = clock();
    __l_timer.last_delta = (now - __l_timer.last_clock) / (double) CLOCKS_PER_SEC;
    __l_timer.last_clock = now;

    __l_timer.average.accumulate += __l_timer.last_delta;
    __l_timer.average.count++;
    __l_timer.average.timer -= __l_timer.last_delta;
    if (__l_timer.average.timer <= 0) {
        __l_timer.average.last_delta = __l_timer.average.accumulate / __l_timer.average.count;
        __l_timer.average.timer += 1;
        __l_timer.average.accumulate = 0;
        __l_timer.average.count = 0;
    }
    return 0;
}

static int __l_timer_sleep(lua_State *L)
{
    usleep((int)luaL_checknumber(L, 1) * 1000000);
    return 0;
}

static int __l_timer_getTime(lua_State *L)
{
    lua_pushnumber(L, clock() / (double) CLOCKS_PER_SEC);
    return 1;
}

static int __l_timer_getDelta(lua_State *L)
{
    lua_pushnumber(L, __l_timer.last_delta);
    return 1;
}

static int __l_timer_getAverageDelta(lua_State *L)
{
    lua_pushnumber(L, __l_timer.average.last_delta);
    return 1;
}

static int __l_timer_getFPS(lua_State *L)
{
    lua_pushnumber(L, (int)(1.0 / __l_timer.average.last_delta));
    return 1;
}

int LOVE_MODULE_INIT(timer)(lua_State *L)
{
    luaL_Reg lib_reg[] = {
        {"step",            __l_timer_step              },
        {"sleep",           __l_timer_sleep             },
        {"getDelta",        __l_timer_getDelta          },
        {"getAverageDelta", __l_timer_getAverageDelta   },
        {"getFPS",          __l_timer_getFPS            },
        {"getTime",         __l_timer_getTime           },
        {0,                 0                           },
    };
    luaL_newlib(L, lib_reg);
    return 1;
}

#endif
