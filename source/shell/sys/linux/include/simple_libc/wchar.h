#ifndef __SIMPLE_LIBC_WCHAR_H__
#define __SIMPLE_LIBC_WCHAR_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./stdio.h"
#   include "./time.h"
#else
#   include <stdio.h>
#   include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_APPLET_USE_LIBC_WCHAR == ENABLED
typedef struct vsf_linux_libc_wchar_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcslen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcscat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcsncat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcschr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcsrchr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcscmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcsncmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcscoll);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcscpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcsncpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcsstr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcstok);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wmemset);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wmemchr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wmemcmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wmemcpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wmemmove);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcstol);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcstof);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcstod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcstold);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcstoll);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcstoull);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcstoul);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcspbrk);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcsspn);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcscspn);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcsxfrm);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wcsftime);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(swprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vswprintf);

    // wctype.h
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswalnum);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswalpha);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswlower);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswupper);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswdigit);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswxdigit);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswcntrl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswgraph);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswspace);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswblank);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswprint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswpunct);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(iswctype);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wctype);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(towlower);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(towupper);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(towctrans);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(wctrans);
} vsf_linux_libc_wchar_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_wchar_vplt_t vsf_linux_libc_wchar_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_LIBC_WCHAR_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_LIBC_WCHAR == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_WCHAR_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_WCHAR_VPLT                                 \
            ((vsf_linux_libc_wchar_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_wchar_vplt))
#   else
#       define VSF_LINUX_APPLET_LIBC_WCHAR_VPLT                                 \
            ((vsf_linux_libc_wchar_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(__NAME)                               \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_WCHAR_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_WCHAR_IMP(...)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_WCHAR_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcslen, size_t, const wchar_t *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcslen)(str);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcscat, wchar_t *, wchar_t *dest, const wchar_t *src) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcscat)(dest, src);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcsncat, wchar_t *, wchar_t *dest, const wchar_t *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcsncat)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcschr, _CONST_RETURN wchar_t *, const wchar_t *str, wchar_t c) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcschr)(str, c);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcsrchr, _CONST_RETURN wchar_t *, const wchar_t *str, wchar_t c) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcsrchr)(str, c);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcscmp, int, const wchar_t *str1, const wchar_t *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcscmp)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcsncmp, int, const wchar_t *str1, const wchar_t *str2, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcsncmp)(str1, str2, n);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcscoll, int, const wchar_t *str1, const wchar_t *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcscoll)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcscpy, wchar_t *, wchar_t *dest, const wchar_t *src) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcscpy)(dest, src);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcsncpy, wchar_t *, wchar_t *dest, const wchar_t *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcsncpy)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcsstr, _CONST_RETURN wchar_t *, const wchar_t *str1, const wchar_t *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcsstr)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcstok, wchar_t *, wchar_t *str, const wchar_t *delim) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcstok)(str, delim);
}

VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wmemset, wchar_t *, wchar_t *dest, wchar_t c, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wmemset)(dest, c, n);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wmemchr, wchar_t *, const wchar_t *str, wchar_t c, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wmemchr)(str, c, n);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wmemcmp, int, const wchar_t *str1, const wchar_t *str2, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wmemcmp)(str1, str2, n);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wmemcpy, wchar_t *, wchar_t *dest, const wchar_t *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wmemcpy)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wmemmove, wchar_t *, wchar_t *dest, const wchar_t *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wmemmove)(dest, src, n);
}

VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcstol, long, const wchar_t *str, wchar_t **ptr, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcstol)(str, ptr, base);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcstof, float, const wchar_t *str, wchar_t **ptr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcstof)(str, ptr);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcstod, double, const wchar_t *str, wchar_t **ptr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcstod)(str, ptr);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcstold, long double, const wchar_t *str, wchar_t **ptr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcstold)(str, ptr);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcstoll, long long, const wchar_t *str, wchar_t **ptr, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcstoll)(str, ptr, base);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcstoull, unsigned long long, const wchar_t *str, wchar_t **ptr, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcstoull)(str, ptr, base);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcstoul, unsigned long, const wchar_t *str, wchar_t **ptr, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcstoul)(str, ptr, base);
}

VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcspbrk, wchar_t *, wchar_t *str, const wchar_t *charset) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcspbrk)(str, charset);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcsspn, size_t, const wchar_t *str, const wchar_t *charset) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcsspn)(str, charset);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcscspn, size_t, const wchar_t *str, const wchar_t *charset) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcscspn)(str, charset);
}
VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcsxfrm, size_t, wchar_t *dest, const wchar_t *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcsxfrm)(dest, src, n);
}

VSF_LINUX_APPLET_LIBC_WCHAR_IMP(wcsftime, size_t, wchar_t *dest, size_t n, const wchar_t *format, const struct tm *timeptr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(wcsftime)(dest, n, format, timeptr);
}

VSF_LINUX_APPLET_LIBC_WCHAR_IMP(vswprintf, int, wchar_t *str, size_t n, const wchar_t *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_WCHAR_ENTRY(vswprintf)(str, n, format, ap);
}
VSF_APPLET_VPLT_FUNC_DECORATOR(swprintf) int swprintf(wchar_t *str, size_t n, const wchar_t *format, ...) {
    int real_size;
    va_list ap;
    va_start(ap, format);
        real_size = vswprintf(str, len, format, ap);
    va_end(ap);
    return real_size;
}
#endif

