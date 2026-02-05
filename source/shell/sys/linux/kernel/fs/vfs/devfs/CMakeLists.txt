# CMakeLists head

vsf_add_sources(
    vsf_linux_devfs.c
)

add_subdirectory(alsa)
add_subdirectory(usbd)
add_subdirectory(drm)
