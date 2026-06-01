# VSF Test Framework — CMake library
# Per vsf-test-generalization PRD: application no longer manually lists
# test source files; this directory is add_subdirectory'd once and
# discovers suites automatically.

vsf_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../source/component/test/vsf_test
)

# Core framework sources
vsf_add_sources(
    ${CMAKE_CURRENT_LIST_DIR}/../../source/component/test/vsf_test/vsf_test.c
    ${CMAKE_CURRENT_LIST_DIR}/../../source/component/test/vsf_test/vsf_test_shell.c
)

# Peripheral test suites — discovered automatically.
# Re-run CMake after adding a new .c file under hal/driver/<periph>/.
file(GLOB_RECURSE USART_SRCS ${CMAKE_CURRENT_LIST_DIR}/hal/driver/usart/*.c)
file(GLOB_RECURSE GPIO_SRCS  ${CMAKE_CURRENT_LIST_DIR}/hal/driver/gpio/*.c)
file(GLOB_RECURSE I2C_SRCS   ${CMAKE_CURRENT_LIST_DIR}/hal/driver/i2c/*.c)
file(GLOB_RECURSE RTC_SRCS   ${CMAKE_CURRENT_LIST_DIR}/hal/driver/rtc/*.c)
file(GLOB_RECURSE FLASH_SRCS ${CMAKE_CURRENT_LIST_DIR}/hal/driver/flash/*.c)
file(GLOB_RECURSE WDT_SRCS   ${CMAKE_CURRENT_LIST_DIR}/hal/driver/wdt/*.c)
file(GLOB_RECURSE ADC_SRCS   ${CMAKE_CURRENT_LIST_DIR}/hal/driver/adc/*.c)
file(GLOB_RECURSE PWM_SRCS   ${CMAKE_CURRENT_LIST_DIR}/hal/driver/pwm/*.c)
file(GLOB_RECURSE TIMER_SRCS ${CMAKE_CURRENT_LIST_DIR}/hal/driver/timer/*.c)
file(GLOB_RECURSE SPI_SRCS   ${CMAKE_CURRENT_LIST_DIR}/hal/driver/spi/*.c)
file(GLOB_RECURSE RNG_SRCS   ${CMAKE_CURRENT_LIST_DIR}/hal/driver/rng/*.c)
file(GLOB_RECURSE DMA_SRCS   ${CMAKE_CURRENT_LIST_DIR}/hal/driver/dma/*.c)
file(GLOB_RECURSE ARCH_SRCS  ${CMAKE_CURRENT_LIST_DIR}/hal/arch/*.c)

vsf_add_sources(
    ${CMAKE_CURRENT_LIST_DIR}/vsf_test_suites.c
    ${USART_SRCS}
    ${GPIO_SRCS}
    ${I2C_SRCS}
    ${RTC_SRCS}
    ${FLASH_SRCS}
    ${WDT_SRCS}
    ${ADC_SRCS}
    ${PWM_SRCS}
    ${TIMER_SRCS}
    ${SPI_SRCS}
    ${RNG_SRCS}
    ${DMA_SRCS}
    ${ARCH_SRCS}
)
