# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    retarget_io.c
)

add_subdirectory(RISCV)
