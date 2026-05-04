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

/*============================ INCLUDES ======================================*/

//! \note do not move this pre-processor statement to other places
#include "../../../vsf_cfg.h"

#ifndef __VSF_ESPIDF_CFG_H__
#define __VSF_ESPIDF_CFG_H__

#include "shell/vsf_shell_wrapper.h"

/*============================ MACROS ========================================*/

// Master switch. User enables this in vsf_usr_cfg.h / board cfg.
#ifndef VSF_USE_ESPIDF
#   define VSF_USE_ESPIDF                       DISABLED
#endif

#ifndef VSF_ESPIDF_ASSERT
#   define VSF_ESPIDF_ASSERT                    VSF_ASSERT
#endif

#define VSF_ESPIDF_WRAPPER(__api)               VSF_SHELL_WRAPPER(vsf_espidf, __api)

// ---------------------------------------------------------------------------
// ESP-IDF API baseline version this sub-system targets.
// Align with a specific IDF release so that header shapes are deterministic.
// Default baseline: v5.x LTS. Adjust when a new baseline is chosen.
// ---------------------------------------------------------------------------
#ifndef VSF_ESPIDF_CFG_VERSION_MAJOR
#   define VSF_ESPIDF_CFG_VERSION_MAJOR         5
#endif
#ifndef VSF_ESPIDF_CFG_VERSION_MINOR
#   define VSF_ESPIDF_CFG_VERSION_MINOR         1
#endif
#ifndef VSF_ESPIDF_CFG_VERSION_PATCH
#   define VSF_ESPIDF_CFG_VERSION_PATCH         0
#endif

// ---------------------------------------------------------------------------
// Per-component enable switches. All OFF by default; opt-in as each module
// is implemented.
// ---------------------------------------------------------------------------

// Core infrastructure
#ifndef VSF_ESPIDF_CFG_USE_LOG
#   define VSF_ESPIDF_CFG_USE_LOG               ENABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_ERR
#   define VSF_ESPIDF_CFG_USE_ERR               ENABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_EVENT
#   define VSF_ESPIDF_CFG_USE_EVENT             ENABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_TIMER
#   define VSF_ESPIDF_CFG_USE_TIMER             ENABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_SYSTEM
#   define VSF_ESPIDF_CFG_USE_SYSTEM            ENABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_RINGBUF
#   define VSF_ESPIDF_CFG_USE_RINGBUF           ENABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_HEAP_CAPS
#   define VSF_ESPIDF_CFG_USE_HEAP_CAPS         ENABLED
#endif

// Storage
#ifndef VSF_ESPIDF_CFG_USE_PARTITION
#   define VSF_ESPIDF_CFG_USE_PARTITION         DISABLED
#endif
// Runtime esp_partition_register_external() / _deregister_external() slot
// pool. Set to 0 to compile the entry points out entirely (callers receive
// ESP_ERR_INVALID_STATE). Compile-time entries from vsf_espidf_cfg_t are
// always available regardless of this switch.
#ifndef VSF_ESPIDF_CFG_PARTITION_DYNAMIC
#   define VSF_ESPIDF_CFG_PARTITION_DYNAMIC     ENABLED
#endif
#ifndef VSF_ESPIDF_CFG_PARTITION_MAX_DYNAMIC
#   define VSF_ESPIDF_CFG_PARTITION_MAX_DYNAMIC 8
#endif
#ifndef VSF_ESPIDF_CFG_USE_NVS
#   define VSF_ESPIDF_CFG_USE_NVS               DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_ESP_FLASH
#   define VSF_ESPIDF_CFG_USE_ESP_FLASH         DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_VFS
#   define VSF_ESPIDF_CFG_USE_VFS               DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_LITTLEFS
#   define VSF_ESPIDF_CFG_USE_LITTLEFS          DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_LITTLEFS_MAX_INSTANCES
#   define VSF_ESPIDF_CFG_LITTLEFS_MAX_INSTANCES 4
#endif
#ifndef VSF_ESPIDF_CFG_USE_FATFS
#   define VSF_ESPIDF_CFG_USE_FATFS             DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_FATFS_MAX_INSTANCES
#   define VSF_ESPIDF_CFG_FATFS_MAX_INSTANCES   2
#endif

// Network
#ifndef VSF_ESPIDF_CFG_USE_NETIF
#   define VSF_ESPIDF_CFG_USE_NETIF             DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_HTTP_CLIENT
#   define VSF_ESPIDF_CFG_USE_HTTP_CLIENT       DISABLED
#endif

