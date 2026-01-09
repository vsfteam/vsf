/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                    *
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

#ifndef __VSF_STATIC_ASSERT_UNIQUE_H__
#define __VSF_STATIC_ASSERT_UNIQUE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 * @file vsf_static_assert_unique.h
 * @brief Generic static assertion macros for uniqueness checking
 *
 * This header file provides flexible enum value uniqueness checking
 * functionality that can be used in any scenario requiring enum value
 * uniqueness validation.
 *
 * Main features:
 * - Multiple check modes: strict (bit-disjoint), loose (value-unique), custom
 * - Support up to 255 parameters checking (0xFF)
 * - Automatically select correct macro based on argument count
 * - Check function can be passed as parameter for custom validation
 *
 * Usage:
 * 1. Include this header file
 * 2. Use VSF_HAL_CHECK_UNIQUE() macro with optional check mode parameter
 *
 * Examples:
 * @code
 * // Define enum values
 * #define MODE_A    0x01
 * #define MODE_B    0x02
 * #define MODE_C    0x04
 *
 * // Default loose mode (value-unique check)
 * VSF_HAL_CHECK_UNIQUE(MODE_A, MODE_B, MODE_C);
 *
 * // Strict mode (bit-disjoint check)
 * VSF_HAL_CHECK_UNIQUE_STRICT(MODE_A, MODE_B, MODE_C);
 *
 * // Custom check function
 * #define CUSTOM_CHECK(__A, __B) \
 *     VSF_STATIC_ASSERT((__A) != (__B) && (__A) > 0 && (__B) > 0)
 * VSF_HAL_CHECK_UNIQUE_CUSTOM(CUSTOM_CHECK, MODE_A, MODE_B, MODE_C);
 * @endcode
 *
 * Check modes:
 * - LOOSE (default): Check that enum values are different (allows overlapping
 * bits)
 * - STRICT: Check that enum values have no overlapping bits (bit-disjoint)
 * - CUSTOM: Use user-provided check function
 */

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

/*\note Static assertions to validate enum value uniqueness.
 *      These assertions check that the redefined enum values are unique within
 *      each functional group using optimized macro-based approach.
 */

/* Wrapper macro for static assertion that works with both static_assert and VSF_STATIC_ASSERT */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) ||              \
    (defined(__cplusplus) && __cplusplus >= 201103L) || \
    defined(static_assert) || defined(_Static_assert)
#   define __VSF_HAL_STATIC_ASSERT(__COND, __NAME1, __NAME2, __MSG)            \
         static_assert((__COND), __MSG)
#else
#   define __VSF_HAL_STATIC_ASSERT(__COND, __NAME1, __NAME2, __MSG)            \
         VSF_STATIC_ASSERT((__COND), VSF_MCONNECT(__NAME1, _, __NAME2))
#endif

/* Default check modes */
#define VSF_HAL_CHECK_MODE_LOOSE(__A, __B)                                     \
  __VSF_HAL_STATIC_ASSERT((__A) != (__B) || ((__A) == 0 && (__B) == 0),        \
                __A, __B, "Enum values must be unique: " #__A " and " #__B)
#define VSF_HAL_CHECK_MODE_STRICT(__A, __B)                                    \
  __VSF_HAL_STATIC_ASSERT(((__A) & (__B)) == 0,                                \
                __A, __B, "Enum values must have disjoint bits: " #__A " and " #__B)

/* Check mode for all values being zero */
#define VSF_HAL_CHECK_MODE_ALL_ZERO(...)                                       \
  __VSF_HAL_STATIC_ASSERT(__VSF_HAL_ZERO_COUNT_ALL_VALUES(__VA_ARGS__) ==      \
                    __VSF_HAL_COUNT_ARGS_ALL_VALUES(__VA_ARGS__),              \
                ALL, ZERO, "All values must be zero")

/* Check mode for all values being non-zero */
#define VSF_HAL_CHECK_MODE_ALL_NON_ZERO(...)                                   \
  __VSF_HAL_STATIC_ASSERT(__VSF_HAL_ZERO_COUNT_ALL_VALUES(__VA_ARGS__) == 0,   \
                ALL, NON_ZERO, "All values must be non-zero")

/* Check mode for specific number of zeros */
#define VSF_HAL_CHECK_MODE_ZERO_COUNT(__COUNT, ...)                            \
  __VSF_HAL_STATIC_ASSERT(__VSF_HAL_ZERO_COUNT_ALL_VALUES(__VA_ARGS__) == (__COUNT), \
                ZERO, COUNT, "Incorrect number of zero values")

/* Generic check function interface */
#define ____VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __A, __B)                  \
  __CHECK_FUNC(__A, __B)

/* Public interface macro for custom check functions */
#define __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __A, __B)                    \
  ____VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __A, __B)

/* Generic zero count operation interface */
#define ____VSF_HAL_ZERO_COUNT_BASE(__A, __B) (((__A) == 0 ? 1 : 0) + (__B))

/* Public interface macro for zero count operations */
#define __VSF_HAL_ZERO_COUNT_BASE(__A, __B)                                    \
  ____VSF_HAL_ZERO_COUNT_BASE(__A, __B)

/* Generic argument count operation interface */
#define ____VSF_HAL_COUNT_ARGS_BASE(__A, __B) (1 + (__B))

/* Public interface macro for argument count operations */
#define __VSF_HAL_COUNT_ARGS_BASE(__A, __B)                                    \
  ____VSF_HAL_COUNT_ARGS_BASE(__A, __B)

#define ____VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(                                 \
    __CHECK_FUNC, __ELEM, __ARG01, __ARG02, __ARG03, __ARG04, __ARG05,         \
    __ARG06, __ARG07, __ARG08, __ARG09, __ARG0A, __ARG0B, __ARG0C, __ARG0D,    \
    __ARG0E, __ARG0F, __ARG10, __ARG11, __ARG12, __ARG13, __ARG14, __ARG15,    \
    __ARG16, __ARG17, __ARG18, __ARG19, __ARG1A, __ARG1B, __ARG1C, __ARG1D,    \
    __ARG1E, __ARG1F, __ARG20, __ARG21, __ARG22, __ARG23, __ARG24, __ARG25,    \
    __ARG26, __ARG27, __ARG28, __ARG29, __ARG2A, __ARG2B, __ARG2C, __ARG2D,    \
    __ARG2E, __ARG2F, __ARG30, __ARG31, __ARG32, __ARG33, __ARG34, __ARG35,    \
    __ARG36, __ARG37, __ARG38, __ARG39, __ARG3A, __ARG3B, __ARG3C, __ARG3D,    \
    __ARG3E, __ARG3F, __ARG40, __ARG41, __ARG42, __ARG43, __ARG44, __ARG45,    \
    __ARG46, __ARG47, __ARG48, __ARG49, __ARG4A, __ARG4B, __ARG4C, __ARG4D,    \
    __ARG4E, __ARG4F, __ARG50, __ARG51, __ARG52, __ARG53, __ARG54, __ARG55,    \
    __ARG56, __ARG57, __ARG58, __ARG59, __ARG5A, __ARG5B, __ARG5C, __ARG5D,    \
    __ARG5E, __ARG5F, __ARG60, __ARG61, __ARG62, __ARG63, __ARG64, __ARG65,    \
    __ARG66, __ARG67, __ARG68, __ARG69, __ARG6A, __ARG6B, __ARG6C, __ARG6D,    \
    __ARG6E, __ARG6F, __ARG70, __ARG71, __ARG72, __ARG73, __ARG74, __ARG75,    \
    __ARG76, __ARG77, __ARG78, __ARG79, __ARG7A, __ARG7B, __ARG7C, __ARG7D,    \
    __ARG7E, __ARG7F, __ARG80, __ARG81, __ARG82, __ARG83, __ARG84, __ARG85,    \
    __ARG86, __ARG87, __ARG88, __ARG89, __ARG8A, __ARG8B, __ARG8C, __ARG8D,    \
    __ARG8E, __ARG8F, __ARG90, __ARG91, __ARG92, __ARG93, __ARG94, __ARG95,    \
    __ARG96, __ARG97, __ARG98, __ARG99, __ARG9A, __ARG9B, __ARG9C, __ARG9D,    \
    __ARG9E, __ARG9F, __ARGA0, __ARGA1, __ARGA2, __ARGA3, __ARGA4, __ARGA5,    \
    __ARGA6, __ARGA7, __ARGA8, __ARGA9, __ARGAA, __ARGAB, __ARGAC, __ARGAD,    \
    __ARGAE, __ARGAF, __ARGB0, __ARGB1, __ARGB2, __ARGB3, __ARGB4, __ARGB5,    \
    __ARGB6, __ARGB7, __ARGB8, __ARGB9, __ARGBA, __ARGBB, __ARGBC, __ARGBD,    \
    __ARGBE, __ARGBF, __ARGC0, __ARGC1, __ARGC2, __ARGC3, __ARGC4, __ARGC5,    \
    __ARGC6, __ARGC7, __ARGC8, __ARGC9, __ARGCA, __ARGCB, __ARGCC, __ARGCD,    \
    __ARGCE, __ARGCF, __ARGD0, __ARGD1, __ARGD2, __ARGD3, __ARGD4, __ARGD5,    \
    __ARGD6, __ARGD7, __ARGD8, __ARGD9, __ARGDA, __ARGDB, __ARGDC, __ARGDD,    \
    __ARGDE, __ARGDF, __ARGE0, __ARGE1, __ARGE2, __ARGE3, __ARGE4, __ARGE5,    \
    __ARGE6, __ARGE7, __ARGE8, __ARGE9, __ARGEA, __ARGEB, __ARGEC, __ARGED,    \
    __ARGEE, __ARGEF, __ARGF0, __ARGF1, __ARGF2, __ARGF3, __ARGF4, __ARGF5,    \
    __ARGF6, __ARGF7, __ARGF8, __ARGF9, __ARGFA, __ARGFB, __ARGFC, __ARGFD,    \
    __ARGFE, __ARGFF, N, ...)                                                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS##N

