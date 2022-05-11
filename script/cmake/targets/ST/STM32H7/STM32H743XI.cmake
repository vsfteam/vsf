set(VSF_HAL_CHIP_VENDOR     ST)
set(VSF_HAL_CHIP_SERIES     STM32H7)
set(VSF_HAL_CHIP_NAME       STM32H743XI)
set(VSF_ARCH_SERIES         arm)
set(VSF_ARCH_NAME           cortex-m4)

set(VSF_TARGET_DEFINITIONS
    "__STMicro__"
    "__STM32H743XI__"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m7.cmake)