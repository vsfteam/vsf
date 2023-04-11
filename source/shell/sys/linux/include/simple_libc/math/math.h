#ifndef __SIMPLE_LIBC_MATH_H__
#define __SIMPLE_LIBC_MATH_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

// for ALIGN and stdint.h
#include "utilities/compiler/compiler.h"

#if VSF_LINUX_APPLET_USE_LIBC_MATH == ENABLED
typedef struct vsf_linux_libc_math_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atan);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(cos);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sin);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tan);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tanh);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(frexp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(modf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ceil);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fabs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(floor);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(acos);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asin);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atan2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(cosh);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sinh);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(exp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ldexp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log10);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pow);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sqrt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmod);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nan);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(copysign);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(logb);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ilogb);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asinh);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(cbrt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nextafter);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(rint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(scalbn);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(exp2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(scalbln);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tgamma);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nearbyint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lrint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(llrint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(round);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lround);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(llround);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(trunc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(remquo);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fdim);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmax);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmin);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fma);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log1p);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(expm1);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(acosh);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atanh);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(remainder);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lgamma);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(erf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(erfc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log2);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(hypot);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atanf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(cosf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sinf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tanf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tanhf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(modff);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ceilf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fabsf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(floorf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(acosf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asinf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atan2f);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(coshf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sinhf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(expf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ldexpf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(logf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log10f);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(powf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sqrtf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmodf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(exp2f);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(scalblnf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tgammaf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nearbyintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lrintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(llrintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(roundf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lroundf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(llroundf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(truncf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(remquof);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fdimf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmaxf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fminf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmaf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nanf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(copysignf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(logbf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ilogbf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asinhf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(cbrtf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nextafterf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(rintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(scalbnf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log1pf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(expm1f);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(acoshf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atanhf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(remainderf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lgammaf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(erff);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(erfcf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log2f);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(hypotf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atanl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(cosl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sinl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tanl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(frexpl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fabsl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log1pl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(expm1l);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atan2l);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(coshl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sinhl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(expl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ldexpl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(logl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(powl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sqrtl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmodl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(hypotl);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(copysignl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nanl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ilogbl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asinhl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(cbrtl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nextafterl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nexttowardf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nexttoward);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nexttowardl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(logbl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log2l);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(rintl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(scalbnl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(exp2l);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(scalblnl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tgammal);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nearbyintl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lrintl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(llrintl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(roundl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lroundl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(llroundl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(truncl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(remquol);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fdiml);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmaxl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fminl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fmal);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(acoshl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atanhl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(remainderl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lgammal);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(erfl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(erfcl);

#if 0
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(infinity);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gamma);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(frexpf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(infinityf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gammaf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tanhl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(modfl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ceill);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(floorl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(acosl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asinl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log10l);
#endif
} vsf_linux_libc_math_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_math_vplt_t vsf_linux_libc_math_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_LIBC_MATH == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_MATH_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_MATH_VPLT                                  \
            ((vsf_linux_libc_math_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_math_vplt))
#   else
#       define VSF_LINUX_APPLET_LIBC_MATH_VPLT                                  \
            ((vsf_linux_libc_math_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_MATH_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_MATH_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_MATH_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_MATH_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_LIBC_MATH_IMP(atan, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atan)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(cos, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double sin(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double tan(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double tanh(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double frexp(double, int *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double modf(double, double *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double ceil(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double fabs(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double floor(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double acos(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double asin(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double atan2(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double cosh(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double sinh(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double exp(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double ldexp(double, int) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double log(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double log10(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double pow(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double sqrt(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double fmod(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double nan(const char *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double copysign(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double logb(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(int ilogb(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double asinh(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double cbrt(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double nextafter(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double rint(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double scalbn(double, int) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double exp2(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double scalbln(double, long int) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double tgamma(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double nearbyint(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long int lrint(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long long int llrint(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double round(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long int lround(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long long int llround(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double trunc(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double remquo(double, double, int *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double fdim(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double fmax(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double fmin(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double fma(double, double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double log1p(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double expm1(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double acosh(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double atanh(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double remainder(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double lgamma(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double erf(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double erfc(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double log2(double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double hypot(double, double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float atanf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float cosf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float sinf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float tanf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float tanhf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float modff(float, float *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float ceilf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float fabsf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float floorf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float acosf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float asinf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float atan2f(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float coshf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float sinhf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float expf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float ldexpf(float, int) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float logf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float log10f(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float powf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float sqrtf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float fmodf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float exp2f(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float scalblnf(float, long int) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float tgammaf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float nearbyintf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long int lrintf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long long int llrintf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float roundf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long int lroundf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long long int llroundf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float truncf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float remquof(float, float, int *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float fdimf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float fmaxf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float fminf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float fmaf(float, float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float nanf(const char *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float copysignf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float logbf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(int ilogbf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float asinhf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float cbrtf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float nextafterf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float rintf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float scalbnf(float, int) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float log1pf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float expm1f(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float acoshf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float atanhf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float remainderf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float lgammaf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float erff(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float erfcf(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float log2f(float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float hypotf(float, float) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double atanl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double cosl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double sinl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double tanl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double frexpl(long double, int *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double fabsl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double log1pl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double expm1l(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double atan2l(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double coshl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double sinhl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double expl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double ldexpl(long double, int) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double logl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double powl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double sqrtl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double fmodl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double hypotl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double copysignl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double nanl(const char *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(int ilogbl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double asinhl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double cbrtl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double nextafterl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(float nexttowardf(float, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(double nexttoward(double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double nexttowardl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double logbl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double log2l(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double rintl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double scalbnl(long double, int) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double exp2l(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double scalblnl(long double, long) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double tgammal(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double nearbyintl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long int lrintl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long long int llrintl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double roundl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long lroundl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long long int llroundl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double truncl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double remquol(long double, long double, int *) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double fdiml(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double fmaxl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double fminl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double fmal(long double, long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double acoshl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double atanhl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double remainderl(long double, long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double lgammal(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double erfl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(long double erfcl(long double) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cos)(a0);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_MATH

extern double atan(double);
extern double cos(double);
extern double sin(double);
extern double tan(double);
extern double tanh(double);
extern double frexp(double, int *);
extern double modf(double, double *);
extern double ceil(double);
extern double fabs(double);
extern double floor(double);

extern double acos(double);
extern double asin(double);
extern double atan2(double, double);
extern double cosh(double);
extern double sinh(double);
extern double exp(double);
extern double ldexp(double, int);
extern double log(double);
extern double log10(double);
extern double pow(double, double);
extern double sqrt(double);
extern double fmod(double, double);

extern double nan(const char *);
extern double copysign(double, double);
extern double logb(double);
extern int ilogb(double);

extern double asinh(double);
extern double cbrt(double);
extern double nextafter(double, double);
extern double rint(double);
extern double scalbn(double, int);

extern double exp2(double);
extern double scalbln(double, long int);
extern double tgamma(double);
extern double nearbyint(double);
extern long int lrint(double);
extern long long int llrint(double);
extern double round(double);
extern long int lround(double);
extern long long int llround(double);
extern double trunc(double);
extern double remquo(double, double, int *);
extern double fdim(double, double);
extern double fmax(double, double);
extern double fmin(double, double);
extern double fma(double, double, double);

extern double log1p(double);
extern double expm1(double);

extern double acosh(double);
extern double atanh(double);
extern double remainder(double, double);
extern double lgamma(double);
extern double erf(double);
extern double erfc(double);
extern double log2(double);

extern double hypot(double, double);

extern float atanf(float);
extern float cosf(float);
extern float sinf(float);
extern float tanf(float);
extern float tanhf(float);
extern float modff(float, float *);
extern float ceilf(float);
extern float fabsf(float);
extern float floorf(float);

extern float acosf(float);
extern float asinf(float);
extern float atan2f(float, float);
extern float coshf(float);
extern float sinhf(float);
extern float expf(float);
extern float ldexpf(float, int);
extern float logf(float);
extern float log10f(float);
extern float powf(float, float);
extern float sqrtf(float);
extern float fmodf(float, float);

extern float exp2f(float);
extern float scalblnf(float, long int);
extern float tgammaf(float);
extern float nearbyintf(float);
extern long int lrintf(float);
extern long long int llrintf(float);
extern float roundf(float);
extern long int lroundf(float);
extern long long int llroundf(float);
extern float truncf(float);
extern float remquof(float, float, int *);
extern float fdimf(float, float);
extern float fmaxf(float, float);
extern float fminf(float, float);
extern float fmaf(float, float, float);

extern float nanf(const char *);
extern float copysignf(float, float);
extern float logbf(float);
extern int ilogbf(float);

extern float asinhf(float);
extern float cbrtf(float);
extern float nextafterf(float, float);
extern float rintf(float);
extern float scalbnf(float, int);
extern float log1pf(float);
extern float expm1f(float);

extern float acoshf(float);
extern float atanhf(float);
extern float remainderf(float, float);
extern float lgammaf(float);
extern float erff(float);
extern float erfcf(float);
extern float log2f(float);
extern float hypotf(float, float);

extern long double atanl(long double);
extern long double cosl(long double);
extern long double sinl(long double);
extern long double tanl(long double);
extern long double frexpl(long double, int *);
extern long double fabsl(long double);
extern long double log1pl(long double);
extern long double expm1l(long double);

extern long double atan2l(long double, long double);
extern long double coshl(long double);
extern long double sinhl(long double);
extern long double expl(long double);
extern long double ldexpl(long double, int);
extern long double logl(long double);
extern long double powl(long double, long double);
extern long double sqrtl(long double);
extern long double fmodl(long double, long double);
extern long double hypotl(long double, long double);

extern long double copysignl(long double, long double);
extern long double nanl(const char *);
extern int ilogbl(long double);
extern long double asinhl(long double);
extern long double cbrtl(long double);
extern long double nextafterl(long double, long double);
extern float nexttowardf(float, long double);
extern double nexttoward(double, long double);
extern long double nexttowardl(long double, long double);
extern long double logbl(long double);
extern long double log2l(long double);
extern long double rintl(long double);
extern long double scalbnl(long double, int);
extern long double exp2l(long double);
extern long double scalblnl(long double, long);
extern long double tgammal(long double);
extern long double nearbyintl(long double);
extern long int lrintl(long double);
extern long long int llrintl(long double);
extern long double roundl(long double);
extern long lroundl(long double);
extern long long int llroundl(long double);
extern long double truncl(long double);
extern long double remquol(long double, long double, int *);
extern long double fdiml(long double, long double);
extern long double fmaxl(long double, long double);
extern long double fminl(long double, long double);
extern long double fmal(long double, long double, long double);

extern long double acoshl(long double);
extern long double atanhl(long double);
extern long double remainderl(long double, long double);
extern long double lgammal(long double);
extern long double erfl(long double);
extern long double erfcl(long double);

#if 0
extern double infinity(void);
extern double gamma(double);
extern float frexpf(float, int *);
extern float infinityf(void);
extern float gammaf(float);
extern long double tanhl(long double);
extern long double modfl(long double, long double *);
extern long double ceill(long double);
extern long double floorl(long double);
extern long double acosl(long double);
extern long double asinl(long double);
extern long double log10l(long double);
#endif

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_MATH

#endif
