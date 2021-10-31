# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_linux_fs.c
)

add_subdirectory(devfs)
