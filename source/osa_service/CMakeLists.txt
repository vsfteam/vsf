# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_osa_service.c
)

add_subdirectory(msg_tree)
