# HAL Driver CMakeLists.txt Template
# VSF_HAL_USE_SPI will be set to ENABLED by driver.h
# if VSF_HAL_USE_SPI is not set, the driver will not be built

vsf_add_sources(
    spi.c
)
