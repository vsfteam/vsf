# CMakeLists head

target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
    macos_generic.c
)

vsf_add_libraries(
    pthread
)