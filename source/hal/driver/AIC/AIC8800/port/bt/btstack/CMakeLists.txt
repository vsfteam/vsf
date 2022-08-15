# CMakeLists head

if(VSF_USE_BTSTACK)
    vsf_add_sources(
        hci_transport_aic8800.c
    )
endif()
