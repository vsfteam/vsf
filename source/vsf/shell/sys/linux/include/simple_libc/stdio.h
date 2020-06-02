#ifndef __STDIO_H__
#define __STDIO_H__

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#pragma clang diagnostic ignored "-Winconsistent-dllimport"
#endif

#include <sys/types.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int FILE;
extern FILE *stdin, *stdout, *stderr;

#ifndef NULL
#   define NULL     (void *)0
#endif

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

#define PATH_MAX    255
#define EOF         0xFF

#define fpos_t      uintmax_t

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
long ftell(FILE *f);
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

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
