# CMakeLists head

target_sources(vsf INTERFACE
    vsf_hal.c
)

add_subdirectory(arch)
add_subdirectory(driver)
