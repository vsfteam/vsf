# CMakeLists head

target_sources(vsf INTERFACE
    segger_rtt_stream.c
)

add_subdirectory(RTT)
