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

/*
 * Clean-room re-implementation of ESP-IDF public API "esp_system.h"
 * (including the stripped-down chip info / reset surface exposed via
 * esp_chip_info.h, esp_mac.h and esp_random.h).
 *
 * Authored from ESP-IDF v5.x public API only. No code or data copied from
 * the ESP-IDF source tree.
 */

#ifndef __VSF_ESPIDF_ESP_SYSTEM_H__
#define __VSF_ESPIDF_ESP_SYSTEM_H__

#include <stdint.h>
#include <stddef.h>

#include "utilities/compiler/compiler.h"

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

typedef enum {
    ESP_RST_UNKNOWN     = 0,    /*!< Reset reason could not be determined     */
    ESP_RST_POWERON     = 1,    /*!< Reset due to power-on event              */
    ESP_RST_EXT         = 2,    /*!< Reset by external pin                    */
    ESP_RST_SW          = 3,    /*!< Software reset via esp_restart           */
    ESP_RST_PANIC       = 4,    /*!< Software reset due to exception/panic    */
    ESP_RST_INT_WDT     = 5,    /*!< Reset due to interrupt watchdog          */
    ESP_RST_TASK_WDT    = 6,    /*!< Reset due to task watchdog               */
    ESP_RST_WDT         = 7,    /*!< Reset due to other watchdogs             */
    ESP_RST_DEEPSLEEP   = 8,    /*!< Reset after exiting deep sleep mode      */
    ESP_RST_BROWNOUT    = 9,    /*!< Brownout reset                           */
    ESP_RST_SDIO        = 10,   /*!< Reset over SDIO                          */
    ESP_RST_USB         = 11,   /*!< Reset by USB peripheral                  */
    ESP_RST_JTAG        = 12,   /*!< Reset by JTAG                            */
    ESP_RST_EFUSE       = 13,   /*!< Reset due to efuse error                 */
    ESP_RST_PWR_GLITCH  = 14,   /*!< Reset due to power glitch detected       */
    ESP_RST_CPU_LOCKUP  = 15,   /*!< Reset due to CPU lock up                 */
} esp_reset_reason_t;

typedef enum {
    CHIP_ESP32      = 1,
    CHIP_ESP32S2    = 2,
    CHIP_ESP32S3    = 9,
    CHIP_ESP32C3    = 5,
    CHIP_ESP32C2    = 12,
    CHIP_ESP32C6    = 13,
    CHIP_ESP32H2    = 16,
    /* VSF host: a synthetic "not a chip" id so downstream code can branch. */
    CHIP_VSF_HOST   = 0xFF,
} esp_chip_model_t;

/* Bitmask features exposed through esp_chip_info_t::features. */
#define CHIP_FEATURE_EMB_FLASH      (1 << 0)
#define CHIP_FEATURE_WIFI_BGN       (1 << 1)
#define CHIP_FEATURE_BLE            (1 << 4)
#define CHIP_FEATURE_BT             (1 << 5)
#define CHIP_FEATURE_IEEE802154     (1 << 6)
#define CHIP_FEATURE_EMB_PSRAM      (1 << 7)

typedef struct {
    esp_chip_model_t    model;          /*!< Chip model identifier            */
    uint32_t            features;       /*!< Bit mask of chip features        */
    uint16_t            revision;       /*!< Chip revision (major*100+minor)  */
    uint8_t             cores;          /*!< Number of CPU cores              */
} esp_chip_info_t;

/*============================ PROTOTYPES ====================================*/

/* Reboot via vsf_arch_reset(). Never returns. */
VSF_CAL_NO_RETURN void esp_restart(void);

/* Last reset reason. The VSF port records ESP_RST_SW when a reboot is
 * initiated via esp_restart(); otherwise returns ESP_RST_POWERON. */
esp_reset_reason_t esp_reset_reason(void);

/* Best-effort free heap size reported by vsf_heap_statistics(). Returns 0
 * if statistics are disabled at VSF build time. */
uint32_t esp_get_free_heap_size(void);

/* On VSF there is no dedicated low-water tracking; returns the same value
 * as esp_get_free_heap_size(). */
uint32_t esp_get_minimum_free_heap_size(void);

/* LCG-based PRNG. Not cryptographically secure - ESP-IDF public contract
 * also does not promise CS quality here, it merely requires uniformity. */
uint32_t esp_random(void);

/* Fill an arbitrary buffer with pseudo-random bytes. */
void esp_fill_random(void *buf, size_t len);

/* Version string compatible with ESP-IDF semantics: "v<M>.<m>.<p>-vsf". */
const char * esp_get_idf_version(void);

/* Lightweight clone of esp_chip_info(). The VSF port declares itself as
 * CHIP_VSF_HOST with single core and no wireless features. */
void esp_chip_info(esp_chip_info_t *out_info);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_SYSTEM_H__
