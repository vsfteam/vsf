#include "vsf.h"

#if VSF_EVM_USE_LUA == ENABLED

#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"
#include "luat_timer.h"

#include "ecma.h"
#include "evm_lua_conf.h"

static const luaL_Reg loadedlibs[] = {
    {"_G", luaopen_base},
//    {"rtos", luaopen_rtos},
//    {"log", luaopen_log},
//    {"wlan", luaopen_wlan},
//    {"socket", luaopen_socket},
//    {"timer", luaopen_timer},
//    {"gpio", luaopen_gpio},
//    {"pwm", luaopen_pwm},
//    {"uart", luaopen_uart},
//    {"http", luaopen_http},
    {NULL, NULL},
};

#ifndef WEAK_LUAT_OS_BSP
WEAK(luat_os_bsp)
const char * luat_os_bsp(void) {
    return "vsf";
}
#endif

void luat_os_reboot(int code) {
    
}

void luat_os_standy(int timeout) {
    luat_timer_mdelay(timeout);
}

void luat_openlibs(lua_State *L) {
    luat_msgbus_init();

    const luaL_Reg *lib;
    for (lib = loadedlibs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);
    }
}

LUA_API lua_State * lua_newstate(lua_Alloc f, void *args) {
    if (NULL == f) {
        f = luat_heap_alloc;
    }
    evm_lua_set_allocf(f, args);

    extern evm_t * evm_port_init(void);
    evm_t *env = evm_port_init();
    if (NULL == env) {
        return NULL;
    }

    evm_err_t err = ecma_module(env);
    if (err != ec_ok) {
        return NULL;
    }

    return evm_lua_new_state(env, EVM_LUA_SIZE_OF_GLOBALS);
}

// patch for un-implemented lua APIs
void lua_close (lua_State *L) {

}

#endif
