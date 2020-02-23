#ifndef __STDIO_H__
#define __STDIO_H__

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#endif

#include <sys/types.h>
#include <stdarg.h>

#define FILE        void
extern FILE *stdin, *stdout, *stderr;

#ifndef NULL
#   define NULL     (void *)0
#endif

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

#define PATH_MAX    255
#define EOF         0xFF

int getchar(void);
int puts(const char *str);
int fputs(const char *str, FILE *f);
int printf(const char *format, ...);
int fprintf(FILE *file, const char *format, ...);

FILE * fopen(const char *filename, const char *mode);
int fclose(FILE *f);
int fseek(FILE *f, long offset, int fromwhere);
long ftell(FILE *f);
size_t fread(const void *ptr, size_t size, size_t nmemb, FILE *f);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f);

int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);

int vsscanf(const char *str, const char *format, va_list ap);
int snscanf(const char *str, size_t size, const char *format, ...);
int sscanf(const char *str, const char *format, ...);


#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
