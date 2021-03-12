# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_usbh_ds4.c
    vsf_usbh_HID.c
    vsf_usbh_nspro.c
    vsf_usbh_xb360.c
)
