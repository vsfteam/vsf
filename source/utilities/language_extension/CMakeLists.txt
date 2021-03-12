# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    code_region.c
    delegate.c
    simple_sprintf.c
    simple_sscanf.c
)
