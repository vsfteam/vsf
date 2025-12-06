# CMakeLists head

vsf_add_sources(
    vsf_linux_core.c
    usb/vsf_linux_core_usb.c
    80211/vsf_linux_core_80211.c
    input/vsf_linux_core_input.c
    hid/vsf_linux_core_hid.c
    led/vsf_linux_core_led.c
    crypto/vsf_linux_core_crypto.c
)
