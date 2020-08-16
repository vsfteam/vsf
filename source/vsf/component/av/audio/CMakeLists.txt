# CMakeLists head

target_sources(vsf INTERFACE
    vsf_audio.c
)

add_subdirectory(decoder)
add_subdirectory(driver)
