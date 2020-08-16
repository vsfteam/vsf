# CMakeLists head

target_sources(vsf INTERFACE
    vsf_service.c
)

add_subdirectory(dynarr)
add_subdirectory(dynstack)
add_subdirectory(heap)
add_subdirectory(json)
add_subdirectory(pbuf)
add_subdirectory(stream)
add_subdirectory(trace)
add_subdirectory(vsfstream)
