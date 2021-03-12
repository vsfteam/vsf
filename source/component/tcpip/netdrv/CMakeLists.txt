# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_netdrv.c
)

add_subdirectory(driver)
