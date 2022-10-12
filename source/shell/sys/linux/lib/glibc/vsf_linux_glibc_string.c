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

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/errno.h"
#   include "../../include/simple_libc/string.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <string.h>
#endif

// for malloc
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED
#   include "../../include/simple_libc/stdlib.h"
#else
#   include <stdlib.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
char * __strdup_ex(vsf_linux_process_t *process, const char *str)
{
    if (str != NULL) {
        char *newstr = __malloc_ex(process, strlen(str) + 1);
        if (newstr != NULL) {
            strcpy(newstr, str);
        }
        return newstr;
    }
    return NULL;
}
#endif

char * strdup(const char *str)
{
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    return __strdup_ex(NULL, str);
#else
    char *result = vsf_heap_strdup(str);
    if (NULL == result) {
        errno = ENOMEM;
    }
    return result;
#endif
}

char * strndup(const char *str, size_t n)
{
    const char *p;
    char *r;

    if (NULL == str) {
        return NULL;
    }

    p = str;
    while (n > 0) {
        if (*p == 0) {
            break;
        }
        p++;
        n--;
    }
    n = (p - str);
    r = __malloc_ex(NULL, 1 + n);
    if (r != NULL) {
        memcpy(r, str, n);
        r[n] = '\0';
    }
    return r;
}

char * strerror(int errnum)
{
    return "unknown";
}

int ffs(int i)
{
    return (int)vsf_ffs32((uint_fast32_t)i);
}

WEAK(strverscmp)
int strverscmp(const char *str1, const char *str2)
{
    return 0;
}

#if VSF_LINUX_APPLET_USE_LIBC_STRING == ENABLED && !defined(__VSF_APPLET__)
#   define VSF_LINUX_APPLET_LIBC_STRING_FUNC(__FUNC)        .__FUNC = __FUNC
__VSF_VPLT_DECORATOR__ vsf_linux_libc_string_vplt_t vsf_linux_libc_string_vplt = {
    .info.entry_num = (sizeof(vsf_linux_libc_string_vplt_t) - sizeof(vsf_vplt_info_t)) / sizeof(void *),

    VSF_LINUX_APPLET_LIBC_STRING_FUNC(ffs),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(memset),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strlen),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strnlen),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strcmp),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strncmp),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strdup),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strcpy),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strncpy),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strcat),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strncat),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strcoll),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strtok),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strxfrm),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strspn),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strcspn),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strpbrk),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strerror),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strcasecmp),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strncasecmp),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(memcpy),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strlcpy),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strstr),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strchr),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(strrchr),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(memmove),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(memcmp),
    VSF_LINUX_APPLET_LIBC_STRING_FUNC(memchr),
};
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STRING
