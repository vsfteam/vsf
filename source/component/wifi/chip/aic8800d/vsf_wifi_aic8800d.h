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
 *  either express or implied. See the License for the specific language     *
 *  governing permissions and limitations under the License.                 *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_WIFI_AIC8800D_H__
#define __VSF_WIFI_AIC8800D_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_wifi_cfg.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_AIC8800D == ENABLED

#include "../../vsf_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ LOGGING HELPERS ===============================*/

#if VSF_WIFI_CFG_CHIP_AIC8800D_LOG_LEVEL >= 1
#   define vsf_wifi_aic8800d_trace_error(...)    vsf_trace_error(__VA_ARGS__)
#else
#   define vsf_wifi_aic8800d_trace_error(...)    ((void)0)
#endif

#if VSF_WIFI_CFG_CHIP_AIC8800D_LOG_LEVEL >= 2
#   define vsf_wifi_aic8800d_trace_info(...)     vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_aic8800d_trace_info(...)     ((void)0)
#endif

#if VSF_WIFI_CFG_CHIP_AIC8800D_LOG_LEVEL >= 4
#   define vsf_wifi_aic8800d_trace_debug(...)    vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_aic8800d_trace_debug(...)    ((void)0)
#endif

/*============================ TYPES =========================================*/

/* Bus operations supplied by the USB host class driver to the chip driver. */
typedef struct vsf_wifi_aic8800d_bus_ops_t {
    /* Base register-bus vtable. The generic wifi layer only uses on_ready;
     * reg_read/reg_write are NULL because AIC8800D is message-based. */
    vsf_wifi_reg_bus_t base;

    /* Submit a raw bulk OUT buffer (already contains USB header + payload).
     * Completion is fire-and-forget; the bus driver owns the URB pool. */
    vsf_err_t (*send)(vsf_wifi_t *wifi,
                      const uint8_t *data, uint16_t len, vsf_wifi_done_t done);

    /* Return true if at least one TX URB slot is available. */
    bool (*can_send)(vsf_wifi_t *wifi);

    /* Set by the bus driver when the USB device is already running FullMAC
     * firmware (runtime PID after boot-ROM firmware load). The chip driver
     * will skip firmware upload and proceed directly to LMAC init. */
    bool skip_firmware_load;
} vsf_wifi_aic8800d_bus_ops_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_wifi_chip_drv_t vsf_wifi_aic8800d_drv;

/* RX entry point called by the USB bus driver for every completed bulk IN URB. */
void vsf_wifi_aic8800d_on_rx(vsf_wifi_t *wifi, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_AIC8800D == ENABLED */
#endif      /* __VSF_WIFI_AIC8800D_H__ */
