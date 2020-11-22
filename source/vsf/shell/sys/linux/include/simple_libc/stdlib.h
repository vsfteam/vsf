#ifndef __SIMPLE_LIBC_STDLIB_H__
#define __SIMPLE_LIBC_STDLIB_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WIN__
#define malloc              __vsf_linux_malloc
#define realloc             __vsf_linux_realloc
#define free                __vsf_linux_free
#define calloc              __vsf_linux_calloc
#endif
#define memalign            __vsf_linux_memalign

#ifndef __WIN__
void * malloc(size_t size);
void * realloc(void *p, size_t size);
void free(void *p);
void * calloc(size_t n, size_t size);
#endif
void * memalign(size_t alignment, size_t size);

char * itoa(int num, char *str, int radix);
int atoi(const char * str);
unsigned long int strtoul(const char *str, char **endptr, int base);
unsigned long long int strtoull(const char *str, char **endptr, int base);
double strtod(const char *str, char **endptr);

int rand(void);
void srand(unsigned int seed);

#ifdef __cplusplus
}
#endif

#endif
