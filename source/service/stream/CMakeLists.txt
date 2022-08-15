# CMakeLists head

vsf_add_sources(
    vsf_stream.c
    vsf_stream_base.c
    vsf_stream_reader.c
    vsf_stream_writer.c
)

add_subdirectory(pbuf_queue)
