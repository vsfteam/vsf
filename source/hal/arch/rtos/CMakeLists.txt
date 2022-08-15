# CMakeLists head

vsf_add_sources(
    rtos_generic.c
)

add_subdirectory_existed(${VSF_ARCH_NAME})
