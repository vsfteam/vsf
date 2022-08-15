# CMakeLists head

vsf_add_sources(
    __common.c
)

add_subdirectory(debug_uart)
add_subdirectory(dram)
add_subdirectory(fb)
add_subdirectory(intc)
add_subdirectory(rtp)
add_subdirectory(uart)
add_subdirectory(usb)