#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __ELEM, ...)         \
  ____VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(                                       \
      __CHECK_FUNC, __ELEM, __VA_ARGS__, _FF, _FE, _FD, _FC, _FB, _FA, _F9,    \
      _F8, _F7, _F6, _F5, _F4, _F3, _F2, _F1, _F0, _EF, _EE, _ED, _EC, _EB,    \
      _EA, _E9, _E8, _E7, _E6, _E5, _E4, _E3, _E2, _E1, _E0, _DF, _DE, _DD,    \
      _DC, _DB, _DA, _D9, _D8, _D7, _D6, _D5, _D4, _D3, _D2, _D1, _D0, _CF,    \
      _CE, _CD, _CC, _CB, _CA, _C9, _C8, _C7, _C6, _C5, _C4, _C3, _C2, _C1,    \
      _C0, _BF, _BE, _BD, _BC, _BB, _BA, _B9, _B8, _B7, _B6, _B5, _B4, _B3,    \
      _B2, _B1, _B0, _AF, _AE, _AD, _AC, _AB, _AA, _A9, _A8, _A7, _A6, _A5,    \
      _A4, _A3, _A2, _A1, _A0, _9F, _9E, _9D, _9C, _9B, _9A, _99, _98, _97,    \
      _96, _95, _94, _93, _92, _91, _90, _8F, _8E, _8D, _8C, _8B, _8A, _89,    \
      _88, _87, _86, _85, _84, _83, _82, _81, _80, _7F, _7E, _7D, _7C, _7B,    \
      _7A, _79, _78, _77, _76, _75, _74, _73, _72, _71, _70, _6F, _6E, _6D,    \
      _6C, _6B, _6A, _69, _68, _67, _66, _65, _64, _63, _62, _61, _60, _5F,    \
      _5E, _5D, _5C, _5B, _5A, _59, _58, _57, _56, _55, _54, _53, _52, _51,    \
      _50, _4F, _4E, _4D, _4C, _4B, _4A, _49, _48, _47, _46, _45, _44, _43,    \
      _42, _41, _40, _3F, _3E, _3D, _3C, _3B, _3A, _39, _38, _37, _36, _35,    \
      _34, _33, _32, _31, _30, _2F, _2E, _2D, _2C, _2B, _2A, _29, _28, _27,    \
      _26, _25, _24, _23, _22, _21, _20, _1F, _1E, _1D, _1C, _1B, _1A, _19,    \
      _18, _17, _16, _15, _14, _13, _12, _11, _10, _0F, _0E, _0D, _0C, _0B,    \
      _0A, _09, _08, _07, _06, _05, _04, _03, _02, _01)                        \
  (__CHECK_FUNC, __ELEM, __VA_ARGS__)

