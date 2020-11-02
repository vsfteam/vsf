# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    rtos_generic.c
)

add_subdirectory_existed(${VSF_ARCH_NAME})
