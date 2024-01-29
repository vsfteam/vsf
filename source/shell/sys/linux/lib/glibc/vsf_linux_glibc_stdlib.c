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
#   include "../../include/spawn.h"
// for waitpid
#   include "../../include/sys/wait.h"
#   include "../../include/simple_libc/stdlib.h"
#   include "../../include/simple_libc/malloc.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <fcntl.h>
#   include <spawn.h>
// for waitpid
#   include <sys/wait.h>
#   include <stdlib.h>
#   include <malloc.h>
#endif
#include <limits.h>

// for memset
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "../../include/simple_libc/string.h"
#else
#   include <string.h>
#endif

#if defined(__WIN__) && !defined(_DEBUG) && VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
#   warning ********windows: In release mode, malloc will be called in some system APIs,\
                and heap monitor will fail if malloc not called in vsf_linux environment ********
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
static void __vsf_linux_heap_trace_alloc(vsf_linux_process_t *process, vsf_liunx_heap_node_t *node,
    void *ptr, size_t size, const char *file, const char *func, int line)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    VSF_LINUX_ASSERT(process != NULL);

    node->ptr = ptr;
    node->size = size;
    node->file = file;
    node->func = func;
    node->line = line;
    vsf_dlist_init_node(vsf_liunx_heap_node_t, node, node);

    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_add_to_tail(vsf_liunx_heap_node_t, node, &process->heap_monitor.list, node);
        process->heap_monitor.usage += size;
        process->heap_monitor.balance++;
//        vsf_trace_debug("0x%p: +%d 0x%p\n", process, node->size, ptr);
    vsf_unprotect_sched(orig);
}

static void __vsf_linux_heap_trace_free(vsf_linux_process_t *process, vsf_liunx_heap_node_t *node, void *ptr)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    VSF_LINUX_ASSERT(process != NULL);

    vsf_protect_t orig = vsf_protect_sched();
//        vsf_trace_debug("0x%p: -%d 0x%p\n", process, node->size, ptr);
        process->heap_monitor.usage -= node->size;
        process->heap_monitor.balance--;
        vsf_dlist_remove(vsf_liunx_heap_node_t, node, &process->heap_monitor.list, node);
    vsf_unprotect_sched(orig);
}

#   if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED
void __free_ex(vsf_linux_process_t *process, void *ptr)
{
    if (ptr != NULL) {
        vsf_liunx_heap_node_t *node = (vsf_liunx_heap_node_t *)((char *)ptr - sizeof(vsf_liunx_heap_node_t));
        __vsf_linux_heap_trace_free(process, node, ptr);
        vsf_linux_process_heap_free(process, (void *)node);
    }
}

void * ____malloc_ex(vsf_linux_process_t *process, size_t size, const char *file, const char *func, int line)
{
    vsf_liunx_heap_node_t *node = vsf_linux_process_heap_malloc(process, size + sizeof(vsf_liunx_heap_node_t));
    if (node != NULL) {
        void *buffer = (void *)((char *)node + sizeof(vsf_liunx_heap_node_t));
        __vsf_linux_heap_trace_alloc(process, node, buffer, size, file, func, line);
        return buffer;
    } else {
        errno = ENOMEM;
    }
    return NULL;
}

void * ____realloc_ex(vsf_linux_process_t *process, void *p, size_t size, const char *file, const char *func, int line)
{
    if (NULL == p) {
        if (size > 0) {
            return ____malloc_ex(process, size, file, func, line);
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
            size_t copy_size = vsf_linux_process_heap_size(process, (uint8_t *)p - sizeof(vsf_liunx_heap_node_t))
                                    - sizeof(vsf_liunx_heap_node_t);
            copy_size = vsf_min(size, copy_size);
            memcpy(new_buff, p, copy_size);
        }
        __free_ex(process, p);
        return new_buff;
    }
}

void * ____calloc_ex(vsf_linux_process_t *process, size_t n, size_t size, const char *file, const char *func, int line)
{
    size_t allsize = n * size;
    void *buf = ____malloc_ex(process, allsize, file, func, line);
    if (buf != NULL) {
        memset(buf, 0, allsize);
    }
    return buf;
}
#   endif
#else
void * ____malloc_ex(vsf_linux_process_t *process, size_t size, const char *file, const char *func, int line)
{
    return __malloc_ex(process, size);
}

void * ____realloc_ex(vsf_linux_process_t *process, void *p, size_t size, const char *file, const char *func, int line)
{
    return __realloc_ex(process, p, size);
}

void * ____calloc_ex(vsf_linux_process_t *process, size_t n, size_t size, const char *file, const char *func, int line)
{
    return __calloc_ex(process, n, size);
}
#endif

