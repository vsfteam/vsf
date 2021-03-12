# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    busybox.c
)

add_subdirectory(shell)
