# if lwip from vendor, do not use liwp in 3rd-party
if(NOT VSF_LWIP_VENDOR)
    if(NOT VSF_LWIP_PATH)
        set(VSF_LWIP_PATH ${VSF_SRC_PATH}/component/3rd-party/lwip/raw)
    endif()

    target_include_directories(${VSF_LIB_NAME} INTERFACE
        ${VSF_SRC_PATH}/component/3rd-party/lwip/port/vsf
    )
endif()

target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_SRC_PATH}/component/3rd-party/lwip/port/lwip_netdrv_adapter.c
)