#if VSF_LINUX_SIMPLE_LIBC_CFG_NO_MM != ENABLED
size_t malloc_usable_size(void *p)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    return vsf_linux_process_heap_size(NULL, (uint8_t *)p - sizeof(vsf_liunx_heap_node_t))
        - sizeof(vsf_liunx_heap_node_t);
#else
    return vsf_linux_process_heap_size(NULL, p);
#endif
}

void * aligned_alloc(size_t alignment, size_t size)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    VSF_LINUX_ASSERT(false);
    return NULL;
#else
    return vsf_linux_process_heap_malloc_aligned(NULL, size, alignment);
#endif
}

void free(void *p)
{
    __free_ex(NULL, p);
}
#endif

void exit(int status)
{
    vsf_linux_exit_process(status, false);
}

int atexit(void (*func)(void))
{
    // atexit maybe called by c-startup in newlib, ignore if linux is not initialized
    if (vsf_linux_is_inited()) {
        vsf_linux_process_t *process = vsf_linux_get_real_process(NULL);
        VSF_LINUX_ASSERT(process != NULL);

        if (process->fn_atexit_num >= dimof(process->fn_atexit)) {
            return -1;
        }
        process->fn_atexit[process->fn_atexit_num++] = func;
    }
    return 0;
}

void _Exit(int status)
{
    vsf_linux_exit_process(status, true);
}

int system(const char * cmd)
{
    if (NULL == cmd) {
        // If command is NULL, then a nonzero value if a shell is
        //    available, or 0 if no shell is available.
        return 1;
    }

    pid_t pid;
    const char *argv[] = { "sh", "-c", cmd, (const char *)NULL };
    if (    (posix_spawn(&pid, "/bin/sh", NULL, NULL, (char * const *)argv, NULL) < 0)
        ||  (pid < 0)) {
        return -1;
    }

    int result;
    if (waitpid(pid, &result, 0) < 0) {
        return -1;
    }
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

int getloadavg(double loadavg[], int nelem)
{
    return -1;
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

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
    VSF_LINUX_ASSERT(memptr != NULL);
    void *ptr = memalign(alignment, size);
    if (NULL == ptr) {
        return ENOMEM;
    }
    *memptr = ptr;
    return 0;
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

int __clearenv_ex(vsf_linux_process_t *process)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    VSF_LINUX_ASSERT(process != NULL);

    char **env = process->__environ;

    if (env != NULL) {
        for (; *env != NULL; env++) {
            __free_ex(process, *env);
            *env = NULL;
        }
        __free_ex(process, env);
        process->__environ = NULL;
    }
    return 0;
}

int putenv(char *string)
{
    const char *str = strchr(string, '=');
    if (NULL == str) {
        return __putenv_ex(NULL, string);
    }

    int namelen = str - string;
    char name[namelen + 1];
    memcpy(name, string, namelen);
    name[namelen] = '\0';

    char *target = __getenv_ex(NULL, name);
    if (target == string + namelen + 1) {
        return 0;
    }

    string = __strdup_ex(NULL, string);
    if (NULL == string) {
        return -1;
    }
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

int clearenv(void)
{
    return __clearenv_ex(NULL);
}
#endif

void abort(void)
{
    VSF_LINUX_ASSERT(false);
}

char *realpath(const char *path, char *resolved_path)
{
    bool is_allocated = false;
    if (NULL == resolved_path) {
        resolved_path = malloc(PATH_MAX);
        if (NULL == resolved_path) {
            return NULL;
        }
        is_allocated = true;
    }
    if (vsf_linux_generate_path(resolved_path, PATH_MAX, NULL, (char *)path)) {
        if (is_allocated) {
            free(resolved_path);
        }
        return NULL;
    }
    return resolved_path;
}

#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED                     \
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED
#   undef malloc
#   undef realloc
#   undef calloc
#endif
void * malloc(size_t size)
{
    return __malloc_ex(NULL, size);
}

void * realloc(void *p, size_t size)
{
    return __realloc_ex(NULL, p, size);
}

void * calloc(size_t n, size_t size)
{
    return __calloc_ex(NULL, n, size);
}

#if VSF_LINUX_APPLET_USE_LIBC_STDLIB == ENABLED && !defined(__VSF_APPLET__)

#if VSF_ARCH_USE_THREAD_REG == ENABLED
// workaround for bsearch and qsort, which will modify process_reg can call user callback
static int __compar(const void *a, const void *b)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT((thread != NULL) && (thread->process_reg_backup.tmp_ptr != NULL));
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    uintptr_t orig = vsf_linux_set_process_reg(process->reg);
    int result = ((int (*)(const void *, const void *))thread->process_reg_backup.tmp_ptr)(a, b);
    vsf_linux_set_process_reg(orig);
    return result;
}

// re-implement bsearch and qsort to avoid calling compar with arch_thread_reg modified
static void * __bsearch(const void *key, const void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *))
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);

    thread->process_reg_backup.tmp_ptr = (void *)compar;
    void *result = bsearch(key, base, nitems, size, __compar);
    thread->process_reg_backup.tmp_ptr = NULL;

    return result;
}

