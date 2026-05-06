/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_ESPIDF_INTERNAL_H__
#define __VSF_ESPIDF_INTERNAL_H__

/*============================ INCLUDES ======================================*/

#include "kernel/vsf_kernel.h"
#include "hal/driver/driver.h"
#include "./vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED

#if VSF_ESPIDF_CFG_USE_PARTITION == ENABLED
#   include "component/mal/vsf_mal.h"
#   include "./include/esp_partition.h"
#endif

#if VSF_ESPIDF_CFG_USE_ESP_FLASH == ENABLED
#   include "component/mal/vsf_mal.h"
#   include "./include/esp_flash.h"
#endif

#if VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED
#   include "component/usb/host/vsf_usbh.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

// Sub-system configuration (pure input). The ESP-IDF compatibility shim is
// a process-wide singleton (esp_* APIs are global functions without an
// instance handle), so configuration is passed by value: the caller fills
// a cfg struct, hands it to vsf_espidf_init(), and the shim copies the
// fields it needs into its own internal state. The cfg object does not
// need to outlive the init call and may live on the stack.
//
// Field semantics:
//   rng    Externally owned RNG source (commonly a vsf_multiplex_rng_t
//          instance cast to vsf_rng_t*). NULL -> the shim falls back to a
//          software PRNG for esp_random / esp_fill_random.
#if VSF_ESPIDF_CFG_USE_PARTITION == ENABLED
// One entry in the compile-time partition table. The shim slices the
// configured root_mal at [offset, offset+size) into a vk_mim_mal_t on
// boot and publishes it as a matching esp_partition_t. All offsets and
// sizes are byte units; erase_size defaults to the root mal's natural
// erase block when set to 0.
typedef struct vsf_espidf_partition_entry_t {
    const char             *label;
    uint8_t                 type;         // esp_partition_type_t
    uint8_t                 subtype;      // esp_partition_subtype_t
    uint8_t                 encrypted : 1;
    uint8_t                 readonly  : 1;
    uint32_t                offset;
    uint32_t                size;
    uint32_t                erase_size;   // 0 -> auto-detect via vk_mal_blksz
} vsf_espidf_partition_entry_t;

// Partition sub-system configuration. root_mal is user-owned and MUST
// outlive the sub-system. The shim never allocates, opens or closes
// root_mal on behalf of the caller; the expected contract is that the
// caller has already called vk_mal_init() on it from a stack-owner
// thread (vsf_thread_t) so that it is in a usable state before being
// handed over.
typedef struct vsf_espidf_partition_cfg_t {
    vk_mal_t                                 *root_mal;
    const vsf_espidf_partition_entry_t       *entries;
    uint16_t                                  entry_count;
} vsf_espidf_partition_cfg_t;
#endif

#if VSF_ESPIDF_CFG_USE_DRIVER_UART == ENABLED
// uart sub-system configuration.
//
// Same pool-injection pattern as gptimer: the caller provides an array
// of vsf_usart_t* instances. uart_port_t values (UART_NUM_0, 1, ...)
// index directly into the pool.
typedef struct vsf_espidf_uart_cfg_t {
    vsf_usart_t *const         *pool;
    uint16_t                    pool_count;
} vsf_espidf_uart_cfg_t;
#endif

#if VSF_ESPIDF_CFG_USE_DRIVER_I2C == ENABLED
// i2c sub-system configuration.
//
// Same pool-injection pattern as gptimer / uart: the caller provides an
// array of vsf_i2c_t* instances. i2c_port_num_t values (0, 1, ...)
// index directly into the pool.
typedef struct vsf_espidf_i2c_cfg_t {
    vsf_i2c_t *const           *pool;
    uint16_t                    pool_count;
} vsf_espidf_i2c_cfg_t;
#endif

#if VSF_ESPIDF_CFG_USE_DRIVER_SPI_MASTER == ENABLED
// spi_master sub-system configuration.
//
// Same pool-injection pattern as gptimer / uart / i2c: the caller
// provides an array of vsf_spi_t* instances. spi_host_device_t values
// (SPI1_HOST, SPI2_HOST, ...) index directly into the pool.
typedef struct vsf_espidf_spi_cfg_t {
    vsf_spi_t *const           *pool;
    uint16_t                    pool_count;
} vsf_espidf_spi_cfg_t;
#endif

