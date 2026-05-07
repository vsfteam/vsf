# vsf_espidf — ESP-IDF compatibility sub-system for VSF

A sibling of `shell/sys/linux/` (vsf_linux). Provides an ESP-IDF-shaped
public API on top of VSF kernel / service / component / HAL layers so that
applications written against ESP-IDF can be compiled and run on VSF-hosted
targets.

## Scope

This sub-system does **not** embed any ESP-IDF source code. All public
headers under `include/` are clean-room re-implementations of the
ESP-IDF public API. Implementations live under `port/` and are thin
bridges onto existing VSF facilities.

VSF espidf targets ESP-IDF **v5.1.0** baseline and covers roughly
**5–10%** of the full ESP-IDF component surface. It focuses on the
software core needed to run typical sensor / I/O / networking
applications, while wireless radios, advanced peripherals, security,
and power management are not in scope (see [Gap Analysis](#gap-analysis)).

## Layout

```
espidf/
├── CMakeLists.txt             # sub-system build entry, gated by VSF_USE_ESPIDF
├── vsf_espidf.h               # internal sub-system header
├── vsf_espidf_cfg.h           # feature switches + IDF baseline version
├── vsf_espidf.c               # sub-system init + per-module init chain
├── README.md                  # <-- this file
├── include/                   # clean-room ESP-IDF public headers (34 files)
│   ├── README.md
│   ├── driver/                # driver/*.h
│   └── usb/                   # usb/*.h
└── port/                      # ESP-IDF-API -> VSF-internal glue (25 port files)
    └── CMakeLists.txt
```

## Implemented Modules

### Stage 1 — Pure-software core (all code ✅)

| Module | VSF Backend | Port File | Lines | Notes |
|---|---|---|---|---|
| `esp_err.h` | header-only + diagnostic helpers | `esp_err_port.c` | 126 | |
| `esp_log.h` | `service/trace` | `esp_log_port.c` | 198 | |
| `esp_event.h` | FreeRTOS shim (`xQueue` + `xTaskCreate`) | `esp_event_port.c` | 500 | no deprecated event API |
| `esp_timer.h` | `vsf_callback_timer` | `esp_timer_port.c` | 172 | |
| `esp_system.h` | VSF reset + entropy + heap stats | `esp_system_port.c` | 226 | |
| `esp_ringbuf.h` | byte ring buffer on vsf_heap | `esp_ringbuf_port.c` | 217 | bytebuf only; item framing planned |
| `esp_heap_caps.h` | `service/heap` (caps collapsed) | `esp_heap_caps_port.c` | 249 | no heap tracing or multi_heap |

### Stage 2 — Storage & filesystem

| Module | VSF Backend | Port File | Lines | Notes |
|---|---|---|---|---|
| `esp_partition.h` | `component/mal` (mim_mal slices) | `esp_partition_port.c` | 515 | `munmap()` → NOT_SUPPORTED |
| `esp_flash.h` | `component/mal` (root mal) | `esp_flash_port.c` | 257 | no encrypted r/w; no SPI chip drivers |
| `nvs.h` / `nvs_flash.h` | custom binary format on partition | `nvs_flash_port.c` | 796 | `nvs_sec_provider` not bridged |
| `esp_vfs.h` | POSIX forwarding stubs | `esp_vfs_port.c` | 50 | register/unregister → NOT_SUPPORTED |
| `esp_littlefs.h` | `component/fs/littlefs` via POSIX | `esp_littlefs_port.c` | 241 | some ops NOT_SUPPORTED |
| `esp_vfs_fat.h` | `component/fs/fatfs` via POSIX | `esp_vfs_fat_port.c` | 259 | no RO mount on spiflash |

### Stage 3 — Network

| Module | VSF Backend | Port File | Lines | Notes |
|---|---|---|---|---|
| `esp_netif.h` | `component/tcpip/netdrv` + lwIP raw | `esp_netif_port.c` | 694 | DHCP client + IP events only |
| `esp_http_client.h` | `component/tcpip/protocol/http/client` + mbedTLS | `esp_http_client_port.c` | 1475 | 42 APIs; 3-feature profile config |

### Stage 4 — Peripheral drivers (code ✅, on-target ⬜)

| Driver | Port File | Lines | Notes |
|---|---|---|---|
| `driver/gptimer.h` | `driver_gptimer_port.c` | 374 | pool-injected; no etm / capture / alarm_action |
| `driver/gpio.h` | `driver_gpio_port.c` | 527 | global io_mapper; ISR / intr → NOT_SUPPORTED |
| `driver/uart.h` | `driver_uart_port.c` | 752 | pool-injected; no pin muxing / pattern detect |
| `driver/i2c_master.h` | `driver_i2c_port.c` | 516 | v5.2 new API only; no slave mode |
| `driver/spi_master.h` | `driver_spi_master_port.c` | 673 | pool-injected; master only |
| `driver/ledc.h` | `driver_ledc_port.c` | 433 | pool-injected; no fade hardware |
| `driver/adc_oneshot.h` | `driver_adc_port.c` | 266 | oneshot only; no continuous / DMA / cali |

### Stage 5 — USB

| Module | VSF Backend | Port File | Lines | Notes |
|---|---|---|---|---|
| `usb_host.h` | `component/usb/host` (vsf_usbh) | `usb_host_port.c` | 638 | |
| `usb_device.h` | `component/usb/device` (vsf_usbd) | `usb_device_port.c` | 550 | |

### Stage 6 — LCD panel

| Module | VSF Backend | Port File | Lines | Notes |
|---|---|---|---|---|
| `esp_lcd_types.h` | header-only types | `esp_lcd_port.c` | 360+ | handle types, config structs, callbacks |
| `esp_lcd_panel_io.h` | config carrier (stub vtable) | `esp_lcd_port.c` | — | IO layer stores transport config; tx/rx are no-ops |
| `esp_lcd_panel_ops.h` | `vk_disp_t` bridge | `esp_lcd_port.c` | — | dispatch to vk_disp_init/refresh/fini |
| `esp_lcd_io_spi.h` | SPI IO config + factory | `esp_lcd_port.c` | — | records CS/DC/pclk/spi_mode |
| `esp_lcd_io_i2c.h` | I2C IO config + factory | `esp_lcd_port.c` | — | records dev_addr/scl_speed |
| `esp_lcd_panel_ssd1306.h` | panel factory | `esp_lcd_port.c` | — | creates 128×64 panel wrapping vk_disp_t |
| `esp_lcd_panel_st7789.h` | panel factory | `esp_lcd_port.c` | — | creates 240×320 panel wrapping vk_disp_t |

Architecture: ESP-IDF's two-layer (IO + Panel) pattern is preserved at the
API level, but the IO vtable is all no-ops and the panel vtable dispatches
to a `vk_disp_t` supplied by the board layer via `vsf_espidf_cfg_t::lcd_disp`.
The board owns the actual display driver (wingdi / ST7789-SPI / SDL2 / etc.)
and the espidf LCD shim is agnostic to the concrete driver type.

## Backend mapping

| ESP-IDF API | VSF backend |
|---|---|
| `esp_err.h` / `esp_check.h` | header-only |
| `esp_log.h` (ESP_LOGx) | `service/trace` + `vsf_trace` |
| `esp_event.h` | `kernel/vsf_eda` |
| `esp_timer.h` | `kernel/vsf_timq` + `vsf_callback_timer` |
| `esp_system.h` (restart/reset_reason/random) | VSF reset + entropy |
| xRingbuffer (`esp_ringbuf.h`) | `service/fifo`, `service/pbuf`, `service/simple_stream` |
| `esp_heap_caps.h` | `service/heap` + `service/pool` (caps collapsed) |
| `esp_partition.h` | `component/mal` (`vsf_mal`) |
| `nvs_flash.h` / `nvs.h` | KV layered on partition or littlefs |
| `esp_vfs.h` + POSIX I/O | `component/fs` + `shell/sys/linux` VFS |
| FatFs / LittleFS / RomFS | `component/fs/driver/{fatfs,littlefs,romfs}` |
| lwIP + BSD sockets | `component/3rd-party/lwip` + `component/tcpip/socket` |
| mbedtls | `component/3rd-party/mbedtls` |
| `esp_http_client.h` | `component/tcpip/protocol/http/client` |
| cJSON / esp_json | not shimmed — applications include cJSON directly |
| pthread | `shell/sys/linux` pthread |
| FreeRTOS FreeRTOS.h, queue, event_groups | VSF FreeRTOS compat layer |
| `esp_netif.h` | `component/tcpip/netdrv` + lwIP glue |
| `driver/gptimer` | `hal/driver/common/template/vsf_template_timer.h` (pool-injected) |
| `driver/gpio` | `hal/driver/common/template/vsf_template_gpio.h` (global io_mapper) |
| `driver/uart` | `hal/driver/common/template/vsf_template_usart.h` (pool-injected) |
| `driver/i2c_master` | `hal/driver/common/template/vsf_template_i2c.h` |
| `driver/spi_master` | `hal/driver/common/template/vsf_template_spi.h` (pool-injected) |
| `driver/ledc` | `hal/driver/common/template/vsf_template_pwm.h` (pool-injected) |
| `driver/adc_oneshot` | `hal/driver/common/template/vsf_template_adc.h` |
| `usb_host.h` | `component/usb/host` (vsf_usbh) |
| `usb_device.h` | `component/usb/device` (vsf_usbd) |
| `esp_lcd_*.h` (IO + panel) | `component/ui/disp` (vsf_disp_t bridge) |

## Enablement

Master switch `VSF_USE_ESPIDF` plus per-component `VSF_ESPIDF_CFG_USE_*`
switches in `vsf_espidf_cfg.h`. All disabled by default except core
infrastructure; opt in per module as needed.

```
VSF_ESPIDF_CFG_USE_LOG               (core, default ON)
VSF_ESPIDF_CFG_USE_ERR               (core, default ON)
VSF_ESPIDF_CFG_USE_EVENT             (core, default ON)
VSF_ESPIDF_CFG_USE_TIMER             (core, default ON)
VSF_ESPIDF_CFG_USE_SYSTEM            (core, default ON)
VSF_ESPIDF_CFG_USE_RINGBUF           (core, default ON)
VSF_ESPIDF_CFG_USE_HEAP_CAPS         (core, default ON)
VSF_ESPIDF_CFG_USE_PARTITION         (storage)
VSF_ESPIDF_CFG_USE_NVS               (storage)
VSF_ESPIDF_CFG_USE_ESP_FLASH         (storage)
VSF_ESPIDF_CFG_USE_VFS               (storage)
VSF_ESPIDF_CFG_USE_LITTLEFS          (storage)
VSF_ESPIDF_CFG_USE_FATFS             (storage)
VSF_ESPIDF_CFG_USE_NETIF             (network)
VSF_ESPIDF_CFG_USE_HTTP_CLIENT       (network)
VSF_ESPIDF_CFG_USE_HTTP_SERVER       (disabled — NOT IMPLEMENTED)
VSF_ESPIDF_CFG_USE_WIFI              (disabled — NOT IMPLEMENTED)
VSF_ESPIDF_CFG_USE_DRIVER_GPIO       (peripheral)
VSF_ESPIDF_CFG_USE_DRIVER_UART       (peripheral)
VSF_ESPIDF_CFG_USE_DRIVER_I2C        (peripheral)
VSF_ESPIDF_CFG_USE_DRIVER_SPI_MASTER (peripheral)
VSF_ESPIDF_CFG_USE_DRIVER_LEDC       (peripheral)
VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER    (peripheral)
VSF_ESPIDF_CFG_USE_DRIVER_ADC        (peripheral)
VSF_ESPIDF_CFG_USE_USB_HOST          (usb)
VSF_ESPIDF_CFG_USE_USB_DEVICE        (usb)
VSF_ESPIDF_CFG_USE_LCD               (display)
```

## Roadmap

Stage 1 (pure-software core, no HW dependency):
`esp_err` → `esp_log` → `esp_event` → `esp_timer` → `esp_system` →
`esp_ringbuf` → `esp_heap_caps`.

Stage 2 (storage & fs): `esp_partition` → `nvs_flash` → `esp_vfs`.

Stage 3 (network): `esp_netif` → `esp_http_client` / `esp_http_server` →
(optional) MQTT via upstream Paho.

Stage 4 (peripheral drivers). Status is tracked on two independent
axes: **code** (shim landed in tree) and **on-target** (verified on real
MCU hardware such as at32f405). Host build under `project/vc.espidf/`
does **not** satisfy on-target verification.

| Driver | Code | On-target | Notes |
|---|---|---|---|
| `driver/gptimer`    | ✅ | ⬜ | pool-injected via `vsf_espidf_cfg_t::gptimer` |
| `driver/gpio`       | ✅ | ⬜ | global `vsf_hw_io_mapper`, no cfg injection |
| `driver/uart`       | ✅ | ⬜ | pool-injected; `usart_stream` ring-buffer backend |
| `driver/i2c`        | ✅ | ⬜ | v5.2 new API only; legacy `driver/i2c.h` deferred |
| `driver/spi_master` | ✅ | ⬜ | pool-injected |
| `driver/ledc`       | ✅ | ⬜ | — |
| `driver/adc`        | ✅ | ⬜ | oneshot only |

A driver is considered fully **done** only when both axes are ✅.

Stage 5 (radio, optional): external Wi-Fi module `esp_wifi` semantic shim.

## Gap Analysis

Overview of ESP-IDF features that are **not implemented** in VSF espidf
as of this writing.

### Completely missing major subsystems

| Category | Missing ESP-IDF components | Notes |
|---|---|---|
| Wi-Fi | `esp_wifi`, `esp_now`, `esp_smartconfig`, `esp_mesh` | requires external Wi-Fi module |
| BT / BLE | `bt` (host/controller/BLE/BLE Mesh/BLE Audio) | requires external HCI |
| 802.15.4 / Thread | `ieee802154`, `openthread` | |
| Ethernet | `esp_eth` (MAC, PHY, netif glue) | |
| HTTP Server | `esp_http_server`, `esp_https_server` | config switch exists but always DISABLED |
| OTA | `app_update` (`esp_ota_ops.h`), `esp_https_ota` | |
| Security | `esp_security` (DS/HMAC/Key Manager), `bootloader_support` (secure boot / flash encrypt), `efuse`, `nvs_sec_provider` | |
| Power Management | `esp_pm` (DFS / light sleep), `esp_hw_support` (sleep / intr_alloc / CPU / clk / rtc / pmu) | |
| RF PHY | `esp_phy` (calibration / control), `esp_coex` (coexistence) | |
| PSRAM | `esp_psram`, `esp_mm` (cache / MMU) | |
| SPIFFS | `spiffs` (`esp_spiffs.h`) | |
| Wear Levelling | `wear_levelling` | VSF uses `component/mal` directly |
| LCD | `esp_lcd` | IO + ST7789/SSD1306 vendor panels bridged to vk_disp_t; RGB/DPI/I80 not bridged |
| HID | `esp_hid` (HID host/device over BLE/USB) | |
| Camera | `esp_driver_cam` | |
| GDB / Tracing | `esp_gdbstub`, `espcoredump`, `app_trace`, `esp_trace`, `perfmon` | |
| TEE | `esp_tee` (Trusted Execution Environment) | |
| Protocomm | `protocomm`, `esp_local_ctrl` | Wi-Fi provisioning infrastructure |
| TCP Transport | `tcp_transport` (TCP/SSL/WS transport abstraction) | |

### Missing peripheral drivers (23 esp_driver_* components)

| Driver | Function | Driver | Function |
|---|---|---|---|
| `ana_cmpr` | Analog comparator | `parlio` | Parallel I/O |
| `bitscrambler` | Bit scrambler | `pcnt` | Pulse counter |
| `cam` | Camera controller | `ppa` | Pixel Processing Accelerator |
| `cordic` | CORDIC co-processor | `rmt` | Remote control (IR) |
| `dac` | DAC (oneshot / continuous / cosine) | `sdm` | Sigma-Delta Modulation |
| `dma` | GDMA / 2D-DMA / async memcpy | `sdio_slave` | SDIO slave |
| `i2s` | I2S (standard / PDM / TDM) | `sdmmc` | SD/MMC host (native) |
| `i3c` | I3C bus | `sdspi` | SD/MMC over SPI |
| `isp` | Image Signal Processor | `touch_sens` | Touch sensor |
| `jpeg` | JPEG codec | `tsens` | Temperature sensor |
| `mcpwm` | Motor Control PWM | `twai` | CAN bus (TWAI) |
| `usb_serial_jtag` | USB Serial/JTAG console | | |

### Missing sub-features in implemented drivers

| Driver | Implemented | Missing sub-APIs |
|---|---|---|
| GPIO | `gpio.h` | `rtc_io.h`, `lp_io.h`, `dedic_gpio.h`, `gpio_etm.h`, `gpio_filter.h`, ISR/intr |
| GPTimer | `gptimer.h` | `gptimer_etm.h`, `get_resolution()`, `get_captured_count()`, `set_alarm_action()` |
| I2C | `i2c_master.h` | `i2c_slave.h`; legacy `driver/i2c.h` |
| SPI | `spi_master.h` | `spi_common.h`, `spi_slave.h`, `spi_slave_hd.h` |
| UART | `uart.h` | `uart_select.h`, `uart_vfs.h`, `uart_wakeup.h`, `uhci.h`, pin muxing |
| LEDC | `ledc.h` | `ledc_etm.h`, `ledc_fade_func_install()` → NOT_SUPPORTED |
| ADC | `adc_oneshot.h` | `adc_continuous.h`, `adc_dma.h`, `adc_cali.h`, `adc_filter.h`, `adc_monitor.h` |

### Known limitations in implemented modules

| Module | Limitation |
|---|---|
| `esp_vfs` | `esp_vfs_register()` / `esp_vfs_unregister()` / `esp_vfs_register_fd_range()` are stubs returning `ESP_ERR_NOT_SUPPORTED`. VSF uses native VFS via `vk_fs_mount`. |
| `esp_flash` | Encrypted read/write returns `ESP_ERR_NOT_SUPPORTED`. No SPI flash chip drivers. No memory-mapped access. |
| `esp_partition` | `esp_partition_munmap()` returns `ESP_ERR_NOT_SUPPORTED` (VSF mim_mal lacks `VSF_MAL_LOCAL_BUFFER`). |
| `esp_ringbuf` | Item-framed ring buffers (NOSPLIT/ALLOWSPLIT) are treated as byte buffers; per-message length prefix not yet implemented. |
| `esp_netif` | DHCP server, PPP, Bridge, SLAAC return `ESP_ERR_NOT_SUPPORTED`. No `esp_netif_sntp.h`. |
| `esp_http_client` | `get_chunk_length()`, `write()`, `open()`, `fetch_headers()`, `flush_response()`, `get_and_clear_last_tls_error()` return `ESP_ERR_NOT_SUPPORTED`. Async mode not implemented. Digest auth only in FULL profile. |
| `esp_event` | No deprecated event loop API (`esp_event_loop.h`). |
| `esp_system` | Missing `esp_debug_helpers.h`, `esp_task_wdt.h`, `esp_ipc.h`, `esp_pm.h`. |
| `esp_timer` | No alarm/wakeup timer. |
| `nvs_flash` | Missing `nvs_bootloader.h`, `nvs_sec_provider.h`. NVS init not yet in main init chain (TODO in `vsf_espidf.c:149`). |
| `usb_host` | `usb_host_lib_handle_events()` variant returns `ESP_ERR_NOT_SUPPORTED`. |

### Missing ESP-IDF infrastructure layers

- **HAL layer** — All 35 `esp_hal_*`components (e.g. `esp_hal_gpio`, `esp_hal_uart`) are not implemented, and are NOT needed by this subsystem. VSF's espidf compatibility layer targets the `driver/*.h` API level (application-facing APIs such as `gpio_set_level()`, `uart_write_bytes()`), not the HAL `_ll.h` level that `esp_hal_*` provides. The HAL role is fulfilled by VSF's own `hal/driver/common/template/vsf_template_*.h` abstraction, which each `driver/*.h` port header bridges to directly. Application code using this subsystem never includes `hal/*.h` headers, so the absence of `esp_hal_*` is a deliberate architectural choice, not a gap.
- **esp_common** — Only `esp_err.h` is provided; missing `esp_check.h`, `esp_compiler.h`, `esp_attr.h`, `esp_macros.h`, `esp_idf_version.h`, `esp_assert.h`.
- **esp_log extensions** — Missing `esp_log_buffer.h`, `esp_log_color.h`, `esp_log_timestamp.h`, etc.
- **esp_heap_caps extensions** — Missing `esp_heap_caps_init.h`, `esp_heap_task_info.h`, `esp_heap_trace.h`, `multi_heap.h`.
- **esp_vfs extensions** — Missing `esp_vfs_dev.h`, `esp_vfs_eventfd.h`, `esp_vfs_semihost.h`.
- **soc** — Missing all SoC capability / peripheral definition headers.

## Validation

Each stage validated by compiling and running matching
`esp-idf/examples/` trees built against this sub-system's headers
(no ESP-IDF source in the build).
