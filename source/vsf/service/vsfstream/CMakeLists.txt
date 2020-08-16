# CMakeLists head

target_sources(vsf INTERFACE
    vsfstream.c
)

add_subdirectory(block_stream)
add_subdirectory(fifo_stream)
add_subdirectory(mem_stream)
