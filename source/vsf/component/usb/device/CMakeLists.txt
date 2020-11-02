# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_usbd.c
)

add_subdirectory(class)
