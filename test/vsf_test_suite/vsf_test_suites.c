/* VSF Test Suite aggregation — update when adding new suites */

#include "vsf_test_suites.h"
#include "vsf_board.h"

/*============================ SCENARIO STATE INSTANCE ========================*/

vsf_test_suite_data_t vsf_test_suite_data;

/*============================ SUITES ========================================*/

const vsf_test_suite_t *vsf_test_suite_list[] = {
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_adc_oneshot,
#endif
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_adc_stream,
#endif
#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_adc_temperature,
#endif
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_dma_mem2mem,
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_dma_mem2mem_irq,
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_dma_scatter_gather,
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_flash_boundary,
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_flash_erase_program_read,
#endif
#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_analog_mode,
#endif
#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_atomic,
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_concurrent_prio,
#endif
#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_direction,
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_exti,
#endif
#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_io_check,
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_irq_latency,
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_irq_lifecycle,
#endif
#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_multi_pin,
#endif
#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_open_drain,
#endif
#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_output_input,
#endif
#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_pinmux,
#endif
#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_systimer_health,
#endif
#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_toggle,
#endif
#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_toggle_freq,
#endif
#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_toggle_stress,
#endif
#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_gpio_write_throughput,
#endif
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_i2c_bus_scan,
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_i2c_eeprom_page,
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_i2c_eeprom_rw,
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_i2c_eeprom_rw_fifo,
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_i2c_slave,
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_i2c_slave_fifo,
#endif
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_pwm_basic,
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_pwm_dual_channel,
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_pwm_irq,
#endif
#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_rng_basic,
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_rtc_alarm,
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_rtc_epoch,
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_rtc_set_get,
#endif
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_spi_async,
#endif
#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_spi_loopback,
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_timer_async,
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_timer_oneshot,
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_timer_periodic,
#endif
#if VSF_TEST_USART_BAUD_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_baud,
#endif
#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_break_signal,
#endif
#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_hw_flow_control,
#endif
#if VSF_TEST_USART_MODE_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_mode,
#endif
#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_request_cancel,
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_request_rx_irq,
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_request_tx_irq,
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_rx_baud,
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_rx_bulk_irq,
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_rx_data,
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_rx_fifo_irq,
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_rx_fifo_threshold,
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_rx_irq,
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_rx_mode,
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_rx_timeout,
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_usart_tx_fifo_irq,
#endif
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_wdt_basic,
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_wdt_reboot,
#endif
#if VSF_TEST_WDT_TIMEOUT_ACCURACY_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_wdt_timeout_accuracy,
#endif
#if VSF_TEST_ARCH_SYSTIMER_FREQ_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_arch_systimer_freq,
#endif
#if VSF_TEST_ARCH_PREEMPT_ENABLE == ENABLED
    (const vsf_test_suite_t *)&vsf_test_arch_preempt,
#endif
};
uint8_t vsf_test_suite_count = (uint8_t)(sizeof(vsf_test_suite_list) / sizeof(vsf_test_suite_list[0]));
