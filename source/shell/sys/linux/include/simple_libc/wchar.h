#ifndef __SIMPLE_LIBC_WCHAR_H__
#define __SIMPLE_LIBC_WCHAR_H__

#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WEOF
#   define WEOF                ((wint_t)-1)
#endif

#ifndef __WIN__
typedef struct {
    unsigned long wc;
    unsigned short byte, state;
} mbstate_t;
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
wchar_t *fgetws(wchar_t *str, int n, FILE *f);
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
int swprintf(wchar_t *str, size_t n, const wchar_t *format, ...);
int vswprintf(wchar_t *str, size_t n, const wchar_t *format, va_list ap);
int vswscanf(const wchar_t *str, const wchar_t *format, va_list ap);

size_t wcslen(const wchar_t *str);
wchar_t * wcscat(wchar_t *dest, const wchar_t *src);
wchar_t *wcsncat(wchar_t *dest, const wchar_t *src, size_t n);
wchar_t * wcschr(const wchar_t *str, wchar_t c);
wchar_t * wcsrchr(const wchar_t *str, wchar_t c);
int wcscmp(const wchar_t *str1, const wchar_t *str2);
int wcsncmp(const wchar_t *str1, const wchar_t *str2, size_t n);
int wcscoll(const wchar_t *str1, const wchar_t *str2);
wchar_t * wcscpy(wchar_t *dest, const wchar_t *src);
wchar_t * wcsncpy(wchar_t *dest, const wchar_t *src, size_t n);
wchar_t *wcsstr(const wchar_t *str1, const wchar_t *str2);
wchar_t *wcstok(wchar_t *str, const wchar_t *delim);

wchar_t *wmemset(wchar_t *dest, wchar_t c, size_t n);
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

wchar_t *wcspbrk(wchar_t *str, const wchar_t *charset);
size_t wcsspn(const wchar_t *str, const wchar_t *charset);
size_t wcscspn(const wchar_t *str, const wchar_t *charset);
size_t wcsxfrm(wchar_t *dest, const wchar_t *src, size_t n);

size_t wcsftime(wchar_t *dest, size_t n, const wchar_t *format, const struct tm *timeptr);

#ifdef __cplusplus
}
#endif

#endif