#ifndef WEOF
#   define WEOF                ((wint_t)-1)
#endif

#ifdef __WIN__
#   ifdef __cplusplus
#       define _CONST_RETURN    const
#   else
#       define _CONST_RETURN
#   endif
#else
#   define _CONST_RETURN

#   if !__IS_COMPILER_IAR__
// mbstate_t is defined in uchar.h in IAR
typedef struct {
    unsigned long wc;
    unsigned short byte, state;
} mbstate_t;
#   endif
#endif

#define getwc_unlocked          getwc
#define getwchar_unlocked       getwchar
#define fgetwc_unlocked         fgetwc
#define fputwc_unlocked         fputwc
#define putwc_unlocked          putwc
#define putwchar_unlocked       putwchar
#define fgetws_unlocked         fgetws
#define fputws_unlocked         fputws

#ifdef __WIN__
typedef struct _Mbstatet _Mbstatet;
typedef _Mbstatet mbstate_t;
#endif

int mbsinit(const mbstate_t *ps);
size_t mbrlen(const char *str, size_t n, mbstate_t *ps);
size_t mbrtowc(wchar_t *pwc, const char *str, size_t n, mbstate_t *ps);
size_t mbsrtowcs(wchar_t *dst, const char **src, size_t n, mbstate_t *ps);
size_t wcrtomb(char *str, wchar_t wc, mbstate_t *ps);
size_t wcsrtombs(char *mbstr, const wchar_t **wcstr, size_t n, mbstate_t *ps);
int wctob(wint_t wchar);

wint_t btowc(int c);

wint_t ungetwc(wint_t c, FILE *f);
wint_t putwchar(wchar_t c);
wint_t putwc(wchar_t c, FILE *f);
wint_t fputwc(wchar_t c, FILE *f);
wint_t getwchar();
wint_t getwc(FILE *f);
wint_t fgetwc(FILE *f);
wchar_t * fgetws(wchar_t *str, int n, FILE *f);
int fputws(const wchar_t *str, FILE *f);
int fwide(FILE *f, int mode);

int wprintf(const wchar_t *format, ...);
int wscanf(const wchar_t *format, ...);
int vwprintf(const wchar_t *format, va_list ap);
int vwscanf(const wchar_t *format, va_list ap);
int fwscanf(FILE *f, const wchar_t *format, ...);
int fwprintf(FILE *f, const wchar_t *format, ...);
int vfwprintf(FILE *f, const wchar_t *format, va_list ap);
int vfwscanf(FILE *f, const wchar_t *format, va_list ap);

int swscanf(const wchar_t *str, const wchar_t *format, ...);
int vswscanf(const wchar_t *str, const wchar_t *format, va_list ap);
int swprintf(wchar_t *str, size_t n, const wchar_t *format, ...);
int vswprintf(wchar_t *str, size_t n, const wchar_t *format, va_list ap);

size_t wcslen(const wchar_t *str);
wchar_t * wcscat(wchar_t *dest, const wchar_t *src);
wchar_t * wcsncat(wchar_t *dest, const wchar_t *src, size_t n);
_CONST_RETURN wchar_t * wcschr(const wchar_t *str, wchar_t c);
_CONST_RETURN wchar_t * wcsrchr(const wchar_t *str, wchar_t c);
int wcscmp(const wchar_t *str1, const wchar_t *str2);
int wcsncmp(const wchar_t *str1, const wchar_t *str2, size_t n);
int wcscoll(const wchar_t *str1, const wchar_t *str2);
wchar_t * wcscpy(wchar_t *dest, const wchar_t *src);
wchar_t * wcsncpy(wchar_t *dest, const wchar_t *src, size_t n);
_CONST_RETURN wchar_t * wcsstr(const wchar_t *str1, const wchar_t *str2);
wchar_t * wcstok(wchar_t *str, const wchar_t *delim);

wchar_t * wmemset(wchar_t *dest, wchar_t c, size_t n);
wchar_t * wmemchr(const wchar_t *str, wchar_t c, size_t n);
int wmemcmp(const wchar_t *str1, const wchar_t *str2, size_t n);
wchar_t * wmemcpy(wchar_t *dest, const wchar_t *src, size_t n);
wchar_t * wmemmove(wchar_t *dest, const wchar_t *src, size_t n);

long wcstol(const wchar_t *str, wchar_t **ptr, int base);
float wcstof(const wchar_t *str, wchar_t **ptr);
double wcstod(const wchar_t *str, wchar_t **ptr);
long double wcstold(const wchar_t *str, wchar_t **ptr);
long long wcstoll(const wchar_t *str, wchar_t **ptr, int base);
unsigned long long wcstoull(const wchar_t *str, wchar_t **ptr, int base);
unsigned long wcstoul(const wchar_t *str, wchar_t **ptr, int base);

wchar_t * wcspbrk(wchar_t *str, const wchar_t *charset);
size_t wcsspn(const wchar_t *str, const wchar_t *charset);
size_t wcscspn(const wchar_t *str, const wchar_t *charset);
size_t wcsxfrm(wchar_t *dest, const wchar_t *src, size_t n);

size_t wcsftime(wchar_t *dest, size_t n, const wchar_t *format, const struct tm *timeptr);

#ifdef __cplusplus
}
#endif

#endif
