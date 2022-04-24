cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Please define environment VSF_PATH")
else()
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
endif()

# compiler configurations
# arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
set(CMAKE_C_FLAGS 
    "-Ofast"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "-Ofast"
    CACHE INTERNAL "C++ compiler common flags"
)
include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)

project(vsf_template)
set(VSF_TARGET AIC8800M)
set(VSF_TARGET_DEFINITIONS
#   hw related definitions for AIC8800M required by sdk
#   hw_platform: pxp(0), fpga(1), asic(2)
    "CFG_HW_PLATFORM=2"
#   rf_mode: null(0), bt_only(1), bt_combo(2), btwifi_combo(3)
    "CFG_RF_MODE=0"
#   rom_ver
    "CFG_ROM_VER=255"
#   dac: left(0), mute(1), right(2), lprd2(3), lmrd2(4)
    "CFG_DACL_MIXER_MODE=1"
    "CFG_DACR_MIXER_MODE=1"
#   pmic
    "CFG_PMIC"
    "CFG_PMIC_VER_LITE"
    "CFG_PMIC_ITF_ASDMA"
#   wifi
    "CFG_WIFI_STACK"
    "CFG_RSSI_DATAPKT"
    "CONFIG_RWNX"
    "CONFIG_RWNX_LWIP"
#   misc
    "CFG_DBG"
    "CFG_IPERF"
#   mic matrix
    "CFG_AIC1000_MIC_MATRIX=0"
)

# VSF_USE configurations
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
set(VSF_USE_LVGL 1)
    set(VSF_LVGL_USE_FREETYPE 1)
set(VSF_USE_FREETYPE 1)
set(VSF_USE_DISP 1)
set(VSF_USE_SDL2 1)
    set(VSF_SDL2_USE_TTF 1)
    set(VSF_SDL2_USE_IMAGE 1)
set(VSF_USE_LIBJPEG_TURBO 1)
set(VSF_USE_LIBPNG 1)
set(VSF_USE_ZLIB 1)
set(VSF_USE_LWIP 1)
    set(VSF_LWIP_VENDOR 1)
set(VSF_USE_BTSTACK 1)
    set(VSF_BTSTACK_USE_LINKKEY_MEMORY 1)
set(VSF_USE_COREMARK 1)
set(VSF_USE_LUA 1)

# include vsf cmake script
set(VSF_CONFIG_PATH ../../../config)
get_filename_component(VSF_LVGL_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/lvgl ABSOLUTE)
get_filename_component(VSF_EVM_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/evm ABSOLUTE)
get_filename_component(VSF_BTSTACK_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/btstack ABSOLUTE)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# linker script
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    LINKER:-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    LINKER:-Map=output.map
)

# add user application source code/include directories
# add template directory
add_subdirectory(../../../demo ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_bin)

# type conversion and show size
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
)