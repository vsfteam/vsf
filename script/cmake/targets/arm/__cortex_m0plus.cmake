if(NOT ARM_CMSIS_PATH)
    set(ARM_CMSIS_PATH ${VSF_SRC_PATH}/utilities/compiler/arm/3rd-party/CMSIS)
endif()

set(VSF_ARCH_SERIES         arm)
set(VSF_ARCH_NAME           cortex-m0plus)

list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${ARM_CMSIS_PATH}/CMSIS/Core/Include
    ${ARM_CMSIS_PATH}/CMSIS/DSP/Include
    ${ARM_CMSIS_PATH}/CMSIS/NN/Include
)

vsf_add_compile_definitions(
    __CPU_ARM__
)

set(CMAKE_C_FLAGS 
    "-mcpu=cortex-m0plus -mthumb ${CMAKE_C_FLAGS}"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "-mcpu=cortex-m0plus -mthumb ${CMAKE_CXX_FLAGS}"
    CACHE INTERNAL "C++ compiler common flags"
)
set(CMAKE_ASM_FLAGS
    "-mcpu=cortex-m0plus -mthumb ${CMAKE_ASM_FLAGS}"
    CACHE INTERNAL "asm compiler common flags"
)
