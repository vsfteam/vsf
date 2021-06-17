cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

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
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_CONFIG_PATH}
)

if(NOT (VSF_HAL_CHIP_VENDOR AND VSF_HAL_CHIP_NAME AND VSF_ARCH_SERIES AND VSF_ARCH_NAME))
    # parse VSF_HAL_CHIP_VENDOR/VSF_HAL_CHIP_NAME/VSF_ARCH_SERIES/VSF_ARCH_NAME
    include(${VSF_CMAKE_ROOT}/targets.cmake)
endif()

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_TARGET_DEFINITIONS}
)

if(VSF_HOST_SYSTEM)
    set(VSF_HOST_SYSTEM_LIB_NAME vsf_host_${VSF_HOST_SYSTEM})
    add_library(${VSF_HOST_SYSTEM_LIB_NAME} STATIC)
    target_include_directories(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
        ${VSF_CONFIG_PATH}
        ${VSF_SRC_PATH}
    )
    target_compile_definitions(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
        ${VSF_TARGET_DEFINITIONS}
    )

    link_directories(${CMAKE_CURRENT_BINARY_DIR})
# TODO: how to link VSF_HOST_SYSTEM_LIB_NAME
#    target_link_libraries(${VSF_LIB_NAME} INTERFACE
#        ${VSF_HOST_SYSTEM_LIB_NAME}
#    )
endif()

include(${VSF_CMAKE_ROOT}/compilers.cmake)
include(${VSF_CMAKE_ROOT}/3rd-party.cmake)

add_subdirectory(${VSF_SRC_PATH} ${CMAKE_CURRENT_BINARY_DIR}/vsf_bin)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/vsf_bin)
