set(VSF_HAL_CHIP_VENDOR     Allwinner)
set(VSF_HAL_CHIP_SERIES     V821)
set(VSF_HAL_CHIP_NAME       V821)
set(VSF_ARCH_SERIES         rv)

set(VSF_TARGET_DEFINITIONS
    "__Allwinner__"
    "__V821__"
    "__CPU_RISCV__"

    ${VSF_TARGET_DEFINITIONS}
)

set(CMAKE_C_FLAGS
    "-march=rv32imfdcxandes -mabi=lp32d -mcmodel=medany ${CMAKE_C_FLAGS}"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS
    "-march=rv32imfdcxandes -mabi=lp32d -mcmodel=medany ${CMAKE_CXX_FLAGS}"
    CACHE INTERNAL "C++ compiler common flags"
)
set(CMAKE_ASM_FLAGS
    "-march=rv32imfdcxandes -mabi=lp32d -mcmodel=medany ${CMAKE_ASM_FLAGS}"
    CACHE INTERNAL "asm compiler common flags"
)
