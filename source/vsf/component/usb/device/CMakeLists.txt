# CMakeLists head

target_sources(vsf INTERFACE
    vsf_usbd.c
)

add_subdirectory(class)
