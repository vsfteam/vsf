# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_hal.c
)

add_subdirectory(arch)
add_subdirectory(driver)
