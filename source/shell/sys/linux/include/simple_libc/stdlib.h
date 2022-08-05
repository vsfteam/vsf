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

#ifndef RAND_MAX
#   ifdef __WIN__
#       define RAND_MAX     0x7FFF
#   else
#       define RAND_MAX     0x7FFF
#   endif
#endif

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        -1

typedef struct {
  int quot;
  int rem;
} div_t;
typedef struct {
  long quot;
  long rem;
} ldiv_t;
typedef struct {
  long long quot;
  long long rem;
} lldiv_t;

#if VSF_LINUX_APPLET_USE_LIBC_STDLIB == ENABLED
typedef struct vsf_linux_libc_stdlib_vplt_t {
    vsf_vplt_info_t info;

    void * (*malloc)(size_t size);
    void * (*realloc)(void *p, size_t size);
    void (*free)(void *p);
    void * (*aligned_alloc)(size_t alignment, size_t size);
    void * (*calloc)(size_t n, size_t size);
    void * (*memalign)(size_t alignment, size_t size);
    // malloc_usable_size should be in malloc.h
    size_t (*malloc_usable_size)(void *p);
    int (*putenv)(char *string);
    char * (*getenv)(const char *name);
    int (*setenv)(const char *name, const char *value, int replace);
    int (*unsetenv)(const char *name);
    char * (*mktemps)(char *template_str, int suffixlen);
    char * (*mktemp)(char *template_str);
    int (*mkstemp)(char *template_str);
    int (*mkostemp)(char *template_str, int flags);
    int (*mkstemps)(char *template_str, int suffixlen);
    int (*mkostemps)(char *template_str, int suffixlen, int flags);
    char * (*mkdtemp)(char *template_str);

    div_t (*div)(int numer, int denom);
    ldiv_t (*ldiv)(long int numer, long int denom);
    lldiv_t (*lldiv)(long long int numer, long long int denom);

    char * (*itoa)(int num, char *str, int radix);
    int (*atoi)(const char * str);
    long int (*atol)(const char *str);
    long long int (*atoll)(const char *str);
    double (*atof)(const char *str);
    long (*strtol)(const char *str, char **endptr, int base);
    unsigned long (*strtoul)(const char *str, char **endptr, int base);
    long long (*strtoll)(const char *str, char **endptr, int base);
    unsigned long long (*strtoull)(const char *str, char **endptr, int base);
    float (*strtof)(const char *str, char **endptr);
    double (*strtod)(const char *str, char **endptr);
    long double (*strtold)(const char *str, char **endptr);

    void * (*bsearch)(const void *key, const void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));
    void (*qsort)(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));

    int (*rand)(void);
    void (*srand)(unsigned int seed);

    void (*abort)(void);
    int (*system)(const char *command);

    void (*exit)(int status);
    int (*atexit)(void (*func)(void));

    int (*abs)(int j);
    long (*labs)(long j);
    long long (*llabs)(long long j);
    intmax_t (*imaxabs)(intmax_t j);
} vsf_linux_libc_stdlib_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_stdlib_vplt_t vsf_linux_libc_stdlib_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_LIBC_STDLIB == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_STDLIB_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_STDLIB_VPLT                                \
            ((vsf_linux_libc_stdlib_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_stdlib))
#   else
#       define VSF_LINUX_APPLET_LIBC_STDLIB_VPLT                                \
            ((vsf_linux_libc_stdlib_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline void * malloc(size_t size) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->malloc(size);
}
static inline void * realloc(void *p, size_t size) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->realloc(p, size);
}
static inline void free(void *p) {
    VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->free(p);
}

static inline void * aligned_alloc(size_t alignment, size_t size) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->aligned_alloc(alignment, size);
}
static inline void * calloc(size_t n, size_t size) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->calloc(n, size);
}
static inline void * memalign(size_t alignment, size_t size) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->memalign(alignment, size);
}

// malloc_usable_size should be in malloc.h
static inline size_t malloc_usable_size(void *p) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->malloc_usable_size(p);
}

