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

    port/evm_port.c
    port/iotjs/evm_main.c
    port/iotjs/modules/evm_module_hal.c
)

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