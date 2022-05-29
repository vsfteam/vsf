# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    usb.c
    usb_common.c

    dc/usbd.c
    hc/usbh.c
)
