# CMakeLists head

if(NOT IS_DIRECTORY ${VSF_PRIVATE_BASE}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
    add_subdirectory_existed(${VSF_HAL_CHIP_SERIES})
else()
    message(STATUS "User defined driver is detected: ${VSF_PRIVATE_BASE}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES}")
endif()
