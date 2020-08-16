# CMakeLists head

target_sources(vsf INTERFACE
    busybox.c
)

add_subdirectory(shell)
