# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_socket.c
)

add_subdirectory(driver)
