set(VSF_HAL_CHIP_VENDOR     T-Head)
set(VSF_HAL_CHIP_SERIES     E900)
set(VSF_HAL_CHIP_NAME       E907)

set(VSF_ARCH_SERIES         rv)
set(VSF_ARCH_NAME           rv64)

vsf_add_compile_definitions(
    __CPU_RV__
    __VSF64__
)

set(CMAKE_C_FLAGS 
    "-march=rv32imafdcpzpsfoperand_xtheade -mabi=ilp32d -mcmodel=medlow ${CMAKE_C_FLAGS}"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "-march=rv32imafdcpzpsfoperand_xtheade -mabi=ilp32d -mcmodel=medlow ${CMAKE_CXX_FLAGS}"
    CACHE INTERNAL "C++ compiler common flags"
)
set(CMAKE_ASM_FLAGS
    "-march=rv32imafdcpzpsfoperand_xtheade -mabi=ilp32d -x assembler-with-cpp -D__ASSEMBLY__ ${CMAKE_ASM_FLAGS}"
    CACHE INTERNAL "asm compiler common flags"
)