#define ____VSF_HAL_ZERO_COUNT_ALL_VALUES(                                     \
    __ARG01, __ARG02, __ARG03, __ARG04, __ARG05, __ARG06, __ARG07, __ARG08,    \
    __ARG09, __ARG0A, __ARG0B, __ARG0C, __ARG0D, __ARG0E, __ARG0F, __ARG10,    \
    __ARG11, __ARG12, __ARG13, __ARG14, __ARG15, __ARG16, __ARG17, __ARG18,    \
    __ARG19, __ARG1A, __ARG1B, __ARG1C, __ARG1D, __ARG1E, __ARG1F, __ARG20,    \
    __ARG21, __ARG22, __ARG23, __ARG24, __ARG25, __ARG26, __ARG27, __ARG28,    \
    __ARG29, __ARG2A, __ARG2B, __ARG2C, __ARG2D, __ARG2E, __ARG2F, __ARG30,    \
    __ARG31, __ARG32, __ARG33, __ARG34, __ARG35, __ARG36, __ARG37, __ARG38,    \
    __ARG39, __ARG3A, __ARG3B, __ARG3C, __ARG3D, __ARG3E, __ARG3F, __ARG40,    \
    __ARG41, __ARG42, __ARG43, __ARG44, __ARG45, __ARG46, __ARG47, __ARG48,    \
    __ARG49, __ARG4A, __ARG4B, __ARG4C, __ARG4D, __ARG4E, __ARG4F, __ARG50,    \
    __ARG51, __ARG52, __ARG53, __ARG54, __ARG55, __ARG56, __ARG57, __ARG58,    \
    __ARG59, __ARG5A, __ARG5B, __ARG5C, __ARG5D, __ARG5E, __ARG5F, __ARG60,    \
    __ARG61, __ARG62, __ARG63, __ARG64, __ARG65, __ARG66, __ARG67, __ARG68,    \
    __ARG69, __ARG6A, __ARG6B, __ARG6C, __ARG6D, __ARG6E, __ARG6F, __ARG70,    \
    __ARG71, __ARG72, __ARG73, __ARG74, __ARG75, __ARG76, __ARG77, __ARG78,    \
    __ARG79, __ARG7A, __ARG7B, __ARG7C, __ARG7D, __ARG7E, __ARG7F, __ARG80,    \
    __ARG81, __ARG82, __ARG83, __ARG84, __ARG85, __ARG86, __ARG87, __ARG88,    \
    __ARG89, __ARG8A, __ARG8B, __ARG8C, __ARG8D, __ARG8E, __ARG8F, __ARG90,    \
    __ARG91, __ARG92, __ARG93, __ARG94, __ARG95, __ARG96, __ARG97, __ARG98,    \
    __ARG99, __ARG9A, __ARG9B, __ARG9C, __ARG9D, __ARG9E, __ARG9F, __ARGA0,    \
    __ARGA1, __ARGA2, __ARGA3, __ARGA4, __ARGA5, __ARGA6, __ARGA7, __ARGA8,    \
    __ARGA9, __ARGAA, __ARGAB, __ARGAC, __ARGAD, __ARGAE, __ARGAF, __ARGB0,    \
    __ARGB1, __ARGB2, __ARGB3, __ARGB4, __ARGB5, __ARGB6, __ARGB7, __ARGB8,    \
    __ARGB9, __ARGBA, __ARGBB, __ARGBC, __ARGBD, __ARGBE, __ARGBF, __ARGC0,    \
    __ARGC1, __ARGC2, __ARGC3, __ARGC4, __ARGC5, __ARGC6, __ARGC7, __ARGC8,    \
    __ARGC9, __ARGCA, __ARGCB, __ARGCC, __ARGCD, __ARGCE, __ARGCF, __ARGD0,    \
    __ARGD1, __ARGD2, __ARGD3, __ARGD4, __ARGD5, __ARGD6, __ARGD7, __ARGD8,    \
    __ARGD9, __ARGDA, __ARGDB, __ARGDC, __ARGDD, __ARGDE, __ARGDF, __ARGE0,    \
    __ARGE1, __ARGE2, __ARGE3, __ARGE4, __ARGE5, __ARGE6, __ARGE7, __ARGE8,    \
    __ARGE9, __ARGEA, __ARGEB, __ARGEC, __ARGED, __ARGEE, __ARGEF, __ARGF0,    \
    __ARGF1, __ARGF2, __ARGF3, __ARGF4, __ARGF5, __ARGF6, __ARGF7, __ARGF8,    \
    __ARGF9, __ARGFA, __ARGFB, __ARGFC, __ARGFD, __ARGFE, __ARGFF, N, ...)     \
  __VSF_HAL_ZERO_COUNT_ALL_VALUES##N

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES(...)                                   \
  ____VSF_HAL_ZERO_COUNT_ALL_VALUES(                                           \
      __VA_ARGS__, _FF, _FE, _FD, _FC, _FB, _FA, _F9, _F8, _F7, _F6, _F5, _F4, \
      _F3, _F2, _F1, _F0, _EF, _EE, _ED, _EC, _EB, _EA, _E9, _E8, _E7, _E6,    \
      _E5, _E4, _E3, _E2, _E1, _E0, _DF, _DE, _DD, _DC, _DB, _DA, _D9, _D8,    \
      _D7, _D6, _D5, _D4, _D3, _D2, _D1, _D0, _CF, _CE, _CD, _CC, _CB, _CA,    \
      _C9, _C8, _C7, _C6, _C5, _C4, _C3, _C2, _C1, _C0, _BF, _BE, _BD, _BC,    \
      _BB, _BA, _B9, _B8, _B7, _B6, _B5, _B4, _B3, _B2, _B1, _B0, _AF, _AE,    \
      _AD, _AC, _AB, _AA, _A9, _A8, _A7, _A6, _A5, _A4, _A3, _A2, _A1, _A0,    \
      _9F, _9E, _9D, _9C, _9B, _9A, _99, _98, _97, _96, _95, _94, _93, _92,    \
      _91, _90, _8F, _8E, _8D, _8C, _8B, _8A, _89, _88, _87, _86, _85, _84,    \
      _83, _82, _81, _80, _7F, _7E, _7D, _7C, _7B, _7A, _79, _78, _77, _76,    \
      _75, _74, _73, _72, _71, _70, _6F, _6E, _6D, _6C, _6B, _6A, _69, _68,    \
      _67, _66, _65, _64, _63, _62, _61, _60, _5F, _5E, _5D, _5C, _5B, _5A,    \
      _59, _58, _57, _56, _55, _54, _53, _52, _51, _50, _4F, _4E, _4D, _4C,    \
      _4B, _4A, _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, _3F, _3E,    \
      _3D, _3C, _3B, _3A, _39, _38, _37, _36, _35, _34, _33, _32, _31, _30,    \
      _2F, _2E, _2D, _2C, _2B, _2A, _29, _28, _27, _26, _25, _24, _23, _22,    \
      _21, _20, _1F, _1E, _1D, _1C, _1B, _1A, _19, _18, _17, _16, _15, _14,    \
      _13, _12, _11, _10, _0F, _0E, _0D, _0C, _0B, _0A, _09, _08, _07, _06,    \
      _05, _04, _03, _02, _01)                                                 \
  (__VA_ARGS__)

