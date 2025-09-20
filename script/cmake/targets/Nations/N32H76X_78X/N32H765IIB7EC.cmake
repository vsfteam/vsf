set(VSF_HAL_CHIP_VENDOR     Nations)
set(VSF_HAL_CHIP_SERIES     N32H76X_78X)
set(VSF_HAL_CHIP_NAME       N32H765IIB7EC)

set(VSF_TARGET_DEFINITIONS
    "__Nations__"
    "__N32H765IIB7EC__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m7.cmake)