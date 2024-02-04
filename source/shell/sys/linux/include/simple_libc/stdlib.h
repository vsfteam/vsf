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

// itoa is non-standard APIs in stdlib, define VSF_LINUX_LIBC_HAS_ITOA to 0 if any confliction
#ifndef VSF_LINUX_LIBC_HAS_ITOA
#   define VSF_LINUX_LIBC_HAS_ITOA          1
#endif

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#   if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR != ENABLED
#define malloc              VSF_LINUX_LIBC_WRAPPER(malloc)
#define calloc              VSF_LINUX_LIBC_WRAPPER(calloc)
#define realloc             VSF_LINUX_LIBC_WRAPPER(realloc)
#   endif
#define aligned_alloc       VSF_LINUX_LIBC_WRAPPER(aligned_alloc)
#define free                VSF_LINUX_LIBC_WRAPPER(free)
#define posix_memalign      VSF_LINUX_LIBC_WRAPPER(posix_memalign)
#define malloc_usable_size  VSF_LINUX_LIBC_WRAPPER(malloc_usable_size)
#define exit                VSF_LINUX_LIBC_WRAPPER(exit)
#define atexit              VSF_LINUX_LIBC_WRAPPER(atexit)
#define _Exit               VSF_LINUX_LIBC_WRAPPER(_Exit)
#define system              VSF_LINUX_LIBC_WRAPPER(system)
#   if VSF_LINUX_LIBC_USE_ENVIRON
#define getenv              VSF_LINUX_LIBC_WRAPPER(getenv)
#define putenv              VSF_LINUX_LIBC_WRAPPER(putenv)
#define setenv              VSF_LINUX_LIBC_WRAPPER(setenv)
#define unsetenv            VSF_LINUX_LIBC_WRAPPER(unsetenv)
#define clearenv            VSF_LINUX_LIBC_WRAPPER(clearenv)
#   endif
#define realpath            VSF_LINUX_LIBC_WRAPPER(realpath)
#define mktemps             VSF_LINUX_LIBC_WRAPPER(mktemps)
#define mktemp              VSF_LINUX_LIBC_WRAPPER(mktemp)
#define mkstemp             VSF_LINUX_LIBC_WRAPPER(mkstemp)
#define mkostemp            VSF_LINUX_LIBC_WRAPPER(mkostemp)
#define mkstemps            VSF_LINUX_LIBC_WRAPPER(mkstemps)
#define mkostemps           VSF_LINUX_LIBC_WRAPPER(mkostemps)
#define mkdtemp             VSF_LINUX_LIBC_WRAPPER(mkdtemp)
#elif defined(__WIN__) && !defined(__VSF_APPLET__)
// avoid conflicts with APIs in ucrt
#define exit                VSF_LINUX_LIBC_WRAPPER(exit)
#define atexit              VSF_LINUX_LIBC_WRAPPER(atexit)
#define _Exit               VSF_LINUX_LIBC_WRAPPER(_Exit)
#define getenv              VSF_LINUX_LIBC_WRAPPER(getenv)
// system("chcp 65001"); will be called in debug_stream driver, wrapper here
#define system              VSF_LINUX_LIBC_WRAPPER(system)
#endif

// syscalls