static void __qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);
    thread->process_reg_backup.tmp_ptr = (void *)compar;
    qsort(base, nitems, size, __compar);
    thread->process_reg_backup.tmp_ptr = NULL;
}
#endif

__VSF_VPLT_DECORATOR__ vsf_linux_libc_stdlib_vplt_t vsf_linux_libc_stdlib_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_libc_stdlib_vplt_t, 0, 0, true),

#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED                     \
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED
    VSF_APPLET_VPLT_ENTRY_FUNC(____malloc_ex),
    VSF_APPLET_VPLT_ENTRY_FUNC(____realloc_ex),
    VSF_APPLET_VPLT_ENTRY_FUNC(____calloc_ex),
#endif

    VSF_APPLET_VPLT_ENTRY_FUNC(malloc),
    VSF_APPLET_VPLT_ENTRY_FUNC(realloc),
    VSF_APPLET_VPLT_ENTRY_FUNC(calloc),
    VSF_APPLET_VPLT_ENTRY_FUNC(free),
    VSF_APPLET_VPLT_ENTRY_FUNC(aligned_alloc),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_memalign),
    // malloc_usable_size should be in malloc.h
    VSF_APPLET_VPLT_ENTRY_FUNC(malloc_usable_size),
#if VSF_LINUX_LIBC_USE_ENVIRON
    VSF_APPLET_VPLT_ENTRY_FUNC(putenv),
    VSF_APPLET_VPLT_ENTRY_FUNC(getenv),
    VSF_APPLET_VPLT_ENTRY_FUNC(setenv),
    VSF_APPLET_VPLT_ENTRY_FUNC(unsetenv),
    VSF_APPLET_VPLT_ENTRY_FUNC(clearenv),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mktemps),
    VSF_APPLET_VPLT_ENTRY_FUNC(mktemp),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkstemp),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkostemp),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkstemps),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkostemps),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkdtemp),

    VSF_APPLET_VPLT_ENTRY_FUNC(div),
    VSF_APPLET_VPLT_ENTRY_FUNC(ldiv),
    VSF_APPLET_VPLT_ENTRY_FUNC(lldiv),

    VSF_APPLET_VPLT_ENTRY_FUNC(itoa),
    VSF_APPLET_VPLT_ENTRY_FUNC(atoi),
    VSF_APPLET_VPLT_ENTRY_FUNC(atol),
    VSF_APPLET_VPLT_ENTRY_FUNC(atoll),
    VSF_APPLET_VPLT_ENTRY_FUNC(atof),
    VSF_APPLET_VPLT_ENTRY_FUNC(strtol),
    VSF_APPLET_VPLT_ENTRY_FUNC(strtoul),
    VSF_APPLET_VPLT_ENTRY_FUNC(strtoll),
    VSF_APPLET_VPLT_ENTRY_FUNC(strtoull),
    VSF_APPLET_VPLT_ENTRY_FUNC(strtof),
    VSF_APPLET_VPLT_ENTRY_FUNC(strtod),
//    VSF_APPLET_VPLT_ENTRY_FUNC(strtold),

#if VSF_ARCH_USE_THREAD_REG == ENABLED
    .fn_bsearch = {
        .name = "bsearch",
        .ptr = (void *)__bsearch,
    },
    .fn_qsort = {
        .name = "qsort",
        .ptr = (void *)__qsort,
    },
#else
    VSF_APPLET_VPLT_ENTRY_FUNC(bsearch),
    VSF_APPLET_VPLT_ENTRY_FUNC(qsort),
#endif

    VSF_APPLET_VPLT_ENTRY_FUNC(rand),
    VSF_APPLET_VPLT_ENTRY_FUNC(srand),

    VSF_APPLET_VPLT_ENTRY_FUNC(abort),
    VSF_APPLET_VPLT_ENTRY_FUNC(system),

    VSF_APPLET_VPLT_ENTRY_FUNC(exit),
    VSF_APPLET_VPLT_ENTRY_FUNC(atexit),
    VSF_APPLET_VPLT_ENTRY_FUNC(_Exit),

    VSF_APPLET_VPLT_ENTRY_FUNC(abs),
    VSF_APPLET_VPLT_ENTRY_FUNC(labs),
    VSF_APPLET_VPLT_ENTRY_FUNC(llabs),
    VSF_APPLET_VPLT_ENTRY_FUNC(imaxabs),
    VSF_APPLET_VPLT_ENTRY_FUNC(getloadavg),
    VSF_APPLET_VPLT_ENTRY_FUNC(realpath),
};
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDLIB
