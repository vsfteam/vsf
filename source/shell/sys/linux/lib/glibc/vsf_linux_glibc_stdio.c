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

void setbuf(FILE *f, char *buf)
{
    // TODO: add implementation
}

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
    // TODO: add implementation
    return 0;
}

void setbuffer(FILE *f, char *buf, size_t size)
{
}

void setlinebuf(FILE *f)
{
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

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

int fseeko64(FILE *f, off64_t offset, int fromwhere)
{
    int fd = ((vsf_linux_fd_t *)f)->fd;
    if (fd < 0) {
        return -1;
    }
    lseek64(fd, offset, fromwhere);
    return 0;
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

off64_t ftello64(FILE *f)
{
    int fd = ((vsf_linux_fd_t *)f)->fd;
    if (fd < 0) {
        return -1;
    }

    return lseek64(fd, 0, SEEK_CUR);
}

off_t ftello(FILE *f)
{
    return (off_t)ftello64(f);
}

long ftell(FILE *f)
{
    return (long)ftello(f);
}

void rewind(FILE *f)
{
    int fd = ((vsf_linux_fd_t *)f)->fd;
    lseek(fd, 0, SEEK_SET);
}

int fgetpos64(FILE *f, fpos_t *pos)
{
    off64_t off64 = ftello64(f);
    if (pos != NULL) {
        *pos = off64;
    }
    return 0;
}

int fsetpos64(FILE *f, const fpos_t *pos)
{
    fseeko64(f, *pos, SEEK_SET);
    return 0;
}

int fgetpos(FILE *f, fpos_t *pos)
{
    off_t off = ftell(f);
    if (pos != NULL) {
        *pos = off;
    }
    return 0;
}

int fsetpos(FILE *f, const fpos_t *pos)
{
    fseek(f, *pos, SEEK_SET);
    return 0;
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
    int fd = ((vsf_linux_fd_t *)f)->fd;
    char ch, *result = str, *cur = NULL;
    int rsize = 0;
    bool is_tty = isatty(fd), is_to_echo = false;
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    vsf_linux_fd_priv_t *priv = sfd->priv;
    vsf_linux_term_priv_t *term_priv = NULL;

    if (is_tty) {
        term_priv = (vsf_linux_term_priv_t *)priv;
        is_to_echo = !(priv->flags & O_NOCTTY) && (term_priv->termios.c_lflag & ECHO);
    }

    while (true) {
        if (term_priv != NULL) {
            term_priv->line_start = !rsize;
        }
        if (read(fd, &ch, 1) != 1) {
            break;
        }
        if (rsize < n - 1) {
            *str = ch;
            cur = str;
        }

        if (is_tty) {
            // some terminal uses \r as enter, if echo is enabled,
            //  add and \n, so the output will be \r\n
            if (('\r' == ch) && is_to_echo) {
               write(STDOUT_FILENO, "\n", 1);
            }
            if (('\b' == ch) || (0x7F == ch)) {
                int back_cnt = vsf_min(rsize, 1);
                rsize -= back_cnt;
                str -= back_cnt;
                continue;
            }
        }

        rsize++;
        str++;
        if ('\r' == ch) {
            str[-1] = '\n';

            // check next possible '\n'
            int old_flags = sfd->priv->flags;
            priv->flags |= O_NONBLOCK;
            ssize_t len = read(fd, &ch, 1);
            priv->flags = old_flags;

            if (len != 1) {
                break;
            }
            if (ch != '\n') {
                sfd->unget_buff = ch;
            }
            break;
        } else if ('\n' == ch) {
            break;
        }
    }
    if (cur != NULL) {
        cur[1] = '\0';
    }
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
    return fprintf(f, "%s", str);
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

int vasprintf(char **strp, const char *format, va_list ap)
{
    int size = vsnprintf(NULL, 0, format, ap);
    char *buff = malloc(size + 1);
    if (NULL == buff) {
        return -1;
    }

    if (strp != NULL) {
        *strp = buff;
    }
    return vsnprintf(buff, size + 1, format, ap);
}

int asprintf(char **strp, const char *format, ...)
{
    int result;
    va_list ap;
    va_start(ap, format);
        result = vasprintf(strp, format, ap);
    va_end(ap);
    return result;
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

int vdprintf(int fd, const char *format, va_list ap)
{
    FILE *f = (FILE *)vsf_linux_fd_get(fd);
    if (NULL == f) {
        return -1;
    }
    return vfprintf(f, format, ap);
}

int dprintf(int fd, const char *format, ...)
{
    int size;
    va_list ap;
    va_start(ap, format);
        size = vdprintf(fd, format, ap);
    va_end(ap);
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

int fiprintf(FILE *f, const char *format, ...)
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
    char ch, *ptr;
    char *strtmp;
    int result = 0, ch_tmp;
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

int vscanf(const char *format, va_list ap)
{
    return vfscanf(stdin, format, ap);
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

int scanf(const char *format, ...)
{
    int result;
    va_list ap;
    va_start(ap, format);
        result = vscanf(format, ap);
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

void perror(const char *str)
{
    fprintf(stderr, "%s: %s\r\n", str, strerror(errno));
}

FILE * popen(const char *command, const char *type)
{
    if ((NULL == type) || ((type[0] != 'r') && (type[0] != 'w'))) {
        return NULL;
    }

    vsf_linux_process_t *process = vsf_linux_create_process(0, VSF_LINUX_CFG_PEOCESS_HEAP_SIZE, 0);
    if (NULL == process) {
        return NULL;
    }

    const char *argv[] = { "sh", "-c", command, (const char *)NULL };
    vsf_linux_process_ctx_t *ctx;
    vsf_linux_process_t *cur_process;
    vsf_linux_main_entry_t entry;
    extern int __vsf_linux_get_exe(char *pathname, int pathname_len, char *cmd, vsf_linux_main_entry_t *entry, bool use_path);
#if __VSF_LINUX_PROCESS_HAS_PATH
    if (__vsf_linux_get_exe(process->path, sizeof(process->path), (char *)command, &entry, true) < 0) {
#else
    if (__vsf_linux_get_exe(NULL, 0, (char *)command, &entry, true) < 0) {
#endif
        goto delete_process_and_fail;
    }

    int fd[2];
    if (pipe(fd) < 0) {
        goto delete_process_and_fail;
    }

    // avoid to use spawn for better performance, skip unnecessary fd operations
    ctx = &process->ctx;
    ctx->entry = entry;

    cur_process = vsf_linux_get_cur_process();
    process->shell_process = cur_process->shell_process;

    __vsf_linux_process_parse_arg(process, NULL, (char * const *)argv);

    vsf_linux_fd_t *sfd;
    extern int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
        const vsf_linux_fd_op_t *op, int fd_desired, vsf_linux_fd_priv_t *priv);
    if (type[0] == 'w') {
        sfd = vsf_linux_fd_get(fd[0]);
        __vsf_linux_fd_create_ex(process, NULL, sfd->op, STDIN_FILENO, sfd->priv);
        close(sfd->fd);
        sfd = vsf_linux_fd_get(fd[1]);
    } else {
        sfd = vsf_linux_fd_get(fd[1]);
        __vsf_linux_fd_create_ex(process, NULL, sfd->op, STDOUT_FILENO, sfd->priv);
        close(sfd->fd);
        sfd = vsf_linux_fd_get(fd[0]);
    }

    vsf_linux_start_process(process);
    return (FILE *)sfd;

delete_process_and_fail:
    vsf_linux_delete_process(process);
    return NULL;
}

int pclose(FILE *stream)
{
    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)stream;
    int result;
    waitpid(sfd->binding.pid, &result, 0);
    close(sfd->fd);
    return result;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

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

static ssize_t __getdelim(char **lineptr, size_t *n, int delimiter, FILE *f)
{
    if ((NULL == lineptr) || (NULL == n)) {
        return -1;
    }

    if ((NULL == *lineptr) || (0 == *n)) {
        *n = 256;
        *lineptr = (char *)malloc(*n);
        if (NULL == *lineptr) {
            return -1;
        }
    }

    ssize_t cur_len = 0;
    char *cur_pos = *lineptr;
    while (true) {
        if (cur_len >= *n - 1) {
            char *new_lineptr = (char *)realloc(*lineptr, 2 * *n);
            if (NULL == new_lineptr) {
                return -1;
            }
            *lineptr = new_lineptr;
            *n = 2 * *n;
            cur_pos = &((*lineptr)[cur_len]);
        }
        if (fread(cur_pos, 1, 1, f) != 1) {
            break;
        }

        cur_len++;
        if (*cur_pos++ == delimiter) {
            break;
        }
    }
    *cur_pos = '\0';
    return 0 == cur_len ? -1 : cur_len;
}

ssize_t getline(char **lineptr, size_t *n, FILE *f)
{
    return __getdelim(lineptr, n, '\n', f);
}

#ifdef __WIN__
int _fseeki64(FILE *f, uint64_t offset, int origin)
{
    return fseeko64(f, offset, origin);
}

void _lock_file(FILE *f)
{
}

void _unlock_file(FILE *f)
{
}

errno_t _get_stream_buffer_pointers(FILE *f, char ***base, char ***ptr, int **cnt)
{
    static int __cnt = 0;
    static char *__base = (char *)&__cnt;
    if (base != NULL) {
        *base = &__base;
    }
    if (ptr != NULL) {
        *ptr = &__base;
    }
    if (cnt != NULL) {
        *cnt = &__cnt;
    }
    return 0;
}
#endif

#if VSF_LINUX_APPLET_USE_LIBC_STDIO == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_libc_stdio_vplt_t vsf_linux_libc_stdio_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_libc_stdio_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(__vsf_linux_stdin),
    VSF_APPLET_VPLT_ENTRY_FUNC(__vsf_linux_stdout),
    VSF_APPLET_VPLT_ENTRY_FUNC(__vsf_linux_stderr),
    VSF_APPLET_VPLT_ENTRY_FUNC(perror),
    VSF_APPLET_VPLT_ENTRY_FUNC(putchar),
    VSF_APPLET_VPLT_ENTRY_FUNC(getchar),
    VSF_APPLET_VPLT_ENTRY_FUNC(fgetc),
    VSF_APPLET_VPLT_ENTRY_FUNC(fputc),
    VSF_APPLET_VPLT_ENTRY_FUNC(getc),
    VSF_APPLET_VPLT_ENTRY_FUNC(putc),
    VSF_APPLET_VPLT_ENTRY_FUNC(ungetc),
    VSF_APPLET_VPLT_ENTRY_FUNC(puts),
    VSF_APPLET_VPLT_ENTRY_FUNC(fputs),
    VSF_APPLET_VPLT_ENTRY_FUNC(fgets),
    VSF_APPLET_VPLT_ENTRY_FUNC(scanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vscanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fscanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vfscanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(printf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vfprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(dprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vdprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fopen),
    VSF_APPLET_VPLT_ENTRY_FUNC(fdopen),
    VSF_APPLET_VPLT_ENTRY_FUNC(freopen),
    VSF_APPLET_VPLT_ENTRY_FUNC(fclose),
    VSF_APPLET_VPLT_ENTRY_FUNC(fileno),
    VSF_APPLET_VPLT_ENTRY_FUNC(fseek),
    VSF_APPLET_VPLT_ENTRY_FUNC(fseeko),
    VSF_APPLET_VPLT_ENTRY_FUNC(fseeko64),
    VSF_APPLET_VPLT_ENTRY_FUNC(ftell),
    VSF_APPLET_VPLT_ENTRY_FUNC(ftello),
    VSF_APPLET_VPLT_ENTRY_FUNC(ftello64),
    VSF_APPLET_VPLT_ENTRY_FUNC(rewind),
    VSF_APPLET_VPLT_ENTRY_FUNC(fgetpos),
    VSF_APPLET_VPLT_ENTRY_FUNC(fsetpos),
    VSF_APPLET_VPLT_ENTRY_FUNC(fgetpos64),
    VSF_APPLET_VPLT_ENTRY_FUNC(fsetpos64),
    VSF_APPLET_VPLT_ENTRY_FUNC(fread),
    VSF_APPLET_VPLT_ENTRY_FUNC(fwrite),
    VSF_APPLET_VPLT_ENTRY_FUNC(fflush),
    VSF_APPLET_VPLT_ENTRY_FUNC(popen),
    VSF_APPLET_VPLT_ENTRY_FUNC(pclose),
    VSF_APPLET_VPLT_ENTRY_FUNC(rename),
    VSF_APPLET_VPLT_ENTRY_FUNC(renameat),
    VSF_APPLET_VPLT_ENTRY_FUNC(ferror),
    VSF_APPLET_VPLT_ENTRY_FUNC(clearerr),
    VSF_APPLET_VPLT_ENTRY_FUNC(feof),
    VSF_APPLET_VPLT_ENTRY_FUNC(sprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vsprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(snprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vsnprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(asprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vasprintf),
    VSF_APPLET_VPLT_ENTRY_FUNC(vsscanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(sscanf),
    VSF_APPLET_VPLT_ENTRY_FUNC(remove),
    VSF_APPLET_VPLT_ENTRY_FUNC(setbuf),
    VSF_APPLET_VPLT_ENTRY_FUNC(setbuffer),
    VSF_APPLET_VPLT_ENTRY_FUNC(setlinebuf),
    VSF_APPLET_VPLT_ENTRY_FUNC(setvbuf),
    VSF_APPLET_VPLT_ENTRY_FUNC(tmpfile),
    VSF_APPLET_VPLT_ENTRY_FUNC(tmpnam),
    VSF_APPLET_VPLT_ENTRY_FUNC(getline),
};
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDIO
