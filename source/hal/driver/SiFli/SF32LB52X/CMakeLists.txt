add_subdirectory(common)
add_subdirectory_existed(${VSF_HAL_CHIP_NAME})

vsf_add_sources(
    ../vendor/drivers/cmsis/sf32lb52x/bf0_pin_const.c
)

vsf_add_include_directories(
    ../vendor/drivers/cmsis/sf32lb52x
)
