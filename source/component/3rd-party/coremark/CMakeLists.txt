if(NOT VSF_COREMARK_PATH)
    set(VSF_COREMARK_PATH ${VSF_SRC_PATH}/component/3rd-party/coremark/raw)
endif()

vsf_add_include_directories(
    port
    ${VSF_COREMARK_PATH}
)

if(${VSF_ARCH_SERIES} STREQUAL "x86")
    if (${VSF_HOST_SYSTEM} STREQUAL "linux")
#       seems bug in printf, remove this after fixed
        vsf_add_compile_definitions(
            HAS_FLOAT=0
        )
    endif()
endif()

vsf_add_sources(
    port/core_portme.c

    ${VSF_COREMARK_PATH}/core_list_join.c
    ${VSF_COREMARK_PATH}/core_main.c
    ${VSF_COREMARK_PATH}/core_matrix.c
    ${VSF_COREMARK_PATH}/core_state.c
    ${VSF_COREMARK_PATH}/core_util.c
)
