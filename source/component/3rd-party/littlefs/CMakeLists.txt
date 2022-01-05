if(NOT VSF_LITTLEFS_PATH)
    set(VSF_LITTLEFS_PATH ${VSF_SRC_PATH}/component/3rd-party/littlefs/raw)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    port
)

target_sources(${VSF_LIB_NAME} INTERFACE
    port/lfs_port.c
)

target_include_directories(${VSF_LIB_NAME} INTERFACE
    ${VSF_LITTLEFS_PATH}
)

target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_LITTLEFS_PATH}/lfs.c
    ${VSF_LITTLEFS_PATH}/lfs_util.c
)
