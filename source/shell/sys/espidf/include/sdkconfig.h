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
 * ESP-IDF compatibility sdkconfig shim.
 *
 * ESP-IDF headers reference CONFIG_* macros generated from Kconfig. In VSF
 * this file maps VSF_ESPIDF_CFG_* switches and reasonable defaults so that
 * unmodified ESP-IDF application code (which #includes "sdkconfig.h") can
 * compile against the VSF espidf shim.
 *
 * Baseline: ESP-IDF v5.1 public API surface.
 */

#ifndef __VSF_ESPIDF_SDKCONFIG_H__
#define __VSF_ESPIDF_SDKCONFIG_H__

#include "../vsf_espidf_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Target identification                                              */
/* ------------------------------------------------------------------ */
#define CONFIG_IDF_TARGET_LINUX             1
#define CONFIG_IDF_TARGET                   "linux"
#define CONFIG_IDF_TARGET_ARCH_XTENSA       0
#define CONFIG_IDF_TARGET_ARCH_RISCV        0
#define CONFIG_IDF_TARGET_LINUX             1

/* ------------------------------------------------------------------ */
/* Compiler control (esp_compiler.h / esp_check.h consumers)          */
/* ------------------------------------------------------------------ */
#define CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT   0
#define CONFIG_COMPILER_OPTIMIZATION_PERF            0
#define CONFIG_COMPILER_STATIC_ANALYZER              0

/* ------------------------------------------------------------------ */
/* SoC capability stubs (consumed by esp_attr.h)                     */
/* ------------------------------------------------------------------ */
#define CONFIG_SOC_RTC_FAST_MEM_SUPPORTED           0
#define CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED           0
#define CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY  0
#define CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY 0
#define CONFIG_SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE   0

#ifndef CONFIG_CACHE_L1_CACHE_LINE_SIZE
#define CONFIG_CACHE_L1_CACHE_LINE_SIZE             32
#endif

/* ------------------------------------------------------------------ */
/* IDF CI build (esp_attr.h IDF_DEPRECATED)                           */
/* ------------------------------------------------------------------ */
#define CONFIG_IDF_CI_BUILD                         0

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_SDKCONFIG_H__
