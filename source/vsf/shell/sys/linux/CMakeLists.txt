# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_linux.c
    vsf_linux_socket.c
)

add_subdirectory(port)
add_subdirectory(lib)
