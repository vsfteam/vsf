# CMakeLists head

vsf_add_sources(
    driver.c
    startup_RP2040.c
)

add_subdirectory(i2c)
add_subdirectory(uart)
add_subdirectory(gpio)
add_subdirectory(rtc)
add_subdirectory(flash)
add_subdirectory(wdt)
add_subdirectory(adc)
add_subdirectory(pwm)
add_subdirectory(timer)
add_subdirectory(spi)
add_subdirectory(rng)
add_subdirectory(dma)

# include boot_stage2
if(NOT TARGET bs2_default_library)
    add_subdirectory(${PICO_SDK_PATH}/src/rp2040/boot_stage2 ${CMAKE_CURRENT_BINARY_DIR}/boot_stage2)
endif()
# Workaround: pico-sdk 2.x boot_stage2 CMake adds
#   target_link_libraries(bs2_default_library INTERFACE "$<TARGET_OBJECTS:bs2_default_library>")
# which causes the .o to appear twice on the link line with CMake 4.x
# (once from OBJECT-library auto-propagation, once from the genex).
set_target_properties(bs2_default_library PROPERTIES INTERFACE_LINK_LIBRARIES "boot_stage2_headers")
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    bs2_default_library
)
