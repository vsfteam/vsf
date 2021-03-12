# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_audio.c
)

add_subdirectory(decoder)
add_subdirectory(driver)
