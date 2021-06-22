cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Please define environment VSF_PATH")
else()
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
endif()

# compiler configurations
# arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
set(CMAKE_C_FLAGS 
    "-Og -Ofast -g3"
)
set(CMAKE_CXX_FLAGS 
    "-Og -Ofast -g3"
)
include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)

project(vsf_template)
set(VSF_TARGET aic8800m)

# VSF_USE configurations
set(VSF_USE_LINUX 1)
set(VSF_USE_LVGL 1)
    set(VSF_LVGL_USE_FREETYPE 1)
set(VSF_USE_FREETYPE 1)
set(VSF_USE_DISP 1)
set(VSF_USE_SDL2 1)
    set(VSF_SDL2_USE_TTF 1)
    set(VSF_SDL2_USE_IMAGE 1)
set(VSF_USE_LIBJPEG_TURBO 1)
set(VSF_USE_LIBPNG 1)
set(VSF_USE_ZLIB 1)
set(VSF_USE_LWIP 1)
    set(VSF_LWIP_VENDOR 1)
set(VSF_USE_BTSTACK 1)
    set(VSF_BTSTACK_USE_LINKKEY_MEMORY 1)
set(VSF_USE_COREMARK 1)
set(VSF_USE_LUA 1)
set(VSF_USE_EVM 1)

# include vsf cmake script
set(VSF_CONFIG_PATH ../../../config)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# linker script
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    LINKER:-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    LINKER:-Map=output.map
)

# add user application source code/include directories
# add template directory
add_subdirectory(../../../demo ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_bin)
