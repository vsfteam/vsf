#ifndef __SIMPLE_LIBC_STRING_H__
#define __SIMPLE_LIBC_STRING_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif

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
#define strdup              VSF_LINUX_LIBC_WRAPPER(strdup)
#endif

#define ffs                 vsf_ffs32

#if VSF_LINUX_APPLET_USE_STDSTRING == ENABLED
typedef struct vsf_linux_stdstring_vplt_t {
    int_fast8_t (*ffs)(uint_fast32_t);
    void * (*memset)(void *s, int ch, size_t n);
    size_t (*strlen)(const char *str);
    size_t (*strnlen)(const char *str, size_t maxlen);
    int (*strcmp)(const char *str1, const char *str2);
    int (*strncmp)(const char *str1, const char *str2, size_t n);
    char * (*strdup)(const char *str);
    char * (*strcpy)(char *dest, const char *src);
    char * (*strncpy)(char *dest, const char *src, size_t n);
    char * (*strcat)(char *dest, const char *src);
    char * (*strncat)(char *dest, const char *str, size_t n);
    int (*strcoll)(const char *str1, const char *str2);
    char * (*strtok)(char *str, const char *delim);
    size_t (*strxfrm)(char *dest, const char *src, size_t n);
    size_t (*strspn)(const char *str1, const char *str2);
    size_t (*strcspn)(const char *str1, const char *str2);
    char * (*strpbrk)(const char *str1, const char *str2);
    char * (*strerror)(int errnum);
    int (*strcasecmp)(const char *s1, const char *s2);
    int (*strncasecmp)(const char *s1, const char *s2, size_t n);
    void * (*memcpy)(void *dest, const void *src, size_t n);
    size_t (*strlcpy)(char *dest, const char *src, size_t n);
    char * (*strstr)(const char *str1, const char *str2);
    char * (*strchr)(const char *str, int c);
    char * (*strrchr)(const char *str, int c);
    void * (*memmove)(void *dest, const void *src, size_t n);
    int (*memcmp)(const void *str1, const void *str2, size_t n);
    void * (*memchr)(const void *buf, int ch, size_t count);
} vsf_linux_stdstring_vplt_t;
#   ifndef __VSF_APPLET__
extern const vsf_linux_stdstring_vplt_t vsf_linux_stdstring_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_STDSTRING == ENABLED

#ifndef VSF_LINUX_APPLET_STDSTRING_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_STDSTRING_VPLT                                  \
            ((vsf_linux_stdstring_vplt_t *)(VSF_LINUX_APPLET_VPLT->stdstring))
#   else
#       define VSF_LINUX_APPLET_STDSTRING_VPLT                                  \
            ((vsf_linux_stdstring_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int ffs(int i) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->ffs(i);
}
static inline void * memset(void *s, int ch, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->memset(s, ch, n);
}
static inline size_t strlen(const char *str) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strlen(str);
}
static inline size_t strnlen(const char *str, size_t maxlen) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strnlen(str, maxlen);
}
static inline int strcmp(const char *str1, const char *str2) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strcmp(str1, str2);
}
static inline int strncmp(const char *str1, const char *str2, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strncmp(str1, str2, n);
}
static inline char * strdup(const char *str) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strdup(str);
}
static inline char * strcpy(char *dest, const char *src) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strcpy(dest, src);
}
static inline char * strncpy(char *dest, const char *src, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strncpy(dest, src, n);
}
static inline char * strcat(char *dest, const char *src) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strcat(dest, src);
}
static inline char * strncat(char *dest, const char *str, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strncat(dest, str, n);
}
static inline int strcoll(const char *str1, const char *str2) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strcoll(str1, str2);
}
static inline char * strtok(char *str, const char *delim) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strtok(str, delim);
}
static inline size_t strxfrm(char *dest, const char *src, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strxfrm(dest, src, n);
}
static inline size_t strspn(const char *str1, const char *str2) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strspn(str1, str2);
}
static inline size_t strcspn(const char *str1, const char *str2) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strcspn(str1, str2);
}
static inline char * strpbrk(const char *str1, const char *str2) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strpbrk(str1, str2);
}
static inline char * strerror(int errnum) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strerror(errnum);
}
static inline int strcasecmp(const char *s1, const char *s2) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strcasecmp(s1, s2);
}
static inline int strncasecmp(const char *s1, const char *s2, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strncasecmp(s1, s2, n);
}
static inline void * memcpy(void *dest, const void *src, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->memcpy(dest, src, n);
}
static inline size_t strlcpy(char *dest, const char *src, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strlcpy(dest, src, n);
}
static inline char * strstr(const char *str1, const char *str2) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strstr(str1, str2);
}
static inline char * strchr(const char *str, int c) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strchr(str, c);
}
static inline char * strrchr(const char *str, int c) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->strrchr(str, c);
}
static inline void * memmove(void *dest, const void *src, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->memmove(dest, src, n);
}
static inline int memcmp(const void *str1, const void *str2, size_t n) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->memcmp(str1, str2, n);
}
static inline void * memchr(const void *buf, int ch, size_t count) {
    return VSF_LINUX_APPLET_STDSTRING_VPLT->memchr(buf, ch, count);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_STDSTRING

void * memset(void *s, int ch, size_t n);
size_t strlen(const char *str);
size_t strnlen(const char *str, size_t maxlen);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char * strdup(const char *str);
char * strcpy(char *dest, const char *src);
char * strncpy(char *dest, const char *src, size_t n);
char * strcat(char *dest, const char *src);
char * strncat(char *dest, const char *str, size_t n);
int strcoll(const char *str1, const char *str2);
char * strtok(char *str, const char *delim);
size_t strxfrm(char *dest, const char *src, size_t n);
size_t strspn(const char *str1, const char *str2);
size_t strcspn(const char *str1, const char *str2);
char * strpbrk(const char *str1, const char *str2);
char * strerror(int errnum);

#if defined(__WIN__)
#   ifdef __cplusplus
#       define _CONST_RETURN    const
#   else
#       define _CONST_RETURN
#   endif

int stricmp(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, size_t n);
#   ifndef strcasecmp
#       define strcasecmp   stricmp
#   endif
#   ifndef strncasecmp
#       define strncasecmp  strnicmp
#   endif
#   if defined(__CPU_X64__)
void * memcpy(void *dest, const void *src, unsigned long long n);
#   else
void * memcpy(void *dest, const void *src, unsigned int n);
#   endif
#else
#   define _CONST_RETURN
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
void * memcpy(void *dest, const void *src, size_t n);
size_t strlcpy(char *dest, const char *src, size_t n);
#endif

_CONST_RETURN char * strstr(const char *str1, const char *str2);
_CONST_RETURN char * strchr(const char *str, int c);
_CONST_RETURN char * strrchr(const char *str, int c);

void * memmove(void *dest, const void *src, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);
_CONST_RETURN void * memchr(const void *buf, int ch, size_t count);
#endif      // // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_STDSTRING

#ifdef __cplusplus
}
#endif

#if     __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
