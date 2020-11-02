# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    stm32f1xx_hal.c
    stm32f1xx_hal_cortex.c
    stm32f1xx_hal_gpio.c
    stm32f1xx_hal_pwr.c
    stm32f1xx_hal_rcc.c
    system_stm32f1xx.c
)
