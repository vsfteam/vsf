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

#if VSF_USE_LINUX == ENABLED

#define VSF_LINUX_INHERIT

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_CFG_PRINT_BUFF_SIZE
#   define VSF_LINUX_CFG_PRINT_BUFF_SIZE        256
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

int stdin_fd = 0, stdout_fd = 1, stderr_fd = 2;
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
    return vsf_linux_get_fd(fd);
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
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    VSF_LINUX_ASSERT(&__vsf_linux_fs_fdop == sfd->op);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    uint64_t new_pos;

    switch (fromwhere) {
    case SEEK_SET:
        new_pos = 0;
        break;
    case SEEK_CUR:
        new_pos = priv->pos;
        break;
    case SEEK_END:
        new_pos = priv->file->size;
        break;
    default:
        VSF_LINUX_ASSERT(false);
        return -1;
    }

    new_pos += offset;
    if (new_pos > priv->file->size) {
        return -1;
    }
    priv->pos = new_pos;
    return 0;
}

long ftell(FILE *f)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    VSF_LINUX_ASSERT(&__vsf_linux_fs_fdop == sfd->op);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;

    return (long)priv->pos;
}

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

size_t fread(const void *ptr, size_t size, size_t nmemb, FILE *f)
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

#endif      // VSF_USE_LINUX
