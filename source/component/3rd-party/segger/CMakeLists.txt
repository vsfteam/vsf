if(NOT VSF_SEGGER_RTT_PATH)
    set(VSF_SEGGER_RTT_PATH ${VSF_SRC_PATH}/component/3rd-party/segger/raw/RTT)
endif()
if(NOT VSF_SEGGER_SYSTEMVIEW_PATH)
    set(VSF_SEGGER_SYSTEMVIEW_PATH ${VSF_SRC_PATH}/component/3rd-party/segger/raw/SystemView)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    ${VSF_SEGGER_RTT_PATH}
    ${VSF_SEGGER_SYSTEMVIEW_PATH}
    ${VSF_SRC_PATH}/component/3rd-party/segger/port/Config
)

target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_SEGGER_RTT_PATH}/SEGGER_RTT.c
)
