# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_linux_socket.c
)

add_subdirectory(inet)
add_subdirectory(unix)
