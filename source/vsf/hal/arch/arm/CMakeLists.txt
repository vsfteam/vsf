# CMakeLists head

set(ARCH_FOR_cortex-m0        armv6s-m        )
set(ARCH_FOR_cortex-m0plus    armv6s-m        )
set(ARCH_FOR_cortex-m3        armv7-m         )
set(ARCH_FOR_cortex-m4        armv7e-m        )
set(ARCH_FOR_cortex-m7        armv7e-m        )
set(ARCH_FOR_cortex-m23       armv8-m.base    )
set(ARCH_FOR_cortex-m33       armv8-m.main+dsp)
set(ARCH_FOR_cortex-m33+nodsp armv8-m.main    )
set(ARCH_FOR_cortex-r4        armv7-r         )
# TODO support cortex-a

set(ARCH_SUB_FOR_cortex-m0        cortex-m    )
set(ARCH_SUB_FOR_cortex-m0plus    cortex-m    )
set(ARCH_SUB_FOR_cortex-m3        cortex-m    )
set(ARCH_SUB_FOR_cortex-m4        cortex-m    )
set(ARCH_SUB_FOR_cortex-m7        cortex-m    )
set(ARCH_SUB_FOR_cortex-m23       cortex-m    )
set(ARCH_SUB_FOR_cortex-m33       cortex-m    )
set(ARCH_SUB_FOR_cortex-m33+nodsp cortex-m    )
set(ARCH_SUB_FOR_cortex-r4        cortex-r    )
# TODO support cortex-a


if(ARCH_FOR_${VSF_HAL_PROCESSOR_NAME})
    set(ARCH_FLAG -march=${ARCH_FOR_${VSF_HAL_PROCESSOR_NAME}})
    set(ARCH_SUB ${ARCH_SUB_FOR_${VSF_HAL_PROCESSOR_NAME}})
endif()


target_compile_options( vsf
  INTERFACE -mabi=aapcs
  INTERFACE ${ARCH_FLAG}
)

target_link_options( vsf
  INTERFACE -mabi=aapcs
  INTERFACE ${ARCH_FLAG}
)

add_subdirectory_existed(${ARCH_SUB})
