if(NOT VSF_NUCONSOLE_PATH)
    set(VSF_NUCONSOLE_PATH ${VSF_SRC_PATH}/component/3rd-party/nuconsole/raw)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    ${VSF_NUCONSOLE_PATH}
)

target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_NUCONSOLE_PATH}/NuConsole.c
)
