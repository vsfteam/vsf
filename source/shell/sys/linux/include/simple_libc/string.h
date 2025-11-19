#ifndef __SIMPLE_LIBC_STRING_H__
#define __SIMPLE_LIBC_STRING_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif
#include "./strings.h"

//! \note libc belongs to compiler layer in utilities, so only include compiler.h
#include "utilities/compiler/compiler.h"

#if     __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#   pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#   pragma clang diagnostic ignored "-Winconsistent-dllimport"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#if     !(VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED)                  \
    ||  !(VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED)
// strdup will be macro in this condition, so no need to wrapper
#define strdup              VSF_LINUX_LIBC_WRAPPER(strdup)
#   endif
#define strndup             VSF_LINUX_LIBC_WRAPPER(strndup)
#endif

#if VSF_LINUX_APPLET_USE_LIBC_STRING == ENABLED
typedef struct vsf_linux_libc_string_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(____strdup_ex);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ffs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(memset);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strlen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strnlen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strcmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strncmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strdup);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strcpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strncpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(stpcpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(stpncpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strcat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strncat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strcoll);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtok);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtok_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strxfrm);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strspn);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strsep);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strcspn);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strpbrk);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strerror);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strcasecmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strncasecmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(memcpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mempcpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strlcpy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strstr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strcasestr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strchr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strrchr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strchrnul);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(memmove);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(memcmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(memchr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(memrchr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strverscmp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strndup);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strsignal);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sigdescr_np);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sigabbrev_np);
} vsf_linux_libc_string_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_string_vplt_t vsf_linux_libc_string_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_LIBC_STRING_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_LIBC_STRING == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_STRING_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_STRING_VPLT                                \
            ((vsf_linux_libc_string_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_string_vplt))
