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

#include "../../vsf_linux.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_CFG_PRINT_BUFF_SIZE
#   define VSF_LINUX_CFG_PRINT_BUFF_SIZE        256
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

FILE *stdin, *stdout, *stderr;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int getchar(void)
{
    int ch;
    fread(&ch, 1, 1, stdin);
    return ch;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f)
{
    int fd;
    ssize_t ret;

    if (f == stdout) {
        fd = STDOUT_FILENO;
    } else if (f == stderr) {
        fd = STDERR_FILENO;
    } else {
//        fd = ;
    }
    ret = write(fd, (void *)ptr, size * nmemb);
    if (ret < 0) {
        ret = 0;
    }
    return (size_t)ret;
}

size_t fread(const void *ptr, size_t size, size_t nmemb, FILE *f)
{
    int fd;
    ssize_t ret;

    if (f == stdin) {
        fd = STDIN_FILENO;
    } else {
//        fd = ;
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

void perror(const char *str)
{
    extern int errno;
    fprintf(stderr, "%s: %s\r\n", str, strerror(errno));
}

#endif      // VSF_USE_LINUX
