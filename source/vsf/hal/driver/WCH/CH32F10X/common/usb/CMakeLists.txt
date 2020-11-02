# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    usb_lv0.c
    usb.c
)

add_subdirectory_existed(usbd)
