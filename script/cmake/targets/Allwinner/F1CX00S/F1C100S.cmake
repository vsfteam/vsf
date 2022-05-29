set(VSF_HAL_CHIP_VENDOR     Allwinner)
set(VSF_HAL_CHIP_SERIES     F1CX00S)
set(VSF_HAL_CHIP_NAME       F1C100S)

set(VSF_TARGET_DEFINITIONS
    "__Allwinner__"
    "__F1C100S__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__arm9.cmake)
