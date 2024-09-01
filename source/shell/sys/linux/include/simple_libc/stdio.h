#ifndef __SIMPLE_LIBC_STDIO_H__
#define __SIMPLE_LIBC_STDIO_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
// for off64_t
#   include "../sys/types.h"
#else
#   include <stddef.h>
// for off64_t
#   include <sys/types.h>
#endif

#include <stdarg.h>
#include <stdint.h>

//! \note libc belongs to compiler layer in utilities, so only include compiler.h
//! IMPORTANT: to avoid including other c headers, define __USE_LOCAL_XXXX__ here
#define __USE_LOCAL_STDBOOL__
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
#define fseeko64            VSF_LINUX_LIBC_WRAPPER(fseeko64)
#define ftell               VSF_LINUX_LIBC_WRAPPER(ftell)
#define ftello              VSF_LINUX_LIBC_WRAPPER(ftello)
#define ftello64            VSF_LINUX_LIBC_WRAPPER(ftello64)
#define rewind              VSF_LINUX_LIBC_WRAPPER(rewind)
#define fgetpos             VSF_LINUX_LIBC_WRAPPER(fgetpos)
#define fsetpos             VSF_LINUX_LIBC_WRAPPER(fsetpos)
#define fgetpos64           VSF_LINUX_LIBC_WRAPPER(fgetpos64)
#define fsetpos64           VSF_LINUX_LIBC_WRAPPER(fsetpos64)
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
#define vdprintf            VSF_LINUX_LIBC_WRAPPER(vdprintf)
#define fprintf             VSF_LINUX_LIBC_WRAPPER(fprintf)
#define dprintf             VSF_LINUX_LIBC_WRAPPER(dprintf)
#define fiprintf            VSF_LINUX_LIBC_WRAPPER(fiprintf)
#define fscanf              VSF_LINUX_LIBC_WRAPPER(fscanf)
#define vfscanf             VSF_LINUX_LIBC_WRAPPER(vfscanf)
#define perror              VSF_LINUX_LIBC_WRAPPER(perror)
#define setvbuf             VSF_LINUX_LIBC_WRAPPER(setvbuf)
#define rename              VSF_LINUX_LIBC_WRAPPER(rename)
#define renameat            VSF_LINUX_LIBC_WRAPPER(renameat)

#define tmpfile             VSF_LINUX_LIBC_WRAPPER(tmpfile)
#define tmpnam              VSF_LINUX_LIBC_WRAPPER(tmpnam)

// usually there is no need to wrap snprintf/vsnprintf.
// __VSF_LINUX_LIBC_STDIO_WRAPPER_SNPRINTF will be enabled on some platform like windows.
#if __VSF_LINUX_LIBC_STDIO_WRAPPER_SNPRINTF == ENABLED
#   define vsnprintf        VSF_LINUX_LIBC_WRAPPER(vsnprintf)
#   define snprintf         VSF_LINUX_LIBC_WRAPPER(snprintf)
#endif

#if defined(_GNU_SOURCE)
#define getline             VSF_LINUX_LIBC_WRAPPER(getline)
#endif
#elif defined(__WIN__) && !defined(__VSF_APPLET__)
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
#define vsnprintf           VSF_LINUX_LIBC_WRAPPER(vsnprintf)
#define snprintf            VSF_LINUX_LIBC_WRAPPER(snprintf)
#endif

typedef off64_t             fpos_t;
typedef off64_t             fpos64_t;

// fpos_t here is not compatible with fpos_t in DLib, so _FPOSOFF MUST be redefined
#if __IS_COMPILER_IAR__
#   undef _FPOSOFF
#   define _FPOSOFF(__FPOS) (__FPOS)
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

#define EOF                 -1

#define _IOFBF              0x0000
#define _IOLBF              0x0040
#define _IONBF              0x0004

#ifndef FILENAME_MAX
#   define FILENAME_MAX     128
#endif

#if VSF_LINUX_APPLET_USE_LIBC_STDIO == ENABLED
typedef struct vsf_linux_libc_stdio_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__vsf_linux_stdin);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__vsf_linux_stdout);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__vsf_linux_stderr);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(perror);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(putchar);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getchar);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fgetc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fputc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(putc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ungetc);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(puts);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fputs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fgets);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(scanf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vscanf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fscanf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vfscanf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(printf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vfprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vdprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vprintf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fopen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fdopen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(freopen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fclose);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fileno);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fseek);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fseeko);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fseeko64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ftell);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ftello);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ftello64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(rewind);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fgetpos);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fsetpos);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fgetpos64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fsetpos64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fread);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fwrite);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fflush);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(popen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pclose);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(rename);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(renameat);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ferror);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(clearerr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(feof);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(snprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsnprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asprintf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vasprintf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsscanf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sscanf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(remove);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setbuf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setbuffer);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setlinebuf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setvbuf);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tmpfile);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tmpnam);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getline);
} vsf_linux_libc_stdio_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_stdio_vplt_t vsf_linux_libc_stdio_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_LIBC_STDIO_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_LIBC_STDIO == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_STDIO_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_STDIO_VPLT                                 \
            ((vsf_linux_libc_stdio_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_stdio_vplt))