#if VSF_ESPIDF_CFG_USE_DRIVER_LEDC == ENABLED
// ledc sub-system configuration.
//
// Pool index equals ledc_mode_t: entry 0 -> LEDC_LOW_SPEED_MODE,
// entry 1 (optional) -> LEDC_HIGH_SPEED_MODE. Each vsf_pwm_t is
// shared by all channels of its speed_mode.
typedef struct vsf_espidf_ledc_cfg_t {
    vsf_pwm_t *const           *pool;
    uint16_t                    pool_count;
} vsf_espidf_ledc_cfg_t;
#endif

#if VSF_ESPIDF_CFG_USE_DRIVER_ADC == ENABLED
// adc (oneshot) sub-system configuration.
//
// Pool index equals adc_unit_t: entry 0 -> ADC_UNIT_1, entry 1 ->
// ADC_UNIT_2. Only one-shot mode is bridged; callers requiring
// continuous/DMA mode will receive ESP_ERR_NOT_SUPPORTED.
typedef struct vsf_espidf_adc_cfg_t {
    vsf_adc_t *const           *pool;
    uint16_t                    pool_count;
} vsf_espidf_adc_cfg_t;
#endif

#if VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER == ENABLED
// gptimer sub-system configuration.
//
// ESP-IDF's gptimer_new_timer() allocates a timer handle opaquely
// (the caller does not choose which hardware timer to use). To bridge
// this onto VSF -- where each vsf_timer_t* is board/MCU-owned and
// statically defined at BSP time -- the caller hands in an array of
// pointers to vsf_timer_t instances that the shim may draw from.
//
//   pool        Array of vsf_timer_t* already brought up by the board
//               BSP. The shim neither initialises nor finalises these
//               instances itself; it only calls vsf_timer_init() /
//               vsf_timer_fini() on them when a gptimer handle is
//               allocated / released. Ownership stays with the caller.
//   pool_count  Number of entries in pool.
//
// A zero-initialised cfg (pool=NULL, pool_count=0) disables gptimer_*
// allocation entirely; gptimer_new_timer() then returns
// ESP_ERR_INVALID_STATE.
typedef struct vsf_espidf_gptimer_cfg_t {
    vsf_timer_t *const         *pool;
    uint16_t                    pool_count;
} vsf_espidf_gptimer_cfg_t;
#endif

#if VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED
// USB Host sub-system configuration.
//
// The board layer provides a pre-configured vk_usbh_t with the HCD
// driver, PHY setup, interrupt routing etc. already set according to
// the actual hardware environment. The espidf shim adds the ESP-IDF
// bridge class driver on top and manages the client/device model.
//
//   usbh  Board-owned vk_usbh_t instance. NULL -> usb_host_install()
//         returns ESP_ERR_INVALID_STATE.
typedef struct vsf_espidf_usb_host_cfg_t {
    vk_usbh_t                  *usbh;
} vsf_espidf_usb_host_cfg_t;
#endif

typedef struct vsf_espidf_cfg_t {
#if VSF_HAL_USE_RNG == ENABLED
    vsf_rng_t *rng;
#endif
#if VSF_ESPIDF_CFG_USE_PARTITION == ENABLED
    vsf_espidf_partition_cfg_t partition;
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER == ENABLED
    vsf_espidf_gptimer_cfg_t   gptimer;
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_UART == ENABLED
    vsf_espidf_uart_cfg_t      uart;
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_I2C == ENABLED
    vsf_espidf_i2c_cfg_t       i2c;
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_SPI_MASTER == ENABLED
    vsf_espidf_spi_cfg_t       spi;
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_LEDC == ENABLED
    vsf_espidf_ledc_cfg_t      ledc;
#endif
#if VSF_ESPIDF_CFG_USE_DRIVER_ADC == ENABLED
    vsf_espidf_adc_cfg_t       adc;
#endif
#if VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED
    vsf_espidf_usb_host_cfg_t  usb_host;
#endif
} vsf_espidf_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

// Sub-system lifecycle. Idempotent: subsequent calls are no-ops. Passing
// NULL is permitted and equivalent to a zero-initialised cfg (all
// peripherals left unset, shim uses built-in fallbacks).
extern void vsf_espidf_init(const vsf_espidf_cfg_t *cfg);

#if VSF_HAL_USE_RNG == ENABLED
// Accessor used by internal port code. Returns the RNG source registered
// via vsf_espidf_init(), or NULL if none was provided (callers should
// fall back to their own default).
extern vsf_rng_t * vsf_espidf_get_rng(void);
#endif

#if VSF_ESPIDF_CFG_USE_USB_HOST == ENABLED
extern vk_usbh_t * vsf_espidf_get_usbh(void);
#endif

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_ESPIDF
#endif      // __VSF_ESPIDF_INTERNAL_H__
