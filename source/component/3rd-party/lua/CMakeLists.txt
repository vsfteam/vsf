target_include_directories(${VSF_LIB_NAME} INTERFACE
    extension/oo
    extension/lovevsf
)
#target_compile_definitions(${VSF_LIB_NAME} INTERFACE
#)

target_sources(${VSF_LIB_NAME} INTERFACE
    extension/lovevsf/modules/l_filesystem.c
    extension/lovevsf/modules/l_font.c
    extension/lovevsf/modules/l_graphics.c
    extension/lovevsf/modules/l_image.c
    extension/lovevsf/modules/l_timer.c
    extension/lovevsf/lovevsf.c

    extension/oo/lua_oo.c

    port/lua_port.c
)

if(NOT (VSF_USE_EVM AND VSF_EVM_USE_LUA))
    if(NOT VSF_LUA_PATH)
        set(VSF_LUA_PATH ${VSF_SRC_PATH}/component/3rd-party/lua/raw)
    endif()

    target_include_directories(${VSF_LIB_NAME} INTERFACE
        ${VSF_LUA_PATH}
    )

    target_sources(${VSF_LIB_NAME} INTERFACE
        ${VSF_LUA_PATH}/lapi.c
        ${VSF_LUA_PATH}/lauxlib.c
        ${VSF_LUA_PATH}/lbaselib.c
        ${VSF_LUA_PATH}/lcode.c
        ${VSF_LUA_PATH}/lcorolib.c
        ${VSF_LUA_PATH}/lctype.c
        ${VSF_LUA_PATH}/ldblib.c
        ${VSF_LUA_PATH}/ldebug.c
        ${VSF_LUA_PATH}/ldo.c
        ${VSF_LUA_PATH}/ldump.c
        ${VSF_LUA_PATH}/lfunc.c
        ${VSF_LUA_PATH}/lgc.c
        ${VSF_LUA_PATH}/linit.c
        ${VSF_LUA_PATH}/liolib.c
        ${VSF_LUA_PATH}/llex.c
        ${VSF_LUA_PATH}/lmathlib.c
        ${VSF_LUA_PATH}/lmem.c
        ${VSF_LUA_PATH}/loadlib.c
        ${VSF_LUA_PATH}/lobject.c
        ${VSF_LUA_PATH}/lopcodes.c
        ${VSF_LUA_PATH}/loslib.c
        ${VSF_LUA_PATH}/lparser.c
        ${VSF_LUA_PATH}/lstate.c
        ${VSF_LUA_PATH}/lstring.c
        ${VSF_LUA_PATH}/lstrlib.c
        ${VSF_LUA_PATH}/ltable.c
        ${VSF_LUA_PATH}/ltablib.c
        ${VSF_LUA_PATH}/ltests.c
        ${VSF_LUA_PATH}/ltm.c
        ${VSF_LUA_PATH}/lundump.c
        ${VSF_LUA_PATH}/lutf8lib.c
        ${VSF_LUA_PATH}/lvm.c
        ${VSF_LUA_PATH}/lzio.c
    )

    add_library(lua_main STATIC
        ${VSF_LUA_PATH}/lua.c
    )
    target_link_libraries(${VSF_LIB_NAME} INTERFACE
        lua_main
    )

    # add vsf configuration for lua_main
    target_compile_definitions(lua_main PRIVATE
        ${VSF_TARGET_DEFINITIONS}
        "main=lua_main"
    )
    target_include_directories(lua_main PRIVATE
        ${VSF_COMMON_INCLUDE_DIRECTORIES}
    )
endif()
