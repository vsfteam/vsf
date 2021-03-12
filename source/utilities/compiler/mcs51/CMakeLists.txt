# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    retarget_io.c
    mcs51_interrupt.c
)
