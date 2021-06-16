cmake_policy(SET CMP0076 OLD)

if(DEFINED ENV{VSF_PATH})
    set(VSF_PATH $ENV{VSF_PATH})
else()
    message(FATAL_ERROR "Please define VSF_PATH")
endif()
set(VSF_CMAKE_ROOT ${VSF_PATH}/script/cmake)

include(${VSF_CMAKE_ROOT}/extensions.cmake)

if(NOT (VSF_HAL_CHIP_VENDOR AND VSF_HAL_CHIP_NAME AND VSF_ARCH_SERIES AND VSF_ARCH_NAME))
    # parse VSF_HAL_CHIP_VENDOR/VSF_HAL_CHIP_NAME/VSF_ARCH_SERIES/VSF_ARCH_NAME
    include(${VSF_CMAKE_ROOT}/targets.cmake)
endif()

set(VSF_LIB_NAME vsf)
add_library(${VSF_LIB_NAME} INTERFACE)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_PATH}/source
)
add_subdirectory(${VSF_PATH}/source ${CMAKE_CURRENT_BINARY_DIR}/vsf)