#if VSF_LINUX_LIBC_USE_ENVIRON
static inline int putenv(char *string) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->putenv(string);
}
static inline char * getenv(const char *name) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->getenv(name);
}
static inline int setenv(const char *name, const char *value, int replace) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->setenv(name, value, replace);
}
static inline int unsetenv(const char *name) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->unsetenv(name);
}
#endif
static inline char * mktemps(char *template_str, int suffixlen) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->mktemps(template_str, suffixlen);
}
static inline char * mktemp(char *template_str) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->mktemp(template_str);
}
static inline int mkstemp(char *template_str) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->mkstemp(template_str);
}
static inline int mkostemp(char *template_str, int flags) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->mkostemp(template_str, flags);
}
static inline int mkstemps(char *template_str, int suffixlen) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->mkstemps(template_str, suffixlen);
}
static inline int mkostemps(char *template_str, int suffixlen, int flags) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->mkostemps(template_str, suffixlen, flags);
}
static inline char * mkdtemp(char *template_str) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->mkdtemp(template_str);
}

static inline div_t div(int numer, int denom) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->div(numer, denom);
}
static inline ldiv_t ldiv(long int numer, long int denom) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->ldiv(numer, denom);
}
static inline lldiv_t lldiv(long long int numer, long long int denom) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->lldiv(numer, denom);
}

static inline char * itoa(int num, char *str, int radix) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->itoa(num, str, radix);
}
static inline int atoi(const char * str) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->atoi(str);
}
static inline long int atol(const char *str) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->atol(str);
}
static inline long long int atoll(const char *str) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->atoll(str);
}
static inline double atof(const char *str) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->atof(str);
}
static inline long strtol(const char *str, char **endptr, int base) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->strtol(str, endptr, base);
}
static inline unsigned long strtoul(const char *str, char **endptr, int base) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->strtoul(str, endptr, base);
}
static inline long long strtoll(const char *str, char **endptr, int base) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->strtoll(str, endptr, base);
}
static inline unsigned long long strtoull(const char *str, char **endptr, int base) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->strtoull(str, endptr, base);
}
static inline float strtof(const char *str, char **endptr) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->strtof(str, endptr);
}
static inline double strtod(const char *str, char **endptr) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->strtod(str, endptr);
}
static inline long double strtold(const char *str, char **endptr) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->strtold(str, endptr);
}

static inline void * bsearch(const void *key, const void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *)) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->bsearch(key, base, nitems, size, compar);
}
static inline void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*)) {
    VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->qsort(base, nitems, size, compar);
}

static inline int rand(void) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->rand();
}
static inline void srand(unsigned int seed) {
    VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->srand(seed);
}
static inline void srandom(unsigned int seed) {
    VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->srand(seed);
}
static inline long int random(void) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->rand();
}

static inline void abort(void) {
    VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->abort();
}
static inline int system(const char *command) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->system(command);
}

static inline void exit(int status) {
    VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->exit(status);
}
static inline int atexit(void (*func)(void)) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->atexit(func);
}

static inline int abs(int j) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->abs(j);
}
static inline long labs(long j) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->labs(j);
}
static inline long long llabs(long long j) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->llabs(j);
}
static inline intmax_t imaxabs(intmax_t j) {
    return VSF_LINUX_APPLET_LIBC_STDLIB_VPLT->imaxabs(j);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STDLIB

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

div_t div(int numer, int denom);
ldiv_t ldiv(long int numer, long int denom);
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

void * bsearch(const void *key, const void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

int rand(void);
void srand(unsigned int seed);
void srandom(unsigned int seed);
long int random(void);

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));

void abort(void);
int system(const char *command);

void exit(int status);
int atexit(void (*func)(void));

int abs(int j);
long labs(long j);
long long llabs(long long j);
intmax_t imaxabs(intmax_t j);
#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STDLIB

int at_quick_exit(void (*func)(void));
void quick_exit(int status);

int mblen(const char *str, size_t n);
// TODO: it seems that IAR does not support wchar_t even if it's defined in stddef.h
#if !__IS_COMPILER_IAR__
size_t mbstowcs(wchar_t *dst, const char *src, size_t len);
int mbtowc(wchar_t *pwc, const char *str, size_t n);
size_t wcstombs(char *str, const wchar_t *pwcs, size_t n);
int wctomb(char *str, wchar_t wchar);
#endif

// _exit should be implemented in retarget
void _exit(int status);
void _Exit(int exit_code);

#ifdef __cplusplus
}
#endif

#endif
