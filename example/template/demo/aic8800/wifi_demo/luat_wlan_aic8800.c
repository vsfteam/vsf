#include "vsf.h"

#if VSF_USE_EVM == ENABLED && VSF_EVM_USE_LUA == ENABLED

#include "luat_base.h"
#include "luat_timer.h"
#include "luat_malloc.h"

#include "fhost_config.h"
#include "fhost.h"
#include "wlan_if.h"

#define WLAN_NONE       0
#define WLAN_STATION    1
#define WLAN_AP         2

extern void set_mac_address(uint8_t *addr);

typedef struct join_info {
    char ssid[64];
    char passwd[64];
} join_info_t;
static join_info_t jinfo = {0};

static int l_wlan_connect(lua_State *L) {
    size_t len;
    const char *passwd = NULL;
    const char *ssid = luaL_checklstring(L, 1, &len);
    strncpy(jinfo.ssid, ssid, len);
    jinfo.ssid[len] = '\0';
    if (lua_isstring(L, 2)) {
        passwd = luaL_checklstring(L, 2, &len);
        strncpy(jinfo.passwd, passwd, len);
        jinfo.passwd[len] = '\0';
    }

    set_mac_address(NULL);
    // wlan_start_sta MUST be called with higher priority than internal wpa(vsf_prio_0).
    vsf_prio_t prio = vsf_thread_set_priority(vsf_prio_1);
        wlan_start_sta((uint8_t *)jinfo.ssid, (uint8_t *)jinfo.passwd, 0);
    vsf_thread_set_priority(prio);

    return 0;
}

static int l_wlan_join_info(lua_State *L) {
    if (jinfo.ssid[0] != '\0') {
        lua_pushstring(L, jinfo.ssid);
        if (jinfo.passwd[0] != '\0') {
            lua_pushstring(L, jinfo.passwd);
            return 2;
        }
        return 1;
    }
    return 0;
}

static int l_wlan_disconnect(lua_State *L) {
    if (wlan_get_connect_status()) {
        wlan_disconnect_sta(0);
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

static int l_wlan_connected(lua_State *L) {
    lua_pushboolean(L, wlan_get_connect_status() ? 1 : 0);
    return 1;
}

static int l_wlan_ready(lua_State *L) {
    lua_pushboolean(L, 1);
    return 1;
}

static struct fhost_cntrl_link *scan_cntrl_link;
static int scan_result_num = 0;
static int l_wlan_scan(lua_State *L) {
    int fhost_vif_idx = 0;
    ipc_host_cntrl_start();

    scan_cntrl_link = fhost_cntrl_cfgrwnx_link_open();
    if (scan_cntrl_link == NULL) {
        return NULL;
    }

    if (fhost_set_vif_type(scan_cntrl_link, fhost_vif_idx, VIF_UNKNOWN, false) ||
        fhost_set_vif_type(scan_cntrl_link, fhost_vif_idx, VIF_STA, false)) {
        fhost_cntrl_cfgrwnx_link_close(scan_cntrl_link);
        return NULL;
    }

    scan_result_num = fhost_scan(scan_cntrl_link, fhost_vif_idx, NULL);

    lua_pushboolean(L, 1);
    return 1;
}

static int l_wlan_scan_get_result(lua_State *L) {
    int num = luaL_optinteger(L, 1, 20);
    num = vsf_min(num, scan_result_num);

    lua_createtable(L, num, 0);
    struct mac_scan_result result;
    for (int i = 0; i < num; i++) {
        fhost_get_scan_results(scan_cntrl_link, i, 1, &result);
        lua_pushinteger(L, i + 1);
        lua_createtable(L, 0, 0);
        // ssid
        lua_pushliteral(L, "ssid");
        lua_pushlstring(L, (const char *)result.ssid.array, strlen((const char *)result.ssid.array));
        lua_settable(L, -3);
        // rssi
        lua_pushliteral(L, "rssi");
        lua_pushinteger(L, result.rssi);
        lua_settable(L, -3);
        // channel
        lua_pushliteral(L, "channel");
        lua_pushinteger(L, phy_freq_to_channel(result.chan->band, result.chan->freq));
        lua_settable(L, -3);
        // band
        lua_pushliteral(L, "band");
        lua_pushinteger(L, result.chan->band);
        lua_settable(L, -3);
        // bssid
        lua_pushliteral(L, "bssid");
        lua_pushlstring(L, (const char *)&result.bssid.array, 6);
        lua_settable(L, -3);

        lua_settable(L, -3);
    }

    VSF_ASSERT(scan_cntrl_link != NULL);
    fhost_cntrl_cfgrwnx_link_close(scan_cntrl_link);
    scan_cntrl_link = NULL;
    return 1;
}

static int l_wlan_get_mac(lua_State *L) {
    uint8_t *mac = get_mac_address();
    if (mac[0] != 0x00) {
        char buff[13];
        sprintf(buff, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        lua_pushlstring(L, buff, 12);
        return 1;
    }
    return 0;
}

static int l_wlan_get_mac_raw(lua_State *L) {
    uint8_t *mac = get_mac_address();
    if (mac[0] != 0x00) {
        lua_pushlstring(L, (const char*)mac, 6);
        return 1;
    }
    return 0;
}

#include "rotable.h"
static const rotable_Reg reg_wlan[] = {
    { "connect",        l_wlan_connect, 0},
    { "disconnect",     l_wlan_disconnect, 0},
    { "connected",      l_wlan_connected, 0},
    { "ready",          l_wlan_ready, 0},
    { "scan",           l_wlan_scan, 0},
    { "scan_get_info",  l_wlan_scan_get_result, 0},
    { "scanResult",     l_wlan_scan_get_result, 0},
    { "getMac",         l_wlan_get_mac, 0},
    { "get_mac",        l_wlan_get_mac, 0},
    { "get_mac_raw",    l_wlan_get_mac_raw, 0},
    //{ "set_mac", l_wlan_set_mac},
    // ---- oneshot
    #if 0
    { "oneshotStart",   l_wlan_oneshot_start, 0},
    { "oneshotStop",    l_wlan_oneshot_stop,  0},
    { "oneshotState",   l_wlan_oneshot_state, 0},
    #endif
    { "lastInfo",       l_wlan_join_info, 0},
    //{ "airkiss_start",  l_wlan_airkiss_start, 0},
    // ---

    { "NONE",           NULL, WLAN_NONE},
    { "STATION",        NULL, WLAN_STATION},
    { "AP",             NULL, WLAN_AP},
	{ NULL,             NULL, 0}
};

LUAMOD_API int luaopen_wlan(lua_State *L) {
    luat_newlib(L, reg_wlan);
    return 1;
}

#endif      // VSF_USE_EVM && VSF_EVM_USE_LUA
