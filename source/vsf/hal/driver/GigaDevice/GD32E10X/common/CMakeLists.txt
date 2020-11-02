# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    __common.c
)

add_subdirectory_existed(io)
add_subdirectory_existed(pm)
add_subdirectory_existed(usart)
add_subdirectory_existed(usb)
add_subdirectory_existed(vendor)
