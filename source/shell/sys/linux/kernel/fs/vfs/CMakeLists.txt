# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_linux_vfs.c
)

add_subdirectory(devfs)
