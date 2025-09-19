set(VSF_HAL_CHIP_VENDOR     FAKE_VENDOR)
set(VSF_HAL_CHIP_SERIES     FAKE_SERIES)
set(VSF_HAL_CHIP_NAME       fake_chip)

set(VSF_TARGET_DEFINITIONS
    "__FAKE_VENDOR__"
    "__FAKE_SERIES__"
    "__FAKE_DEVICE__"

    ${VSF_TARGET_DEFINITIONS}
)

# Use the generic ARM Cortex-M4 configuration as the base
include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m4.cmake)

set(VSF_TARGET_PATH ${VSF_SRC_PATH}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${VSF_TARGET_PATH}
    ${VSF_TARGET_PATH}/common
    ${VSF_TARGET_PATH}/FAKE_DEVICE
)
