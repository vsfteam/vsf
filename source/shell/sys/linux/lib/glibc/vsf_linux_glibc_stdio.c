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

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define _GNU_SOURCE
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/sys/types.h"
#   include "../../include/sys/wait.h"      // for waitpid
#   include "../../include/simple_libc/stdio.h"
#   include "../../include/errno.h"
#   include "../../include/fcntl.h"
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <sys/wait.h>                    // for waitpid
#   include <stdio.h>
#   include <errno.h>
#   include <fcntl.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "../../include/simple_libc/string.h"
#else
#   include <string.h>
#endif

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_CFG_PRINT_BUFF_SIZE
#   define VSF_LINUX_CFG_PRINT_BUFF_SIZE        256
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern const vsf_linux_fd_op_t __vsf_linux_fs_fdop;

/*============================ IMPLEMENTATION ================================*/

FILE * __vsf_linux_stdin(void)
{
    return (FILE *)vsf_linux_fd_get(STDIN_FILENO);
}

FILE * __vsf_linux_stdout(void)
{
    return (FILE *)vsf_linux_fd_get(STDOUT_FILENO);
}

FILE * __vsf_linux_stderr(void)
{
    return (FILE *)vsf_linux_fd_get(STDERR_FILENO);
}

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
    // TODO: add implementation
    return 0;
}

int ungetc(int ch, FILE *f)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    if (sfd->unget_buff != EOF) {
        return EOF;
    }

    sfd->unget_buff = ch;
    return ch;
}

int getc(FILE *f)
{
    int ch = 0;
    if (1 != fread(&ch, 1, 1, f)) {
        return EOF;
    }
    return ch;
}

int getchar(void)
{
    return getc(stdin);
}

FILE * fopen(const char *filename, const char *mode)
{
    int flags = 0;
    if (mode != NULL) {
        char operate = *mode++;
        if ((*mode == 'b') || (*mode == 't')) {
            mode++;
        }
        switch (operate) {
        case 'a':
            flags = O_WRONLY | O_CREAT | O_APPEND;
            break;
        case 'r':
            flags = O_RDONLY;
            break;
        case 'w':
            flags = O_WRONLY | O_CREAT | O_TRUNC;
            break;
        }
        if (*mode == '+') {
            flags &= ~(O_RDONLY | O_WRONLY);
            flags |= O_RDWR;
        }
    }

    int fd = open(filename, flags);
    if (fd < 0) {
        return NULL;
    }
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    return (FILE *)sfd;
}

FILE * fdopen(int fd, const char *mode)
{
    return (FILE *)vsf_linux_fd_get(fd);
}

FILE * freopen(const char *filename, const char *mode, FILE *f)
{
    return f;
}

int fclose(FILE *f)
{
    if (NULL == f) {
        return -1;
    }

    int fd = ((vsf_linux_fd_t *)f)->fd;
    if (fd < 0) {
        return EOF;
    }

    int err = close(fd);
    return err != 0 ? EOF : 0;
}

int fileno(FILE *stream)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)stream;
    return sfd->fd;
}

int fseeko(FILE *f, off_t offset, int fromwhere)
{
    int fd = ((vsf_linux_fd_t *)f)->fd;
    if (fd < 0) {
        return -1;
    }
    lseek(fd, offset, fromwhere);
    return 0;
}

int fseek(FILE *f, long offset, int fromwhere)
{
    return fseeko(f, offset, fromwhere);
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

off64_t ftello64(FILE *f)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    VSF_LINUX_ASSERT(&__vsf_linux_fs_fdop == sfd->op);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;

    return vk_file_tell(priv->file);
}

void rewind(FILE *f)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    VSF_LINUX_ASSERT(&__vsf_linux_fs_fdop == sfd->op);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;

    vk_file_seek(priv->file, 0, VSF_FILE_SEEK_SET);
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

off_t ftello(FILE *f)
{
    return (off_t)ftello64(f);
}

long ftell(FILE *f)
{
    return (long)ftello(f);
}

int fgetpos(FILE *f, fpos_t *pos)
{
    if (pos != NULL) {
        *pos = ftell(f);
    }
    return 0;
}

