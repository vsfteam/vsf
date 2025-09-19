set(VSF_HAL_CHIP_VENDOR     MyVendor)
set(VSF_HAL_CHIP_SERIES     MySeries)
set(VSF_HAL_CHIP_NAME       MYCHIP)

set(VSF_TARGET_DEFINITIONS
    "__MYVENDOR__"
    "__MYSERIES__"
    "__MYDEVICE__"

    ${VSF_TARGET_DEFINITIONS}
)

# 使用通用的ARM Cortex-M4配置作为基础
include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m4.cmake)

set(VSF_TARGET_PATH ${VSF_SRC_PATH}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${VSF_TARGET_PATH}
    ${VSF_TARGET_PATH}/common
    ${VSF_TARGET_PATH}/MyDevice
)
