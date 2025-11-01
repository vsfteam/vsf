vsf_add_sources(
    driver.c
    startup_AT32F402_405.c
)

add_subdirectory_existed(flash)
add_subdirectory_existed(gpio)
add_subdirectory_existed(i2c)
add_subdirectory_existed(spi)
add_subdirectory_existed(usart)
add_subdirectory_existed(usb)
