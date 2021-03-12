# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_usbh.c
)

add_subdirectory(class)
