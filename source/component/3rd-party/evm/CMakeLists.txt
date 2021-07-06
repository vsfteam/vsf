if(NOT VSF_EVM_PATH)
    set(VSF_EVM_PATH ${VSF_SRC_PATH}/component/3rd-party/evm/raw)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    port
    extension/oo
    ${VSF_EVM_PATH}/include
)

target_sources(${VSF_LIB_NAME} INTERFACE
    extension/oo/evm_oo.c
    extension/usbh/evm_module_usbh.c

    port/evm_port.c
    port/iotjs/evm_main.c
)

if(VSF_EVM_USE_LUA)
    target_compile_definitions(${VSF_LIB_NAME} INTERFACE
#       needed by webclient
        __linux__
    )

    target_include_directories(${VSF_LIB_NAME} INTERFACE
        ${VSF_EVM_PATH}/modules/eluaos/include
        ${VSF_EVM_PATH}/components/webclient/inc
    )

    target_sources(${VSF_LIB_NAME} INTERFACE
        port/eluaos/luat_fs_vsf.c
        port/eluaos/luat_gpio_vsf.c
#        port/eluaos/luat_http_vsf.c
        port/eluaos/luat_log_vsf.c
        port/eluaos/luat_netclient_vsf.c
        port/eluaos/luat_malloc_vsf.c
        port/eluaos/luat_msgbus_vsf.c
        port/eluaos/luat_socket_vsf.c
        port/eluaos/luat_timer_vsf.c
        port/eluaos/luat_uart_vsf.c
        port/eluaos/luat_vsf_base.c

#        ${VSF_EVM_PATH}/components/webclient/src/webclient.c
#        ${VSF_EVM_PATH}/components/webclient/src/webclient_file.c
#        ${VSF_EVM_PATH}/components/webclient/src/webclient_http.c

        ${VSF_EVM_PATH}/modules/eluaos/lualib/bget.c
        ${VSF_EVM_PATH}/modules/eluaos/lualib/lbaselib.c
        ${VSF_EVM_PATH}/modules/eluaos/lualib/linit.c
        ${VSF_EVM_PATH}/modules/eluaos/lualib/rotable.c

        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_lib_gpio.c
#        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_lib_http.c
        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_lib_log.c
        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_lib_rtos.c
        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_lib_socket.c
        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_lib_timer.c
        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_lib_uart.c
        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_luat_bin.c
        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_main.c
        ${VSF_EVM_PATH}/modules/eluaos/modules/luat_malloc_weak.c
    )
endif()

if(${VSF_ARCH_SERIES} STREQUAL "x86")
    if(${VSF_HOST_SYSTEM} STREQUAL "linux")
        if(${VSF_HOST_SYSTEM32})
            set(VSF_EVM_LIB_PATH ${VSF_EVM_PATH}/lib/x86/linux32)
        elseif(${VSF_HOST_SYSTEM64})
            set(VSF_EVM_LIB_PATH ${VSF_EVM_PATH}/lib/x86/linux)
        endif()
    endif()
elseif(${VSF_ARCH_SERIES} STREQUAL "arm")
    if(${VSF_ARCH_NAME} STREQUAL "cortex-m3")
        set(VSF_EVM_LIB_PATH ${VSF_EVM_PATH}/lib/arm/cortex-m3)
    elseif(${VSF_ARCH_NAME} STREQUAL "cortex-m4")
        set(VSF_EVM_LIB_PATH ${VSF_EVM_PATH}/lib/arm/cortex-m4)
    endif()
endif()

if(VSF_EVM_LIB_PATH)
    file(GLOB VSF_EVM_LIBS ${VSF_EVM_LIB_PATH}/*)
    target_link_libraries(${VSF_LIB_NAME} INTERFACE
        ${VSF_EVM_LIBS}
    )
else()
    if(${VSF_ARCH_NAME})
        message(FATAL_ERROR "evm has no library for ${VSF_ARCH_SERIES}/${VSF_ARCH_NAME}")
    elseif(${VSF_HOST_SYSTEM})
        message(FATAL_ERROR "evm has no library for ${VSF_ARCH_SERIES}/${VSF_HOST_SYSTEM}")
    endif()
endif()