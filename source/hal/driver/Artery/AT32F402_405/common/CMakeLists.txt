vsf_add_sources(
    driver.c
    startup_AT32F405.c
    interrupt_swi.c
)

add_subdirectory_existed(usb)
