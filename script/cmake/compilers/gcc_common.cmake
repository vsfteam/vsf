# CMakeLists head

set(CMAKE_SYSTEM_NAME           Generic)
set(CMAKE_SYSTEM_PROCESSOR      ${VSF_HAL_ARCH_SERIES})

if (NOT DEFINED GCC_PREFIX)
    message(FATAL_ERROR "Please define GCC_PREFIX!!!, eg: arm-none-eabi")
endif()

set(CMAKE_C_COMPILER ${GCC_PREFIX}-gcc CACHE INTERNAL "c compiler")
set(CMAKE_CXX_COMPILER ${GCC_PREFIX}-g++ CACHE INTERNAL "cxx compiler")
set(CMAKE_ASM_COMPILER ${GCC_PREFIX}-gcc CACHE INTERNAL "asm compiler")

set(CMAKE_OBJCOPY ${GCC_PREFIX}-objcopy CACHE INTERNAL "objcopy")
set(CMAKE_OBJDUMP ${GCC_PREFIX}-objdump CACHE INTERNAL "objdump")
set(CMAKE_SIZE ${GCC_PREFIX}-size CACHE INTERNAL "size")

set(CMAKE_C_FLAGS 
    "${CMAKE_C_FLAGS} -std=gnu11 -Wall -fno-builtin -fno-strict-aliasing -fdata-sections -ffunction-sections"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -fpermissive -Wall -fno-builtin -fno-strict-aliasing -fdata-sections -ffunction-sections"
    CACHE INTERNAL "C++ compiler common flags"
)

# Optimization Level
#   -O0       None
#   -Os       Optimize for size
#   -O1       Optimize
#   -O2       Optimize more
#   -O3       Optimize most
#   -Og       Optimize for debug
#   -Ofast    Optimize for speed
if (DEFINED GCC_C_OPTIMIZATION)
    set(CMAKE_C_FLAGS_DEBUG ${GCC_C_OPTIMIZATION} CACHE STRING "c debug compiler flags")
else()
    set(CMAKE_C_FLAGS_DEBUG "-Og -g3" CACHE STRING "c debug compiler flags")
endif()
if (DEFINED GCC_CXX_OPTIMIZATION)
    set(CMAKE_CXX_FLAGS_DEBUG ${GCC_CXX_OPTIMIZATION} CACHE STRING "cxx debug compiler flags")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3" CACHE STRING "cxx debug compiler flags")
endif()
if (DEFINED GCC_ASM_OPTIMIZATION)
    set(CMAKE_ASM_FLAGS_DEBUG ${GCC_ASM_OPTIMIZATION} CACHE STRING "asm debug compiler flags")
else()
    set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "asm debug compiler flags")
endif()

if (DEFINED GCC_SPEC)
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -flto --specs=${GCC_SPEC}.specs"
        CACHE STRING "exe link flags")
else()
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -flto"
        CACHE STRING "exe link flags")
endif()

set(CMAKE_C_COMPILER_WORKS ON)
set(CMAKE_CXX_COMPILER_WORKS ON)
set(CMAKE_ASM_COMPILER_WORKS ON)