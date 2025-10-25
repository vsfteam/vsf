vsf_add_sources(
    driver.c
    startup_n32h76x_78x.c
)

add_subdirectory_existed(gpio)
add_subdirectory_existed(usart)
add_subdirectory_existed(flash)
add_subdirectory_existed(usb)
#add_subdirectory_existed(sdio)
#add_subdirectory_existed(i2c)
#add_subdirectory_existed(spi)