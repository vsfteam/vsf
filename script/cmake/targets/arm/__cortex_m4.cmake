if(NOT ARM_CMSIS_PATH)
    set(ARM_CMSIS_PATH ${VSF_SRC_PATH}/utilities/compiler/arm/3rd-party/CMSIS)
endif()

list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${ARM_CMSIS_PATH}/CMSIS/Core/Include
    ${ARM_CMSIS_PATH}/CMSIS/DSP/Include
    ${ARM_CMSIS_PATH}/CMSIS/NN/Include
)

list(APPEND VSF_TARGET_DEFINITIONS
    "__ARM_ARCH_PROFILE='M'"
)