if(VSF_HOST_HAS_WEBUSB)
    target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
        vsf_webusb_hcd.cc
    )
endif()
