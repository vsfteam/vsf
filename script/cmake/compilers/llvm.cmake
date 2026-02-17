# use this file for llvm bitcode target only
if(NOT VSF_APPLET OR NOT VSF_APPLET_SUFFIX STREQUAL ".bc" OR NOT VSF_TARGET STREQUAL CortexM4)
    message(FATAL_ERROR "this file is only for llvm bitcode applet target, .bc suffix")
endif()

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

find_program(LLVM_LINK llvm-link REQUIRED)

# setjmp is not supported when generating llvm target. It's target specified
# TODO: disable other target specified operations
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -emit-llvm -c --target=armv7m-none-eabi -D__PROGRAM_START=_start"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -emit-llvm -c --target=armv7m-none-eabi -D__PROGRAM_START=_start"
    CACHE INTERNAL "C++ compiler common flags"
)

set(CMAKE_C_COMPILER_WORKS ON)
set(CMAKE_CXX_COMPILER_WORKS ON)