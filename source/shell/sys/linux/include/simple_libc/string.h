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

#ifdef __cplusplus
}
#endif

#if     __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
