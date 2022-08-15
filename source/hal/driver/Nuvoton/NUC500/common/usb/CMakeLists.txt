# CMakeLists head

vsf_add_sources(
    usb_lv0.c
    usb.c
)

add_subdirectory_existed(dc)
add_subdirectory_existed(ohci)
