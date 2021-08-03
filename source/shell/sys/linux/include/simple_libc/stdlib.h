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

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#define malloc              VSF_LINUX_LIBC_WRAPPER(malloc)
#define aligned_alloc       VSF_LINUX_LIBC_WRAPPER(aligned_alloc)
#define realloc             VSF_LINUX_LIBC_WRAPPER(realloc)
#define free                VSF_LINUX_LIBC_WRAPPER(free)
#define calloc              VSF_LINUX_LIBC_WRAPPER(calloc)
#define memalign            VSF_LINUX_LIBC_WRAPPER(memalign)
#define exit                VSF_LINUX_LIBC_WRAPPER(exit)
#define atexit              VSF_LINUX_LIBC_WRAPPER(atexit)
#define system              VSF_LINUX_LIBC_WRAPPER(system)
#endif

void * malloc(size_t size);
void * aligned_alloc(size_t alignment, size_t size);
void * realloc(void *p, size_t size);
void free(void *p);
void * calloc(size_t n, size_t size);
void * memalign(size_t alignment, size_t size);

int abs(int x);
typedef struct {
  int quot;
  int rem;
} div_t;
div_t div(int numer, int denom);
typedef struct {
  long int quot;
  long int rem;
} ldiv_t;
ldiv_t ldiv(long int numer, long int denom);
typedef struct {
  long long quot;
  long long rem;
} lldiv_t;
lldiv_t lldiv(long long int numer, long long int denom);

char * itoa(int num, char *str, int radix);
int atoi(const char * str);
long int atol(const char *str);
long long int atoll(const char *str);
double atof(const char *str);
long strtol(const char *str, char **endptr, int base);
unsigned long strtoul(const char *str, char **endptr, int base);
long long strtoll(const char *str, char **endptr, int base);
unsigned long long strtoull(const char *str, char **endptr, int base);
float strtof(const char *str, char **endptr);
double strtod(const char *str, char **endptr);
long double strtold(const char *str, char **endptr);

void *bsearch(const void *key, const void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

int mblen(const char *str, size_t n);

// TODO: it seems that IAR does not support wchar_t even if it's defined in stddef.h
#if !__IS_COMPILER_IAR__
size_t mbstowcs(wchar_t *dst, const char *src, size_t len);
int mbtowc(wchar_t *pwc, const char *str, size_t n);
size_t wcstombs(char *str, const wchar_t *pwcs, size_t n);
int wctomb(char *str, wchar_t wchar);
#endif

#ifndef RAND_MAX
#   ifdef __WIN__
#       define RAND_MAX     0x7FFF
#   else
#       define RAND_MAX     0x7FFF
#   endif
#endif
int rand(void);
void srand(unsigned int seed);

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));

void abort(void);
char * getenv(const char *name);
int system(const char *command);

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        -1
void exit(int status);
int atexit(void (*func)(void));
void _Exit(int exit_code);
int at_quick_exit(void (*func)(void));
void quick_exit(int status);

#ifdef __cplusplus
}
#endif

#endif
