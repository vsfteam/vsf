/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

/*============================ INCLUDES ======================================*/
//! \note do not move this pre-processor statement to other places
#include "component/vsf_component_cfg.h"

#ifndef __VSF_WIFI_CFG_H__
#define __VSF_WIFI_CFG_H__

/*============================ MACROS ========================================*/

#ifndef VSF_USE_WIFI
#   define VSF_USE_WIFI                 DISABLED
#endif

#ifndef VSF_WIFI_ASSERT
#   define VSF_WIFI_ASSERT              VSF_ASSERT
#endif

/*
 * Chip driver enable switches.
 *
 * Each chip family has its own ENABLE switch so applications can choose
 * which chip drivers to compile in.  The switches are bus-agnostic; the
 * same chip driver object can be used by USB / SDIO / SPI shims.
 *
 * A bus shim (e.g. vsf_usbh_wifi_rt2x00) MUST require at least one chip driver
 * to be enabled when the bus class itself is enabled.
 */
#ifndef VSF_WIFI_USE_RT28XX
#   define VSF_WIFI_USE_RT28XX          ENABLED
#endif

#ifndef VSF_WIFI_USE_MT76
#   define VSF_WIFI_USE_MT76            ENABLED
#endif

/*
 * Per-module log level.
 *
 *   0 = silent    : no trace output (assertions still fire)
 *   1 = error     : runtime errors only
 *   2 = info      : key life-cycle events (ready, link up/down, scan done,
 *                   connect success/fail, DHCP bound)
 *   3 = verbose   : normal runtime logs (TX build, reg script steps,
 *                   single URB completion)
 *   4 = debug     : per-frame/beacon/RX delivery counters
 *
 * Sub-module macros fall back to VSF_WIFI_CFG_LOG_LEVEL when not defined.
 */
#ifndef VSF_WIFI_CFG_LOG_LEVEL
#   define VSF_WIFI_CFG_LOG_LEVEL           2
#endif
#ifndef VSF_WIFI_CFG_CHIP_RT28XX_LOG_LEVEL
#   define VSF_WIFI_CFG_CHIP_RT28XX_LOG_LEVEL   VSF_WIFI_CFG_LOG_LEVEL
#endif
#ifndef VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL
#   define VSF_WIFI_CFG_CHIP_MT76_LOG_LEVEL      VSF_WIFI_CFG_LOG_LEVEL
#endif
#ifndef VSF_WIFI_CFG_BUS_RT2X00_LOG_LEVEL
#   define VSF_WIFI_CFG_BUS_RT2X00_LOG_LEVEL    VSF_WIFI_CFG_LOG_LEVEL
#endif
#ifndef VSF_WIFI_CFG_NETDRV_LOG_LEVEL
#   define VSF_WIFI_CFG_NETDRV_LOG_LEVEL        VSF_WIFI_CFG_LOG_LEVEL
#endif

/*
 * WPA2-PSK (CCMP) support switch.
 *
 * The WPA software stack (PBKDF2 / PRF / EAPOL MIC / AES key-unwrap / CCMP)
 * is implemented on top of mbedtls, so it can only be enabled when mbedtls is
 * compiled in.  Defaults to follow VSF_USE_MBEDTLS.
 */
#ifndef VSF_WIFI_USE_WPA
#   if defined(VSF_USE_MBEDTLS) && (VSF_USE_MBEDTLS == ENABLED)
#       define VSF_WIFI_USE_WPA         ENABLED
#   else
#       define VSF_WIFI_USE_WPA         DISABLED
#   endif
#endif

/*
 * 802.11 keepalive: send a null data frame at regular intervals to prevent
 * the AP from deauthenticating an idle STA.  The interval is derived from
 * the BSS Max Idle Period IE advertised by the AP when available, otherwise
 * the default below is used.  Set to 0 to disable.
 */
#ifndef VSF_WIFI_CFG_KEEPALIVE_PERIOD_MS
#   define VSF_WIFI_CFG_KEEPALIVE_PERIOD_MS         10000
#endif

/* Hard lower bound for the dynamic keepalive period (avoid excessive airtime). */
#ifndef VSF_WIFI_CFG_KEEPALIVE_MIN_PERIOD_MS
#   define VSF_WIFI_CFG_KEEPALIVE_MIN_PERIOD_MS     2000
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
