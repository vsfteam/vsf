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

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(frexpf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tanhl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(modfl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ceill);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(floorl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(acosl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asinl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(log10l);
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
VSF_LINUX_APPLET_LIBC_MATH_IMP(sin, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sin)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tan, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tan)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tanh, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tanh)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(frexp, double, double a0, int *a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(frexp)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(modf, double, double a0, double *a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(modf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ceil, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ceil)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fabs, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fabs)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(floor, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(floor)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(acos, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(acos)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(asin, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(asin)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(atan2, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atan2)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(cosh, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cosh)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(sinh, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sinh)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(exp, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(exp)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ldexp, double, double a0, int a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ldexp)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log10, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log10)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(pow, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(pow)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(sqrt, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sqrt)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmod, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmod)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nan, double, const char *a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nan)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(copysign, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(copysign)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(logb, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(logb)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ilogb, int, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ilogb)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(asinh, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(asinh)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(cbrt, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cbrt)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nextafter, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nextafter)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(rint, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(rint)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(scalbn, double, double a0, int a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(scalbn)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(exp2, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(exp2)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(scalbln, double, double a0, long int a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(scalbln)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tgamma, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tgamma)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nearbyint, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nearbyint)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lrint, long int, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lrint)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(llrint, long long int, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(llrint)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(round, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(round)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lround, long int, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lround)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(llround, long long int, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(llround)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(trunc, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(trunc)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(remquo, double, double a0, double a1, int *a2) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(remquo)(a0, a1, a2);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fdim, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fdim)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmax, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmax)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmin, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmin)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fma, double, double a0, double a1, double a2) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fma)(a0, a1, a2);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log1p, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log1p)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(expm1, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(expm1)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(acosh, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(acosh)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(atanh, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atanh)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(remainder, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(remainder)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lgamma, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lgamma)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(erf, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(erf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(erfc, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(erfc)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log2, double, double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log2)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(hypot, double, double a0, double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(hypot)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(atanf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atanf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(cosf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cosf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(sinf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sinf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tanf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tanf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tanhf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tanhf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(modff, float, float a0, float *a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(modff)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ceilf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ceilf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fabsf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fabsf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(floorf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(floorf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(acosf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(acosf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(asinf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(asinf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(atan2f, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atan2f)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(coshf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(coshf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(sinhf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sinhf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(expf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(expf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ldexpf, float, float a0, int a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ldexpf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(logf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(logf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log10f, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log10f)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(powf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(powf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(sqrtf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sqrtf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmodf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmodf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(exp2f, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(exp2f)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(scalblnf, float, float a0, long int a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(scalblnf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tgammaf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tgammaf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nearbyintf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nearbyintf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lrintf, long int, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lrintf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(llrintf, long long int, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(llrintf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(roundf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(roundf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lroundf, long int, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lroundf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(llroundf, long long int, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(llroundf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(truncf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(truncf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(remquof, float, float a0, float a1, int *a2) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(remquof)(a0, a1, a2);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fdimf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fdimf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmaxf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmaxf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fminf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fminf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmaf, float, float a0, float a1, float a2) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmaf)(a0, a1, a2);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nanf, float, const char *a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nanf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(copysignf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(copysignf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(logbf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(logbf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ilogbf, int, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ilogbf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(asinhf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(asinhf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(cbrtf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cbrtf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nextafterf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nextafterf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(rintf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(rintf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(scalbnf, float, float a0, int a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(scalbnf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log1pf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log1pf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(expm1f, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(expm1f)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(acoshf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(acoshf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(atanhf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atanhf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(remainderf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(remainderf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lgammaf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lgammaf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(erff, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(erff)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(erfcf, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(erfcf)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log2f, float, float a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log2f)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(hypotf, float, float a0, float a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(hypotf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(atanl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atanl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(cosl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cosl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(sinl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sinl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tanl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tanl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(frexpl, long double, long double a0, int *a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(frexpl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fabsl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fabsl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log1pl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log1pl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(expm1l, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(expm1l)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(atan2l, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atan2l)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(coshl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(coshl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(sinhl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sinhl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(expl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(expl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ldexpl, long double, long double a0, int a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ldexpl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(logl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(logl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(powl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(powl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(sqrtl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(sqrtl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmodl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmodl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(hypotl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(hypotl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(copysignl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(copysignl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nanl, long double, const char *a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nanl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ilogbl, int, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ilogbl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(asinhl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(asinhl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(cbrtl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(cbrtl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nextafterl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nextafterl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nexttowardf, float, float a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nexttowardf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nexttoward, double, double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nexttoward)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nexttowardl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nexttowardl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(logbl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(logbl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log2l, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log2l)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(rintl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(rintl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(scalbnl, long double, long double a0, int a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(scalbnl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(exp2l, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(exp2l)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(scalblnl, long double, long double a0, long a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(scalblnl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tgammal, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tgammal)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(nearbyintl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(nearbyintl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lrintl, long int, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lrintl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(llrintl, long long int, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(llrintl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(roundl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(roundl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lroundl, long, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lroundl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(llroundl, long long int, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(llroundl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(truncl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(truncl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(remquol, long double, long double a0, long double a1, int *a2) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(remquol)(a0, a1, a2);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fdiml, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fdiml)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmaxl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmaxl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fminl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fminl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(fmal, long double, long double a0, long double a1, long double a2) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(fmal)(a0, a1, a2);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(acoshl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(acoshl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(atanhl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(atanhl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(remainderl, long double, long double a0, long double a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(remainderl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(lgammal, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(lgammal)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(erfl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(erfl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(erfcl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(erfcl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(frexpf, float, float a0, int *a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(frexpf)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(tanhl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(tanhl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(modfl, long double, long double a0, long double *a1) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(modfl)(a0, a1);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(ceill, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(ceill)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(floorl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(floorl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(acosl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(acosl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(asinl, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(asinl)(a0);
}
VSF_LINUX_APPLET_LIBC_MATH_IMP(log10l, long double, long double a0) {
    return VSF_LINUX_APPLET_LIBC_MATH_ENTRY(log10l)(a0);
}

#elif   !defined(__SIMPLE_LIBC_MATH_VPLT_ONLY___)

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

extern float frexpf(float, int *);
extern long double tanhl(long double);
extern long double modfl(long double, long double *);
extern long double ceill(long double);
extern long double floorl(long double);
extern long double acosl(long double);
extern long double asinl(long double);
extern long double log10l(long double);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_MATH

#endif
