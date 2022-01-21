# CMakeLists head

if(VSF_LINUX_TERMINFO_FILES_PATH)
    target_include_directories(${VSF_LIB_NAME} INTERFACE
        ${VSF_LINUX_TERMINFO_FILES_PATH}
    )
else()
    target_include_directories(${VSF_LIB_NAME} INTERFACE
        files/terminfo
    )
endif()

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_linux_vfs.c
)

add_subdirectory(devfs)
