# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Not defined environment variable:VSF_PATH")
else()
    message(STATUS "Get VSF_PATH: $ENV{VSF_PATH}")
endif()

file(GLOB cmakelists_vsf ./CMakeLists.vsf.txt)
file(COPY  ${cmakelists_vsf} DESTINATION $ENV{VSF_PATH})
file(RENAME  $ENV{VSF_PATH}/CMakeLists.vsf.txt $ENV{VSF_PATH}/CMakeLists.txt)

set(EXTRA_COMPONENT_DIRS $ENV{VSF_PATH})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

get_filename_component(PROJ_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJ_NAME})
