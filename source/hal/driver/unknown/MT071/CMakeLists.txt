# CMakeLists head

vsf_add_sources(
    driver.c
    startup_mt071.c
)

add_subdirectory_existed(usb)
add_subdirectory_existed(usart)
