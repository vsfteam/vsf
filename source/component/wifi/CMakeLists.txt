# CMakeLists head

vsf_add_sources(
    vsf_wifi.c
    vsf_wifi_crypto.c
    vsf_wifi_wpa.c
    chip/rt28xx/vsf_wifi_rt28xx.c
    chip/rt28xx/vsf_wifi_rt2870_firmware.c
)
