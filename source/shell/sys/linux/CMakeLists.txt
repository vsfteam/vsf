# CMakeLists head

if(VSF_USE_LINUX)
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        include
    )
    if(VSF_LINUX_USE_SIMPLE_LIBC)
        target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
            include/simple_libc
        )
    endif()

    target_sources(${VSF_LIB_NAME} INTERFACE
        vsf_linux.c
        vsf_linux_socket.c
        vsf_linux_devfs.c
    )

    add_subdirectory(port)
    add_subdirectory(lib)
endif()