// --------------------------------------------------------------------------
// esp_http_client feature profile + atomic switches.
//
// The 5 atomic switches below toggle optional pieces of the esp_http_client
// compatibility shim. They are grouped behind a single PROFILE knob so that
// most users pick one of 3 presets and only the advanced users need to
// override individual switches.
//
//   MINIMAL  - smallest footprint; HTTP/HTTPS basic GET/POST only.
//              No redirect following, no Digest auth, no bundled CA store,
//              no cooperative cancel, no async mode. Basic auth is ALWAYS
//              compiled in (cost < 50 LOC, not worth a flag).
//   STANDARD - default. MINIMAL + HTTP 3xx redirect follow + 401 challenge
//              retry loop. +~40 LOC vs MINIMAL.
//   FULL     - STANDARD + Digest auth + esp_crt_bundle_attach + cooperative
//              cancel. +~120 LOC + CA bundle payload (~10..500KB) vs STANDARD.
//              Does NOT enable async mode (that flag stays opt-in, see below).
//
// Any atomic switch a user predefines before this header is included wins
// over the profile default, regardless of PROFILE value.
// --------------------------------------------------------------------------
#define VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE_MINIMAL      0
#define VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE_STANDARD     1
#define VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE_FULL         2
#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE
#   define VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE           VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE_STANDARD
#endif

// 3xx redirect following + 401 "WWW-Authenticate: Basic" challenge retry.
// Cost: ~40 LOC + one extra header-parse pass per hop.
#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_USE_REDIRECT
#   if VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE >= VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE_STANDARD
#       define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_REDIRECT      ENABLED
#   else
#       define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_REDIRECT      DISABLED
#   endif
#endif

// HTTP Digest (RFC 7616) authentication parser + response hashing.
// Cost: ~50 LOC + pulls MD5 + realm/nonce/qop state. HTTP Basic auth
// remains unconditionally available in all profiles.
#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_USE_DIGEST_AUTH
#   if VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE >= VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE_FULL
#       define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_DIGEST_AUTH   ENABLED
#   else
#       define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_DIGEST_AUTH   DISABLED
#   endif
#endif

// esp_crt_bundle_attach(): verify server cert against a compiled-in CA store.
// Cost: ~40 LOC wiring + CA bundle blob (~10..500KB depending on which bundle).
// When DISABLED, callers must pass cert_pem explicitly; otherwise TLS will
// run with MBEDTLS_SSL_VERIFY_NONE-equivalent semantics (see TLS layer).
#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_USE_CRT_BUNDLE
#   if VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE >= VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE_FULL
#       define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_CRT_BUNDLE    ENABLED
#   else
#       define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_CRT_BUNDLE    DISABLED
#   endif
#endif

// Cooperative cancellation: esp_http_client_cancel_request() sets a flag
// that the perform loop checks between chunks. Does NOT preempt a blocked
// recv() -- use esp_http_client_set_timeout_ms() to bound that.
// Cost: ~30 LOC + one bool field per client.
#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_USE_CANCEL
#   if VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE >= VSF_ESPIDF_HTTP_CLIENT_CFG_PROFILE_FULL
#       define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_CANCEL        ENABLED
#   else
#       define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_CANCEL        DISABLED
#   endif
#endif

// Async mode: esp_http_client_perform() returns ESP_ERR_HTTP_EAGAIN instead
// of blocking; caller polls until completion. Requires a non-blocking
// transport port. Cost: ~1500+ LOC (full state machine replicating IDF's
// dispatch loop). Because of the size, this stays opt-in even when
// PROFILE == FULL -- users enabling async must predefine it explicitly.
#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_USE_ASYNC
#   define VSF_ESPIDF_HTTP_CLIENT_CFG_USE_ASYNC             DISABLED
#endif

// Working buffer size used by the shim when the caller does not provide
// their own via esp_http_client_config_t::buffer_size. Must be >= the
// largest expected header line (ESP-IDF default is 512).
#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_BUFFER_SIZE
#   define VSF_ESPIDF_HTTP_CLIENT_CFG_BUFFER_SIZE           512
#endif

#ifndef VSF_ESPIDF_CFG_USE_HTTP_SERVER
#   define VSF_ESPIDF_CFG_USE_HTTP_SERVER       DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_WIFI
#   define VSF_ESPIDF_CFG_USE_WIFI              DISABLED
#endif

// Peripheral drivers (bridged to vsf_template_*)
#ifndef VSF_ESPIDF_CFG_USE_DRIVER_GPIO
#   define VSF_ESPIDF_CFG_USE_DRIVER_GPIO       DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_DRIVER_UART
#   define VSF_ESPIDF_CFG_USE_DRIVER_UART       DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_DRIVER_I2C
#   define VSF_ESPIDF_CFG_USE_DRIVER_I2C        DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_DRIVER_SPI_MASTER
#   define VSF_ESPIDF_CFG_USE_DRIVER_SPI_MASTER DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_DRIVER_LEDC
#   define VSF_ESPIDF_CFG_USE_DRIVER_LEDC       DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER
#   define VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER    DISABLED
#endif
#ifndef VSF_ESPIDF_CFG_USE_DRIVER_ADC
#   define VSF_ESPIDF_CFG_USE_DRIVER_ADC        DISABLED
#endif

#endif      // __VSF_ESPIDF_CFG_H__
