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

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/errno.h"
#else
#   include <unistd.h>
#   include <errno.h>
#endif
#include <stdio.h>
#include <stdatomic.h>

#if VSF_LINUX_APPLET_USE_LIBC_SETJMP == ENABLED && !defined(__VSF_APPLET__)
#   include <setjmp.h>
#   define __SIMPLE_LIBC_SETJMP_VPLT_ONLY__
#   include "../../include/simple_libc/setjmp/setjmp.h"
#endif
#if VSF_LINUX_APPLET_USE_LIBC_MATH == ENABLED && !defined(__VSF_APPLET__)
#   include <math.h>
#   define __SIMPLE_LIBC_MATH_VPLT_ONLY__
#   include "../../include/simple_libc/math/math.h"
#endif
#if VSF_LINUX_APPLET_USE_LIBC_WCHAR == ENABLED && !defined(__VSF_APPLET__)
#   include <wchar.h>
#   include <wctype.h>
#endif

#include <resolv.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_APPLET_USE_LIBC_SETJMP == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_libc_setjmp_vplt_t vsf_linux_libc_setjmp_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_libc_setjmp_vplt_t, 0, 0, true),

#ifdef VSF_ARCH_SETJMP
    VSF_APPLET_VPLT_ENTRY_FUNC_EX(fn_setjmp, "setjmp", VSF_ARCH_SETJMP),
#else
    VSF_APPLET_VPLT_ENTRY_FUNC(setjmp),
#endif
#ifdef VSF_ARCH_LONGJMP
    VSF_APPLET_VPLT_ENTRY_FUNC_EX(fn_longjmp, "longjmp", VSF_ARCH_LONGJMP),
#else
    VSF_APPLET_VPLT_ENTRY_FUNC(longjmp),
#endif
};
#endif

