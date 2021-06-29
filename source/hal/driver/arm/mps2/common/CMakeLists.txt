# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    __common.c
)

add_subdirectory(debug_uart)
# add_subdirectory(V2M-MPS2_CMx_BSP)