# CMakeLists head

target_sources(vsf INTERFACE
    vsf_netdrv.c
)

add_subdirectory(driver)