#if VSF_LINUX_APPLET_USE_LIBC_MATH == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_libc_math_vplt_t vsf_linux_libc_math_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_libc_math_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(atan),
    VSF_APPLET_VPLT_ENTRY_FUNC(cos),
    VSF_APPLET_VPLT_ENTRY_FUNC(sin),
    VSF_APPLET_VPLT_ENTRY_FUNC(tan),
    VSF_APPLET_VPLT_ENTRY_FUNC(tanh),
    VSF_APPLET_VPLT_ENTRY_FUNC(frexp),
    VSF_APPLET_VPLT_ENTRY_FUNC(modf),
    VSF_APPLET_VPLT_ENTRY_FUNC(ceil),
    VSF_APPLET_VPLT_ENTRY_FUNC(fabs),
    VSF_APPLET_VPLT_ENTRY_FUNC(floor),
    VSF_APPLET_VPLT_ENTRY_FUNC(acos),
    VSF_APPLET_VPLT_ENTRY_FUNC(asin),
    VSF_APPLET_VPLT_ENTRY_FUNC(atan2),
    VSF_APPLET_VPLT_ENTRY_FUNC(cosh),
    VSF_APPLET_VPLT_ENTRY_FUNC(sinh),
    VSF_APPLET_VPLT_ENTRY_FUNC(exp),
    VSF_APPLET_VPLT_ENTRY_FUNC(ldexp),
    VSF_APPLET_VPLT_ENTRY_FUNC(log),
    VSF_APPLET_VPLT_ENTRY_FUNC(log10),
    VSF_APPLET_VPLT_ENTRY_FUNC(pow),
    VSF_APPLET_VPLT_ENTRY_FUNC(sqrt),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmod),
    VSF_APPLET_VPLT_ENTRY_FUNC(nan),
    VSF_APPLET_VPLT_ENTRY_FUNC(copysign),
    VSF_APPLET_VPLT_ENTRY_FUNC(logb),
    VSF_APPLET_VPLT_ENTRY_FUNC(ilogb),
    VSF_APPLET_VPLT_ENTRY_FUNC(asinh),
    VSF_APPLET_VPLT_ENTRY_FUNC(cbrt),
    VSF_APPLET_VPLT_ENTRY_FUNC(nextafter),
    VSF_APPLET_VPLT_ENTRY_FUNC(rint),
    VSF_APPLET_VPLT_ENTRY_FUNC(scalbn),
    VSF_APPLET_VPLT_ENTRY_FUNC(exp2),
    VSF_APPLET_VPLT_ENTRY_FUNC(scalbln),
    VSF_APPLET_VPLT_ENTRY_FUNC(tgamma),
    VSF_APPLET_VPLT_ENTRY_FUNC(nearbyint),
    VSF_APPLET_VPLT_ENTRY_FUNC(lrint),
    VSF_APPLET_VPLT_ENTRY_FUNC(llrint),
    VSF_APPLET_VPLT_ENTRY_FUNC(round),
    VSF_APPLET_VPLT_ENTRY_FUNC(lround),
    VSF_APPLET_VPLT_ENTRY_FUNC(llround),
    VSF_APPLET_VPLT_ENTRY_FUNC(trunc),
    VSF_APPLET_VPLT_ENTRY_FUNC(remquo),
    VSF_APPLET_VPLT_ENTRY_FUNC(fdim),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmax),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmin),
    VSF_APPLET_VPLT_ENTRY_FUNC(fma),
    VSF_APPLET_VPLT_ENTRY_FUNC(log1p),
    VSF_APPLET_VPLT_ENTRY_FUNC(expm1),
    VSF_APPLET_VPLT_ENTRY_FUNC(acosh),
    VSF_APPLET_VPLT_ENTRY_FUNC(atanh),
    VSF_APPLET_VPLT_ENTRY_FUNC(remainder),
    VSF_APPLET_VPLT_ENTRY_FUNC(lgamma),
    VSF_APPLET_VPLT_ENTRY_FUNC(erf),
    VSF_APPLET_VPLT_ENTRY_FUNC(erfc),
    VSF_APPLET_VPLT_ENTRY_FUNC(log2),
    VSF_APPLET_VPLT_ENTRY_FUNC(hypot),
    VSF_APPLET_VPLT_ENTRY_FUNC(atanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(cosf),
    VSF_APPLET_VPLT_ENTRY_FUNC(sinf),
    VSF_APPLET_VPLT_ENTRY_FUNC(tanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(tanhf),
    VSF_APPLET_VPLT_ENTRY_FUNC(modff),
    VSF_APPLET_VPLT_ENTRY_FUNC(ceilf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fabsf),
    VSF_APPLET_VPLT_ENTRY_FUNC(floorf),
    VSF_APPLET_VPLT_ENTRY_FUNC(acosf),
    VSF_APPLET_VPLT_ENTRY_FUNC(asinf),
    VSF_APPLET_VPLT_ENTRY_FUNC(atan2f),
    VSF_APPLET_VPLT_ENTRY_FUNC(coshf),
    VSF_APPLET_VPLT_ENTRY_FUNC(sinhf),
    VSF_APPLET_VPLT_ENTRY_FUNC(expf),
    VSF_APPLET_VPLT_ENTRY_FUNC(ldexpf),
    VSF_APPLET_VPLT_ENTRY_FUNC(logf),
    VSF_APPLET_VPLT_ENTRY_FUNC(log10f),
    VSF_APPLET_VPLT_ENTRY_FUNC(powf),
    VSF_APPLET_VPLT_ENTRY_FUNC(sqrtf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmodf),
    VSF_APPLET_VPLT_ENTRY_FUNC(exp2f),
    VSF_APPLET_VPLT_ENTRY_FUNC(scalblnf),
    VSF_APPLET_VPLT_ENTRY_FUNC(tgammaf),
    VSF_APPLET_VPLT_ENTRY_FUNC(nearbyintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(lrintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(llrintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(roundf),
    VSF_APPLET_VPLT_ENTRY_FUNC(lroundf),
    VSF_APPLET_VPLT_ENTRY_FUNC(llroundf),
    VSF_APPLET_VPLT_ENTRY_FUNC(truncf),
    VSF_APPLET_VPLT_ENTRY_FUNC(remquof),
    VSF_APPLET_VPLT_ENTRY_FUNC(fdimf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmaxf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fminf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmaf),
    VSF_APPLET_VPLT_ENTRY_FUNC(nanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(copysignf),
    VSF_APPLET_VPLT_ENTRY_FUNC(logbf),
    VSF_APPLET_VPLT_ENTRY_FUNC(ilogbf),
    VSF_APPLET_VPLT_ENTRY_FUNC(asinhf),
    VSF_APPLET_VPLT_ENTRY_FUNC(cbrtf),
    VSF_APPLET_VPLT_ENTRY_FUNC(nextafterf),
    VSF_APPLET_VPLT_ENTRY_FUNC(rintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(scalbnf),
    VSF_APPLET_VPLT_ENTRY_FUNC(log1pf),
    VSF_APPLET_VPLT_ENTRY_FUNC(expm1f),
    VSF_APPLET_VPLT_ENTRY_FUNC(acoshf),
    VSF_APPLET_VPLT_ENTRY_FUNC(atanhf),
    VSF_APPLET_VPLT_ENTRY_FUNC(remainderf),
    VSF_APPLET_VPLT_ENTRY_FUNC(lgammaf),
    VSF_APPLET_VPLT_ENTRY_FUNC(erff),
    VSF_APPLET_VPLT_ENTRY_FUNC(erfcf),
    VSF_APPLET_VPLT_ENTRY_FUNC(log2f),
    VSF_APPLET_VPLT_ENTRY_FUNC(hypotf),
    VSF_APPLET_VPLT_ENTRY_FUNC(frexpf),

#if VSF_LINUX_APPLET_LIBC_MATH_LONG_DOULBE == ENABLED
    VSF_APPLET_VPLT_ENTRY_FUNC(atanl),
    VSF_APPLET_VPLT_ENTRY_FUNC(cosl),
    VSF_APPLET_VPLT_ENTRY_FUNC(sinl),
    VSF_APPLET_VPLT_ENTRY_FUNC(tanl),
    VSF_APPLET_VPLT_ENTRY_FUNC(frexpl),
    VSF_APPLET_VPLT_ENTRY_FUNC(fabsl),
    VSF_APPLET_VPLT_ENTRY_FUNC(log1pl),
    VSF_APPLET_VPLT_ENTRY_FUNC(expm1l),
    VSF_APPLET_VPLT_ENTRY_FUNC(atan2l),
    VSF_APPLET_VPLT_ENTRY_FUNC(coshl),
    VSF_APPLET_VPLT_ENTRY_FUNC(sinhl),
    VSF_APPLET_VPLT_ENTRY_FUNC(expl),
    VSF_APPLET_VPLT_ENTRY_FUNC(ldexpl),
    VSF_APPLET_VPLT_ENTRY_FUNC(logl),
    VSF_APPLET_VPLT_ENTRY_FUNC(powl),
    VSF_APPLET_VPLT_ENTRY_FUNC(sqrtl),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmodl),
    VSF_APPLET_VPLT_ENTRY_FUNC(hypotl),
    VSF_APPLET_VPLT_ENTRY_FUNC(copysignl),
    VSF_APPLET_VPLT_ENTRY_FUNC(nanl),
    VSF_APPLET_VPLT_ENTRY_FUNC(ilogbl),
    VSF_APPLET_VPLT_ENTRY_FUNC(asinhl),
    VSF_APPLET_VPLT_ENTRY_FUNC(cbrtl),
    VSF_APPLET_VPLT_ENTRY_FUNC(nextafterl),
#ifndef __COMPILER_HAS_NO_NEXTTOWARDF_IN_MATH__
    VSF_APPLET_VPLT_ENTRY_FUNC(nexttowardf),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(nexttoward),
    VSF_APPLET_VPLT_ENTRY_FUNC(nexttowardl),
    VSF_APPLET_VPLT_ENTRY_FUNC(logbl),
    VSF_APPLET_VPLT_ENTRY_FUNC(log2l),
    VSF_APPLET_VPLT_ENTRY_FUNC(rintl),
    VSF_APPLET_VPLT_ENTRY_FUNC(scalbnl),
    VSF_APPLET_VPLT_ENTRY_FUNC(exp2l),
    VSF_APPLET_VPLT_ENTRY_FUNC(scalblnl),
    VSF_APPLET_VPLT_ENTRY_FUNC(tgammal),
    VSF_APPLET_VPLT_ENTRY_FUNC(nearbyintl),
    VSF_APPLET_VPLT_ENTRY_FUNC(lrintl),
    VSF_APPLET_VPLT_ENTRY_FUNC(llrintl),
    VSF_APPLET_VPLT_ENTRY_FUNC(roundl),
    VSF_APPLET_VPLT_ENTRY_FUNC(lroundl),
    VSF_APPLET_VPLT_ENTRY_FUNC(llroundl),
    VSF_APPLET_VPLT_ENTRY_FUNC(truncl),
    VSF_APPLET_VPLT_ENTRY_FUNC(remquol),
    VSF_APPLET_VPLT_ENTRY_FUNC(fdiml),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmaxl),
    VSF_APPLET_VPLT_ENTRY_FUNC(fminl),
    VSF_APPLET_VPLT_ENTRY_FUNC(fmal),
    VSF_APPLET_VPLT_ENTRY_FUNC(acoshl),
    VSF_APPLET_VPLT_ENTRY_FUNC(atanhl),
    VSF_APPLET_VPLT_ENTRY_FUNC(remainderl),
    VSF_APPLET_VPLT_ENTRY_FUNC(lgammal),
    VSF_APPLET_VPLT_ENTRY_FUNC(erfl),
    VSF_APPLET_VPLT_ENTRY_FUNC(erfcl),
    VSF_APPLET_VPLT_ENTRY_FUNC(log10l),
    VSF_APPLET_VPLT_ENTRY_FUNC(tanhl),
    VSF_APPLET_VPLT_ENTRY_FUNC(modfl),
    VSF_APPLET_VPLT_ENTRY_FUNC(ceill),
    VSF_APPLET_VPLT_ENTRY_FUNC(floorl),
    VSF_APPLET_VPLT_ENTRY_FUNC(acosl),
    VSF_APPLET_VPLT_ENTRY_FUNC(asinl),
#endif
};
#endif

#if VSF_LINUX_APPLET_USE_LIBC_WCHAR == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_libc_wchar_vplt_t vsf_linux_libc_wchar_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_libc_wchar_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(wcslen),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcscat),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcsncat),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcschr),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcsrchr),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcscmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcsncmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcscoll),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcscpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcsncpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcsstr),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcstok),

    VSF_APPLET_VPLT_ENTRY_FUNC(wmemset),
    VSF_APPLET_VPLT_ENTRY_FUNC(wmemchr),
    VSF_APPLET_VPLT_ENTRY_FUNC(wmemcmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(wmemcpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(wmemmove),

    VSF_APPLET_VPLT_ENTRY_FUNC(wcstol),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcstoll),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcstoull),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcstoul),

    VSF_APPLET_VPLT_ENTRY_FUNC(wcspbrk),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcsspn),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcscspn),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcsxfrm),

    VSF_APPLET_VPLT_ENTRY_FUNC(swprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vswprintf),

    // wctype.h
    VSF_APPLET_VPLT_ENTRY_FUNC(iswalnum),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswalpha),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswlower),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswupper),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswdigit),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswxdigit),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswcntrl),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswgraph),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswspace),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswblank),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswprint),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswpunct),
    VSF_APPLET_VPLT_ENTRY_FUNC(iswctype),
    VSF_APPLET_VPLT_ENTRY_FUNC(wctype),
    VSF_APPLET_VPLT_ENTRY_FUNC(towlower),
    VSF_APPLET_VPLT_ENTRY_FUNC(towupper),
    VSF_APPLET_VPLT_ENTRY_FUNC(wctrans),
    // towctrans will introduce other internal APIs in gcc
