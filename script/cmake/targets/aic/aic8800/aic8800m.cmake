set(VSF_HAL_CHIP_VENDOR     AIC)
set(VSF_HAL_CHIP_SERIES     AIC8800)
set(VSF_HAL_CHIP_NAME       AIC8800M)
set(VSF_ARCH_SERIES         arm)
set(VSF_ARCH_NAME           cortex-m4)

set(VSF_TARGET_DEFINITIONS
    "__AIC__"
    "__AIC8800__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m4.cmake)
