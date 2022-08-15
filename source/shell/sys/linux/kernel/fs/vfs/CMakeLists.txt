# CMakeLists head

if(VSF_LINUX_TERMINFO_FILES_PATH)
    vsf_add_include_directories(
        ${VSF_LINUX_TERMINFO_FILES_PATH}
    )
else()
    vsf_add_include_directories(
        files/terminfo
    )
endif()

vsf_add_sources(
    vsf_linux_vfs.c
)

add_subdirectory(devfs)
