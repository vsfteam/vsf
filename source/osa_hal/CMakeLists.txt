# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_osa_hal.c
)

add_subdirectory(driver)
#add_subdirectory(sw_peripheral)