int fsetpos(FILE *f, const fpos_t *pos)
{
    return fseek(f, *pos, SEEK_SET);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f)
{
    ssize_t bytes_requested = size * nmemb;
    ssize_t ret;
    if (0 == bytes_requested) {
        return 0;
    }

    int fd = ((vsf_linux_fd_t *)f)->fd;
    if (fd < 0) {
        return 0;
    }

    ret = write(fd, (void *)ptr, size * nmemb);
    if (ret < 0) {
        return 0;
    }
    return (size_t)(bytes_requested == ret ? nmemb : ret / size);
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    ssize_t bytes_requested = size * nmemb;
    ssize_t ret;
    if (0 == bytes_requested) {
        return 0;
    }

    int fd = sfd->fd;
    if (fd < 0) {
        return 0;
    }

    int pre_read = 0;
    if (sfd->unget_buff != EOF) {
        pre_read = 1;
        *(uint8_t *)ptr = sfd->unget_buff;
        ptr = (uint8_t *)ptr + 1;
        sfd->unget_buff = EOF;
    }

    bytes_requested -= pre_read;
    ret = read(fd, (void *)ptr, bytes_requested);
    if (ret < 0) {
        return 0;
    }

    bytes_requested += pre_read;
    ret += pre_read;
    return (size_t)(bytes_requested == ret ? nmemb : ret / size);
}

int fflush(FILE *f)
{
    return 0;
}

int fgetc(FILE *f)
{
    int ch = 0;
    if (1 != fread(&ch, 1, 1, f)) {
        return EOF;
    }
    return ch;
}

