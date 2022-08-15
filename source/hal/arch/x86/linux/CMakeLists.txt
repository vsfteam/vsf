# CMakeLists head

target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
    linux_generic.c
)

vsf_add_libraries(
    pthread
    rt
)