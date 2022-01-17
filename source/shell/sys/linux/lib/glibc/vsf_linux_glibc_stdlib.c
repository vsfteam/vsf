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

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED
#   define __VSF_LINUX_FS_CLASS_INHERIT__
#endif
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_CHECK == ENABLED
#   define __VSF_LINUX_CLASS_INHERIT__
#endif

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/simple_libc/stdlib.h"
#else
#   include <unistd.h>
#   include <stdlib.h>
#endif

// for memset
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "../../include/simple_libc/string.h"
#else
#   include <string.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
char **environ = NULL;
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED
static int __vsf_linux_heap_close(vsf_linux_fd_t *sfd);
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED
const vsf_linux_fd_op_t __vsf_linux_heap_fdop = {
    .fn_close           = __vsf_linux_heap_close,
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED
static int __vsf_linux_heap_close(vsf_linux_fd_t *sfd)
{
    return 0;
}
#endif

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_CHECK == ENABLED
static void __vsf_linux_heap_check_malloc(size_t size)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    vsf_protect_t orig = vsf_protect_sched();
        process->heap_usage += size;
        process->heap_balance++;
    vsf_unprotect_sched(orig);
}

static void __vsf_linux_heap_check_free(size_t size)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    vsf_protect_t orig = vsf_protect_sched();
        process->heap_usage -= size;
        process->heap_balance--;
    vsf_unprotect_sched(orig);
}
#endif

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_TRACE == ENABLED
void * __malloc(size_t size)
#else
void * malloc(size_t size)
#endif
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED
    size += sizeof(vsf_linux_fd_t);
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)vsf_heap_malloc(size);
    if (sfd != NULL) {
        memset(sfd, 0, sizeof(*sfd));
        sfd->op = &__vsf_linux_heap_fdop;
        vsf_linux_fd_add(sfd, -1);
        return (void *)&sfd[1];
    }
    return NULL;
#elif VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_CHECK == ENABLED
    size += sizeof(size_t);
    size_t *ret = vsf_heap_malloc(size);
    if (ret != NULL) {
        *ret = size;
        __vsf_linux_heap_check_malloc(size);
        return (void *)(ret + 1);
    }
    return NULL;
#else
    return vsf_heap_malloc(size);
#endif
}

void * aligned_alloc(size_t alignment, size_t size)
{
#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED                          \
    ||  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_CHECK == ENABLED
    VSF_LINUX_ASSERT(false);
#else
    return vsf_heap_malloc_aligned(size, alignment);
#endif
}

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_TRACE == ENABLED
void * __realloc(void *p, size_t size)
#else
void * realloc(void *p, size_t size)
#endif
{
#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED                          \
    ||  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_CHECK == ENABLED
    if (NULL == p) {
        if (size > 0) {
            return malloc(size);
        }
        return NULL;
    } else if (0 == size) {
        if (p != NULL) {
            free(p);
        }
        return NULL;
    } else {
        void *new_buff = malloc(size);
        if (new_buff != NULL) {
            memcpy(new_buff, p, size);
        }
        free(p);
        return new_buff;
    }
#else
    return vsf_heap_realloc(p, size);
#endif
}

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_TRACE == ENABLED
void __free(void *p)
#else
void free(void *p)
#endif
{
    if (p != NULL) {
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED
        vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)p - 1;
        close(sfd->fd);
#elif VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_CHECK == ENABLED
        size_t *size = (size_t *)p - 1;
        __vsf_linux_heap_check_free(*size);
        vsf_heap_free(size);
#else
        vsf_heap_free(p);
#endif
    }
}

void * calloc(size_t n, size_t size)
{
    size_t allsize = n * size;
    void *buf = malloc(allsize);
    if (buf != NULL) {
        memset(buf, 0, allsize);
    }
    return buf;
}

void exit(int status)
{
    // exit process is not supported, can exit current thread only
    vsf_thread_exit();
}

int atexit(void (*func)(void))
{
    // TODO: since exit is not fully supported, any meaning to support atexit?
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

char * mktemp(char *template_str)
{
    VSF_LINUX_ASSERT(false);
    return NULL;
}

int mkostemps(char *template, int suffixlen, int flags)
{
    VSF_LINUX_ASSERT(false);
    return -1;
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
int putenv(char *string)
{
    const char * str = strchr(string, '=');
    bool is_to_set = str != NULL;
    size_t namelen = is_to_set ? str - string : strlen(string);
    size_t size = 0;
    bool is_match = false;

    char **env = environ, **env_removed = NULL;
    if (env != NULL) {
        for (char **env_tmp = env; *env_tmp != NULL; env_tmp++, size++) {
            if (    !strncmp(string, *env_tmp, namelen)
                &&  ((*env_tmp)[namelen] == '=')) {
                is_match = true;
                free(*env_tmp);
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
        environ = env = (char **)realloc(env, (size + 2) * sizeof(char *));
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

char * getenv(const char *name)
{
    size_t namelen = strlen(name);
    char **env = environ;

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

int setenv(const char *name, const char *value, int replace)
{
    size_t namelen = strlen(name), valuelen = strlen(value);
    if (!replace) {
        if (getenv(name) != NULL) {
            return 0;
        }
    }

    char *env_str = malloc(namelen + valuelen + 2);
    if (NULL == env_str) {
        return -1;
    }
    memcpy(env_str, name, namelen);
    env_str[namelen] = '=';
    memcpy(env_str + namelen + 1, value, valuelen);
    env_str[namelen + valuelen + 1] = '\0';
    return putenv(env_str);
}

int unsetenv(const char *name)
{
    return putenv((char *)name);
}
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDLIB
