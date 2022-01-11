# CMakeLists head

if(VSF_HOST_SYSTEM)
    target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
        vsf_linfs.c
    )
endif()