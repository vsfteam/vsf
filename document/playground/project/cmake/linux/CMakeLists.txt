cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Please define environment VSF_PATH")
else()
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
endif()

project(vsf_playground)
set(VSF_TARGET x64/linux)

# compiler configurations
# if compiler is not set, cmake will find one

# VSF_USE configurations

# include vsf cmake script
set(VSF_CONFIG_PATH ../../..)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# add user application source code/include directories
cmake_policy(SET CMP0076 NEW)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    ../../../main.c
)
