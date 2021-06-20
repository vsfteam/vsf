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
        vsf_linux_devfs.c
    )
    if(VSF_USE_LWIP)
        target_sources(${VSF_LIB_NAME} INTERFACE
            vsf_linux_socket.c
        )
        target_compile_definitions(${VSF_LIB_NAME} INTERFACE
            "LWIP_SO_RCVBUF=1"
        )
    endif()

    add_subdirectory(port)
    add_subdirectory(lib)
endif()
