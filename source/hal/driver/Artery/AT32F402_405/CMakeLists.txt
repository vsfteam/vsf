add_subdirectory(common)
add_subdirectory_existed(${VSF_HAL_CHIP_NAME})

vsf_add_include_directories(
    common
    common/vendor/libraries/cmsis/cm4/device_support
    common/vendor/libraries/drivers/inc
)