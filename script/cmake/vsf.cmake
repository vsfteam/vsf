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
get_filename_component(VSF_CONFIG_PATH ${VSF_CONFIG_PATH} ABSOLUTE)

include(${VSF_CMAKE_ROOT}/targets.cmake)

set(VSF_COMMON_INCLUDE_DIRECTORIES
    ${VSF_CONFIG_PATH}
    ${VSF_SRC_PATH}
    ${VSF_TARGET_INCLUDE_DIRECTORIES}
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_TARGET_DEFINITIONS}
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_COMMON_INCLUDE_DIRECTORIES}
)

if(VSF_HOST_SYSTEM)
    set(VSF_HOST_SYSTEM_LIB_NAME vsf_host_${VSF_HOST_SYSTEM})
    add_library(${VSF_HOST_SYSTEM_LIB_NAME} STATIC)
    target_include_directories(${VSF_HOST_SYSTEM_LIB_NAME} PRIVATE
        ${VSF_COMMON_INCLUDE_DIRECTORIES}
    )
    target_compile_definitions(${VSF_HOST_SYSTEM_LIB_NAME} PRIVATE
        ${VSF_TARGET_DEFINITIONS}
    )

    # no idea why can not simply add ${VSF_HOST_SYSTEM_LIB_NAME}
    target_link_libraries(${VSF_LIB_NAME} INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${VSF_HOST_SYSTEM_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
endif()

include(${VSF_CMAKE_ROOT}/compilers.cmake)

add_subdirectory(${VSF_SRC_PATH} ${CMAKE_CURRENT_BINARY_DIR}/vsf_bin)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/vsf_bin)

# libraries MUST be placed at the end
target_link_libraries(${VSF_LIB_NAME} INTERFACE
#   -lm
    m
)