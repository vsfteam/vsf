set(VSF_HAL_CHIP_VENDOR     x86)
set(VSF_ARCH_SERIES         x86)

set(VSF_HOST_SYSTEM         linux)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC "__LINUX__" "__CPU_X86__")
