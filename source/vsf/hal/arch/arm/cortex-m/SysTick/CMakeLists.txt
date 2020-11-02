# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    systick_internal.c
    systick.c
)
