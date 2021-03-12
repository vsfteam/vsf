# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_disp.c
)

add_subdirectory(driver)
