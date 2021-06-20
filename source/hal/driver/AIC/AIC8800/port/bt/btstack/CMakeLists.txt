# CMakeLists head

if(VSF_USE_BTSTACK)
    target_sources(${VSF_LIB_NAME} INTERFACE
        hci_transport_aic8800.c
    )
endif()
