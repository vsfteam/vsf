# CMakeLists head

vsf_add_sources(
    driver.c
    startup_RP2040.c
)

add_subdirectory(i2c)

vsf_add_include_directories(
    vendor/generated/pico_base
)

# include boot_stage2
add_subdirectory(${PICO_SDK_PATH}/src/rp2_common/boot_stage2 ${CMAKE_CURRENT_BINARY_DIR}/boot_stage2)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    bs2_default_library
)
