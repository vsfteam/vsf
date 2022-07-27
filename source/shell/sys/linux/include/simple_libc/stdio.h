#ifndef __SIMPLE_LIBC_STDIO_H__
#define __SIMPLE_LIBC_STDIO_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif

#include <stdarg.h>
#include <stdint.h>

//! \note libc belongs to compiler layer in utilities, so only include compiler.h
#include "utilities/compiler/compiler.h"

#if     __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#   pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#   pragma clang diagnostic ignored "-Winconsistent-dllimport"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// in cstdio, common macro overrides will be undefined
#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#define ferror              VSF_LINUX_LIBC_WRAPPER(ferror)
#define clearerr            VSF_LINUX_LIBC_WRAPPER(clearerr)
#define getc                VSF_LINUX_LIBC_WRAPPER(getc)
#define getchar             VSF_LINUX_LIBC_WRAPPER(getchar)
#define putc                VSF_LINUX_LIBC_WRAPPER(putc)
#define putchar             VSF_LINUX_LIBC_WRAPPER(putchar)
#define ungetc              VSF_LINUX_LIBC_WRAPPER(ungetc)
#define feof                VSF_LINUX_LIBC_WRAPPER(feof)


#define fopen               VSF_LINUX_LIBC_WRAPPER(fopen)
#define fdopen              VSF_LINUX_LIBC_WRAPPER(fdopen)
#define freopen             VSF_LINUX_LIBC_WRAPPER(freopen)
#define fclose              VSF_LINUX_LIBC_WRAPPER(fclose)
#define fileno              VSF_LINUX_LIBC_WRAPPER(fileno)
#define fseek               VSF_LINUX_LIBC_WRAPPER(fseek)
#define fseeko              VSF_LINUX_LIBC_WRAPPER(fseeko)
#define ftell               VSF_LINUX_LIBC_WRAPPER(ftell)
#define ftello              VSF_LINUX_LIBC_WRAPPER(ftello)
#define ftello64            VSF_LINUX_LIBC_WRAPPER(ftello64)
#define rewind              VSF_LINUX_LIBC_WRAPPER(rewind)
#define fgetpos             VSF_LINUX_LIBC_WRAPPER(fgetpos)
#define fsetpos             VSF_LINUX_LIBC_WRAPPER(fsetpos)
#define fwrite              VSF_LINUX_LIBC_WRAPPER(fwrite)
#define fread               VSF_LINUX_LIBC_WRAPPER(fread)
#define fflush              VSF_LINUX_LIBC_WRAPPER(fflush)
#define fgetc               VSF_LINUX_LIBC_WRAPPER(fgetc)
#define fgets               VSF_LINUX_LIBC_WRAPPER(fgets)
#define gets                VSF_LINUX_LIBC_WRAPPER(gets)
#define fputc               VSF_LINUX_LIBC_WRAPPER(fputc)
#define fputs               VSF_LINUX_LIBC_WRAPPER(fputs)
#define puts                VSF_LINUX_LIBC_WRAPPER(puts)
#define printf              VSF_LINUX_LIBC_WRAPPER(printf)
#define vprintf             VSF_LINUX_LIBC_WRAPPER(vprintf)
#define vfprintf            VSF_LINUX_LIBC_WRAPPER(vfprintf)
#define fprintf             VSF_LINUX_LIBC_WRAPPER(fprintf)
#define fscanf              VSF_LINUX_LIBC_WRAPPER(fscanf)
#define vfscanf             VSF_LINUX_LIBC_WRAPPER(vfscanf)
#define perror              VSF_LINUX_LIBC_WRAPPER(perror)
#define setvbuf             VSF_LINUX_LIBC_WRAPPER(setvbuf)
#define rename              VSF_LINUX_LIBC_WRAPPER(rename)
#define renameat            VSF_LINUX_LIBC_WRAPPER(renameat)

#define tmpfile             VSF_LINUX_LIBC_WRAPPER(tmpfile)
#define tmpnam              VSF_LINUX_LIBC_WRAPPER(tmpnam)

