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

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/sys/types.h"
#   include "../../include/simple_libc/stdio.h"
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <stdio.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "../../include/simple_libc/string.h"
#else
#   include <string.h>
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_CFG_PRINT_BUFF_SIZE
#   define VSF_LINUX_CFG_PRINT_BUFF_SIZE        256
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

static int stdin_fd = STDIN_FILENO, stdout_fd = STDOUT_FILENO, stderr_fd = STDERR_FILENO;
FILE *stdin = (FILE *)&stdin_fd, *stdout = (FILE *)&stdout_fd, *stderr = (FILE *)&stderr_fd;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern const vsf_linux_fd_op_t __vsf_linux_fs_fdop;

/*============================ IMPLEMENTATION ================================*/

static int __get_fd(FILE *f)
{
    if (stdin == f) {
        return STDIN_FILENO;
    } else if (stdout == f) {
        return STDOUT_FILENO;
    } else if (stderr == f) {
        return STDERR_FILENO;
    } else {
        vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
        VSF_LINUX_ASSERT(&__vsf_linux_fs_fdop == sfd->op);
        return sfd->fd;
    }
}

int getchar(void)
{
    int ch = EOF;
    fread(&ch, 1, 1, stdin);
    return ch;
}

FILE * fopen(const char *filename, const char *mode)
{
    int fd = open(filename, 0);
    if (fd < 0) {
        return NULL;
    }
    return (FILE *)vsf_linux_get_fd(fd);
}

int fclose(FILE *f)
{
    int fd = __get_fd(f);
    if (fd < 0) {
        return EOF;
    }

    int err = close(fd);
    return err != 0 ? EOF : 0;
}

int fseek(FILE *f, long offset, int fromwhere)
{
    int fd = __get_fd(f);
    if (fd < 0) {
        return -1;
    }
    return lseek(fd, offset, fromwhere);
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

long ftell(FILE *f)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    VSF_LINUX_ASSERT(&__vsf_linux_fs_fdop == sfd->op);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;

    return (long)priv->pos;
}

void rewind(FILE *f)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    VSF_LINUX_ASSERT(&__vsf_linux_fs_fdop == sfd->op);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;

    priv->pos = 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f)
{
    ssize_t ret;
    int fd = __get_fd(f);
    if (fd < 0) {
        return EOF;
    }

    ret = write(fd, (void *)ptr, size * nmemb);
    if (ret < 0) {
        ret = 0;
    }
    return (size_t)ret;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f)
{
    ssize_t ret;
    int fd = __get_fd(f);
    if (fd < 0) {
        return EOF;
    }

    ret = read(fd, (void *)ptr, size * nmemb);
    if (ret < 0) {
        ret = 0;
    }
    return (size_t)ret;
}

char * fgets(char *str, int n, FILE *f)
{
    char *result = str;
    int rsize = 0;

    while (rsize < n - 1) {
        if (fread(str, 1, 1, f) != 1) {
            break;
        }
        rsize++;
        str++;
        if ('\n' == str[-1]) {
            break;
        }
    }
    str[0] = '\0';
    return rsize > 0 ? result : NULL;
}

// insecure
char * gets(char *str)
{
    VSF_LINUX_ASSERT(false);
    return NULL;
}

int fputs(const char *str, FILE *f)
{
    return fwrite(str, strlen(str), 1, f);
}

int puts(const char *str)
{
    return fputs(str, stdout);
}

static int __print_arg(FILE *f, const char *format, va_list ap)
{
    char buff[VSF_LINUX_CFG_PRINT_BUFF_SIZE];
    int size = vsnprintf(buff, sizeof(buff), format, ap);
    return fwrite(buff, size, 1, f);
}

int printf(const char *format, ...)
{
    int size;
    va_list ap;
    va_start(ap, format);
        size = __print_arg(stdout, format, ap);
    va_end(ap);
    return size;
}

int fprintf(FILE *file, const char *format, ...)
{
    int size;
    va_list ap;
    va_start(ap, format);
        size = __print_arg(file, format, ap);
    va_end(ap);
    return size;
}

#if !__IS_COMPILER_IAR__
void perror(const char *str)
{
    extern int errno;
    fprintf(stderr, "%s: %s\r\n", str, strerror(errno));
}
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_STDIO
