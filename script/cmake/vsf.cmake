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

set(VSF_LIB_NAME vsf)
add_library(${VSF_LIB_NAME} INTERFACE)

function(vsf_add_sources)
    target_sources(${VSF_LIB_NAME} INTERFACE ${ARGN})
endfunction()
function(vsf_add_include_directories)
    target_include_directories(${VSF_LIB_NAME} INTERFACE ${ARGN})
endfunction()
function(vsf_add_compile_definitions)
    target_compile_definitions(${VSF_LIB_NAME} INTERFACE ${ARGN})
endfunction()
function(vsf_add_libraries)
    target_link_libraries(${VSF_LIB_NAME} INTERFACE ${ARGN})
endfunction()

add_executable(${CMAKE_PROJECT_NAME} "")
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_LIB_NAME}
)

if(DEFINED VSF_CONFIG_PATH)
    get_filename_component(VSF_CONFIG_PATH ${VSF_CONFIG_PATH} ABSOLUTE)
endif()

include(${VSF_CMAKE_ROOT}/targets.cmake)

set(VSF_COMMON_INCLUDE_DIRECTORIES
    ${VSF_CONFIG_PATH}
    ${VSF_SRC_PATH}
    ${VSF_TARGET_INCLUDE_DIRECTORIES}
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_TARGET_DEFINITIONS}
    __VSF__
)
if(VSF_APPLET)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        __VSF_APPLET__
        __OOC_CPP__
    )
endif()
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${VSF_COMMON_INCLUDE_DIRECTORIES}
)
set(VSF_TARGET_NAME ${CMAKE_PROJECT_NAME})

if(NOT VSF_APPLET AND VSF_HOST_SYSTEM)
    set(VSF_HOST_SYSTEM_LIB_NAME vsf_host_${VSF_HOST_SYSTEM})
    add_library(${VSF_HOST_SYSTEM_LIB_NAME} STATIC)
    target_include_directories(${VSF_HOST_SYSTEM_LIB_NAME} PRIVATE
        ${VSF_COMMON_INCLUDE_DIRECTORIES}
    )
    target_compile_definitions(${VSF_HOST_SYSTEM_LIB_NAME} PRIVATE
        ${VSF_TARGET_DEFINITIONS}
    )

    # no idea why can not simply add ${VSF_HOST_SYSTEM_LIB_NAME}
    vsf_add_libraries(
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${VSF_HOST_SYSTEM_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
endif()

include(${VSF_CMAKE_ROOT}/compilers.cmake)

if(NOT VSF_APPLET)
    add_subdirectory(${VSF_SRC_PATH} ${CMAKE_CURRENT_BINARY_DIR}/vsf_bin)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/vsf_bin)

    # libraries are not always suitable for xip elfloader, because relocate to .text sections will maybe generated.
    #   So do not use libraries in applet mode.
    # libraries MUST be placed at the end
    vsf_add_libraries(
        m
    )
else()
    if(NOT VSF_USE_LINUX)
        message(WARNING "VSF_APPLET is for vsf.linux, so VSF_USE_LINUX MUST be enabled")
        set(VSF_USE_LINUX 1)
        set(VSF_LINUX_USE_SIMPLE_LIBC 1)
    endif()

    vsf_add_compile_definitions(
        # itoa is non-standard API, so remove it for applet
        VSF_LINUX_LIBC_HAS_ITOA=0
    )
    vsf_add_include_directories(
        # linux include directories
        ${VSF_SRC_PATH}/shell/sys/linux/include
        ${VSF_SRC_PATH}/shell/sys/linux/include/simple_libc
        ${VSF_SRC_PATH}/shell/sys/linux/include/simple_libc/inttypes
        # use original math.h for better compatibility
        #${VSF_SRC_PATH}/shell/sys/linux/include/simple_libc/math
        ${VSF_SRC_PATH}/shell/sys/linux/include/simple_libc/stdint
        ${VSF_SRC_PATH}/shell/sys/linux/include/simple_libc/assert
    )
    if((${VSF_ARCH_SERIES} STREQUAL "arm") OR (${VSF_ARCH_SERIES} STREQUAL "x64"))
        # setjmp implementation only supports arm and x64
        vsf_add_include_directories(
            ${VSF_SRC_PATH}/shell/sys/linux/include/simple_libc/setjmp
        )
    endif()
    if(VSF_APPLET_USE_GETOPT)
        vsf_add_sources(
            ${VSF_SRC_PATH}/shell/sys/linux/lib/3rd-party/getopt/getopt_long.c
        )
    endif()
    if(VSF_APPLET_USE_REGEX)
        vsf_add_sources(
            ${VSF_SRC_PATH}/shell/sys/linux/lib/3rd-party/regex/regcomp.c
            ${VSF_SRC_PATH}/shell/sys/linux/lib/3rd-party/regex/regerror.c
            ${VSF_SRC_PATH}/shell/sys/linux/lib/3rd-party/regex/regexec.c
            ${VSF_SRC_PATH}/shell/sys/linux/lib/3rd-party/regex/regfree.c
        )
    endif()
    if(VSF_APPLET_USE_CTYPE)
        vsf_add_sources(
            # for __vsf_linux_ctype used in ctype.h
            ${VSF_SRC_PATH}/shell/sys/linux/lib/glibc/vsf_linux_glibc_ctype.c
        )
    endif()
#    vsf_add_sources(
#        ${VSF_SRC_PATH}/shell/sys/linux/lib/vsf_linux_applet_lib.c
#    )

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        SUFFIX ".elf"
    )
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    )
endif()
