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
 * Clean-room re-implementation of ESP-IDF public API "esp_attr.h".
 *
 * Provides function / variable / type attributes used across ESP-IDF.
 * Covers two categories:
 *
 *   1. Universal compiler attributes (PACKED, ALIGN, INLINE, NOINLINE, etc.)
 *      — mapped to compiler built-ins, no platform dependency.
 *
 *   2. Platform-specific memory-region attributes (IRAM_ATTR, DRAM_ATTR,
 *      RTC_*, DMA_*, COREDUMP_*) — deferred. These require per-chip linker-
 *      script knowledge that VSF does not abstract at this layer. Application
 *      code that transitively includes these macros will hit a no-op stub.
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_ATTR_H__
#define __VSF_ESPIDF_ESP_ATTR_H__

#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* 1.  Universal compiler attributes                                  */
/* ------------------------------------------------------------------ */

#define ROMFN_ATTR          /* ESP-specific, no-op on VSF */

/* Packed struct member */
#ifndef PACKED_ATTR
#   define PACKED_ATTR              __attribute__((packed))
#endif

/* Word (4-byte) alignment */
#ifndef WORD_ALIGNED_ATTR
#   define WORD_ALIGNED_ATTR        __attribute__((aligned(4)))
#endif

/* Force inlining */
#ifndef FORCE_INLINE_ATTR
#   define FORCE_INLINE_ATTR        static inline __attribute__((always_inline))
#endif

/* Prevent inlining */
#ifndef NOINLINE_ATTR
#   define NOINLINE_ATTR            __attribute__((noinline))
#endif

/* Mark a char array as not null-terminated (GCC 15+) */
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ >= 15)
#   define NONSTRING_ATTR           __attribute__((nonstring))
#else
#   define NONSTRING_ATTR
#endif

/* C++: enum as bit-flag with operator overloads */
#ifdef __cplusplus
#   define _FLAG_ATTR_IMPL(TYPE, INT_TYPE)                                       \
        FORCE_INLINE_ATTR constexpr TYPE operator~ (TYPE a) { return (TYPE)~(INT_TYPE)a; } \
        FORCE_INLINE_ATTR constexpr TYPE operator| (TYPE a, TYPE b) { return (TYPE)((INT_TYPE)a | (INT_TYPE)b); } \
        FORCE_INLINE_ATTR constexpr TYPE operator& (TYPE a, TYPE b) { return (TYPE)((INT_TYPE)a & (INT_TYPE)b); } \
        FORCE_INLINE_ATTR constexpr TYPE operator^ (TYPE a, TYPE b) { return (TYPE)((INT_TYPE)a ^ (INT_TYPE)b); } \
        FORCE_INLINE_ATTR constexpr TYPE operator>> (TYPE a, int b) { return (TYPE)((INT_TYPE)a >> b); } \
        FORCE_INLINE_ATTR constexpr TYPE operator<< (TYPE a, int b) { return (TYPE)((INT_TYPE)a << b); } \
        FORCE_INLINE_ATTR TYPE& operator|=(TYPE& a, TYPE b) { a = a | b; return a; } \
        FORCE_INLINE_ATTR TYPE& operator&=(TYPE& a, TYPE b) { a = a & b; return a; } \
        FORCE_INLINE_ATTR TYPE& operator^=(TYPE& a, TYPE b) { a = a ^ b; return a; } \
        FORCE_INLINE_ATTR TYPE& operator>>=(TYPE& a, int b) { a = a >> b; return a; } \
        FORCE_INLINE_ATTR TYPE& operator<<=(TYPE& a, int b) { a = a << b; return a; }
#   define _FLAG_ATTR_U32(TYPE)     _FLAG_ATTR_IMPL(TYPE, uint32_t)
#   define FLAG_ATTR(TYPE)          _FLAG_ATTR_U32(TYPE)
#else
#   define FLAG_ATTR(TYPE)
#endif

/* Deprecation: active only in CI builds (controlled via sdkconfig.h) */
#include "sdkconfig.h"
#ifdef CONFIG_IDF_CI_BUILD
#   define IDF_DEPRECATED(REASON)   __attribute__((deprecated(REASON)))
#else
#   define IDF_DEPRECATED(REASON)
#endif

/* ------------------------------------------------------------------ */
/* 2.  Section-placement internal macros (used by platform attrs)     */
/* ------------------------------------------------------------------ */
#define _ESP_COUNTER_STRINGIFY(COUNTER)   #COUNTER

#if defined(__APPLE__) && defined(__MACH__)
#   define _SECTION_ATTR_IMPL(SECTION, COUNTER)
#   define _SECTION_FORCE_ATTR_IMPL(SECTION, COUNTER)
#else
#   define _SECTION_ATTR_IMPL(SECTION, COUNTER) \
        __attribute__((section(SECTION "." _ESP_COUNTER_STRINGIFY(COUNTER))))
#   define _SECTION_FORCE_ATTR_IMPL(SECTION, COUNTER) \
        __attribute__((noinline, section(SECTION "." _ESP_COUNTER_STRINGIFY(COUNTER))))
#endif

/* ------------------------------------------------------------------ */
/* 3.  Platform memory-region attributes  (deferred — no-op stubs)    */
/* ------------------------------------------------------------------ */
#define IRAM_ATTR
#define FORCE_IRAM_ATTR
#define DRAM_ATTR
#define SPM_IRAM_ATTR
#define FORCE_SPM_IRAM_ATTR
#define SPM_DRAM_ATTR
#define TCM_IRAM_ATTR
#define FORCE_TCM_IRAM_ATTR
#define TCM_DRAM_ATTR
#define NOLOAD_ATTR
#define IRAM_8BIT_ACCESSIBLE            0
#define IRAM_DATA_ATTR
#define COREDUMP_IRAM_DATA_ATTR
#define IRAM_BSS_ATTR
#define DMA_ATTR
#define DRAM_DMA_ALIGNED_ATTR            WORD_ALIGNED_ATTR
#define DRAM_STR(str)                   (str)
#define RTC_DATA_ATTR
#define RTC_NOINIT_ATTR
#define RTC_RODATA_ATTR
#define COREDUMP_RTC_DATA_ATTR
#define RTC_SLOW_ATTR
#define RTC_IRAM_ATTR
#define RTC_FAST_ATTR
#define COREDUMP_RTC_FAST_ATTR
#define COREDUMP_NOINIT_ATTR
#define __NOINIT_ATTR
#define EXT_RAM_BSS_ATTR
#define COREDUMP_EXTRAM_ATTR
#define EXT_RAM_NOINIT_ATTR
#define COREDUMP_EXTRAM_NOINIT_ATTR
#define COREDUMP_DRAM_ATTR

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_ATTR_H__
