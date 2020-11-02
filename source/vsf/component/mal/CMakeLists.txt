# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_mal.c
)

add_subdirectory(driver)
