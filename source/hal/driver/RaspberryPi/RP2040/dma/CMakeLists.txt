# HAL Driver CMakeLists.txt Template
# VSF_HAL_USE_DMA will be set to ENABLED by driver.h
# if VSF_HAL_USE_DMA is not set, the driver will not be built

vsf_add_sources(
    dma.c
)
