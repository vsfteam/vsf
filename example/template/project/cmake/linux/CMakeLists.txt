cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Please define environment VSF_PATH")
else()
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
endif()

project(vsf_template)
add_executable(vsf_template)
set(VSF_TARGET x64_linux)

# add vsf configurations
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# add user application source code/include directories
add_subdirectory(
    ../../../demo
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ../../../config
)
