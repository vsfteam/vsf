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

#define _IOFBF              0x0000
#define _IOLBF              0x0040
#define _IONBF              0x0004

#if VSF_LINUX_APPLET_USE_LIBC_STDIO == ENABLED
typedef struct vsf_linux_libc_stdio_vplt_t {
    vsf_vplt_info_t info;

    FILE * (*__vsf_linux_stdin)(void);
    FILE * (*__vsf_linux_stdout)(void);
    FILE * (*__vsf_linux_stderr)(void);

    void (*perror)(const char *str);

    int (*putchar)(int ch);
    int (*getchar)(void);
    int (*fgetc)(FILE *f);
    int (*fputc)(int ch, FILE *f);
    int (*getc)(FILE *f);
    int (*putc)(int ch, FILE *f);
    int (*ungetc)(int c, FILE *f);
    int (*puts)(const char *str);
    int (*fputs)(const char *str, FILE *f);
    char * (*fgets)(char *str, int n, FILE *f);

    int (*scanf)(const char *format, ...);
    int (*vscanf)(const char *format, va_list ap);
    int (*fscanf)(FILE *f, const char *format, ...);
    int (*vfscanf)(FILE *f, const char *format, va_list ap);

    int (*printf)(const char *format, ...);
    int (*fprintf)(FILE *f, const char *format, ...);
    int (*vfprintf)(FILE *f, const char *format, va_list ap);
    int (*vprintf)(const char *format, va_list ap);

    FILE * (*fopen)(const char *filename, const char *mode);
    FILE * (*fdopen)(int fildes, const char *mode);
    FILE * (*freopen)(const char *filename, const char *mode, FILE *f);
    int (*fclose)(FILE *f);
    int (*fileno)(FILE *f);
    int (*fseek)(FILE *f, long offset, int fromwhere);
    int (*fseeko)(FILE *f, off_t offset, int fromwhere);
    long (*ftell)(FILE *f);
    off_t (*ftello)(FILE *f);
    off64_t (*ftello64)(FILE *f);
    void (*rewind)(FILE *f);
    int (*fgetpos)(FILE *f, fpos_t *pos);
    int (*fsetpos)(FILE *f, const fpos_t *pos);
    size_t (*fread)(void *ptr, size_t size, size_t nmemb, FILE *f);
    size_t (*fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *f);
    int (*fflush)(FILE *f);

    FILE * (*popen)(const char *command, const char *type);
    int (*pclose)(FILE *stream);

    int (*rename)(const char *old_filename, const char *new_filename);
    int (*renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);

    int (*ferror)(FILE *f);
    void (*clearerr)(FILE *f);
    int (*feof)(FILE *f);

    int (*sprintf)(char *str, const char *format, ...);
    int (*vsprintf)(char *str, const char *format, va_list ap);
    int (*snprintf)(char *str, size_t size, const char *format, ...);
    int (*vsnprintf)(char *str, size_t size, const char* format, va_list ap);

    int (*vsscanf)(const char *str, const char *format, va_list ap);
    int (*vsnscanf)(const char *str, size_t size, const char *format, va_list ap);
    int (*snscanf)(const char *str, size_t size, const char *format, ...);
    int (*sscanf)(const char *str, const char *format, ...);

    int (*remove)(const char *filename);

    void (*setbuf)(FILE *f, char *buf);
    void (*setbuffer)(FILE *f, char *buf, size_t size);
    void (*setlinebuf)(FILE *f);
    int (*setvbuf)(FILE *f, char *buf, int mode, size_t size);
} vsf_linux_libc_stdio_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_stdio_vplt_t vsf_linux_libc_stdio_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_LIBC_STDIO == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_STDIO_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_STDIO_VPLT                                 \
            ((vsf_linux_libc_stdio_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_stdio))
#   else
#       define VSF_LINUX_APPLET_LIBC_STDIO_VPLT                                 \
            ((vsf_linux_libc_stdio_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline FILE * __vsf_linux_stdin(void) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->__vsf_linux_stdin();
}
static inline FILE * __vsf_linux_stdout(void) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->__vsf_linux_stdout();
}
static inline FILE * __vsf_linux_stderr(void) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->__vsf_linux_stderr();
}
static inline void perror(const char *str) {
    VSF_LINUX_APPLET_LIBC_STDIO_VPLT->perror(str);
}
static inline void setbuf(FILE *f, char *buf) {
    VSF_LINUX_APPLET_LIBC_STDIO_VPLT->setbuf(f, buf);
}
static inline void setbuffer(FILE *f, char *buf, size_t size) {
    VSF_LINUX_APPLET_LIBC_STDIO_VPLT->setbuffer(f, buf, size);
}
static inline void setlinebuf(FILE *f) {
    VSF_LINUX_APPLET_LIBC_STDIO_VPLT->setlinebuf(f);
}
static inline int setvbuf(FILE *f, char *buf, int mode, size_t size) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->setvbuf(f, buf, mode, size);
}
static inline int putchar(int ch) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->putchar(ch);
}
static inline int getchar(void) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->getchar();
}
static inline int fgetc(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fgetc(f);
}
static inline int fputc(int ch, FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fputc(ch, f);
}
static inline int getc(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->getc(f);
}
static inline int putc(int ch, FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->putc(ch, f);
}
static inline int ungetc(int c, FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->ungetc(c, f);
}
static inline int puts(const char *str) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->puts(str);
}
static inline int fputs(const char *str, FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fputs(str, f);
}
static inline char *fgets(char *str, int n, FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fgets(str, n, f);
}

