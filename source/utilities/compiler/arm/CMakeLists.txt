# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    retarget_io.c
    arm_compiler.c
)
