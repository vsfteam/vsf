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
 * Clean-room re-implementation of ESP-IDF public API "esp_compiler.h".
 *
 * Provides compiler-hint macros (likely/unlikely, diagnostic push/pop,
 * designated-initialiser workarounds). Compiler identity uses the same
 * __IS_COMPILER_* convention as VSF's utilities/compiler layer.
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_COMPILER_H__
#define __VSF_ESPIDF_ESP_COMPILER_H__

/* ------------------------------------------------------------------ */
/* Compiler identity (mirrors VSF utilities/compiler detect headers)  */
/* ------------------------------------------------------------------ */
#ifndef __IS_COMPILER_LLVM__
#   if defined(__clang__)
#       define __IS_COMPILER_LLVM__               1
#   endif
#endif
#ifndef __IS_COMPILER_GCC__
#   if defined(__GNUC__) && !defined(__clang__)
#       define __IS_COMPILER_GCC__                1
#   endif
#endif

/* ------------------------------------------------------------------ */
/* likely / unlikely — branch-prediction hints                        */
/* ------------------------------------------------------------------ */
#if __IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__
#   ifndef likely
#       define likely(x)       __builtin_expect(!!(x), 1)
#   endif
#   ifndef unlikely
#       define unlikely(x)     __builtin_expect(!!(x), 0)
#   endif
#else
#   ifndef likely
#       define likely(x)       (x)
#   endif
#   ifndef unlikely
#       define unlikely(x)     (x)
#   endif
#endif

/* ------------------------------------------------------------------ */
/* Designated-initialiser workarounds (C99 / C++17 / C++20)           */
/* ------------------------------------------------------------------ */
#if defined(__cplusplus) && (__cplusplus >= 202002L)
#   define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(member, value)   .member = value,
#   define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(member)        .member = { },
#elif defined(__cplusplus) && (__cplusplus < 202002L)
#   define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(member, value)   { .member = value },
#   define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(member)        .member = { },
#else
#   define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(member, value)   .member = value,
#   define ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(member)
#endif

/* ------------------------------------------------------------------ */
/* Diagnostic push / pop (GCC / Clang / IAR)                          */
/* ------------------------------------------------------------------ */
#define __ESP_COMPILER_PRAGMA__(string)    _Pragma(#string)
#define _ESP_COMPILER_PRAGMA_(string)      __ESP_COMPILER_PRAGMA__(string)

#if __IS_COMPILER_LLVM__
#   define ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE(warning)                           \
        __ESP_COMPILER_PRAGMA__(clang diagnostic push)                             \
        __ESP_COMPILER_PRAGMA__(clang diagnostic ignored "-Wunknown-warning-option") \
        __ESP_COMPILER_PRAGMA__(clang diagnostic ignored warning)
#   define ESP_COMPILER_DIAGNOSTIC_POP(warning)                                   \
        __ESP_COMPILER_PRAGMA__(clang diagnostic pop)
#elif __IS_COMPILER_GCC__
#   define ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE(warning)                           \
        __ESP_COMPILER_PRAGMA__(GCC diagnostic push)                               \
        __ESP_COMPILER_PRAGMA__(GCC diagnostic ignored "-Wpragmas")                \
        __ESP_COMPILER_PRAGMA__(GCC diagnostic ignored warning)
#   define ESP_COMPILER_DIAGNOSTIC_POP(warning)                                   \
        __ESP_COMPILER_PRAGMA__(GCC diagnostic pop)
#else
#   define ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE(warning)
#   define ESP_COMPILER_DIAGNOSTIC_POP(warning)
#endif

/* ------------------------------------------------------------------ */
/* Static analyzer hint (clang-analyzer only)                         */
/* ------------------------------------------------------------------ */
#if defined(__clang_analyzer__)
#   define ESP_STATIC_ANALYZER_CHECK(_expr_, _ret_)                               \
        do { if ((_expr_)) { return (_ret_); } } while(0)
#else
#   define ESP_STATIC_ANALYZER_CHECK(_expr_, _ret_)
#endif

#endif      // __VSF_ESPIDF_ESP_COMPILER_H__
