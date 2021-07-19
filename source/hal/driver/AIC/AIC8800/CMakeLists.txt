# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    driver.c
    startup_AIC8800.c
)

add_subdirectory(debug_uart)
add_subdirectory(port)
add_subdirectory(spi)
add_subdirectory(uart)
add_subdirectory(usb)
add_subdirectory(gpio)
add_subdirectory(adc)

include(vendor.cmake)
