# CMakeLists head

target_sources(vsf INTERFACE
    vsf_osa_service.c
)

add_subdirectory(msg_tree)
