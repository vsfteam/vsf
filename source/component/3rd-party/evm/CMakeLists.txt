if(NOT VSF_EVM_PATH)
    set(VSF_EVM_PATH ${VSF_SRC_PATH}/component/3rd-party/evm/raw)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    port
    extension/oo
    ${VSF_EVM_PATH}/include
)
#target_compile_definitions(${VSF_LIB_NAME} INTERFACE
#)

target_sources(${VSF_LIB_NAME} INTERFACE
    extension/oo/evm_oo.c
    extension/usbh/evm_module_usbh.c

    port/evm_main.c
)

if(${VSF_ARCH_SERIES} STREQUAL "x86")
    if(${VSF_HOST_SYSTEM} STREQUAL "linux")
        if(${VSF_HOST_SYSTEM32})
            target_link_libraries(${VSF_LIB_NAME} INTERFACE
                ${VSF_EVM_PATH}/lib/x86/linux32/libecma.a
                ${VSF_EVM_PATH}/lib/x86/linux32/libejs.a
            )
        elseif(${VSF_HOST_SYSTEM64})
            target_link_libraries(${VSF_LIB_NAME} INTERFACE
                ${VSF_EVM_PATH}/lib/x86/linux/libecma.a
                ${VSF_EVM_PATH}/lib/x86/linux/libejs.a
            )
        endif()
    else()
        message(FATAL_ERROR "evm has no library for windows")
    endif()
elseif(${VSF_ARCH_SERIES} STREQUAL "arm")
    if(${VSF_ARCH_NAME} STREQUAL "cortex-m3")
        target_link_libraries(${VSF_LIB_NAME} INTERFACE
            ${VSF_EVM_PATH}/lib/arm/cortex-m3/libecma.a
            ${VSF_EVM_PATH}/lib/arm/cortex-m3/libejs.a
        )
    elseif(${VSF_ARCH_NAME} STREQUAL "cortex-m4")
        target_link_libraries(${VSF_LIB_NAME} INTERFACE
            ${VSF_EVM_PATH}/lib/arm/cortex-m4/libecma.a
            ${VSF_EVM_PATH}/lib/arm/cortex-m4/libejs.a
        )
    else()
        message(FATAL_ERROR "evm has no library for ${VSF_ARCH_NAME}")
    endif()
endif()