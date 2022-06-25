set(VSF_HAL_CHIP_VENDOR     generic)
set(VSF_ARCH_SERIES         generic)

set(VSF_HOST_SYSTEM         linux)
set(VSF_HOST_SYSTEM32       1)
set(VSF_HOST_HAS_LIBUSB     1)

set(VSF_TARGET_DEFINITIONS
    "__LINUX__"
    "__CPU_GENERIC__"
)

# no idea why, but seems this makes the world quiet
set(CMAKE_C_FLAGS 
    "-Wno-builtin-declaration-mismatch"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "-Wno-builtin-declaration-mismatch"
    CACHE INTERNAL "C++ compiler common flags"
)