#define ____VSF_HAL_COUNT_ARGS_ALL_VALUES(                                     \
    __ARG01, __ARG02, __ARG03, __ARG04, __ARG05, __ARG06, __ARG07, __ARG08,    \
    __ARG09, __ARG0A, __ARG0B, __ARG0C, __ARG0D, __ARG0E, __ARG0F, __ARG10,    \
    __ARG11, __ARG12, __ARG13, __ARG14, __ARG15, __ARG16, __ARG17, __ARG18,    \
    __ARG19, __ARG1A, __ARG1B, __ARG1C, __ARG1D, __ARG1E, __ARG1F, __ARG20,    \
    __ARG21, __ARG22, __ARG23, __ARG24, __ARG25, __ARG26, __ARG27, __ARG28,    \
    __ARG29, __ARG2A, __ARG2B, __ARG2C, __ARG2D, __ARG2E, __ARG2F, __ARG30,    \
    __ARG31, __ARG32, __ARG33, __ARG34, __ARG35, __ARG36, __ARG37, __ARG38,    \
    __ARG39, __ARG3A, __ARG3B, __ARG3C, __ARG3D, __ARG3E, __ARG3F, __ARG40,    \
    __ARG41, __ARG42, __ARG43, __ARG44, __ARG45, __ARG46, __ARG47, __ARG48,    \
    __ARG49, __ARG4A, __ARG4B, __ARG4C, __ARG4D, __ARG4E, __ARG4F, __ARG50,    \
    __ARG51, __ARG52, __ARG53, __ARG54, __ARG55, __ARG56, __ARG57, __ARG58,    \
    __ARG59, __ARG5A, __ARG5B, __ARG5C, __ARG5D, __ARG5E, __ARG5F, __ARG60,    \
    __ARG61, __ARG62, __ARG63, __ARG64, __ARG65, __ARG66, __ARG67, __ARG68,    \
    __ARG69, __ARG6A, __ARG6B, __ARG6C, __ARG6D, __ARG6E, __ARG6F, __ARG70,    \
    __ARG71, __ARG72, __ARG73, __ARG74, __ARG75, __ARG76, __ARG77, __ARG78,    \
    __ARG79, __ARG7A, __ARG7B, __ARG7C, __ARG7D, __ARG7E, __ARG7F, __ARG80,    \
    __ARG81, __ARG82, __ARG83, __ARG84, __ARG85, __ARG86, __ARG87, __ARG88,    \
    __ARG89, __ARG8A, __ARG8B, __ARG8C, __ARG8D, __ARG8E, __ARG8F, __ARG90,    \
    __ARG91, __ARG92, __ARG93, __ARG94, __ARG95, __ARG96, __ARG97, __ARG98,    \
    __ARG99, __ARG9A, __ARG9B, __ARG9C, __ARG9D, __ARG9E, __ARG9F, __ARGA0,    \
    __ARGA1, __ARGA2, __ARGA3, __ARGA4, __ARGA5, __ARGA6, __ARGA7, __ARGA8,    \
    __ARGA9, __ARGAA, __ARGAB, __ARGAC, __ARGAD, __ARGAE, __ARGAF, __ARGB0,    \
    __ARGB1, __ARGB2, __ARGB3, __ARGB4, __ARGB5, __ARGB6, __ARGB7, __ARGB8,    \
    __ARGB9, __ARGBA, __ARGBB, __ARGBC, __ARGBD, __ARGBE, __ARGBF, __ARGC0,    \
    __ARGC1, __ARGC2, __ARGC3, __ARGC4, __ARGC5, __ARGC6, __ARGC7, __ARGC8,    \
    __ARGC9, __ARGCA, __ARGCB, __ARGCC, __ARGCD, __ARGCE, __ARGCF, __ARGD0,    \
    __ARGD1, __ARGD2, __ARGD3, __ARGD4, __ARGD5, __ARGD6, __ARGD7, __ARGD8,    \
    __ARGD9, __ARGDA, __ARGDB, __ARGDC, __ARGDD, __ARGDE, __ARGDF, __ARGE0,    \
    __ARGE1, __ARGE2, __ARGE3, __ARGE4, __ARGE5, __ARGE6, __ARGE7, __ARGE8,    \
    __ARGE9, __ARGEA, __ARGEB, __ARGEC, __ARGED, __ARGEE, __ARGEF, __ARGF0,    \
    __ARGF1, __ARGF2, __ARGF3, __ARGF4, __ARGF5, __ARGF6, __ARGF7, __ARGF8,    \
    __ARGF9, __ARGFA, __ARGFB, __ARGFC, __ARGFD, __ARGFE, __ARGFF, N, ...)     \
  __VSF_HAL_COUNT_ARGS_ALL_VALUES##N

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES(...)                                   \
  ____VSF_HAL_COUNT_ARGS_ALL_VALUES(                                           \
      __VA_ARGS__, _FF, _FE, _FD, _FC, _FB, _FA, _F9, _F8, _F7, _F6, _F5, _F4, \
      _F3, _F2, _F1, _F0, _EF, _EE, _ED, _EC, _EB, _EA, _E9, _E8, _E7, _E6,    \
      _E5, _E4, _E3, _E2, _E1, _E0, _DF, _DE, _DD, _DC, _DB, _DA, _D9, _D8,    \
      _D7, _D6, _D5, _D4, _D3, _D2, _D1, _D0, _CF, _CE, _CD, _CC, _CB, _CA,    \
      _C9, _C8, _C7, _C6, _C5, _C4, _C3, _C2, _C1, _C0, _BF, _BE, _BD, _BC,    \
      _BB, _BA, _B9, _B8, _B7, _B6, _B5, _B4, _B3, _B2, _B1, _B0, _AF, _AE,    \
      _AD, _AC, _AB, _AA, _A9, _A8, _A7, _A6, _A5, _A4, _A3, _A2, _A1, _A0,    \
      _9F, _9E, _9D, _9C, _9B, _9A, _99, _98, _97, _96, _95, _94, _93, _92,    \
      _91, _90, _8F, _8E, _8D, _8C, _8B, _8A, _89, _88, _87, _86, _85, _84,    \
      _83, _82, _81, _80, _7F, _7E, _7D, _7C, _7B, _7A, _79, _78, _77, _76,    \
      _75, _74, _73, _72, _71, _70, _6F, _6E, _6D, _6C, _6B, _6A, _69, _68,    \
      _67, _66, _65, _64, _63, _62, _61, _60, _5F, _5E, _5D, _5C, _5B, _5A,    \
      _59, _58, _57, _56, _55, _54, _53, _52, _51, _50, _4F, _4E, _4D, _4C,    \
      _4B, _4A, _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, _3F, _3E,    \
      _3D, _3C, _3B, _3A, _39, _38, _37, _36, _35, _34, _33, _32, _31, _30,    \
      _2F, _2E, _2D, _2C, _2B, _2A, _29, _28, _27, _26, _25, _24, _23, _22,    \
      _21, _20, _1F, _1E, _1D, _1C, _1B, _1A, _19, _18, _17, _16, _15, _14,    \
      _13, _12, _11, _10, _0F, _0E, _0D, _0C, _0B, _0A, _09, _08, _07, _06,    \
      _05, _04, _03, _02, _01)                                                 \
  (__VA_ARGS__)

