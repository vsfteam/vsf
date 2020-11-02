# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_shell.c
)

add_subdirectory(simple)
