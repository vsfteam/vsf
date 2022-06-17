# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vendor/system_stm32H7xx.c

    __common.c
    startup_STM32H7.c
)

add_subdirectory_existed(io)
add_subdirectory_existed(pm)
add_subdirectory_existed(usart)
add_subdirectory_existed(usb)
add_subdirectory_existed(iic)
add_subdirectory_existed(spi)