static inline int vscanf(const char *format, va_list ap) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->vscanf(format, ap);
}
static inline int vfscanf(FILE *f, const char *format, va_list ap) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->vfscanf(f, format, ap);
}
static inline int scanf(const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vscanf(format, ap);
    va_end(ap);
    return result;
}
static inline int fscanf(FILE *f, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vfscanf(f, format, ap);
    va_end(ap);
    return result;
}

static inline int vfprintf(FILE *f, const char *format, va_list ap) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->vfprintf(f, format, ap);
}
static inline int vprintf(const char *format, va_list ap) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->vprintf(format, ap);
}
static inline int printf(const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vprintf(format, ap);
    va_end(ap);
    return result;
}
static inline int fprintf(FILE *f, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vfprintf(f, format, ap);
    va_end(ap);
    return result;
}

static inline FILE * fopen(const char *filename, const char *mode) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fopen(filename, mode);
}
static inline FILE * fdopen(int fildes, const char *mode) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fdopen(fildes, mode);
}
static inline FILE * freopen(const char *filename, const char *mode, FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->freopen(filename, mode, f);
}
static inline int fclose(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fclose(f);
}
static inline int fileno(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fileno(f);
}
static inline int fseek(FILE *f, long offset, int fromwhere) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fseek(f, offset, fromwhere);
}
static inline int fseeko(FILE *f, off_t offset, int fromwhere) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fseeko(f, offset, fromwhere);
}
static inline long ftell(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->ftell(f);
}
static inline off_t ftello(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->ftello(f);
}
static inline off64_t ftello64(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->ftello64(f);
}
static inline void rewind(FILE *f) {
    VSF_LINUX_APPLET_LIBC_STDIO_VPLT->rewind(f);
}
static inline int fgetpos(FILE *f, fpos_t *pos) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fgetpos(f, pos);
}
static inline int fsetpos(FILE *f, const fpos_t *pos) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fsetpos(f, pos);
}
static inline size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fread(ptr, size, nmemb, f);
}
static inline size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fwrite(ptr, size, nmemb, f);
}
static inline int fflush(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->fflush(f);
}

static inline FILE * popen(const char *command, const char *type) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->popen(command, type);
}
static inline int pclose(FILE *stream) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->pclose(stream);
}

static inline int rename(const char *old_filename, const char *new_filename) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->rename(old_filename, new_filename);
}
static inline int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->renameat(olddirfd, oldpath, newdirfd, newpath);
}

static inline int ferror(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->ferror(f);
}
static inline void clearerr(FILE *f) {
    VSF_LINUX_APPLET_LIBC_STDIO_VPLT->clearerr(f);
}
static inline int feof(FILE *f) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->feof(f);
}

static inline int vsprintf(char *str, const char *format, va_list ap) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->vsprintf(str, format, ap);
}
static inline int vsnprintf(char *str, size_t size, const char* format, va_list ap) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->vsnprintf(str, size, format, ap);
}
static inline int sprintf(char *str, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsprintf(str, format, ap);
    va_end(ap);
    return result;
}
static inline int snprintf(char *str, size_t size, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsnprintf(str, size, format, ap);
    va_end(ap);
    return result;
}

static inline int vsscanf(const char *str, const char *format, va_list ap) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->vsscanf(str, format, ap);
}
static inline int vsnscanf(const char *str, size_t size, const char *format, va_list ap) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->vsnscanf(str, size, format, ap);
}
static inline int snscanf(const char *str, size_t size, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsnscanf(str, size, format, ap);
    va_end(ap);
    return result;
}
static inline int sscanf(const char *str, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsscanf(str, format, ap);
    va_end(ap);
    return result;
}

static inline int remove(const char *filename) {
    return VSF_LINUX_APPLET_LIBC_STDIO_VPLT->remove(filename);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STDIO

FILE * __vsf_linux_stdin(void);
FILE * __vsf_linux_stdout(void);
FILE * __vsf_linux_stderr(void);

void perror(const char *str);

void setbuf(FILE *f, char *buf);
void setbuffer(FILE *f, char *buf, size_t size);
void setlinebuf(FILE *f);
int setvbuf(FILE *f, char *buf, int mode, size_t size);

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
int vprintf(const char *format, va_list ap);

FILE * fopen(const char *filename, const char *mode);
FILE * fdopen(int fildes, const char *mode);
FILE * freopen(const char *filename, const char *mode, FILE *f);
int fclose(FILE *f);
int fileno(FILE *f);
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
int vsnscanf(const char *str, size_t size, const char *format, va_list ap);
int snscanf(const char *str, size_t size, const char *format, ...);
int sscanf(const char *str, const char *format, ...);

int remove(const char *filename);
#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STDIO

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