char * fgets(char *str, int n, FILE *f)
{
    char *result = str;
    int rsize = 0;

    while (rsize < n - 1) {
        if (fread(str, 1, 1, f) != 1) {
            break;
        }

        if (stdin == f) {
            if ('\n' == *str) {
                continue;
            }
            if (('\b' == *str) || (0x7F == *str)) {
                int back_cnt = vsf_min(rsize, 1);
                rsize -= back_cnt;
                str -= back_cnt;
                continue;
            }
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

int fputc(int ch, FILE *f)
{
    if (1 != fwrite(&ch, 1, 1, f)) {
        return EOF;
    }
    return ch;
}

int fputs(const char *str, FILE *f)
{
    return fprintf(f, "%s\n", str);
}

int puts(const char *str)
{
    return printf("%s\n", str);
}

int putc(int ch, FILE *f)
{
    if (1 != fwrite(&ch, 1, 1, f)) {
        return EOF;
    }
    return ch;
}

int putchar(int c)
{
    return putc(c, stdout);
}

int vfprintf(FILE *f, const char *format, va_list ap)
{
    char buff[VSF_LINUX_CFG_PRINT_BUFF_SIZE];
    va_list ap_temp;
    int size;

    va_copy(ap_temp, ap);
    size = vsnprintf(buff, sizeof(buff), format, ap);
    if (size >= sizeof(buff)) {
        char *buff = malloc(size + 1);
        if (NULL == buff) {
            return -1;
        }

        size = vsnprintf(buff, size + 1, format, ap_temp);
        if (size > 0) {
            if (size != fwrite(buff, 1, size, f)) {
                size = -1;
            }
        }
        free(buff);
        return size;
    }
    if (size != fwrite(buff, 1, size, f)) {
        return -1;
    }
    return size;
}

int vprintf(const char *format, va_list arg)
{
    return vfprintf(stdout, format, arg);
}

int printf(const char *format, ...)
{
    int size;
    va_list ap;
    va_start(ap, format);
        size = vfprintf(stdout, format, ap);
    va_end(ap);
    return size;
}

int fprintf(FILE *f, const char *format, ...)
{
    int size;
    va_list ap;
    va_start(ap, format);
        size = vfprintf(f, format, ap);
    va_end(ap);
    return size;
}

static bool __is_in_seq(char ch, const char *seq, int seq_len)
{
    for (int i = 0; i < seq_len; i++) {
        if (ch == seq[i]) {
            return true;
        }
    }
    return false;
}

int vfscanf(FILE *f, const char *format, va_list ap)
{
    char ch, ch_tmp, *ptr;
    char *strtmp;
    int result = 0;
    unsigned long long arg_uinteger;

    goto skip_space;
    while (*format != '\0') {
        ch = *format++;
        switch (ch) {
        case '%': {
                int width = 0;
                union {
                    struct {
                        unsigned is_signed      : 1;
                        unsigned is_negative    : 1;
                    };
                    unsigned all;
                } flags;
                int radix;

                int seqlen;
                const char *seq;

                flags.all = 0;
                if ('*' == *format) {
                    // TODO:
                } else {
                    width = strtoull(format, &strtmp, 0);
                    format = strtmp;
                }

                ch = *format++;
                switch (ch) {
                case '[':
                    seq = format;
                    while (true) {
                        ch = *format++;
                        if (!ch) {
                            return -1;
                        }
                        if (ch == ']') {
                            break;
                        }
                    }
                    seqlen = format - seq - 1;

                    ptr = va_arg(ap, char *);
                    if (!width) { width = -1; }

                    while (width > 0) {
                        ch_tmp = fgetc(f); if (ch_tmp == EOF) { break; }
                        if (!__is_in_seq(ch_tmp, seq, seqlen)) {
                            break;
                        }
                        *ptr++ = ch_tmp;
                        width--;
                    }
                    result++;
                    break;
                case 'u':
                    flags.is_signed = 0;
                    radix = 10;
                    goto parse_integer;
                case 'i':
                case 'd':
                    flags.is_signed = 1;
                    radix = 10;
                    goto parse_integer;
                case 'o':
                    flags.is_signed = 0;
                    radix = 8;
                    goto parse_integer;
                case 'x':
                case 'X':
                    flags.is_signed = 0;
                    radix = 16;
                    goto parse_integer;

                parse_integer:
                    if (flags.is_signed) {
                        ch_tmp = fgetc(f); if (ch_tmp == EOF) { goto end; }
                        if (ch_tmp == '-') {
                            flags.is_negative = true;
                        } else {
                            flags.is_negative = false;
                            if (ch_tmp != '+') {
                                ungetc(ch_tmp, f);
                            }
                        }
                    }

                    arg_uinteger = 0;
                    if (!width) { width = -1; }
                    while ((width < 0) || (width-- > 0)) {
                        ch_tmp = fgetc(f); if (ch_tmp == EOF) { break; }
                        ch_tmp = tolower(ch_tmp);
                        if (ch_tmp >= '0' && ch_tmp <= '9') {
                            ch_tmp -= '0';
                        } else if (ch_tmp >= 'a' && ch_tmp <= 'f') {
                            ch_tmp -= 'a';
                            ch_tmp += 10;
                        } else {
                            ungetc(ch_tmp, f);
                            break;
                        }
                        if (ch_tmp >= radix) {
                            ungetc(ch_tmp, f);
                            break;
                        }

                        arg_uinteger *= radix;
                        arg_uinteger += ch_tmp;
                    }

                    if (flags.is_negative) {
                        *va_arg(ap, int *) = (int)-arg_uinteger;
                    } else {
                        *va_arg(ap, unsigned int *) = (unsigned int)arg_uinteger;
                    }
                    result++;
                    break;
                case 'f':
                    VSF_LINUX_ASSERT(false);
                    return -1;
                case 'c':
                    ch_tmp = fgetc(f); if (ch_tmp == EOF) { goto end; }
                    *va_arg(ap, char *) = ch_tmp;
                    result++;
                    break;
                case 's': {
                        char *ptr = va_arg(ap, char *);
                        if (width) { width--; /* reserved for '\0' */ }
                        else { width = -1; }

                        while ((width < 0) || (width-- > 0)) {
                            ch_tmp = fgetc(f); if (ch_tmp == EOF) { break; }
                            if (!isspace(ch_tmp)) {
                                *ptr++ = ch_tmp;
                            } else {
                                ungetc(ch_tmp, f);
                                break;
                            }
                        }
                        *ptr = '\0';
                        result++;
                    }
                    break;
                }
            }
            break;
        case ' ':
        skip_space:
            while (true) {
                ch_tmp = fgetc(f); if (ch_tmp == EOF) { goto end; }
                if (!isspace(ch_tmp)) {
                    ungetc(ch_tmp, f);
                    break;
                }
            }
            break;
        default:
            ch_tmp = fgetc(f); if (ch_tmp == EOF) { goto end; }
            if (ch_tmp != ch) {
                goto end;
            }
            break;
        }
    }
end:
    return result ? result : EOF;
}

int fscanf(FILE *f, const char *format, ...)
{
    int result;
    va_list ap;
    va_start(ap, format);
        result = vfscanf(f, format, ap);
    va_end(ap);
    return result;
}

int ferror(FILE *f)
{
    return 0;
}

void clearerr(FILE *f)
{
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

int feof(FILE *f)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)f;
    if (sfd->op->fn_eof != NULL) {
        return sfd->op->fn_eof(sfd);
    }
    return 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if !__IS_COMPILER_IAR__
void perror(const char *str)
{
    fprintf(stderr, "%s: %s\r\n", str, strerror(errno));
}
#endif

FILE * popen(const char *command, const char *type)
{
    if ((NULL == type) || ((type[0] != 'r') && (type[0] != 'w'))) {
        return NULL;
    }

    extern vsf_linux_process_t * __vsh_prepare_process(char *cmd, int fd_in, int fd_out);
    vsf_linux_process_t *process = NULL;
    vsf_linux_fd_t *sfd;
    int fd[2];
    pipe(fd);

    if (type[0] == 'w') {
        process = __vsh_prepare_process((char *)command, fd[0], -1);
        close(fd[0]);
        sfd = vsf_linux_fd_get(fd[1]);
    } else if (type[0] == 'r') {
        process = __vsh_prepare_process((char *)command, -1, fd[1]);
        close(fd[1]);
        sfd = vsf_linux_fd_get(fd[0]);
    } else {
        VSF_LINUX_ASSERT(false);
        return NULL;
    }

    if (NULL == process) {
        close(sfd->fd);
        return NULL;
    }
    sfd->binding.pid = process->id.pid;
    vsf_linux_start_process(process);
    return (FILE *)sfd;
}

int pclose(FILE *stream)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)stream;
    int result;
    waitpid(sfd->binding.pid, &result, 0);
    close(sfd->fd);
    return result;
}

// TODO: implement tmpnam and tmpfile
FILE * tmpfile(void)
{
    VSF_LINUX_ASSERT(false);
    return NULL;
}

char * tmpnam(char *str)
{
    VSF_LINUX_ASSERT(false);
    return NULL;
}

int rename(const char *old_filename, const char *new_filename)
{
    if (!strcmp(old_filename, "/")) {
        return -1;
    }
    extern int __vsf_linux_fs_rename(const char *pathname, const char *toname);
    return __vsf_linux_fs_rename(old_filename, new_filename);
}

int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath)
{
    extern vk_file_t * __vsf_linux_get_fs_ex(vsf_linux_process_t *process, int fd);
    vk_file_t *olddir = __vsf_linux_get_fs_ex(NULL, olddirfd);
    vk_file_t *newdir = __vsf_linux_get_fs_ex(NULL, newdirfd);
    if (    !(olddir->attr & VSF_FILE_ATTR_DIRECTORY)
        ||  !(newdir->attr & VSF_FILE_ATTR_DIRECTORY)) {
        errno = ENOTDIR;
        return -1;
    }

    vk_file_rename(olddir, oldpath, newdir, newpath);
    if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
        return -1;
    }
    return 0;
}

int remove(const char * pathname)
{
    int fd = open(pathname, 0);
    if (fd < 0) {
        return -1;
    }
    close(fd);

    if (    (unlink(pathname) != 0)
        &&  (rmdir(pathname) != 0)) {
        return -1;
    }
    return 0;
}

ssize_t getline(char **lineptr, size_t *n, FILE *f)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDIO
