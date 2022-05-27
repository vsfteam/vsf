/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/errno.h"
#   include "../../include/fcntl.h"
#   include "../../include/simple_libc/stdlib.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <fcntl.h>
#   include <stdlib.h>
#endif
#include <limits.h>

// for memset
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "../../include/simple_libc/string.h"
#else
#   include <string.h>
#endif

/*============================ MACROS ========================================*/

#ifdef VSF_ARCH_PROVIDE_HEAP
#   define VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN           vsf_arch_heap_alignment()
#else
#   define VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN           VSF_HEAP_ALIGN
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
static size_t __vsf_linux_heap_trace_alloc(vsf_linux_process_t *process, void *ptr, size_t size, va_list ap)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    VSF_LINUX_ASSERT(process != NULL);

    ssize_t i;
    vsf_protect_t orig = vsf_protect_sched();
#   if VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH > 0
        i = vsf_bitmap_ffz(&process->heap_monitor.bitmap, VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH);
        // if assert here, please increate VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH
        VSF_LINUX_ASSERT(i >= 0);
        // can not go on, stalls here even if assert is not enabled
        if (i < 0) { while(1); }

        process->heap_monitor.nodes[i].ptr = ptr;
        process->heap_monitor.nodes[i].size = size - sizeof(size_t);
        vsf_bitmap_set(&process->heap_monitor.bitmap, i);
#   else
        i = size;
#   endif
        process->heap_monitor.info.usage += size - sizeof(size_t);
        process->heap_monitor.info.balance++;
    vsf_unprotect_sched(orig);
    return (size_t)i;
}

static void __vsf_linux_heap_trace_free(vsf_linux_process_t *process, size_t i, void *ptr, va_list ap)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    VSF_LINUX_ASSERT(process != NULL);

    vsf_protect_t orig = vsf_protect_sched();
#   if VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH > 0
        VSF_LINUX_ASSERT(i < VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH);
        VSF_LINUX_ASSERT(vsf_bitmap_get(&process->heap_monitor.bitmap, i));
        VSF_LINUX_ASSERT(process->heap_monitor.nodes[i].ptr == ptr);
        process->heap_monitor.info.usage -= process->heap_monitor.nodes[i].size;
        vsf_bitmap_clear(&process->heap_monitor.bitmap, i);
#   else
        process->heap_monitor.info.usage -= i;
#   endif
        process->heap_monitor.info.balance--;
    vsf_unprotect_sched(orig);
}

void * __malloc_ex(vsf_linux_process_t *process, int size, ...)
{
    size += VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN;
    size_t *i = vsf_heap_malloc(size);
    if (i != NULL) {
        void *buffer = (void *)((char *)i + VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN);

        va_list ap;
        va_start(ap, size);
            *i = __vsf_linux_heap_trace_alloc(process, buffer, size, ap);
        va_end(ap);
        return buffer;
    } else {
        errno = ENOMEM;
    }
    return NULL;
}

void * __realloc_ex(vsf_linux_process_t *process, void *p, size_t size, ...)
{
    if (NULL == p) {
        if (size > 0) {
            return __malloc_ex(process, size);
        }
        return NULL;
    } else if (0 == size) {
        if (p != NULL) {
            __free_ex(process, p);
        }
        return NULL;
    } else {
        void *new_buff = __malloc_ex(process, size);
        if (new_buff != NULL) {
            size_t copy_size = vsf_heap_size((uint8_t *)p - VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN)
                                    - VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN;
            copy_size = vsf_min(size, copy_size);
            memcpy(new_buff, p, copy_size);
        }
        __free_ex(process, p);
        return new_buff;
    }
}

void __free_ex(vsf_linux_process_t *process, void *ptr, ...)
{
    if (ptr != NULL) {
        size_t *i = (size_t *)((char *)ptr - VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN);
        va_list ap;
        va_start(ap, ptr);
            __vsf_linux_heap_trace_free(process, *i, ptr, ap);
        va_end(ap);
        vsf_heap_free(i);
    }
}

void * __calloc_ex(vsf_linux_process_t *process, size_t n, size_t size, ...)
{
    size_t allsize = n * size;
    void *buf = __malloc_ex(process, allsize);
    if (buf != NULL) {
        memset(buf, 0, allsize);
    }
    return buf;
}
#endif

void * malloc(size_t size)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    return __malloc_ex(NULL, size);
#else
    void *result = vsf_heap_malloc(size);
    if (NULL == result) {
        errno = ENOMEM;
    }
    return result;
#endif
}

size_t malloc_usable_size(void *p)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    return vsf_heap_size((uint8_t *)p - VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN)
        - VSF_LINUX_SIMPLE_STDLIB_HEAP_ALIGN;
#else
    return vsf_heap_size((uint8_t *)p);
#endif
}

void * aligned_alloc(size_t alignment, size_t size)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    VSF_LINUX_ASSERT(false);
    return NULL;
#else
    void *result = vsf_heap_malloc_aligned(size, alignment);
    if (NULL == result) {
        errno = ENOMEM;
    }
    return result;
#endif
}

void * realloc(void *p, size_t size)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    return __realloc_ex(NULL, p, size);
#else
    void *result = vsf_heap_realloc(p, size);
    if (NULL == result) {
        errno = ENOMEM;
    }
    return result;
#endif
}

void free(void *p)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    __free_ex(NULL, p);
#else
    vsf_heap_free(p);
#endif
}

void * calloc(size_t n, size_t size)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    return __calloc_ex(NULL, n, size);
#else
    void *result = vsf_heap_calloc(n, size);
    if (NULL == result) {
        errno = ENOMEM;
    }
    return result;
#endif
}

void exit(int status)
{
    vsf_linux_exit_process(status);
}

int atexit(void (*func)(void))
{
    // atexit maybe called by c-startup in newlib, ignore if linux is not initialized
    if (vsf_linux_is_inited()) {
        vsf_linux_process_t *process = vsf_linux_get_cur_process();
        VSF_LINUX_ASSERT(process != NULL);
        process->fn_atexit = func;
    }
    return 0;
}

int system(const char * cmd)
{
    char *cmd_in_ram = strdup(cmd);
    if (NULL == cmd_in_ram) {
        return -1;
    }

    extern int __vsh_run_cmd(char *cmd);
    int result = __vsh_run_cmd(cmd_in_ram);
    free(cmd_in_ram);
    return result;
}

char * mktemps(char *template, int suffixlen)
{
    static const char __letters[] =
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";

    int len = strlen(template);
    if (    (len < (6 + suffixlen))
        ||  memcmp(&template[len - 6 - suffixlen], "XXXXXX", 6)) {
        errno = EINVAL;
        return NULL;
    }

    char *XXXXXX = &template[len - 6 - suffixlen];
    uint64_t value = vsf_systimer_get_tick(), value_tmp;
    value = value ^ bswap_64(value);

    for (int retry = 0; retry < 4; value += 7777, retry++) {
        value_tmp = value;
        XXXXXX[0] = __letters[value_tmp % dimof(__letters)];
        value_tmp /= dimof(__letters);
        XXXXXX[1] = __letters[value_tmp % dimof(__letters)];
        value_tmp /= dimof(__letters);
        XXXXXX[2] = __letters[value_tmp % dimof(__letters)];
        value_tmp /= dimof(__letters);
        XXXXXX[3] = __letters[value_tmp % dimof(__letters)];
        value_tmp /= dimof(__letters);
        XXXXXX[4] = __letters[value_tmp % dimof(__letters)];
        value_tmp /= dimof(__letters);
        XXXXXX[5] = __letters[value_tmp % dimof(__letters)];

        if (open(template, 0) < 0) {
            break;
        }
    }

    return template;
}

char * mktemp(char *template)
{
    return mktemps(template, 0);
}

int mkostemps(char *template, int suffixlen, int flags)
{
    char *name = mktemps(template, suffixlen);
    if (NULL == name) {
        return -1;
    }

    return open(name, flags | O_CREAT);
}

int mkostemp(char *template, int flags)
{
    return mkostemps(template, 0, flags);
}

int mkstemps(char *template, int suffixlen)
{
    return mkostemps(template, suffixlen, 0);
}

int mkstemp(char *template)
{
    return mkostemps(template, 0, 0);
}

char * mkdtemp(char *template)
{
    char *name = mktemp(template);
    if (NULL == name) {
        return NULL;
    }

    return (mkdir(name, 0) < 0) ? NULL : name;
}

#if !__IS_COMPILER_IAR__
int abs(int j)
{
    return (j < 0 ? -j : j);
}

long labs(long j)
{
    return (j < 0 ? -j : j);
}

long long llabs(long long j)
{
    return (j < 0 ? -j : j);
}
#endif

intmax_t imaxabs(intmax_t j)
{
    return (j < 0 ? -j : j);
}

