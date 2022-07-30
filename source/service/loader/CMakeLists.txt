# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_loader.c
)

add_subdirectory(elfloader)
