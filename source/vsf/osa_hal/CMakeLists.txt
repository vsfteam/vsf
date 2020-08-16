# CMakeLists head

target_sources(vsf INTERFACE
    vsf_osa_hal.c
)

add_subdirectory(driver)
#add_subdirectory(sw_peripheral)