#if VSF_LINUX_SIMPLE_STDLIB_USE_STRTOXX == ENABLED
static int __strtoradix(const char *str, char **endptr, int base)
{
    bool is_negative = false;
    char ch;
    VSF_LINUX_ASSERT(str != NULL);

    while (isspace(ch = *str)) { str++; }

    if (ch == '-') {
        is_negative = true;
        ch = *++str;
    } else if (ch == '+') {
        ch = *++str;
    }
    if (!(base & ~16) && (ch == '0') && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
        base = 16;
    } else if (!(base & ~2) && (ch == '0') && (str[1] == 'b' || str[1] == 'B')) {
        str += 2;
        base = 2;
    }
    if (0 == base) {
        base = ch == '0' ? 8 : 10;
    }

    if (endptr != NULL) {
        *endptr = (char *)str;
    }
    return is_negative ? -base : base;
}

static unsigned long long __strtonum(const char *str, char **endptr, int base, unsigned long long max)
{
    unsigned long long result = 0, tmp_result;
    const char *str_cur = str;
    char ch;
    bool is_valid = false;

    while (true) {
        ch = *str_cur++;
        if (isdigit(ch)) {
            ch -= '0';
        } else if (isalpha(ch)) {
            ch -= ((isupper(ch) ? 'A' : 'a') - 10);
        } else {
            break;
        }
        if (ch >= base) {
            break;
        }

        tmp_result = result * base + ch;
        if (tmp_result > max) {
            break;
        }
        result = tmp_result;
        is_valid = true;
    }

    if (endptr != NULL) {
        *endptr = (char *)(is_valid ? str_cur - 1 : str);
    }
    return result;
}

long strtol(const char *str, char **endptr, int base)
{
    char *str_cur;
    bool is_negative = false;
    unsigned long long max;

    base = __strtoradix(str, &str_cur, base);
    if (base < 0) {
        is_negative = true;
        base = -base;
        max = -LONG_MIN;
    } else {
        max = LONG_MAX;
    }

    return (is_negative ? -1 : 1) * (long)__strtonum(str_cur, endptr, base, max);
}

long long strtoll(const char *str, char **endptr, int base)
{
    char *str_cur;
    bool is_negative = false;
    unsigned long long max;

    base = __strtoradix(str, &str_cur, base);
    if (base < 0) {
        is_negative = true;
        base = -base;
        max = -LLONG_MIN;
    } else {
        max = LLONG_MAX;
    }

    return (is_negative ? -1 : 1) * (long long)__strtonum(str_cur, endptr, base, max);
}

unsigned long strtoul(const char *str, char **endptr, int base)
{
    char *str_cur;
    unsigned long long max;

    base = __strtoradix(str, &str_cur, base);
    if (base < 0) {
        if (endptr != NULL) {
            *endptr = (char *)str;
        }
        return 0;
    } else {
        max = ULONG_MAX;
    }

    return (unsigned long)__strtonum(str_cur, endptr, base, max);
}

unsigned long long strtoull(const char *str, char **endptr, int base)
{
    char *str_cur;
    unsigned long long max;

    base = __strtoradix(str, &str_cur, base);
    if (base < 0) {
        if (endptr != NULL) {
            *endptr = (char *)str;
        }
        return 0;
    } else {
        max = ULLONG_MAX;
    }

    return __strtonum(str_cur, endptr, base, max);
}

double strtod(const char *str, char **endptr)
{
    double number = 0;
    int exponent;
    bool is_negative;
    char *str_tmp;
    unsigned long long value;

    while (isspace(*str)) { str++; }

    is_negative = false;
    switch (*str) {
    case '-':
        is_negative = true;
    case '+':
        str++;
        break;
    }

    value = __strtonum(str, &str_tmp, 10, ULLONG_MAX);
    if (str_tmp == str) {
        goto error;
    }
    number = (double)value;
    str = str_tmp;
    if (*str == '.') {
        str++;
        value = __strtonum(str, &str_tmp, 10, ULLONG_MAX);
        if (str_tmp != str) {
            unsigned long long ten_n = 1;
            exponent = str_tmp - str;
            for (int i = 0; i < exponent; i++) {
                ten_n *= 10;
            }
            number += (double)value / ten_n;
            str = str_tmp;
        }
    }
    if (is_negative) {
        number = -number;
    }

    if (*str == 'e' || *str == 'E') {
        double ten_n = 10.;
        switch (*++str) {
        case '-':
            ten_n = 0.1;
        case '+':
            str++;
            break;
        }

        exponent = (int)__strtonum(str, &str_tmp, 10, 308);
        while (exponent) {
            if (exponent & 1) {
                number *= ten_n;
            }
            exponent >>= 1;
            ten_n *= ten_n;
        }
    }
    
    return number;
error:
    errno = ERANGE;
    return 0.0;
}

