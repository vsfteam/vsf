# CMakeLists head

vsf_add_sources(
    vsf_audio.c
)

add_subdirectory(decoder)
add_subdirectory(driver)