#if defined(_GNU_SOURCE)
#define getline             VSF_LINUX_LIBC_WRAPPER(getline)
#endif
#elif defined(__WIN__)
// avoid conflicts with APIs in ucrt
#define fopen               VSF_LINUX_LIBC_WRAPPER(fopen)
#define freopen             VSF_LINUX_LIBC_WRAPPER(freopen)
#define fclose              VSF_LINUX_LIBC_WRAPPER(fclose)
#define fseek               VSF_LINUX_LIBC_WRAPPER(fseek)
#define ftell               VSF_LINUX_LIBC_WRAPPER(ftell)
#define fflush              VSF_LINUX_LIBC_WRAPPER(fflush)
#define putc                VSF_LINUX_LIBC_WRAPPER(putc)
#define putchar             VSF_LINUX_LIBC_WRAPPER(putchar)
#define fputc               VSF_LINUX_LIBC_WRAPPER(fputc)
#define fprintf             VSF_LINUX_LIBC_WRAPPER(fprintf)
#endif

#ifdef __WIN__
#   define sprintf_s        snprintf
#   define _scprintf        printf

static inline unsigned char swprintf_s(const wchar_t *c, ...)
{
    return 0;
}
static inline unsigned char _scwprintf(const wchar_t *c, ...)
{
    return 0;
}
#endif

typedef int FILE;
extern FILE * __vsf_linux_stdin(void);
extern FILE * __vsf_linux_stdout(void);
extern FILE * __vsf_linux_stderr(void);
// use extern functions with no dependent on shell.linux
#define stdin               __vsf_linux_stdin()
#define stdout              __vsf_linux_stdout()
#define stderr              __vsf_linux_stderr()

#define BUFSIZ              512
#define L_tmpnam            260

#define SEEK_SET            0   // same value as VSF_FILE_SEEK_SET
#define SEEK_CUR            1   // same value as VSF_FILE_SEEK_CUR
#define SEEK_END            2   // same value as VSF_FILE_SEEK_END

#ifndef PATH_MAX
#   define PATH_MAX         255
#endif
#define EOF                 -1

typedef intmax_t            fpos_t;

void perror(const char *str);

int putchar(int ch);
int getchar(void);
int fgetc(FILE *f);
int fputc(int ch, FILE *f);
int getc(FILE *f);
int putc(int ch, FILE *f);
int ungetc(int c, FILE *f);
int puts(const char *str);
int fputs(const char *str, FILE *f);
char *fgets(char *str, int n, FILE *f);

int scanf(const char *format, ...);
int vscanf(const char *format, va_list ap);
int fscanf(FILE *f, const char *format, ...);
int vfscanf(FILE *f, const char *format, va_list ap);

int printf(const char *format, ...);
int fprintf(FILE *f, const char *format, ...);
int vfprintf(FILE *f, const char *format, va_list ap);
int vprintf(const char *format, va_list arg);

FILE * fopen(const char *filename, const char *mode);
FILE * fdopen(int fildes, const char *mode);
FILE * freopen(const char *filename, const char *mode, FILE *f);
int fclose(FILE *f);
int fileno(FILE *stream);
int fseek(FILE *f, long offset, int fromwhere);
int fseeko(FILE *f, off_t offset, int fromwhere);
long ftell(FILE *f);
off_t ftello(FILE *f);
off64_t ftello64(FILE *f);
void rewind(FILE *f);
int fgetpos(FILE *f, fpos_t *pos);
int fsetpos(FILE *f, const fpos_t *pos);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f);
int fflush(FILE *f);

FILE * popen(const char *command, const char *type);
int pclose(FILE *stream);

void setbuf(FILE *f, char *buf);
#define _IOFBF              0x0000
#define _IOLBF              0x0040
#define _IONBF              0x0004
int setvbuf(FILE *f, char *buffer, int mode, size_t size);

int rename(const char *old_filename, const char *new_filename);
int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);

int ferror(FILE *f);
void clearerr(FILE *f);
int feof(FILE *f);

int sprintf(char *str, const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char* format, va_list ap);

int vsscanf(const char *str, const char *format, va_list ap);
int snscanf(const char *str, size_t size, const char *format, ...);
int sscanf(const char *str, const char *format, ...);

int remove(const char *filename);
int rename(const char *oldname, const char *newname);

FILE * tmpfile(void);
char * tmpnam(char *str);

#if defined(_GNU_SOURCE)
ssize_t getline(char **lineptr, size_t *n, FILE *f);
#endif

#ifdef __WIN__
void _lock_file(FILE *f);
void _unlock_file(FILE *f);
errno_t _get_stream_buffer_pointers(FILE *f, char ***base, char ***ptr, int **cnt);
int _fseeki64(FILE *f, uint64_t offset, int origin);
#endif

#ifdef __cplusplus
}
#endif

#if     __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
