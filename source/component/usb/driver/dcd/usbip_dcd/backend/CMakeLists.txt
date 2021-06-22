# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_usbip_dcd_lwip.c
    vsf_usbip_dcd_win.c
)
