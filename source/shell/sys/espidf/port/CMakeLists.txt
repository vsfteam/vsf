# CMakeLists head
#
# port/ - bridge layer between ESP-IDF-shaped public API (include/) and VSF
# internal services (kernel/eda, service/trace, service/fifo, component/mal,
# component/fs, component/tcpip, hal/driver/*, etc.).
#
# Source files are added as each component lands. Every port file is guarded
# internally by its component's VSF_ESPIDF_CFG_USE_* switch, so listing a file
# here is safe even before the component's switch is enabled.

vsf_add_sources(
    esp_err_port.c         # -> pure header + 2 diagnostic helpers
    esp_log_port.c         # -> service/trace (vsf_trace)
    esp_timer_port.c       # -> vsf_callback_timer
    esp_system_port.c      # -> vsf_arch_reset / vsf_heap / PRNG
    esp_ringbuf_port.c     # -> byte ring buffer on vsf_heap
    esp_heap_caps_port.c   # -> vsf_heap (caps collapsed, statistics bridged)
    esp_event_port.c       # -> FreeRTOS shim (xQueue + xTaskCreate)
    esp_partition_port.c   # -> component/mal (mim_mal slices over a user root mal)
    nvs_flash_port.c       # -> partition-backed KV store (custom binary format)
    esp_flash_port.c       # -> component/mal (root mal, no slice, no SPI driver)
    esp_vfs_port.c         # -> POSIX forwarding stub (register APIs return NOT_SUPPORTED)
    esp_littlefs_port.c    # -> component/fs/littlefs via linux POSIX mount
    esp_vfs_fat_port.c     # -> component/fs/fatfs via linux POSIX mount (auto MBR)
    driver_gptimer_port.c  # -> vsf_template_timer.h (caller-supplied vsf_timer_t pool)
    driver_gpio_port.c     # -> vsf_template_gpio.h (global vsf_hw_io_mapper, no cfg injection)
    driver_uart_port.c     # -> vsf_template_usart.h (pool-injected vsf_usart_t, blocking I/O)
    driver_i2c_port.c      # -> vsf_template_i2c.h (new driver API, bus-device model)
    driver_spi_master_port.c # -> vsf_template_spi.h (pool-injected vsf_spi_t, blocking I/O)
    driver_ledc_port.c     # -> vsf_template_pwm.h (pool-injected vsf_pwm_t)
    driver_adc_port.c      # -> vsf_template_adc.h (oneshot only)
    esp_netif_port.c       # -> component/tcpip/netdrv + lwip raw API
    esp_http_client_port.c # -> component/tcpip/protocol/http/client + mbedtls TLS
    usb_host_port.c        # -> component/usb/host (vsf_usbh bridge)
    usb_device_port.c      # -> component/usb/device (vsf_usbd bridge)
    esp_lcd_port.c         # -> component/ui/disp (vsf_disp_t bridge, IO stub + panel vtable)
)

# vsf_add_sources(
#     esp_event_deprecated.c # placeholder
#     driver_uart_port.c     # -> vsf_template_usart.h (moved to active)
#     driver_i2c_port.c      # -> (moved to active)
#     driver_spi_master_port.c # -> (moved to active)
#     driver_ledc_port.c     # -> (moved to active)
#     driver_adc_port.c      # -> (moved to active)
# )
