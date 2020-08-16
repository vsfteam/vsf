# CMakeLists head

target_sources(vsf INTERFACE
    vsf_stream.c
    vsf_stream_base.c
    vsf_stream_reader.c
    vsf_stream_writer.c
)

add_subdirectory(pbuf_queue)
