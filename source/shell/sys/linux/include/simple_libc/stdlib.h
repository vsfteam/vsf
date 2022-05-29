#ifndef __SIMPLE_LIBC_STDLIB_H__
#define __SIMPLE_LIBC_STDLIB_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif
#include <stdint.h>

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
#define malloc_usable_size  VSF_LINUX_LIBC_WRAPPER(malloc_usable_size)
#define exit                VSF_LINUX_LIBC_WRAPPER(exit)
#define atexit              VSF_LINUX_LIBC_WRAPPER(atexit)
#define system              VSF_LINUX_LIBC_WRAPPER(system)
#   if VSF_LINUX_LIBC_USE_ENVIRON
#define getenv              VSF_LINUX_LIBC_WRAPPER(getenv)
#define putenv              VSF_LINUX_LIBC_WRAPPER(putenv)
#define setenv              VSF_LINUX_LIBC_WRAPPER(setenv)
#define unsetenv            VSF_LINUX_LIBC_WRAPPER(unsetenv)
#   endif
#define mktemps             VSF_LINUX_LIBC_WRAPPER(mktemps)
#define mktemp              VSF_LINUX_LIBC_WRAPPER(mktemp)
#define mkstemp             VSF_LINUX_LIBC_WRAPPER(mkstemp)
#define mkostemp            VSF_LINUX_LIBC_WRAPPER(mkostemp)
#define mkstemps            VSF_LINUX_LIBC_WRAPPER(mkstemps)
#define mkostemps           VSF_LINUX_LIBC_WRAPPER(mkostemps)
#define mkdtemp             VSF_LINUX_LIBC_WRAPPER(mkdtemp)
#elif defined(__WIN__)
// avoid conflicts with APIs in ucrt
#define exit                VSF_LINUX_LIBC_WRAPPER(exit)
#define atexit              VSF_LINUX_LIBC_WRAPPER(atexit)
#define getenv              VSF_LINUX_LIBC_WRAPPER(getenv)
// system("chcp 65001"); will be called in debug_stream driver, wrapper here
#define system              VSF_LINUX_LIBC_WRAPPER(system)
#endif

void * malloc(size_t size);
void * realloc(void *p, size_t size);
void free(void *p);

void * aligned_alloc(size_t alignment, size_t size);
void * calloc(size_t n, size_t size);
void * memalign(size_t alignment, size_t size);

// malloc_usable_size should be in malloc.h
size_t malloc_usable_size(void *p);

#if VSF_LINUX_LIBC_USE_ENVIRON
int putenv(char *string);
char * getenv(const char *name);
int setenv(const char *name, const char *value, int replace);
int unsetenv(const char *name);
#endif
char * mktemps(char *template_str, int suffixlen);
char * mktemp(char *template_str);
int mkstemp(char *template_str);
int mkostemp(char *template_str, int flags);
int mkstemps(char *template_str, int suffixlen);
int mkostemps(char *template_str, int suffixlen, int flags);
char * mkdtemp(char *template_str);

int abs(int x);
typedef struct {
  int quot;
  int rem;
} div_t;
div_t div(int numer, int denom);
typedef struct {
  long quot;
  long rem;
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
void srandom(unsigned int seed);
long int random(void);

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));

void abort(void);
char * getenv(const char *name);
int system(const char *command);

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        -1
void exit(int status);
// _exit should be implemented in retarget
void _exit(int status);
int atexit(void (*func)(void));
void _Exit(int exit_code);
int at_quick_exit(void (*func)(void));
void quick_exit(int status);

int abs(int j);
long labs(long j);
long long llabs(long long j);
intmax_t imaxabs(intmax_t j);

#ifdef __cplusplus
}
#endif

#endif
