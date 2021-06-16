set(VSF_HAL_CHIP_VENDOR     x86)
set(VSF_ARCH_SERIES         x86)

set(VSF_HOST_SYSTEM         win)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC "__WIN__" "__CPU_X64__")