float strtof(const char *str, char **endptr)
{
    return (float)strtod(str, endptr);
}

int atoi(const char *str)
{
    return strtol(str, NULL, 10);
}

long int atol(const char *str)
{
    return strtol(str, NULL, 10);
}

long long int atoll(const char *str)
{
    return strtoll(str, NULL, 10);
}

double atof(const char *str)
{
    return strtod(str, NULL);
}
#endif

char * itoa(int num, char *str, int radix)
{
    char index[] = "0123456789ABCDEF";
    unsigned unum;
    int i = 0, j, k;

    if (radix == 10 && num < 0) {
        unum = (unsigned)-num;
        str[i++] = '-';
    } else {
        unum = (unsigned)num;
    }

    do{
        str[i++] = index[unum % (unsigned)radix];
        unum /= radix;
   } while (unum);

    str[i] = '\0';
    if (str[0] == '-') {
        k = 1;
    } else {
        k = 0;
    }

    for (j = k; j <= (i - 1) / 2; j++) {
        char temp;
        temp = str[j];
        str[j] = str[i - 1 + k - j];
        str[i - 1 + k - j] = temp;
    }
    return str;
}

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
int __putenv_ex(vsf_linux_process_t *process, char *string)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    VSF_LINUX_ASSERT(process != NULL);

    char ***__environ = &process->__environ;
    const char * str = strchr(string, '=');
    bool is_to_set = str != NULL;
    size_t namelen = is_to_set ? str - string : strlen(string);
    size_t size = 0;
    bool is_match = false;

    char **env = *__environ, **env_removed = NULL;
    if (env != NULL) {
        for (char **env_tmp = env; *env_tmp != NULL; env_tmp++, size++) {
            if (    !strncmp(string, *env_tmp, namelen)
                &&  ((*env_tmp)[namelen] == '=')) {
                is_match = true;
                __free_ex(process, *env_tmp);
                if (is_to_set) {
                    *env_tmp = string;
                    VSF_LINUX_ASSERT(*env_tmp != NULL);
                    return 0;
                } else {
                    env_removed = env_tmp;
                    *env_tmp = NULL;
                }
            }
        }
    }

    if (is_to_set) {
        VSF_LINUX_ASSERT(!is_match);
        *__environ = env = (char **)__realloc_ex(process, env, (size + 2) * sizeof(char *));
        if (NULL == env) {
            return -1;
        }
        env[size] = string;
        env[size + 1] = NULL;
    } else if (env_removed != NULL) {
        *env_removed = env[size - 1];
        env[size - 1] = NULL;
    }
    return 0;
}

char * __getenv_ex(vsf_linux_process_t *process, const char *name)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    VSF_LINUX_ASSERT(process != NULL);

    size_t namelen = strlen(name);
    char **env = process->__environ;

    if (NULL == env) {
        return NULL;
    }

    for (; *env != NULL; env++) {
        if (    !strncmp(name, *env, namelen)
            &&  ((*env)[namelen] == '=')) {
            return *env + namelen + 1;
        }
    }
    return NULL;
}

int __setenv_ex(vsf_linux_process_t *process, const char *name, const char *value, int replace)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    VSF_LINUX_ASSERT(process != NULL);

    size_t namelen = strlen(name), valuelen = strlen(value);
    if (!replace) {
        if (__getenv_ex(process, name) != NULL) {
            return 0;
        }
    }

    char *env_str = __malloc_ex(process, namelen + valuelen + 2);
    if (NULL == env_str) {
        return -1;
    }
    memcpy(env_str, name, namelen);
    env_str[namelen] = '=';
    memcpy(env_str + namelen + 1, value, valuelen);
    env_str[namelen + valuelen + 1] = '\0';
    return __putenv_ex(process, env_str);
}

int putenv(char *string)
{
    return __putenv_ex(NULL, string);
}

char * getenv(const char *name)
{
    return __getenv_ex(NULL, name);
}

int setenv(const char *name, const char *value, int replace)
{
    return __setenv_ex(NULL, name, value, replace);
}

int unsetenv(const char *name)
{
    return putenv((char *)name);
}
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDLIB
