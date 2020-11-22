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

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED

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

#ifndef __WIN__
void * malloc(size_t size)
{
    return vsf_heap_malloc(size);
}

void * realloc(void *p, size_t size)
{
    if (NULL == p) {
        return vsf_heap_malloc(size);
    }
    if (0 == size) {
        if (p != NULL) {
            vsf_heap_free(p);
        }
        return NULL;
    }
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

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_STDLIB
