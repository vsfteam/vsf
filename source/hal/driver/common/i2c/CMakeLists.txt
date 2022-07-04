# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    i2c_common.c
    i2c_request.c
    multiplex_i2c.c
)

