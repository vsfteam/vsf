if(NOT VSF_QRCODE_PATH)
    set(VSF_QRCODE_PATH ${VSF_SRC_PATH}/component/3rd-party/qrcode/raw)
endif()

vsf_add_include_directories(
    ${VSF_QRCODE_PATH}/src
)

vsf_add_sources(
    ${VSF_QRCODE_PATH}/src/qrcode.c
)