#   else
#       define VSF_LINUX_APPLET_LIBC_STRING_VPLT                                \
            ((vsf_linux_libc_string_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_STRING_ENTRY(__NAME)                              \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_STRING_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_STRING_IMP(...)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_STRING_VPLT, __VA_ARGS__)

#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
typedef struct vsf_linux_process_t vsf_linux_process_t;
VSF_LINUX_APPLET_LIBC_STRING_IMP(____strdup_ex, char *, vsf_linux_process_t *process, const char *str, const char *file, const char *func, int line) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(____strdup_ex)(process, str, file, func, line);
}
#endif
VSF_LINUX_APPLET_LIBC_STRING_IMP(ffs, int, int i) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(ffs)(i);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(memset, void *, void *s, int ch, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(memset)(s, ch, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strlen, size_t, const char *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strlen)(str);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strnlen, size_t, const char *str, size_t maxlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strnlen)(str, maxlen);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strcmp, int, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strcmp)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strncmp, int, const char *str1, const char *str2, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strncmp)(str1, str2, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strdup, char *, const char *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strdup)(str);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strcpy, char *, char *dest, const char *src) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strcpy)(dest, src);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strncpy, char *, char *dest, const char *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strncpy)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(stpcpy, char *, char *dest, const char *src) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(stpcpy)(dest, src);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(stpncpy, char *, char *dest, const char *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(stpncpy)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strcat, char *, char *dest, const char *src) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strcat)(dest, src);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strncat, char *, char *dest, const char *str, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strncat)(dest, str, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strcoll, int, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strcoll)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strtok, char *, char *str, const char *delim) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strtok)(str, delim);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strtok_r, char *, char *str, const char *delim, char **saveptr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strtok_r)(str, delim, saveptr);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strxfrm, size_t, char *dest, const char *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strxfrm)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strspn, size_t, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strspn)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strsep, char *, char **strp, const char *delim) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strsep)(strp, delim);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strcspn, size_t, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strcspn)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strpbrk, char *, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strpbrk)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strerror, char *, int errnum) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strerror)(errnum);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strcasecmp, int, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strcasecmp)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strncasecmp, int, const char *str1, const char *str2, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strncasecmp)(str1, str2, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(memcpy, void *, void *dest, const void *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(memcpy)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(mempcpy, void *, void *dest, const void *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(mempcpy)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strlcpy, size_t, char *dest, const char *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strlcpy)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strstr, char *, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strstr)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strcasestr, char *, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strcasestr)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strchr, char *, const char *str, int c) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strchr)(str, c);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strrchr, char *, const char *str, int c) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strrchr)(str, c);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strchrnul, char *, const char *str, int c) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strchrnul)(str, c);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(memmove, void *, void *dest, const void *src, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(memmove)(dest, src, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(memcmp, int, const void *str1, const void *str2, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(memcmp)(str1, str2, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(memchr, void *, const void *buf, int ch, size_t count) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(memchr)(buf, ch, count);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(memrchr, void *, const void *buf, int ch, size_t count) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(memrchr)(buf, ch, count);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strverscmp, int, const char *str1, const char *str2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strverscmp)(str1, str2);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strndup, char *, const char *str, size_t n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strndup)(str, n);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(strsignal, char *, int sig) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(strsignal)(sig);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(sigdescr_np, const char *, int sig) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(sigdescr_np)(sig);
}
VSF_LINUX_APPLET_LIBC_STRING_IMP(sigabbrev_np, const char *, int sig) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STRING_ENTRY(sigabbrev_np)(sig);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STRING

int ffs(int i);
void * memset(void *s, int ch, size_t n);
size_t strlen(const char *str);
size_t strnlen(const char *str, size_t maxlen);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
int strverscmp(const char *str1, const char *str2);

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
typedef struct vsf_linux_process_t vsf_linux_process_t;
char * ____strdup_ex(vsf_linux_process_t *process, const char *str, const char *file, const char *func, int line);
#endif

#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED                     \
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED
#   define strdup(__str)            ____strdup_ex(NULL, (char *)(__str), __FILE__, __FUNCTION__, __LINE__)
#else
char * strdup(const char *str);
#endif
char * strndup(const char *str, size_t n);
char * strcpy(char *dest, const char *src);
char * strncpy(char *dest, const char *src, size_t n);
ssize_t strscpy(char *dest, const char *src, size_t n);
char * stpcpy(char *dest, const char *src);
char * stpncpy(char *dest, const char *src, size_t n);
char * strcat(char *dest, const char *src);
char * strncat(char *dest, const char *str, size_t n);
int strcoll(const char *str1, const char *str2);
char * strtok(char *str, const char *delim);
char * strtok_r(char *str, const char *delim, char **saveptr);
size_t strxfrm(char *dest, const char *src, size_t n);
size_t strspn(const char *str1, const char *str2);
char * strsep(char **strp, const char *delim);
size_t strcspn(const char *str1, const char *str2);
char * strpbrk(const char *str1, const char *str2);
char * strerror(int errnum);
// XSI-compliant
int strerror_r(int errnum, char *buf, size_t buflen);

#if defined(__WIN__)
#   ifndef _CONST_RETURN
#       ifdef __cplusplus
#           define _CONST_RETURN    const
#       else
#           define _CONST_RETURN
#       endif
#   endif

#   ifndef __VSF_APPLET__
int stricmp(const char *str1, const char *str2);
int strnicmp(const char *str1, const char *str2, size_t n);
#       ifndef strcasecmp
#           define strcasecmp   stricmp
#       endif
#       ifndef strncasecmp
#           define strncasecmp  strnicmp
#       endif
#   endif

#   if defined(__CPU_X64__)
void * memcpy(void *dest, const void *src, unsigned long long n);
#   else
void * memcpy(void *dest, const void *src, unsigned int n);
#   endif
#else
#   define _CONST_RETURN
int strcasecmp(const char *str1, const char *str2);
int strncasecmp(const char *str1, const char *str2, size_t n);
void * memcpy(void *dest, const void *src, size_t n);
size_t strlcpy(char *dest, const char *src, size_t n);
#endif
void * mempcpy(void *dest, const void *src, size_t n);

_CONST_RETURN char * strstr(const char *str1, const char *str2);
_CONST_RETURN char * strchr(const char *str, int c);
_CONST_RETURN char * strrchr(const char *str, int c);
_CONST_RETURN char * strchrnul(const char *str, int c);
_CONST_RETURN void * memchr(const void *buf, int ch, size_t count);
_CONST_RETURN void * memrchr(const void *buf, int ch, size_t count);
char * strcasestr(const char *haystack, const char *needle);

void * memmove(void *dest, const void *src, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);

char * strsignal(int sig);
const char * sigdescr_np(int sig);
const char * sigabbrev_np(int sig);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STRING

#ifdef __cplusplus
}
#endif

#if     __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
