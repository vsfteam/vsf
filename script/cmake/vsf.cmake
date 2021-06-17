cmake_policy(SET CMP0076 NEW)

if(DEFINED ENV{VSF_PATH})
    set(VSF_SRC_PATH $ENV{VSF_PATH}/source)
else()
    message(FATAL_ERROR "Please define VSF_PATH")
endif()
set(VSF_CMAKE_ROOT $ENV{VSF_PATH}/script/cmake)

include(${VSF_CMAKE_ROOT}/extensions.cmake)

set(VSF_LIB_NAME vsf)
add_library(${VSF_LIB_NAME} INTERFACE)

add_executable(${CMAKE_PROJECT_NAME} "")
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_LIB_NAME}
)

if(NOT (VSF_HAL_CHIP_VENDOR AND VSF_HAL_CHIP_NAME AND VSF_ARCH_SERIES AND VSF_ARCH_NAME))
    # parse VSF_HAL_CHIP_VENDOR/VSF_HAL_CHIP_NAME/VSF_ARCH_SERIES/VSF_ARCH_NAME
    include(${VSF_CMAKE_ROOT}/targets.cmake)
endif()
include(${VSF_CMAKE_ROOT}/compilers.cmake)
include(${VSF_CMAKE_ROOT}/3rd-party.cmake)

add_subdirectory(${VSF_SRC_PATH} ${CMAKE_CURRENT_BINARY_DIR}/vsf)
