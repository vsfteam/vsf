# CMakeLists head

target_sources(vsf INTERFACE
    vsf_arch_abstraction.c
)

add_subdirectory(${VSF_HAL_ARCH_SERIES})
