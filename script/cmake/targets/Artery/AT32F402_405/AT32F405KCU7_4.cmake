set(VSF_HAL_CHIP_VENDOR     Artery)
set(VSF_HAL_CHIP_SERIES     AT32F402_405)
set(VSF_HAL_CHIP_NAME       AT32F405KCU7_4)

set(VSF_TARGET_DEFINITIONS
    "__Artery__"
    "__AT32F405KCU7_4__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m4.cmake)