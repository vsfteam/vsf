#ifndef __SIMPLE_LIBC_STDIO_H__
#define __SIMPLE_LIBC_STDIO_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif
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
#define ferror              __vsf_linux_ferror
#define clearerr            __vsf_linux_clearerr
#define feof                __vsf_linux_feof
#define getc                __vsf_linux_getc
#define getchar             __vsf_linux_getchar
#define putc                __vsf_linux_putc
#define putchar             __vsf_linux_putchar


#define fopen               __vsf_linux_fopen
#define freopen             __vsf_linux_freopen
#define fclose              __vsf_linux_fclose
#define fseek               __vsf_linux_fseek
#define fseeko              __vsf_linux_fseeko
#define ftell               __vsf_linux_ftell
#define ftello              __vsf_linux_ftello
#define ftello64            __vsf_linux_ftello64
#define rewind              __vsf_linux_rewind
#define fwrite              __vsf_linux_fwrite
#define fread               __vsf_linux_fread
#define fflush              __vsf_linux_fflush
#define fgets               __vsf_linux_fgets
#define gets                __vsf_linux_gets
#define fputs               __vsf_linux_fputs
#define puts                __vsf_linux_puts
#define printf              __vsf_linux_printf
#define fprintf             __vsf_linux_fprintf
#define fscanf              __vsf_linux_fscanf
#define perror              __vsf_linux_perror
#define setvbuf             __vsf_linux_setvbuf

#define stdin               __vsf_linux_stdin
#define stdout              __vsf_linux_stdout
#define stderr              __vsf_linux_stderr

typedef int FILE;
extern FILE *stdin, *stdout, *stderr;

#define BUFSIZ              512
#define L_tmpnam            260

#define SEEK_SET            0
#define SEEK_CUR            1
#define SEEK_END            2

#define PATH_MAX            255
#define EOF                 0xFF

#define fpos_t              uintmax_t

void perror(const char *str);

#define _IOFBF              0x0000
#define _IOLBF              0x0040
#define _IONBF              0x0004
int setvbuf(FILE *stream, char *buffer, int mode, size_t size);

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
int fscanf(FILE *f, const char *format, ...);
int vfscanf(FILE *f, const char *format, va_list ap);
int vscanf(const char *format, va_list ap);

int printf(const char *format, ...);
int fprintf(FILE *f, const char *format, ...);
int vfprintf(FILE *f, const char *format, va_list ap);
int vprintf(const char *format, va_list arg);

FILE * fopen(const char *filename, const char *mode);
FILE * freopen(const char *filename, const char *mode, FILE *f);
int fclose(FILE *f);
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

void setbuf(FILE *f, char *buf);
int setvbuf(FILE *f, char *buf, int type, unsigned size);

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

#ifdef __WIN__
FILE * tmpfile(void);
char * tmpnam(char *str);
#endif

#ifdef __cplusplus
}
#endif

#if     __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
