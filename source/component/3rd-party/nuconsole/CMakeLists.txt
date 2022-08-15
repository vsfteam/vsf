if(NOT VSF_NUCONSOLE_PATH)
    set(VSF_NUCONSOLE_PATH ${VSF_SRC_PATH}/component/3rd-party/nuconsole/raw)
endif()

vsf_add_include_directories(
    ${VSF_NUCONSOLE_PATH}
)

vsf_add_sources(
    ${VSF_NUCONSOLE_PATH}/NuConsole.c
)
