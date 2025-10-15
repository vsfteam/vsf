cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0076 NEW)

#if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../../../../.. ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
#endif()
message(STATUS "VSF_PATH: $ENV{VSF_PATH}")

# compiler configurations
message(STATUS "VSF_ARCH: ${VSF_ARCH}")
if(VSF_ARCH STREQUAL "ARM")
    # arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
    set(CMAKE_C_FLAGS 
        "-Ofast -Wno-unused-variable"
        CACHE INTERNAL "C compiler common flags"
    )
    set(CMAKE_CXX_FLAGS 
        "-Ofast -Wno-unused-variable"
        CACHE INTERNAL "C++ compiler common flags"
    )
    include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)
elseif(VSF_ARCH STREQUAL "Riscv")
elseif(VSF_ARCH STREQUAL "Windows")
    set(CMAKE_C_FLAGS 
        "-Ofast -Wno-inconsistent-dllimport"
        CACHE INTERNAL "C compiler common flags"
    )
    set(CMAKE_CXX_FLAGS 
        "-Ofast -Wno-inconsistent-dllimport"
        CACHE INTERNAL "C++ compiler common flags"
    )
elseif(VSF_ARCH STREQUAL "linux")
else()
    message(FATAL_ERROR "VSF_ARCH: '${VSF_ARCH}' not supported")
endif()

project(vsf_hal C CXX ASM)
# use commandline to pass VSF_TARGET:
#   cmake -GNinja -DVSF_TARGET=TARGET PATH
#set(VSF_TARGET N32H765IIB7EC)
# add target definitions here
#set(VSF_TARGET_DEFINITIONS
#)

# include vsf cmake script
set(VSF_CONFIG_PATH ./inc)
include($ENV{VSF_PATH}/script/cmake/vsf_hal.cmake)

# application configurations
vsf_add_compile_definitions(
    __VSF__
    __VSF_OOC__
)
vsf_add_sources(
    src/vsf_hal.c
)
if(VSF_ARCH STREQUAL "ARM")
elseif(VSF_ARCH STREQUAL "Riscv")
elseif(VSF_ARCH STREQUAL "Windows")
    vsf_add_compile_definitions(
        VSF_HAL_DISTBUS_AS_REAL_DRIVER
        VSF_HAL_USE_DISTBUS=ENABLED
        VSF_USE_HEAP=ENABLED
        VSF_USE_DISTBUS=ENABLED
        VSF_HAL_DISTBUS_USE_GPIO=ENABLED
        VSF_HAL_DISTBUS_USE_ADC=ENABLED
        VSF_HAL_DISTBUS_USE_PWM=ENABLED
        VSF_HAL_DISTBUS_USE_I2C=ENABLED
        VSF_HAL_DISTBUS_USE_SPI=ENABLED
        VSF_HAL_DISTBUS_USE_USART=ENABLED
        VSF_HAL_DISTBUS_USE_SDIO=ENABLED
        VSF_HAL_DISTBUS_USE_I2S=ENABLED
        VSF_HAL_DISTBUS_USE_USBD=ENABLED
        VSF_HAL_DISTBUS_USE_USBH=ENABLED
    )
elseif(VSF_ARCH STREQUAL "linux")
endif()