#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0(__CHECK_FUNC, __ELEM)

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_0() (0)

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_0() (0)

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_01(__ARG01) ((__ARG01) == 0)

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_01(__ARG01) (1)

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_02(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_01(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_03(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_02(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_04(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_03(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_05(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_04(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_06(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_05(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_07(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_06(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_08(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_07(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_09(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_08(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_0A(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_09(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_0B(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_0A(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_0C(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_0B(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_0D(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_0C(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_0E(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_0D(__VA_ARGS__))

#define __VSF_HAL_COUNT_ARGS_ALL_VALUES_0F(__ARG01, ...)                       \
  __VSF_HAL_COUNT_ARGS_BASE(1, __VSF_HAL_COUNT_ARGS_ALL_VALUES_0E(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_02(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_01(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_03(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_02(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_04(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_03(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_05(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_04(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_06(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_05(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_07(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_06(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_08(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_07(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_09(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_08(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_0A(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_09(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_0B(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_0A(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_0C(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_0B(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_0D(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_0C(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_0E(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_0D(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_0F(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_0E(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_10(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_0F(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_11(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_10(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_12(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_11(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_13(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_12(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_14(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_13(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_15(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_14(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_16(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_15(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_17(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_16(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_18(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_17(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_19(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_18(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_1A(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_19(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_1B(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_1A(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_1C(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_1B(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_1D(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_1C(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_1E(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_1D(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_1F(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_1E(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_20(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_1F(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_21(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_20(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_22(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_21(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_23(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_22(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_24(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_23(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_25(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_24(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_26(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_25(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_27(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_26(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_28(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_27(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_29(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_28(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_2A(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_29(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_2B(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_2A(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_2C(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_2B(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_2D(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_2C(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_2E(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_2D(__VA_ARGS__))

#define __VSF_HAL_ZERO_COUNT_ALL_VALUES_2F(__ARG01, ...)                       \
  __VSF_HAL_ZERO_COUNT_BASE(__ARG01,                                           \
                            __VSF_HAL_ZERO_COUNT_ALL_VALUES_2E(__VA_ARGS__))


#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_01(__CHECK_FUNC, __ELEM, __ARG01)  \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01)

#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_02(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_01(__CHECK_FUNC, __ELEM, __VA_ARGS__)

#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_03(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_02(__CHECK_FUNC, __ELEM, __VA_ARGS__)

#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_04(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_03(__CHECK_FUNC, __ELEM, __VA_ARGS__)

#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_05(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_04(__CHECK_FUNC, __ELEM, __VA_ARGS__)

#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_06(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_05(__CHECK_FUNC, __ELEM, __VA_ARGS__)

#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_07(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_06(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_08(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_07(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_09(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_08(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_09(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_10(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_0F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_11(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_10(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_12(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_11(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_13(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_12(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_14(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_13(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_15(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_14(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_16(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_15(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_17(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_16(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_18(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_17(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_19(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_18(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_19(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_20(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_1F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_21(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_20(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_22(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_21(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_23(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_22(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_24(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_23(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_25(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_24(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_26(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_25(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_27(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_26(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_28(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_27(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_29(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_28(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_29(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_30(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_2F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_31(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_30(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_32(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_31(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_33(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_32(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_34(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_33(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_35(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_34(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_36(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_35(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_37(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_36(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_38(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_37(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_39(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_38(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_39(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_40(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_3F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_41(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_40(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_42(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_41(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_43(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_42(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_44(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_43(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_45(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_44(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_46(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_45(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_47(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_46(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_48(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_47(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_49(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_48(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_49(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_50(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_4F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_51(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_50(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_52(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_51(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_53(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_52(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_54(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_53(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_55(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_54(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_56(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_55(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_57(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_56(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_58(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_57(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_59(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_58(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_59(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_60(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_5F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_61(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_60(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_62(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_61(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_63(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_62(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_64(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_63(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_65(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_64(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_66(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_65(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_67(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_66(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_68(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_67(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_69(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_68(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_69(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_70(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_6F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_71(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_70(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_72(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_71(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_73(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_72(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_74(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_73(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_75(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_74(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_76(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_75(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_77(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_76(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_78(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_77(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_79(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_78(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_79(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_80(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_7F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_81(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_80(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_82(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_81(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_83(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_82(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_84(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_83(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_85(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_84(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_86(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_85(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_87(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_86(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_88(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_87(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_89(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_88(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_89(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_90(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_8F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_91(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_90(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_92(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_91(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_93(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_92(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_94(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_93(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_95(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_94(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_96(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_95(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_97(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_96(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_98(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_97(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_99(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_98(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9A(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_99(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9B(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9A(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9C(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9B(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9D(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9C(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9E(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9D(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9F(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9E(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A0(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_9F(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A1(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A0(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A2(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A1(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A3(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A2(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A4(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A3(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A5(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A4(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A6(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A5(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A7(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A6(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A8(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A7(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A9(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A8(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AA(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_A9(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AB(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AA(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AC(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AB(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AD(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AC(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AE(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AD(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AF(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AE(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B0(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_AF(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B1(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B0(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B2(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B1(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B3(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B2(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B4(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B3(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B5(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B4(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B6(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B5(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B7(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B6(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B8(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B7(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B9(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B8(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BA(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_B9(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BB(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BA(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BC(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BB(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BD(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BC(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BE(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BD(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BF(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BE(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C0(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_BF(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C1(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C0(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C2(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C1(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C3(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C2(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C4(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C3(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C5(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C4(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C6(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C5(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C7(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C6(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C8(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C7(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C9(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C8(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CA(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_C9(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CB(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CA(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CC(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CB(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CD(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CC(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CE(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CD(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CF(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CE(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D0(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_CF(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D1(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D0(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D2(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D1(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D3(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D2(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D4(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D3(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D5(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D4(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D6(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D5(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D7(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D6(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D8(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D7(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D9(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D8(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DA(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_D9(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DB(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DA(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DC(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DB(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DD(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DC(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DE(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DD(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DF(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DE(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E0(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_DF(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E1(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E0(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E2(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E1(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E3(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E2(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E4(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E3(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E5(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E4(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E6(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E5(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E7(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E6(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E8(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E7(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E9(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E8(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EA(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_E9(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EB(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EA(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EC(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EB(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_ED(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EC(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EE(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_ED(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EF(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EE(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F0(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_EF(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F1(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F0(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F2(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F1(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F3(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F2(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F4(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F3(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F5(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F4(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F6(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F5(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F7(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F6(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F8(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F7(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F9(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F8(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FA(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_F9(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FB(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FA(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FC(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FB(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FD(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FC(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FE(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FD(__CHECK_FUNC, __ELEM, __VA_ARGS__)
#define __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FF(__CHECK_FUNC, __ELEM, __ARG01,  \
                                               ...)                            \
  __VSF_HAL_CHECK_UNIQUE_BASE(__CHECK_FUNC, __ELEM, __ARG01);                  \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS_FE(__CHECK_FUNC, __ELEM, __VA_ARGS__)

#define __VSF_HAL_CHECK_UNIQUE_01(__CHECK_FUNC, __A)                           \
  /* Single parameter - no uniqueness check needed */                         \
  __VSF_HAL_STATIC_ASSERT(1, SINGLE, PARAM, "")

#define __VSF_HAL_CHECK_UNIQUE_02(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_01(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_03(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_02(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_04(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_03(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_05(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_04(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_06(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_05(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_07(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_06(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_08(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_07(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_09(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_08(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_0A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_09(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_0B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_0A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_0C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_0B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_0D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_0C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_0E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_0D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_0F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_0E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_10(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_0F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_11(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_10(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_12(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_11(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_13(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_12(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_14(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_13(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_15(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_14(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_16(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_15(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_17(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_16(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_18(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_17(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_19(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_18(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_1A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_19(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_1B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_1A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_1C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_1B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_1D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_1C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_1E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_1D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_1F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_1E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_20(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_1F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_21(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_20(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_22(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_21(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_23(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_22(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_24(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_23(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_25(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_24(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_26(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_25(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_27(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_26(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_28(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_27(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_29(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_28(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_2A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_29(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_2B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_2A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_2C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_2B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_2D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_2C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_2E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_2D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_2F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_2E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_30(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_2F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_31(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_30(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_32(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_31(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_33(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_32(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_34(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_33(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_35(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_34(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_36(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_35(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_37(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_36(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_38(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_37(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_39(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_38(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_3A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_39(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_3B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_3A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_3C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_3B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_3D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_3C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_3E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_3D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_3F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_3E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_40(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_3F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_41(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_40(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_42(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_41(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_43(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_42(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_44(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_43(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_45(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_44(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_46(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_45(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_47(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_46(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_48(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_47(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_49(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_48(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_4A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_49(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_4B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_4A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_4C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_4B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_4D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_4C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_4E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_4D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_4F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_4E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_50(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_4F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_51(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_50(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_52(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_51(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_53(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_52(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_54(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_53(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_55(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_54(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_56(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_55(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_57(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_56(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_58(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_57(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_59(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_58(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_5A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_59(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_5B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_5A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_5C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_5B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_5D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_5C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_5E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_5D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_5F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_5E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_60(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_5F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_61(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_60(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_62(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_61(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_63(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_62(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_64(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_63(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_65(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_64(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_66(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_65(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_67(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_66(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_68(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_67(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_69(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_68(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_6A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_69(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_6B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_6A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_6C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_6B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_6D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_6C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_6E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_6D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_6F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_6E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_70(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_6F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_71(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_70(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_72(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_71(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_73(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_72(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_74(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_73(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_75(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_74(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_76(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_75(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_77(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_76(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_78(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_77(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_79(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_78(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_7A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_79(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_7B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_7A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_7C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_7B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_7D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_7C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_7E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_7D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_7F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_7E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_80(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_7F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_81(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_80(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_82(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_81(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_83(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_82(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_84(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_83(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_85(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_84(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_86(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_85(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_87(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_86(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_88(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_87(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_89(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_88(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_8A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_89(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_8B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_8A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_8C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_8B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_8D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_8C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_8E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_8D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_8F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_8E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_90(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_8F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_91(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_90(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_92(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_91(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_93(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_92(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_94(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_93(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_95(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_94(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_96(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_95(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_97(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_96(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_98(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_97(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_99(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_98(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_9A(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_99(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_9B(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_9A(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_9C(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_9B(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_9D(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_9C(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_9E(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_9D(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_9F(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_9E(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A0(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_9F(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A1(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A0(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A2(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A1(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A3(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A2(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A4(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A3(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A5(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A4(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A6(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A5(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A7(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A6(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A8(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A7(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_A9(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A8(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_AA(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_A9(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_AB(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_AA(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_AC(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_AB(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_AD(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_AC(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_AE(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_AD(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_AF(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_AE(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B0(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_AF(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B1(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B0(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B2(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B1(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B3(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B2(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B4(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B3(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B5(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B4(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B6(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B5(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B7(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B6(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B8(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B7(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_B9(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B8(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_BA(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_B9(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_BB(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_BA(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_BC(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_BB(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_BD(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_BC(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_BE(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_BD(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_BF(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_BE(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C0(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_BF(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C1(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C0(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C2(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C1(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C3(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C2(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C4(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C3(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C5(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C4(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C6(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C5(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C7(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C6(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C8(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C7(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_C9(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C8(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_CA(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_C9(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_CB(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_CA(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_CC(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_CB(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_CD(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_CC(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_CE(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_CD(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_CF(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_CE(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D0(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_CF(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D1(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D0(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D2(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D1(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D3(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D2(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D4(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D3(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D5(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D4(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D6(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D5(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D7(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D6(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D8(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D7(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_D9(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D8(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_DA(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_D9(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_DB(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_DA(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_DC(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_DB(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_DD(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_DC(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_DE(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_DD(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_DF(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_DE(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E0(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_DF(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E1(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E0(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E2(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E1(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E3(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E2(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E4(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E3(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E5(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E4(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E6(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E5(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E7(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E6(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E8(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E7(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_E9(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E8(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_EA(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_E9(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_EB(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_EA(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_EC(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_EB(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_ED(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_EC(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_EE(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_ED(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_EF(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_EE(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F0(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_EF(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F1(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F0(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F2(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F1(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F3(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F2(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F4(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F3(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F5(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F4(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F6(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F5(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F7(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F6(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F8(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F7(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_F9(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F8(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_FA(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_F9(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_FB(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_FA(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_FC(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_FB(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_FD(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_FC(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_FE(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_FD(__CHECK_FUNC, __VA_ARGS__)
#define __VSF_HAL_CHECK_UNIQUE_FF(__CHECK_FUNC, __A, ...)                      \
  __VSF_HAL_CHECK_ELEMENT_WITH_OTHERS(__CHECK_FUNC, __A, __VA_ARGS__);         \
  __VSF_HAL_CHECK_UNIQUE_FE(__CHECK_FUNC, __VA_ARGS__)

#define __VSF_HAL_CHECK_UNIQUE(                                                \
    __CHECK_FUNC, __ARG01, __ARG02, __ARG03, __ARG04, __ARG05, __ARG06,        \
    __ARG07, __ARG08, __ARG09, __ARG0A, __ARG0B, __ARG0C, __ARG0D, __ARG0E,    \
    __ARG0F, __ARG10, __ARG11, __ARG12, __ARG13, __ARG14, __ARG15, __ARG16,    \
    __ARG17, __ARG18, __ARG19, __ARG1A, __ARG1B, __ARG1C, __ARG1D, __ARG1E,    \
    __ARG1F, __ARG20, __ARG21, __ARG22, __ARG23, __ARG24, __ARG25, __ARG26,    \
    __ARG27, __ARG28, __ARG29, __ARG2A, __ARG2B, __ARG2C, __ARG2D, __ARG2E,    \
    __ARG2F, __ARG30, __ARG31, __ARG32, __ARG33, __ARG34, __ARG35, __ARG36,    \
    __ARG37, __ARG38, __ARG39, __ARG3A, __ARG3B, __ARG3C, __ARG3D, __ARG3E,    \
    __ARG3F, __ARG40, __ARG41, __ARG42, __ARG43, __ARG44, __ARG45, __ARG46,    \
    __ARG47, __ARG48, __ARG49, __ARG4A, __ARG4B, __ARG4C, __ARG4D, __ARG4E,    \
    __ARG4F, __ARG50, __ARG51, __ARG52, __ARG53, __ARG54, __ARG55, __ARG56,    \
    __ARG57, __ARG58, __ARG59, __ARG5A, __ARG5B, __ARG5C, __ARG5D, __ARG5E,    \
    __ARG5F, __ARG60, __ARG61, __ARG62, __ARG63, __ARG64, __ARG65, __ARG66,    \
    __ARG67, __ARG68, __ARG69, __ARG6A, __ARG6B, __ARG6C, __ARG6D, __ARG6E,    \
    __ARG6F, __ARG70, __ARG71, __ARG72, __ARG73, __ARG74, __ARG75, __ARG76,    \
    __ARG77, __ARG78, __ARG79, __ARG7A, __ARG7B, __ARG7C, __ARG7D, __ARG7E,    \
    __ARG7F, __ARG80, __ARG81, __ARG82, __ARG83, __ARG84, __ARG85, __ARG86,    \
    __ARG87, __ARG88, __ARG89, __ARG8A, __ARG8B, __ARG8C, __ARG8D, __ARG8E,    \
    __ARG8F, __ARG90, __ARG91, __ARG92, __ARG93, __ARG94, __ARG95, __ARG96,    \
    __ARG97, __ARG98, __ARG99, __ARG9A, __ARG9B, __ARG9C, __ARG9D, __ARG9E,    \
    __ARG9F, __ARGA0, __ARGA1, __ARGA2, __ARGA3, __ARGA4, __ARGA5, __ARGA6,    \
    __ARGA7, __ARGA8, __ARGA9, __ARGAA, __ARGAB, __ARGAC, __ARGAD, __ARGAE,    \
    __ARGAF, __ARGB0, __ARGB1, __ARGB2, __ARGB3, __ARGB4, __ARGB5, __ARGB6,    \
    __ARGB7, __ARGB8, __ARGB9, __ARGBA, __ARGBB, __ARGBC, __ARGBD, __ARGBE,    \
    __ARGBF, __ARGC0, __ARGC1, __ARGC2, __ARGC3, __ARGC4, __ARGC5, __ARGC6,    \
    __ARGC7, __ARGC8, __ARGC9, __ARGCA, __ARGCB, __ARGCC, __ARGCD, __ARGCE,    \
    __ARGCF, __ARGD0, __ARGD1, __ARGD2, __ARGD3, __ARGD4, __ARGD5, __ARGD6,    \
    __ARGD7, __ARGD8, __ARGD9, __ARGDA, __ARGDB, __ARGDC, __ARGDD, __ARGDE,    \
    __ARGDF, __ARGE0, __ARGE1, __ARGE2, __ARGE3, __ARGE4, __ARGE5, __ARGE6,    \
    __ARGE7, __ARGE8, __ARGE9, __ARGEA, __ARGEB, __ARGEC, __ARGED, __ARGEE,    \
    __ARGEF, __ARGF0, __ARGF1, __ARGF2, __ARGF3, __ARGF4, __ARGF5, __ARGF6,    \
    __ARGF7, __ARGF8, __ARGF9, __ARGFA, __ARGFB, __ARGFC, __ARGFD, __ARGFE,    \
    __ARGFF, N, ...)                                                           \
  __VSF_HAL_CHECK_UNIQUE##N

/* Default loose mode (value-unique check) - allows overlapping bits */
#define VSF_HAL_CHECK_UNIQUE(...)                                              \
  __VSF_HAL_CHECK_UNIQUE(                                                      \
      VSF_HAL_CHECK_MODE_LOOSE, __VA_ARGS__, _FF, _FE, _FD, _FC, _FB, _FA,     \
      _F9, _F8, _F7, _F6, _F5, _F4, _F3, _F2, _F1, _F0, _EF, _EE, _ED, _EC,    \
      _EB, _EA, _E9, _E8, _E7, _E6, _E5, _E4, _E3, _E2, _E1, _E0, _DF, _DE,    \
      _DD, _DC, _DB, _DA, _D9, _D8, _D7, _D6, _D5, _D4, _D3, _D2, _D1, _D0,    \
      _CF, _CE, _CD, _CC, _CB, _CA, _C9, _C8, _C7, _C6, _C5, _C4, _C3, _C2,    \
      _C1, _C0, _BF, _BE, _BD, _BC, _BB, _BA, _B9, _B8, _B7, _B6, _B5, _B4,    \
      _B3, _B2, _B1, _B0, _AF, _AE, _AD, _AC, _AB, _AA, _A9, _A8, _A7, _A6,    \
      _A5, _A4, _A3, _A2, _A1, _A0, _9F, _9E, _9D, _9C, _9B, _9A, _99, _98,    \
      _97, _96, _95, _94, _93, _92, _91, _90, _8F, _8E, _8D, _8C, _8B, _8A,    \
      _89, _88, _87, _86, _85, _84, _83, _82, _81, _80, _7F, _7E, _7D, _7C,    \
      _7B, _7A, _79, _78, _77, _76, _75, _74, _73, _72, _71, _70, _6F, _6E,    \
      _6D, _6C, _6B, _6A, _69, _68, _67, _66, _65, _64, _63, _62, _61, _60,    \
      _5F, _5E, _5D, _5C, _5B, _5A, _59, _58, _57, _56, _55, _54, _53, _52,    \
      _51, _50, _4F, _4E, _4D, _4C, _4B, _4A, _49, _48, _47, _46, _45, _44,    \
      _43, _42, _41, _40, _3F, _3E, _3D, _3C, _3B, _3A, _39, _38, _37, _36,    \
      _35, _34, _33, _32, _31, _30, _2F, _2E, _2D, _2C, _2B, _2A, _29, _28,    \
      _27, _26, _25, _24, _23, _22, _21, _20, _1F, _1E, _1D, _1C, _1B, _1A,    \
      _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _0F, _0E, _0D, _0C,    \
      _0B, _0A, _09, _08, _07, _06, _05, _04, _03, _02,                        \
      _01)(VSF_HAL_CHECK_MODE_LOOSE, __VA_ARGS__)

/* Strict mode (bit-disjoint check) - ensures no overlapping bits */
#define VSF_HAL_CHECK_UNIQUE_STRICT(...)                                       \
  __VSF_HAL_CHECK_UNIQUE(                                                      \
      VSF_HAL_CHECK_MODE_STRICT, __VA_ARGS__, _FF, _FE, _FD, _FC, _FB, _FA,    \
      _F9, _F8, _F7, _F6, _F5, _F4, _F3, _F2, _F1, _F0, _EF, _EE, _ED, _EC,    \
      _EB, _EA, _E9, _E8, _E7, _E6, _E5, _E4, _E3, _E2, _E1, _E0, _DF, _DE,    \
      _DD, _DC, _DB, _DA, _D9, _D8, _D7, _D6, _D5, _D4, _D3, _D2, _D1, _D0,    \
      _CF, _CE, _CD, _CC, _CB, _CA, _C9, _C8, _C7, _C6, _C5, _C4, _C3, _C2,    \
      _C1, _C0, _BF, _BE, _BD, _BC, _BB, _BA, _B9, _B8, _B7, _B6, _B5, _B4,    \
      _B3, _B2, _B1, _B0, _AF, _AE, _AD, _AC, _AB, _AA, _A9, _A8, _A7, _A6,    \
      _A5, _A4, _A3, _A2, _A1, _A0, _9F, _9E, _9D, _9C, _9B, _9A, _99, _98,    \
      _97, _96, _95, _94, _93, _92, _91, _90, _8F, _8E, _8D, _8C, _8B, _8A,    \
      _89, _88, _87, _86, _85, _84, _83, _82, _81, _80, _7F, _7E, _7D, _7C,    \
      _7B, _7A, _79, _78, _77, _76, _75, _74, _73, _72, _71, _70, _6F, _6E,    \
      _6D, _6C, _6B, _6A, _69, _68, _67, _66, _65, _64, _63, _62, _61, _60,    \
      _5F, _5E, _5D, _5C, _5B, _5A, _59, _58, _57, _56, _55, _54, _53, _52,    \
      _51, _50, _4F, _4E, _4D, _4C, _4B, _4A, _49, _48, _47, _46, _45, _44,    \
      _43, _42, _41, _40, _3F, _3E, _3D, _3C, _3B, _3A, _39, _38, _37, _36,    \
      _35, _34, _33, _32, _31, _30, _2F, _2E, _2D, _2C, _2B, _2A, _29, _28,    \
      _27, _26, _25, _24, _23, _22, _21, _20, _1F, _1E, _1D, _1C, _1B, _1A,    \
      _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _0F, _0E, _0D, _0C,    \
      _0B, _0A, _09, _08, _07, _06, _05, _04, _03, _02,                        \
      _01)(VSF_HAL_CHECK_MODE_STRICT, __VA_ARGS__)

/* Custom mode - use user-provided check function */
#define VSF_HAL_CHECK_UNIQUE_CUSTOM(__CHECK_FUNC, ...)                         \
  __VSF_HAL_CHECK_UNIQUE(                                                      \
      __CHECK_FUNC, __VA_ARGS__, _FF, _FE, _FD, _FC, _FB, _FA, _F9, _F8, _F7,  \
      _F6, _F5, _F4, _F3, _F2, _F1, _F0, _EF, _EE, _ED, _EC, _EB, _EA, _E9,    \
      _E8, _E7, _E6, _E5, _E4, _E3, _E2, _E1, _E0, _DF, _DE, _DD, _DC, _DB,    \
      _DA, _D9, _D8, _D7, _D6, _D5, _D4, _D3, _D2, _D1, _D0, _CF, _CE, _CD,    \
      _CC, _CB, _CA, _C9, _C8, _C7, _C6, _C5, _C4, _C3, _C2, _C1, _C0, _BF,    \
      _BE, _BD, _BC, _BB, _BA, _B9, _B8, _B7, _B6, _B5, _B4, _B3, _B2, _B1,    \
      _B0, _AF, _AE, _AD, _AC, _AB, _AA, _A9, _A8, _A7, _A6, _A5, _A4, _A3,    \
      _A2, _A1, _A0, _9F, _9E, _9D, _9C, _9B, _9A, _99, _98, _97, _96, _95,    \
      _94, _93, _92, _91, _90, _8F, _8E, _8D, _8C, _8B, _8A, _89, _88, _87,    \
      _86, _85, _84, _83, _82, _81, _80, _7F, _7E, _7D, _7C, _7B, _7A, _79,    \
      _78, _77, _76, _75, _74, _73, _72, _71, _70, _6F, _6E, _6D, _6C, _6B,    \
      _6A, _69, _68, _67, _66, _65, _64, _63, _62, _61, _60, _5F, _5E, _5D,    \
      _5C, _5B, _5A, _59, _58, _57, _56, _55, _54, _53, _52, _51, _50, _4F,    \
      _4E, _4D, _4C, _4B, _4A, _49, _48, _47, _46, _45, _44, _43, _42, _41,    \
      _40, _3F, _3E, _3D, _3C, _3B, _3A, _39, _38, _37, _36, _35, _34, _33,    \
      _32, _31, _30, _2F, _2E, _2D, _2C, _2B, _2A, _29, _28, _27, _26, _25,    \
      _24, _23, _22, _21, _20, _1F, _1E, _1D, _1C, _1B, _1A, _19, _18, _17,    \
      _16, _15, _14, _13, _12, _11, _10, _0F, _0E, _0D, _0C, _0B, _0A, _09,    \
      _08, _07, _06, _05, _04, _03, _02, _01)(__CHECK_FUNC, __VA_ARGS__)

/* All-zero mode - check that all values are zero using zero count */
#define VSF_HAL_CHECK_UNIQUE_ALL_ZERO(...)                                     \
  VSF_HAL_CHECK_MODE_ALL_ZERO(__VA_ARGS__)

/* All-non-zero mode - check that all values are non-zero using zero count */
#define VSF_HAL_CHECK_UNIQUE_ALL_NON_ZERO(...)                                 \
  VSF_HAL_CHECK_MODE_ALL_NON_ZERO(__VA_ARGS__)

/* Zero count mode - check that exactly COUNT values are zero */
#define VSF_HAL_CHECK_UNIQUE_ZERO_COUNT(__COUNT, ...)                          \
  VSF_HAL_CHECK_MODE_ZERO_COUNT(__COUNT, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* __VSF_STATIC_ASSERT_UNIQUE_H__ */
