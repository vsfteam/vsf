# if lwip from vendor, do not use liwp in 3rd-party
if(NOT VSF_LWIP_VENDOR)
    if(NOT VSF_LWIP_PATH)
        set(VSF_LWIP_PATH ${VSF_SRC_PATH}/component/3rd-party/lwip/raw)
    endif()
endif()