#if !__IS_COMPILER_GCC__
    VSF_APPLET_VPLT_ENTRY_FUNC(towctrans),
#endif

    // If APIs below are included, there will conflict with APIs implemented
    //  when VSF_LINUX_SIMPLE_STDLIB_USE_STRTOXX is set.
    // TODO: implement these APIs
#if VSF_LINUX_SIMPLE_STDLIB_USE_STRTOXX != ENABLED
    VSF_APPLET_VPLT_ENTRY_FUNC(wcstof),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcstod),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcstold),
    VSF_APPLET_VPLT_ENTRY_FUNC(wcsftime),
#endif
};
#endif

void vsf_linux_glibc_init(void)
{

}

// b64_ntop and b64_pton from https://github.com/yasuoka/base64

int b64_ntop(const unsigned char *src, int srclen, char *dst, size_t dstlen)
{
    int i, j, expect_siz;
    uint32_t bit24;
    const char b64str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    expect_siz = ((srclen + 2) / 3) * 4 + 1;

    if (dst == NULL)
        return (expect_siz);
    if (dstlen < expect_siz)
        return (-1);

    for (i = 0, j = 0; i < srclen; i += 3) {
        bit24 = src[i] << 16;
        if (i + 1 < srclen)
            bit24 |= src[i + 1] << 8;
        if (i + 2 < srclen)
            bit24 |= src[i + 2];

        dst[j++] = b64str[(bit24 & 0xfc0000) >> 18];
        dst[j++] = b64str[(bit24 & 0x03f000) >> 12];
        if (i + 1 < srclen)
            dst[j++] = b64str[(bit24 & 0x000fc0) >> 6];
        else
            dst[j++] = '=';
        if (i + 2 < srclen)
            dst[j++] = b64str[(bit24 & 0x00003f)];
        else
            dst[j++] = '=';
    }
    dst[j] = '\0';

    return j;
}

