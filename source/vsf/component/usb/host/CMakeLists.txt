# CMakeLists head

target_sources(vsf INTERFACE
    vsf_usbh.c
)

add_subdirectory(class)
