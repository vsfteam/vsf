# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    driver.c
    startup_mt071.c
)

add_subdirectory_existed(usb)
add_subdirectory_existed(usart)