#   else
#       define VSF_LINUX_APPLET_LIBC_STDIO_VPLT                                 \
            ((vsf_linux_libc_stdio_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(__NAME)                               \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_STDIO_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_STDIO_IMP(...)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_STDIO_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_LIBC_STDIO_IMP(__vsf_linux_stdin, FILE *, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(__vsf_linux_stdin)();
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(__vsf_linux_stdout, FILE *, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(__vsf_linux_stdout)();
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(__vsf_linux_stderr, FILE *, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(__vsf_linux_stderr)();
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(perror, void, const char *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(perror)(str);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(setbuf, void, FILE *f, char *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(setbuf)(f, buf);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(setbuffer, void, FILE *f, char *buf, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(setbuffer)(f, buf, size);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(setlinebuf, void, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(setlinebuf)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(setvbuf, int, FILE *f, char *buf, int mode, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(setvbuf)(f, buf, mode, size);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(putchar, int, int ch) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(putchar)(ch);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(getchar, int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(getchar)();
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fgetc, int, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fgetc)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fputc, int, int ch, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fputc)(ch, f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(getc, int, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(getc)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(putc, int, int ch, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(putc)(ch, f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(ungetc, int, int c, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(ungetc)(c, f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(puts, int, const char *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(puts)(str);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fputs, int, const char *str, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fputs)(str, f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fgets, char *, char *str, int n, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fgets)(str, n, f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vscanf, int, const char *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vscanf)(format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vfscanf, int, FILE *f, const char *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vfscanf)(f, format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vfprintf, int, FILE *f, const char *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vfprintf)(f, format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vdprintf, int, int fd, const char *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vdprintf)(fd, format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vprintf, int, const char *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vprintf)(format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fopen, FILE *, const char *filename, const char *mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fopen)(filename, mode);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fdopen, FILE *, int fildes, const char *mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fdopen)(fildes, mode);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(freopen, FILE *, const char *filename, const char *mode, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(freopen)(filename, mode, f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fclose, int, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fclose)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fileno, int, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fileno)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fseek, int, FILE *f, long offset, int fromwhere) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fseek)(f, offset, fromwhere);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fseeko, int, FILE *f, off_t offset, int fromwhere) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fseeko)(f, offset, fromwhere);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fseeko64, int, FILE *f, off64_t offset, int fromwhere) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fseeko64)(f, offset, fromwhere);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(ftell, long, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(ftell)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(ftello, off_t, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(ftello)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(ftello64, off64_t, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(ftello64)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(rewind, void, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(rewind)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fgetpos, int, FILE *f, fpos_t *pos) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fgetpos)(f, pos);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fsetpos, int, FILE *f, const fpos_t *pos) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fsetpos)(f, pos);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fgetpos64, int, FILE *f, fpos64_t *pos) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fgetpos64)(f, pos);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fsetpos64, int, FILE *f, const fpos64_t *pos) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fsetpos64)(f, pos);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fread, size_t, void *ptr, size_t size, size_t nmemb, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fread)(ptr, size, nmemb, f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fwrite, size_t, const void *ptr, size_t size, size_t nmemb, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fwrite)(ptr, size, nmemb, f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(fflush, int, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(fflush)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(popen, FILE *, const char *command, const char *type) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(popen)(command, type);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(pclose, int, FILE *stream) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(pclose)(stream);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(rename, int, const char *old_filename, const char *new_filename) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(rename)(old_filename, new_filename);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(renameat, int, int olddirfd, const char *oldpath, int newdirfd, const char *newpath) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(renameat)(olddirfd, oldpath, newdirfd, newpath);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(ferror, int, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(ferror)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(clearerr, void, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(clearerr)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(feof, int, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(feof)(f);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vsprintf, int, char *str, const char *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vsprintf)(str, format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vsnprintf, int, char *str, size_t size, const char* format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vsnprintf)(str, size, format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vasprintf, int, char **strp, const char *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vasprintf)(strp, format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(vsscanf, int, const char *str, const char *format, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(vsscanf)(str, format, ap);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(remove, int, const char *filename) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(remove)(filename);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(tmpfile, FILE *, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(tmpfile)();
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(tmpnam, char *, char *str) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(tmpnam)(str);
}
VSF_LINUX_APPLET_LIBC_STDIO_IMP(getline, ssize_t, char **lineptr, size_t *n, FILE *f) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_STDIO_ENTRY(getline)(lineptr, n, f);
}

VSF_APPLET_VPLT_FUNC_DECORATOR(scanf) int scanf(const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vscanf(format, ap);
    va_end(ap);
    return result;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(fscanf) int fscanf(FILE *f, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vfscanf(f, format, ap);
    va_end(ap);
    return result;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(printf) int printf(const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vprintf(format, ap);
    va_end(ap);
    return result;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(fprintf) int fprintf(FILE *f, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vfprintf(f, format, ap);
    va_end(ap);
    return result;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(dprintf) int dprintf(int fd, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vdprintf(fd, format, ap);
    va_end(ap);
    return result;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(sprintf) int sprintf(char *str, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsprintf(str, format, ap);
    va_end(ap);
    return result;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(snprintf) int snprintf(char *str, size_t size, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsnprintf(str, size, format, ap);
    va_end(ap);
    return result;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(asprintf) int asprintf(char **strp, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vasprintf(strp, format, ap);
    va_end(ap);
    return result;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(sscanf) int sscanf(const char *str, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsscanf(str, format, ap);
    va_end(ap);
    return result;
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
char * fgets(char *str, int n, FILE *f);

int scanf(const char *format, ...);
int vscanf(const char *format, va_list ap);
int fscanf(FILE *f, const char *format, ...);
int vfscanf(FILE *f, const char *format, va_list ap);

int printf(const char *format, ...);
int fprintf(FILE *f, const char *format, ...);
int vfprintf(FILE *f, const char *format, va_list ap);
int dprintf(int fd, const char *format, ...);
int vdprintf(int fd, const char *format, va_list ap);
int fiprintf(FILE *f, const char *format, ...);
int vprintf(const char *format, va_list ap);

FILE * fopen(const char *filename, const char *mode);
FILE * fdopen(int fildes, const char *mode);
FILE * freopen(const char *filename, const char *mode, FILE *f);
int fclose(FILE *f);
int fileno(FILE *f);
int fseek(FILE *f, long offset, int fromwhere);
int fseeko(FILE *f, off_t offset, int fromwhere);
int fseeko64(FILE *f, off64_t offset, int fromwhere);
long ftell(FILE *f);
off_t ftello(FILE *f);
off64_t ftello64(FILE *f);
void rewind(FILE *f);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f);
int fflush(FILE *f);
int fgetpos(FILE *f, fpos_t *pos);
int fsetpos(FILE *f, const fpos_t *pos);
int fgetpos64(FILE *f, fpos_t *pos);
int fsetpos64(FILE *f, const fpos_t *pos);

FILE * popen(const char *command, const char *type);
int pclose(FILE *stream);

int rename(const char *old_filename, const char *new_filename);
int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);

int ferror(FILE *f);
void clearerr(FILE *f);
int feof(FILE *f);

int sprintf(char *str, const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char* format, va_list ap);
int asprintf(char **strp, const char *format, ...);
int vasprintf(char **strp, const char *format, va_list ap);

int vsscanf(const char *str, const char *format, va_list ap);
int sscanf(const char *str, const char *format, ...);

int remove(const char *filename);

FILE * tmpfile(void);
char * tmpnam(char *str);
#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_STDIO

// use static inline for unlocked APIs for better compatibility
//  MACROs will have side effects
static inline int getc_unlocked(FILE *f) {
    return getc(f);
}
static inline int getchar_unlocked(void) {
    return getchar();
}
static inline int putc_unlocked(int c, FILE *f){
    return putc(c, f);
}
static inline int putchar_unlocked(int c) {
    return putchar(c);
}
static inline void clearerr_unlocked(FILE *f) {
    clearerr(f);
}
static inline int feof_unlocked(FILE *f) {
    return feof(f);
}
static inline int ferror_unlocked(FILE *f) {
    return ferror(f);
}
static inline int fileno_unlocked(FILE *f) {
    return fileno(f);
}
static inline int fflush_unlocked(FILE *f) {
    return fflush(f);
}
static inline int fgetc_unlocked(FILE *f) {
    return fgetc(f);
}
static inline int fputc_unlocked(int ch, FILE *f) {
    return fputc(ch, f);
}
static inline size_t fread_unlocked(void *ptr, size_t size, size_t nmemb, FILE *f) {
    return fread(ptr, size, nmemb, f);
}
static inline size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *f) {
    return fwrite(ptr, size, nmemb, f);
}
static inline int fputs_unlocked(const char *str, FILE *f) {
    return fputs(str, f);
}
static inline char * fgets_unlocked(char *str, int n, FILE *f) {
    return fgets(str, n, f);
}
static inline FILE *fopen64(const char *filename, const char *mode) {
    return fopen(filename, mode);
}

ssize_t getline(char **lineptr, size_t *n, FILE *f);

#if defined(__WIN__) && !defined(__VSF_APPLET__)
// wrapper for original _lock_file/_unlock_file called in win c++ libs
#define _lock_file                  VSF_LINUX_LIBC_WRAPPER(_lock_file)
#define _unlock_file                VSF_LINUX_LIBC_WRAPPER(_unlock_file)
#define _get_stream_buffer_pointers VSF_LINUX_LIBC_WRAPPER(_get_stream_buffer_pointers)
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
