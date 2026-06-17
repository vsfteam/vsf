/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.           *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software       *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_WIFI_RT28XX_H__
#define __VSF_WIFI_RT28XX_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_wifi.h"

#if VSF_WIFI_USE_RT28XX == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

/*
 * RT28xx chip-private bus operations.
 *
 * The first member MUST be vsf_wifi_reg_bus_t so the generic wifi layer can
 * treat the structure as a plain register bus.  The USB bus driver embeds the
 * standard register ops here and appends RT28xx-specific primitives (e.g. the
 * USB ep0 vendor_request used for USB_DEVICE_MODE) after the shared part.
 */
typedef struct vsf_wifi_rt28xx_bus_ops_t {
    vsf_wifi_reg_bus_t base;
    vsf_err_t (*vendor_request)(vsf_wifi_t *wifi, uint8_t request,
                                uint16_t value, uint16_t index,
                                vsf_wifi_done_t done);
} vsf_wifi_rt28xx_bus_ops_t;

/*============================ GLOBAL VARIABLES ==============================*/

/*
 * Firmware blob declared by chip/rt28xx/vsf_wifi_rt2870_firmware.c (weak
 * stub).  Link a strong override to ship the real rt2870.bin; the rt28xx
 * chip driver uses size > 0 as the trigger to invoke run_blob during
 * firmware_load.
 */
extern const uint8_t  __rt2870_firmware_data[];
extern const uint32_t __rt2870_firmware_size;

/*============================ LOGGING HELPERS ===============================*/

#if VSF_WIFI_CFG_CHIP_RT28XX_LOG_LEVEL >= 1
#   define vsf_wifi_chip_rt28xx_trace_error(...)   vsf_trace_error(__VA_ARGS__)
#else
#   define vsf_wifi_chip_rt28xx_trace_error(...)   ((void)0)
#endif

#if VSF_WIFI_CFG_CHIP_RT28XX_LOG_LEVEL >= 2
#   define vsf_wifi_chip_rt28xx_trace_info(...)    vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_chip_rt28xx_trace_info(...)    ((void)0)
#endif

#if VSF_WIFI_CFG_CHIP_RT28XX_LOG_LEVEL >= 4
#   define vsf_wifi_chip_rt28xx_trace_debug(...)   vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_chip_rt28xx_trace_debug(...)   ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif      // VSF_WIFI_USE_RT28XX
#endif      // __VSF_WIFI_RT28XX_H__
