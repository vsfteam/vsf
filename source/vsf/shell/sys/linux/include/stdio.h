#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdarg.h>

#define FILE        void
extern FILE *stdin, *stdout, *stderr;

int getchar(void);
int puts(const char *str);
int fputs(const char *str, FILE *f);
int printf(const char *format, ...);
int fprintf(FILE *file, const char *format, ...);

size_t fread(const void *ptr, size_t size, size_t nmemb, FILE *f);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f);

int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);

#endif
