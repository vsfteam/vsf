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
 * Clean-room re-implementation of ESP-IDF public API "esp_macros.h".
 *
 * General-purpose helper macros used across ESP-IDF:
 *   CHOOSE_MACRO_VA_ARG  — variadic overload selection
 *   ESP_VA_NARG          — count VA_ARGS arguments
 *   ESP_UNUSED           — suppress unused-variable warning
 *   ESP_INFINITE_LOOP    — intentional infinite loop
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_MACROS_H__
#define __VSF_ESPIDF_ESP_MACROS_H__

#include <assert.h>
#include "esp_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* CHOOSE_MACRO_VA_ARG — pick macro based on whether __VA_ARGS__ is   */
/* empty. Dual-path: C++20 __VA_OPT__ vs GNU ##__VA_ARGS__            */
/* ------------------------------------------------------------------ */
#if defined(__cplusplus) && (__cplusplus > 201703L)
#   define _ESP_CHOOSE_VA_ARG_INN_IMPL(...)    __VA_OPT__(0)
#   define _ESP_CHOOSE_VA_ARG_INN(one, MACRO1, MACRO2, ...)  MACRO1
#   define CHOOSE_MACRO_VA_ARG(MACRO_WITH_ARGS, MACRO_WITH_NO_ARGS, ...)     \
        _ESP_CHOOSE_VA_ARG_INN(_ESP_CHOOSE_VA_ARG_INN_IMPL(__VA_ARGS__)      \
            __VA_OPT__(,) MACRO_WITH_ARGS, MACRO_WITH_NO_ARGS, 0)
#else
#   define _ESP_CHOOSE_VA_ARG_INN(one, two, MACRO1, MACRO2, ...)  MACRO1
#   define CHOOSE_MACRO_VA_ARG(MACRO_WITH_ARGS, MACRO_WITH_NO_ARGS, ...)     \
        _ESP_CHOOSE_VA_ARG_INN(0, ##__VA_ARGS__,                             \
            MACRO_WITH_ARGS, MACRO_WITH_NO_ARGS, 0)
#endif

/* ------------------------------------------------------------------ */
/* ESP_VA_NARG — count number of arguments (0..16)                    */
/* ------------------------------------------------------------------ */
#ifndef ESP_VA_NARG
#   define ESP_VA_NARG(...)   _ESP_NARG(_0, ##__VA_ARGS__, _ESP_RSEQ_N())

#   define _ESP_NARG(...)     _ESP_GET_NTH_ARG(__VA_ARGS__)
#   define _ESP_GET_NTH_ARG( \
          _01,_02,_03,_04,_05,_06,_07,_08,_09,_10, \
          _11,_12,_13,_14,_15,_16,N,...) N
#   define _ESP_RSEQ_N() \
          15,14,13,12,11,10, \
          9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#endif

/* ------------------------------------------------------------------ */
/* ESP_UNUSED — suppress "unused variable" warning                    */
/* ------------------------------------------------------------------ */
#ifndef ESP_UNUSED
#   define ESP_UNUSED(x)    ((void)(x))
#endif

/* ------------------------------------------------------------------ */
/* ESP_INFINITE_LOOP — intentional infinite loop with diag suppression */
/* ------------------------------------------------------------------ */
#define ESP_INFINITE_LOOP()                                                    \
    do {                                                                       \
        ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-infinite-loop")        \
        while(1);                                                              \
        ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-infinite-loop")                \
    } while(1)

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_MACROS_H__
