if(NOT VSF_LITTLEFS_PATH)
    set(VSF_LITTLEFS_PATH ${VSF_SRC_PATH}/component/3rd-party/littlefs/raw)
endif()

vsf_add_include_directories(
    port
)

vsf_add_sources(
    port/lfs_port.c
)

vsf_add_include_directories(
    ${VSF_LITTLEFS_PATH}
)

vsf_add_sources(
    ${VSF_LITTLEFS_PATH}/lfs.c
    ${VSF_LITTLEFS_PATH}/lfs_util.c
)
