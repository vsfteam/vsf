if(NOT ARM_CMSIS_PATH)
    set(ARM_CMSIS_PATH ${VSF_SRC_PATH}/utilities/compiler/arm)
endif()

set(VSF_ARCH_SERIES         arm)
set(VSF_ARCH_NAME           arm9)
list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${ARM_CMSIS_PATH}/CMSIS/Core_Legacy/Include
)
set(VSF_TARGET_DEFINITIONS
    "__CPU_ARM9__"
    ${VSF_TARGET_DEFINITIONS}
)

set(CMAKE_C_FLAGS 
    "-mtune=arm926ej-s -mfloat-abi=soft -marm -mno-thumb-interwork -mno-unaligned-access ${CMAKE_C_FLAGS}"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "-mtune=arm926ej-s -mfloat-abi=soft -marm -mno-thumb-interwork -mno-unaligned-access ${CMAKE_CXX_FLAGS}"
    CACHE INTERNAL "C++ compiler common flags"
)
set(CMAKE_ASM_FLAGS
    "-mtune=arm926ej-s -mfloat-abi=soft -marm -mno-thumb-interwork -mno-unaligned-access ${CMAKE_ASM_FLAGS}"
    CACHE INTERNAL "asm compiler common flags"
)
