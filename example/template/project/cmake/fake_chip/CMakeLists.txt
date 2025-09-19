cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../../../../../ ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
endif()

# compiler configurations
# arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
set(CMAKE_C_FLAGS
    "-Os"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS
    "-Os"
    CACHE INTERNAL "C++ compiler common flags"
)
if(NOT CMAKE_C_COMPILER)
    include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)
else()
    set(CMAKE_C_COMPILER_WORKS ON)
    set(CMAKE_CXX_COMPILER_WORKS ON)
    set(CMAKE_ASM_COMPILER_WORKS ON)
endif()

project(vsf_fake_chip_template C CXX ASM)
set(VSF_TARGET FAKE_CHIP)
set(VSF_TARGET_DEFINITIONS
    "__FAKE_VENDOR__"
    "__FAKE_SERIES__"
    "__FAKE_DEVICE__"
)

# include vsf cmake script
set(VSF_CONFIG_PATH ../../../config)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    LINKER:-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    LINKER:-Map=output.map
)

# add user application source code/include directories
# add cross_task_test.c instead of full demo directory to avoid multiple main() conflicts
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ../../../demo/kernel_test/cross_task_test.c
)

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
