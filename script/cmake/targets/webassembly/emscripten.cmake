# use fake x86/linux with __CPU_EMSCRIPTEN__ defined
set(VSF_HAL_CHIP_VENDOR     x86)
set(VSF_ARCH_SERIES         x86)

set(VSF_HOST_SYSTEM         linux)
set(VSF_HOST_SYSTEM32       1)

set(VSF_TARGET_DEFINITIONS
    "__LINUX__"
    "__CPU_WEBASSEMBLY__"
)