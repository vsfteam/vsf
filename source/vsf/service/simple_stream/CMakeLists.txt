# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_simple_stream.c
)

add_subdirectory(block_stream)
add_subdirectory(fifo_stream)
add_subdirectory(mem_stream)
