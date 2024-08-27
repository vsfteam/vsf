vsf_add_sources(
    driver.c
    vendor/Source/system_gd32h7xx.c
)

add_subdirectory_existed(io)
add_subdirectory_existed(gpio)
add_subdirectory_existed(usart)
add_subdirectory_existed(flash)
add_subdirectory_existed(usb)
add_subdirectory_existed(sdio)
add_subdirectory_existed(fb)
