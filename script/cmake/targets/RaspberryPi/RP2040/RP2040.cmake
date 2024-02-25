set(VSF_HAL_CHIP_VENDOR     RaspberryPi)
set(VSF_HAL_CHIP_SERIES     RP2040)
set(VSF_HAL_CHIP_NAME       RP2040)

set(VSF_TARGET_DEFINITIONS
    "__RaspberryPi__"
    "__RP2040__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m0.cmake)

set(VSF_TARGET_PATH ${VSF_SRC_PATH}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
