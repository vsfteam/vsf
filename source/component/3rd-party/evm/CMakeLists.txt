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

target_link_libraries(${VSF_LIB_NAME} INTERFACE
    ${VSF_EVM_PATH}/lib/arm/cortex-m4/libecma.a
    ${VSF_EVM_PATH}/lib/arm/cortex-m4/libejs.a
)
