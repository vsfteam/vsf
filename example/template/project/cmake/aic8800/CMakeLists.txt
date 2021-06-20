cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Please define environment VSF_PATH")
else()
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
endif()

project(vsf_template)
set(VSF_TARGET aic8800m)

# compiler configurations
# arm-none-eabi-gcc will not pass compiler checker of cmake
include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)

# VSF_USE configurations
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
set(VSF_USE_LVGL 1)
    set(VSF_LVGL_USE_FREETYPE 1)
set(VSF_USE_FREETYPE 1)
set(VSF_USE_DISP 1)
set(VSF_USE_SDL2 1)

# include vsf cmake script
set(VSF_CONFIG_PATH ../../../config)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# add user application source code/include directories
# add template directory
add_subdirectory(../../../demo ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_bin)
