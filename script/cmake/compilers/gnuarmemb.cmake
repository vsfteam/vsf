# CMakeLists head

set(CMAKE_SYSTEM_NAME           Generic)
set(CMAKE_SYSTEM_PROCESSOR      ${VSF_HAL_ARCH_SERIES})

set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE INTERNAL "c compiler")
set(CMAKE_CXX_COMPILER arm-none-eabi-g++ CACHE INTERNAL "cxx compiler")
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc CACHE INTERNAL "asm compiler")

set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE INTERNAL "objcopy")
set(CMAKE_OBJDUMP arm-none-eabi-objdump CACHE INTERNAL "objdump")
set(CMAKE_SIZE arm-none-eabi-size CACHE INTERNAL "size")

set(CMAKE_C_FLAGS 
    "${CMAKE_C_FLAGS} -std=gnu11 -Wall -fno-builtin -fno-strict-aliasing -fdata-sections -fms-extensions -ffunction-sections"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -fpermissive -Wall -fno-builtin -fno-strict-aliasing -fdata-sections -fms-extensions -ffunction-sections"
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
set(CMAKE_C_FLAGS_DEBUG "-Og -g3" CACHE STRING "c debug compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3" CACHE STRING "cxx debug compiler flags")
set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "asm debug compiler flags")

#set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections --specs=nosys.specs"
#  CACHE INTERNAL "exe link flags")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -flto --specs=nano.specs"
  CACHE STRING "exe link flags")

set(CMAKE_C_COMPILER_WORKS ON)
set(CMAKE_CXX_COMPILER_WORKS ON)
set(CMAKE_ASM_COMPILER_WORKS ON)