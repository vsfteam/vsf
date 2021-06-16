cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Please define environment VSF_PATH")
else()
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
endif()

project(vsf_template)
set(VSF_TARGET x64_linux)

# define 3rd-party libraries
set(VSF_USE_LVGL)

# include vsf cmake script
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# add user application source code/include directories
# add template directory
add_subdirectory(../../.. ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME})
