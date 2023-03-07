cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# VSF begin

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Not defined environment variable:VSF_PATH")
else()
    message(STATUS "Get VSF_PATH: $ENV{VSF_PATH}")
endif()

# arch/driver
set(VSF_HAL_CHIP_VENDOR     BouffaloLab)
set(VSF_HAL_CHIP_SERIES     BL61X)
set(VSF_HAL_CHIP_NAME       BL616)
set(VSF_ARCH_SERIES         rtos)
set(VSF_ARCH_NAME           freertos)
set(VSF_TARGET_NAME         app)

# modules
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
    set(VSF_LINUX_USE_LIBTUV 1)

function(vsf_add_sources)
    target_sources(app PRIVATE ${ARGN})
endfunction()
function(vsf_add_include_directories)
    target_include_directories(app PRIVATE ${ARGN})
endfunction()
function(vsf_add_compile_definitions)
    target_compile_definitions(app PRIVATE ${ARGN})
endfunction()
function(vsf_add_libraries)
    message(FATAL_ERROR "libraries not supported")
endfunction()

vsf_add_compile_definitions(
    __BouffaloLab__ __BL616__
    __OOC_CPP__
    __VSF__
    __linux__
)
include($ENV{VSF_PATH}/script/cmake/extensions.cmake)
add_subdirectory($ENV{VSF_PATH}/source vsf_bin)
set(VSF_CONFIG_PATH .)

vsf_add_include_directories(.)
vsf_add_include_directories($ENV{BL_SDK_BASE}/components/os/freertos/include)
vsf_add_sources(./vsf_main.c)

# VSF end

sdk_add_include_directories($ENV{VSF_PATH}/source .)
sdk_set_main_file(main.c)

project(vsf_app)
