# CMakeLists head

if(VSF_USE_LINUX)
    target_include_directories(${VSF_TARGET_NAME} BEFORE PUBLIC
        include
        include/libusb
    )
    if(VSF_LINUX_USE_SIMPLE_LIBC)
        target_include_directories(${VSF_TARGET_NAME} PUBLIC
            include/simple_libc
        )
    endif()

    vsf_add_sources(
        vsf_linux.c
    )

    add_subdirectory(port)
    add_subdirectory(lib)
    add_subdirectory(app)
    add_subdirectory(kernel)
endif()
