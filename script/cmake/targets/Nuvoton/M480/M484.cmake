set(VSF_HAL_CHIP_VENDOR     Nuvoton)
set(VSF_HAL_CHIP_SERIES     M480)
set(VSF_HAL_CHIP_NAME       M484)

set(VSF_TARGET_DEFINITIONS
    "__Nuvoton__"
    "__M484__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m4.cmake)
