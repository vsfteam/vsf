# CMakeLists head

if(NOT IS_DIRECTORY ${VSF_PRIVATE_BASE}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
    add_subdirectory_existed(${VSF_HAL_CHIP_SERIES})
else()
    message(STATUS "User defined driver is detected: ${VSF_PRIVATE_BASE}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES}")
endif()

if(VSF_HAL_CHIP_SERIES STREQUAL SF32LB52X)
    add_subdirectory(common/SF32LB5XX)
endif()

vsf_add_sources(
    vendor/drivers/hal/bf0_hal_cortex.c
    vendor/drivers/hal/bf0_hal_dma.c
    vendor/drivers/hal/bf0_hal_gpio.c
    vendor/drivers/hal/bf0_hal_hpaon.c
    vendor/drivers/hal/bf0_hal_lcdc.c
    vendor/drivers/hal/bf0_hal_mpi.c
    vendor/drivers/hal/bf0_hal_mpi_ex.c
    vendor/drivers/hal/bf0_hal_mpi_psram.c
    vendor/drivers/hal/bf0_hal_pinmux.c
    vendor/drivers/hal/bf0_hal_pmu.c
    vendor/drivers/hal/bf0_hal_rcc.c
    vendor/drivers/hal/flash_table.c
    vendor/drivers/hal/sifli_bbm.c
)

vsf_add_include_directories(
    vendor/drivers/cmsis/Include
    vendor/drivers/Include
    vendor/customer/boards/include
    vendor/external/CMSIS/Include
)
