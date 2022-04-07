# use fake x86/linux with __CPU_EMSCRIPTEN__ defined
set(VSF_HAL_CHIP_VENDOR     x86)
set(VSF_ARCH_SERIES         x86)

set(VSF_HOST_SYSTEM         linux)
set(VSF_HOST_SYSTEM32       1)
set(VSF_HOST_HAS_LIBUSB     0)

set(VSF_TARGET_DEFINITIONS
    "__LINUX__"
    "__CPU_WEBASSEMBLY__"
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    "-s LLD_REPORT_UNDEFINED"
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -s PTHREAD_POOL_SIZE_STRICT=0")
