# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    usb_lv0.c
    usb.c
    usb_common.c
)

add_subdirectory_existed(dc)
add_subdirectory_existed(ohci)
add_subdirectory_existed(hc)