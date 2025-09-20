cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

if(DEFINED ENV{VSF_PATH})
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
    set(VSF_SRC_PATH $ENV{VSF_PATH}/source)
else()
    message(FATAL_ERROR "Please define VSF_PATH")
endif()
set(VSF_CMAKE_ROOT $ENV{VSF_PATH}/script/cmake)

include(${VSF_CMAKE_ROOT}/extensions.cmake)

set(VSF_LIB_NAME vsf_hal)
add_library(${VSF_LIB_NAME} STATIC)

function(vsf_add_sources)
    target_sources(${VSF_LIB_NAME} PUBLIC ${ARGN})
endfunction()
function(vsf_add_include_directories)
    target_include_directories(${VSF_LIB_NAME} PUBLIC ${ARGN})
endfunction()
function(vsf_add_compile_definitions)
    target_compile_definitions(${VSF_LIB_NAME} PUBLIC ${ARGN})
endfunction()
function(vsf_add_libraries)
    target_link_libraries(${VSF_LIB_NAME} PUBLIC ${ARGN})
endfunction()

if(DEFINED VSF_CONFIG_PATH)
    get_filename_component(VSF_CONFIG_PATH ${VSF_CONFIG_PATH} ABSOLUTE)
endif()

if(DEFINED VSF_CMAKE_TARGETS)
    include(${VSF_CMAKE_TARGETS})
else()
    include(${VSF_CMAKE_ROOT}/targets.cmake)
endif()

set(VSF_COMMON_INCLUDE_DIRECTORIES
    ${VSF_CONFIG_PATH}
    ${VSF_SRC_PATH}
    ${VSF_TARGET_INCLUDE_DIRECTORIES}
)

vsf_add_compile_definitions(
    ${VSF_TARGET_DEFINITIONS}
    __VSF__
    __VSF_CPP__
)
vsf_add_include_directories(
    ${VSF_COMMON_INCLUDE_DIRECTORIES}
)

include(${VSF_CMAKE_ROOT}/compilers.cmake)

add_subdirectory($ENV{VSF_PATH}/source/hal ${CMAKE_CURRENT_BINARY_DIR}/vsf_hal_bin)
add_subdirectory($ENV{VSF_PATH}/source/utilities ${CMAKE_CURRENT_BINARY_DIR}/vsf_utilities_bin)
add_subdirectory($ENV{VSF_PATH}/source/service ${CMAKE_CURRENT_BINARY_DIR}/vsf_service_bin)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/vsf_hal_bin)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/vsf_utilities_bin)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/vsf_service_bin)

# libraries are not always suitable for xip elfloader, because relocate to .text sections will maybe generated.
#   So do not use libraries in applet mode.
# libraries MUST be placed at the end
vsf_add_libraries(
    m
)
