# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_service.c
)

add_subdirectory(dynarr)
add_subdirectory(dynstack)
add_subdirectory(fifo)
add_subdirectory(heap)
add_subdirectory(json)
add_subdirectory(pbuf)
add_subdirectory(pool)
add_subdirectory(stream)
add_subdirectory(trace)
add_subdirectory(simple_stream)
