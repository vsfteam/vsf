set(VSF_HAL_CHIP_VENDOR     RaspberryPi)
set(VSF_HAL_CHIP_SERIES     RP2040)
set(VSF_HAL_CHIP_NAME       RP2040)

set(VSF_TARGET_DEFINITIONS
    "__RaspberryPi__"
    "__RP2040__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m0plus.cmake)

list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${PICO_SDK_PATH}/src/rp2_common/cmsis/stub/CMSIS/Device/RaspberryPi/RP2040/Include
)
