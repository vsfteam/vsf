# CMakeLists head

target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
    linux_generic.c
)

target_link_libraries(${VSF_LIB_NAME} INTERFACE
    pthread
    rt
)