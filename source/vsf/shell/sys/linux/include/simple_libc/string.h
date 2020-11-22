#ifndef __SIMPLE_LIBC_STRING_H__
#define __SIMPLE_LIBC_STRING_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
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

#define strdup              __vsf_linux_strdup

void * memset(void *s, int ch, size_t n);
size_t strlen(const char *str);
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
int stricmp(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, size_t n);
#   if defined(__CPU_X64__)
void * memcpy(void *dest, const void *src, unsigned long long n);
#   else
void * memcpy(void *dest, const void *src, unsigned int n);
#   endif
// strcasecmp and strncasecmp are not implement in __WIN__
static inline int strcasecmp(const char *s1, const char *s2)
{
    return stricmp(s1, s2);
}
static inline int strncasecmp(char *s1, char *s2, size_t n)
{
    return strnicmp(s1, s2, n);
}

const char * strstr(const char *str1, const char *str2);
const char * strchr(const char *str, int c);
const char * strrchr(const char *str, int c);
#else
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(char *s1, char *s2, size_t n);

void * memcpy(void *dest, const void *src, size_t n);

char * strstr(const char *str1, const char *str2);
char * strchr(const char *str, int c);
char * strrchr(const char *str, int c);
#endif
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);
const void *memchr(const void *buf, int ch, size_t count);

#ifdef __cplusplus
}
#endif

#if     __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

// TODO: add cpp related code outside extern "C"
#ifdef __cplusplus
#   ifdef __WIN__
//#       include <xstring>
#   endif
#endif

#endif
