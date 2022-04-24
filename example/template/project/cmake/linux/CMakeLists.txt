cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Please define environment VSF_PATH")
else()
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
endif()

project(vsf_template)
set(VSF_TARGET x64/linux)

# compiler configurations
# if compiler is not set, cmake will find one

# VSF_USE configurations
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
set(VSF_USE_LVGL 1)
    set(VSF_LVGL_USE_FREETYPE 1)
set(VSF_USE_FREETYPE 1)
set(VSF_USE_DISP 1)
    set(VSF_DISP_USE_SDL2 1)
set(VSF_USE_SDL2 1)
    set(VSF_SDL2_USE_TTF 1)
    set(VSF_SDL2_USE_IMAGE 1)
set(VSF_USE_LIBJPEG_TURBO 1)
set(VSF_USE_LIBPNG 1)
set(VSF_USE_ZLIB 1)
set(VSF_USE_COREMARK 1)
set(VSF_USE_LUA 1)
set(VSF_USE_MBEDTLS 1)
set(VSF_USE_LITTLEFS 1)

# include vsf cmake script
set(VSF_CONFIG_PATH ../../../config)
get_filename_component(VSF_LVGL_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/lvgl ABSOLUTE)
get_filename_component(VSF_EVM_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/evm ABSOLUTE)
get_filename_component(VSF_MBEDTLS_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/mbedtls ABSOLUTE)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# add user application source code/include directories
# add template directory
add_subdirectory(../../../demo ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_bin)
