# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    cortex_m_generic.c
)

add_subdirectory(SysTick)
