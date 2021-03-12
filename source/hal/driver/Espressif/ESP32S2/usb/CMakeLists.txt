# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    usb.c
    usb_common.c
    usb_lv0.c
    dc/usbd.c
    hc/usbh.c
)