#define __NR_exit           exit

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

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(____malloc_ex);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(____realloc_ex);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(____calloc_ex);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(malloc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(realloc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(calloc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(free);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(aligned_alloc);
    // originally memalign, removed, so if add other API, put here first
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(empty_slot);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_memalign);
    // malloc_usable_size should be in malloc.h
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(malloc_usable_size);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(putenv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getenv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setenv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(unsetenv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(clearenv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mktemps);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mktemp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkstemp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkostemp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkstemps);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkostemps);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkdtemp);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(div);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ldiv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lldiv);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(itoa);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atoi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atol);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atoll);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atof);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtol);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtoul);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtoll);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtoull);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtof);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strtold);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(bsearch);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(qsort);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(rand);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(srand);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(abort);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(system);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(exit);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(atexit);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(_Exit);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(abs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(labs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(llabs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(imaxabs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getloadavg);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(realpath);
} vsf_linux_libc_stdlib_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_stdlib_vplt_t vsf_linux_libc_stdlib_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_LIBC_STDLIB_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_LIBC_STDLIB == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_STDLIB_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_STDLIB_VPLT                                \
            ((vsf_linux_libc_stdlib_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_stdlib_vplt))
#   else
#       define VSF_LINUX_APPLET_LIBC_STDLIB_VPLT                                \
            ((vsf_linux_libc_stdlib_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(__NAME)                              \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_STDLIB_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_STDLIB_IMP(...)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_STDLIB_VPLT, __VA_ARGS__)

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(____malloc_ex, void *, size_t size, const char *file, const char *func, int line) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(____malloc_ex)(size, file, func, line);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(____realloc_ex, void *, void *p, size_t size, const char *file, const char *func, int line) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(____realloc_ex)(p, size, file, func, line);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(____calloc_ex, void *, size_t n, size_t size, const char *file, const char *func, int line) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(____calloc_ex)(n, size, file, func, line);
}
#endif
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(malloc, void *, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(malloc)(size);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(realloc, void *, void *p, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(realloc)(p, size);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(calloc, void *, size_t n, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(calloc)(n, size);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(free, void, void *p) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(free)(p);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(aligned_alloc, void *, size_t alignment, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(aligned_alloc)(alignment, size);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(posix_memalign, int, void **memptr, size_t alignment, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(posix_memalign)(memptr, alignment, size);
}
// malloc_usable_size should be in malloc.h
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(malloc_usable_size, size_t, void *p) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(malloc_usable_size)(p);
}

#if VSF_LINUX_LIBC_USE_ENVIRON
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(putenv, int, char *string) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(putenv)(string);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(getenv, char *, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(getenv)(name);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(setenv, int, const char *name, const char *value, int replace) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(setenv)(name, value, replace);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(unsetenv, int, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(unsetenv)(name);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(clearenv, int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(clearenv)();
}
#endif
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(mktemps, char *, char *template_str, int suffixlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(mktemps)(template_str, suffixlen);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(mktemp, char *, char *template_str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(mktemp)(template_str);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(mkstemp, int, char *template_str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(mkstemp)(template_str);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(mkostemp, int, char *template_str, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(mkostemp)(template_str, flags);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(mkstemps, int, char *template_str, int suffixlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(mkstemps)(template_str, suffixlen);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(mkostemps, int, char *template_str, int suffixlen, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(mkostemps)(template_str, suffixlen, flags);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(mkdtemp, char *, char *template_str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(mkdtemp)(template_str);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(div, div_t, int numer, int denom) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(div)(numer, denom);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(ldiv, ldiv_t, long int numer, long int denom) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(ldiv)(numer, denom);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(lldiv, lldiv_t, long long int numer, long long int denom) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(lldiv)(numer, denom);
}
#if VSF_LINUX_LIBC_HAS_ITOA
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(itoa, char *, int num, char *str, int radix) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(itoa)(num, str, radix);
}
#endif
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(atoi, int, const char * str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(atoi)(str);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(atol, long int, const char *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(atol)(str);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(atoll, long long int, const char *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(atoll)(str);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(atof, double, const char *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(atof)(str);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(strtol, long, const char *str, char **endptr, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(strtol)(str, endptr, base);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(strtoul, unsigned long, const char *str, char **endptr, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(strtoul)(str, endptr, base);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(strtoll, long long, const char *str, char **endptr, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(strtoll)(str, endptr, base);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(strtoull, unsigned long long, const char *str, char **endptr, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(strtoull)(str, endptr, base);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(strtof, float, const char *str, char **endptr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(strtof)(str, endptr);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(strtod, double, const char *str, char **endptr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(strtod)(str, endptr);
}
//VSF_LINUX_APPLET_LIBC_STDLIB_IMP(strtold, long double, const char *str, char **endptr) {
//    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
//    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(strtold)(str, endptr);
//}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(bsearch, void *, const void *key, const void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *)) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(bsearch)(key, base, nitems, size, compar);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(qsort, void, void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*)) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(qsort)(base, nitems, size, compar);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(rand, int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(rand)();
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(srand, void, unsigned int seed) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(srand)(seed);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(srandom, void, unsigned int seed) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(srand)(seed);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(random, long int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(rand)();
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(abort, void, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(abort)();
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(system, int, const char *command) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(system)(command);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(exit, void, int status) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(exit)(status);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(atexit, int, void (*func)(void)) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(atexit)(func);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(_Exit, void, int status) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(_Exit)(status);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(abs, int, int j) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(abs)(j);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(labs, long, long j) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(labs)(j);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(llabs, long long, long long j) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(llabs)(j);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(imaxabs, intmax_t, intmax_t j) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(imaxabs)(j);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(getloadavg, int, double loadavg[], int nelem) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(getloadavg)(loadavg, nelem);
}
VSF_LINUX_APPLET_LIBC_STDLIB_IMP(realpath, char *, const char *path, char *resolved_path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDLIB_ENTRY(realpath)(path, resolved_path);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STDLIB

#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
typedef struct vsf_linux_process_t vsf_linux_process_t;
#   if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED
void * ____malloc_ex(vsf_linux_process_t *process, size_t size, const char *file, const char *func, int line);
void * ____calloc_ex(vsf_linux_process_t *process, size_t n, size_t size, const char *file, const char *func, int line);
void * ____realloc_ex(vsf_linux_process_t *process, void *p, size_t size, const char *file, const char *func, int line);
#       define malloc(__size)               ____malloc_ex(NULL, (__size), __FILE__, __FUNCTION__, __LINE__)
#       define calloc(__n, __size)          ____calloc_ex(NULL, (__n), (__size), __FILE__, __FUNCTION__, __LINE__)
#       define realloc(__ptr, __size)       ____realloc_ex(NULL, (__ptr), (__size), __FILE__, __FUNCTION__, __LINE__)
#   else
void * ____malloc_ex(vsf_linux_process_t *process, size_t size);
void * ____calloc_ex(vsf_linux_process_t *process, size_t n, size_t size);
void * ____realloc_ex(vsf_linux_process_t *process, void *p, size_t size);
#       define malloc(__size)               ____malloc_ex(NULL, (__size))
#       define calloc(__n, __size)          ____calloc_ex(NULL, (__n), (__size))
#       define realloc(__ptr, __size)       ____realloc_ex(NULL, (__ptr), (__size))
#   endif
#else
void * malloc(size_t size);
void * realloc(void *p, size_t size);
void * calloc(size_t n, size_t size);
#endif

void free(void *p);

void * aligned_alloc(size_t alignment, size_t size);
int posix_memalign(void **memptr, size_t alignment, size_t size);

// malloc_usable_size should be in malloc.h
size_t malloc_usable_size(void *p);

#if VSF_LINUX_LIBC_USE_ENVIRON
int putenv(char *string);
char * getenv(const char *name);
int setenv(const char *name, const char *value, int replace);
int unsetenv(const char *name);
int clearenv(void);
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

#if VSF_LINUX_LIBC_HAS_ITOA
char * itoa(int num, char *str, int radix);
#endif
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
//long double strtold(const char *str, char **endptr);

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
void _Exit(int exit_code);

int abs(int j);
long labs(long j);
long long llabs(long long j);
intmax_t imaxabs(intmax_t j);

int getloadavg(double loadavg[], int nelem);
char *realpath(const char *path, char *resolved_path);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STDLIB

static inline void * reallocarray(void *p, size_t nmemb, size_t size) {
    return realloc(p, nmemb * size);
}

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

#ifdef __cplusplus
}
#endif

#endif
