/* VSF Test Suite aggregation header — update when adding new peripheral or arch tests */
#ifndef __VSF_TEST_SUITES_H__
#define __VSF_TEST_SUITES_H__

#include "vsf_board.h"

#include "hal/driver/adc/vsf_test_adc.h"
#include "hal/driver/dma/vsf_test_dma.h"
#include "hal/driver/flash/vsf_test_flash.h"
#include "hal/driver/gpio/vsf_test_gpio.h"
#include "hal/driver/i2c/vsf_test_i2c.h"
#include "hal/driver/pwm/vsf_test_pwm.h"
#include "hal/driver/rng/vsf_test_rng.h"
#include "hal/driver/rtc/vsf_test_rtc.h"
#include "hal/driver/spi/vsf_test_spi.h"
#include "hal/driver/timer/vsf_test_timer.h"
#include "hal/driver/usart/vsf_test_usart.h"
#include "hal/driver/wdt/vsf_test_wdt.h"
#include "hal/arch/vsf_test_arch.h"

typedef union {
#if VSF_TEST_ADC_ENABLE == ENABLED
    vsf_test_adc_data_t adc;
#endif
#if VSF_TEST_DMA_ENABLE == ENABLED
    vsf_test_dma_data_t dma;
#endif
#if VSF_TEST_FLASH_ENABLE == ENABLED
    vsf_test_flash_data_t flash;
#endif
#if VSF_TEST_GPIO_ENABLE == ENABLED
    vsf_test_gpio_data_t gpio;
#endif
#if VSF_TEST_I2C_ENABLE == ENABLED
    vsf_test_i2c_data_t i2c;
#endif
#if VSF_TEST_RTC_ENABLE == ENABLED
    vsf_test_rtc_data_t rtc;
#endif
#if VSF_TEST_SPI_ENABLE == ENABLED
    vsf_test_spi_data_t spi;
#endif
#if VSF_TEST_TIMER_ENABLE == ENABLED
    vsf_test_timer_data_t timer;
#endif
#if VSF_TEST_USART_ENABLE == ENABLED
    vsf_test_usart_data_t usart;
#endif
#if VSF_TEST_ARCH_ENABLE == ENABLED
    vsf_test_arch_data_t arch;
#endif
} vsf_test_suite_data_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_test_suite_data_t vsf_test_suite_data;
extern const vsf_test_suite_t *vsf_test_suite_list[];
extern uint8_t vsf_test_suite_count;

#endif /* __VSF_TEST_SUITES_H__ */
