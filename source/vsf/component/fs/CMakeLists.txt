# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_fs.c
)

add_subdirectory(driver)
