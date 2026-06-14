/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
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

#ifndef __VSF_WIFI_MT76_H__
#define __VSF_WIFI_MT76_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_wifi_cfg.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED

#include "../../vsf_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define MT76_RX_URB_SIZE            2048
#define MT76_TX_URB_SIZE            2048
#define MT76_MCU_RESP_SIZE          2048

#define MT76_EEPROM_SIZE            512

/*============================ TYPES =========================================*/

/* MT76 bus operations.
 *
 * These are chip-level communication primitives, not USB-specific calls.  On
 * USB the bus driver maps them to ep0 vendor requests and bulk URBs; on other
 * buses (e.g. SDIO) the same chip driver would use a different bus_ops
 * implementation that maps to the appropriate host/controller primitives. */
typedef struct vsf_wifi_mt76_bus_ops_t {
    /* Send an MCU command / configuration request.  On USB this becomes an
     * ep0 vendor request; the exact request layout is bus-driver private. */
    vsf_err_t (*mcu_cmd)(vsf_wifi_t *wifi,
        uint8_t req, uint8_t req_type, uint16_t value, uint16_t index,
        void *buf, uint16_t len, vsf_wifi_done_t done);

    /* Submit a TX frame (probe/auth/assoc/data).  On USB this is a bulk OUT
     * transfer on one of the AC endpoints. */
    vsf_err_t (*tx_frame)(vsf_wifi_t *wifi,
        const uint8_t *data, uint16_t len, uint8_t queue_idx);

    /* Start / re-arm a single RX reception.  On USB this is a bulk IN URB. */
    vsf_err_t (*rx_submit)(vsf_wifi_t *wifi,
        uint8_t *buf, uint16_t len, uint8_t queue_idx);
} vsf_wifi_mt76_bus_ops_t;

typedef struct mt76_wifi_priv {
    void                    *bus_priv;

    uint32_t                asic_rev;
    uint8_t                 mac_addr[6];
    uint8_t                 eeprom[MT76_EEPROM_SIZE];

    uint8_t                 mcu_seq;
    uint8_t                 state;

    /* async operation context */
    vsf_wifi_done_t         pending_done;
    uint16_t                pending_cmd;

    /* buffers */
    uint8_t                 rx_buf[MT76_RX_URB_SIZE];
    uint8_t                 tx_buf[MT76_TX_URB_SIZE];
    uint8_t                 mcu_resp_buf[MT76_MCU_RESP_SIZE];
} mt76_wifi_priv_t;

/*============================ LOGGING HELPERS ===============================*/

#if VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL >= 1
#   define vsf_wifi_chip_mt76_trace_error(...)  vsf_trace_error(__VA_ARGS__)
#else
#   define vsf_wifi_chip_mt76_trace_error(...)  ((void)0)
#endif

#if VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL >= 2
#   define vsf_wifi_chip_mt76_trace_info(...)   vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_chip_mt76_trace_info(...)   ((void)0)
#endif

#if VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL >= 4
#   define vsf_wifi_chip_mt76_trace_debug(...)  vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_chip_mt76_trace_debug(...)  ((void)0)
#endif

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_wifi_chip_drv_t vsf_wifi_mt76_drv;

/*============================ PROTOTYPES ====================================*/

extern const uint8_t __mt76_rom_patch_firmware_data[];
extern const uint32_t __mt76_rom_patch_firmware_size;
extern const uint8_t __mt76_main_firmware_data[];
extern const uint32_t __mt76_main_firmware_size;

#ifdef __cplusplus
}
#endif

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
#endif      /* __VSF_WIFI_MT76_H__ */