int b64_pton(char *src, unsigned char *dst, size_t dstlen)
{
    int i, j, k;
    uint32_t val3 = 0;
    const char b64_tbl[] =
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x3e\xff\xff\xff\x3f"
        "\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\xff\xff\xff\x00\xff\xff"
        "\xff\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e"
        "\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\xff\xff\xff\xff\xff"
        "\xff\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28"
        "\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";

    for (i = j = k = 0; src[i] != '\0'; i++) {
        if (isspace(src[i]))
            continue;
        if (b64_tbl[(u_char)src[i]] == (char)0xff)
            return(-1);
        val3 |= b64_tbl[(u_char)src[i]];
        if (src[i] != '=') {
            if (dst != NULL && k >= (ssize_t)dstlen)
                return (-1);
            if (j % 4 == 1) {
                if (dst != NULL)
                    dst[k] = val3 >> 4;
                k++;
            } else if (j % 4 == 2) {
                if (dst != NULL)
                    dst[k] = val3 >> 2;
                k++;
            } else if (j % 4 == 3) {
                if (dst != NULL)
                    dst[k] = val3;
                k++;
            }
        }
        val3 <<= 6;
        j++;
    }
    if (j % 4 != 0)
        return (-1);

    return (k);
}

// atomic

VSF_CAL_WEAK(atomic_thread_fence)
void atomic_thread_fence(enum memory_order order)
{
}

VSF_CAL_WEAK(atomic_signal_fence)
void atomic_signal_fence(enum memory_order m)
{
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC
