/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void * malloc(size_t size)
{
    return vsf_heap_malloc(size);
}

void * aligned_alloc(size_t alignment, size_t size)
{
    return vsf_heap_malloc_aligned(size, alignment);
}

void * realloc(void *p, size_t size)
{
    return vsf_heap_realloc(p, size);
}

void free(void *p)
{
    if (p != NULL) {
        vsf_heap_free(p);
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

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDLIB
