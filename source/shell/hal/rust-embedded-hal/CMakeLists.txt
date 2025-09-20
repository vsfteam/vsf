cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0076 NEW)

#if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../../../.. ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
#endif()
message(STATUS "VSF_PATH: $ENV{VSF_PATH}")

# compiler configurations
# arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
set(CMAKE_C_FLAGS 
    "-Ofast -Wno-unused-variable"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "-Ofast -Wno-unused-variable"
    CACHE INTERNAL "C++ compiler common flags"
)
include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)

project(vsf_hal C CXX ASM)
set(VSF_TARGET N32H765IIB7EC)
# add target definitions here
#set(VSF_TARGET_DEFINITIONS
#)

# include vsf cmake script
set(VSF_CONFIG_PATH ./inc)
include($ENV{VSF_PATH}/script/cmake/vsf_hal.cmake)

# application configurations
vsf_add_compile_definitions(
    __VSF__
    __VSF_OOC__
)
