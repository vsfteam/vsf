# use this file for llvm bitcode target only

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -fno-vectorize -fno-slp-vectorize"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fno-vectorize -fno-slp-vectorize"
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
set(CMAKE_C_FLAGS_DEBUG "-O3 -emit-llvm" CACHE STRING "c debug compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "-O3 -emit-llvm" CACHE STRING "cxx debug compiler flags")
set(CMAKE_ASM_FLAGS_DEBUG "-O3 -emit-llvm" CACHE INTERNAL "asm debug compiler flags")

set(CMAKE_C_COMPILER_WORKS ON)
set(CMAKE_CXX_COMPILER_WORKS ON)