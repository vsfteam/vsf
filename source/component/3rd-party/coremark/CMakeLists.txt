if(NOT VSF_COREMARK_PATH)
    set(VSF_COREMARK_PATH ${VSF_SRC_PATH}/component/3rd-party/COREMARK/raw)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    port
    ${VSF_COREMARK_PATH}
)
#target_compile_definitions(${VSF_LIB_NAME} INTERFACE
#)

target_sources(${VSF_LIB_NAME} INTERFACE
    port/core_portme.c

    ${VSF_COREMARK_PATH}/core_list_join.c
    ${VSF_COREMARK_PATH}/core_main.c
    ${VSF_COREMARK_PATH}/core_matrix.c
    ${VSF_COREMARK_PATH}/core_state.c
    ${VSF_COREMARK_PATH}/core_util.c
)
