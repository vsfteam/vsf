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

#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
char * ____strdup_ex(vsf_linux_process_t *process, const char *str,
            const char *file, const char *func, int line
)
{
    if (str != NULL) {
        char *newstr = ____malloc_ex(process, strlen(str) + 1, file, func, line);
        if (newstr != NULL) {
            strcpy(newstr, str);
        }
        return newstr;
    }
    return NULL;
}
#endif

void * mempcpy(void *dest, const void *src, size_t len)
{
    return (void *)((char *)memcpy(dest, src, len) + len);
}

ssize_t strscpy(char *dest, const char *src, size_t n)
{
    ssize_t srclen = strlen(src);
    ssize_t copylen = srclen >= n ? n - 1 : srclen;
    memcpy(dest, src, copylen);
    dest[copylen] = '\0';
    return copylen >= srclen ? copylen : -E2BIG;
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

void * memrchr(const void *str, int ch, size_t len)
{
    const char *end = (const char *)str + len - 1;

    if (len == 0) {
        return NULL;
    }

    while (*end != ch) {
        if (end == (const char *)str) {
            return NULL;
        }
        --end;
    }

    return (void *)end;
}

char * strchrnul(const char *s, int c)
{
    while (*s != '\0' && *s != c) {
        s++;
    }
    return (char *)s;
}

char * stpcpy(char *dest, const char *src)
{
    size_t len = strlen(src);
    return (char *)memcpy(dest, src, len + 1) + len;
}

char *stpncpy(char *dest, const char *src, size_t n)
{
    size_t size = strnlen(src, n);
    memcpy(dest, src, size);
    dest += size;
    if (size == n) {
        return dest;
    }
    return memset(dest, '\0', n - size);
}

char * strerror(int errnum)
{
    return "unknown";
}

int strerror_r(int errnum, char *buf, size_t buflen)
{
    strncpy(buf, strerror(errnum), buflen);
    return 0;
}

int ffs(int i)
{
    return (int)vsf_ffs32((uint_fast32_t)i) + 1;
}

VSF_CAL_WEAK(strverscmp)
int strverscmp(const char *str1, const char *str2)
{
    return 0;
}

char * strsignal(int sig)
{
    return "unknown";
}
const char * sigdescr_np(int sig)
{
    return "unknown";
}
const char * sigabbrev_np(int sig)
{
    return "unknown";
}

#if     !(VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED)                  \
    ||  !(VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED)
// strdup will be wrappered to ____strdup_ex if condition not met
char * strdup(const char *str)
{
    return __strdup_ex(NULL, str);
}
#endif

#if VSF_LINUX_APPLET_USE_LIBC_STRING == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_libc_string_vplt_t vsf_linux_libc_string_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_libc_string_vplt_t, 0, 0, true),

#if     VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
    VSF_APPLET_VPLT_ENTRY_FUNC(____strdup_ex),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(ffs),
    VSF_APPLET_VPLT_ENTRY_FUNC(memset),
    VSF_APPLET_VPLT_ENTRY_FUNC(strlen),
    VSF_APPLET_VPLT_ENTRY_FUNC(strnlen),
    VSF_APPLET_VPLT_ENTRY_FUNC(strcmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(strncmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(strdup),
    VSF_APPLET_VPLT_ENTRY_FUNC(strcpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(strncpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(stpcpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(stpncpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(strcat),
    VSF_APPLET_VPLT_ENTRY_FUNC(strncat),
    VSF_APPLET_VPLT_ENTRY_FUNC(strcoll),
    VSF_APPLET_VPLT_ENTRY_FUNC(strtok),
    VSF_APPLET_VPLT_ENTRY_FUNC(strtok_r),
    VSF_APPLET_VPLT_ENTRY_FUNC(strxfrm),
    VSF_APPLET_VPLT_ENTRY_FUNC(strspn),
    VSF_APPLET_VPLT_ENTRY_FUNC(strsep),
    VSF_APPLET_VPLT_ENTRY_FUNC(strcspn),
    VSF_APPLET_VPLT_ENTRY_FUNC(strpbrk),
    VSF_APPLET_VPLT_ENTRY_FUNC(strerror),
    VSF_APPLET_VPLT_ENTRY_FUNC(strcasecmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(strncasecmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(memcpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(mempcpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(strlcpy),
    VSF_APPLET_VPLT_ENTRY_FUNC(strstr),
    VSF_APPLET_VPLT_ENTRY_FUNC(strcasestr),
    VSF_APPLET_VPLT_ENTRY_FUNC(strchr),
    VSF_APPLET_VPLT_ENTRY_FUNC(strrchr),
    VSF_APPLET_VPLT_ENTRY_FUNC(strchrnul),
    VSF_APPLET_VPLT_ENTRY_FUNC(memmove),
    VSF_APPLET_VPLT_ENTRY_FUNC(memcmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(memchr),
    VSF_APPLET_VPLT_ENTRY_FUNC(memrchr),
    VSF_APPLET_VPLT_ENTRY_FUNC(strverscmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(strndup),
    VSF_APPLET_VPLT_ENTRY_FUNC(strsignal),
    VSF_APPLET_VPLT_ENTRY_FUNC(sigdescr_np),
    VSF_APPLET_VPLT_ENTRY_FUNC(sigabbrev_np),
};
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STRING
