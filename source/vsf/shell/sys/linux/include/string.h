#ifndef __STRING_H__
#define __STRING_H__

#include "sys/types.h"

void * memset(void *s, int ch, size_t n);
size_t strlen(const char *str);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char * strdup(const char *str);
char * strcpy(char *dest, const char *src);
char * strncpy(char *dest, const char *src, size_t n);
char * strcat(char *dest, const char *src);
char * strstr(const char *str1, const char *str2);
char * strchr(const char *str, int c);
char * strrchr(const char *str, int c);

int strcasecmp (const char *s1, const char *s2);

#if defined(__WIN__)
int stricmp(const char *s1, const char *s2);
void * memcpy(void *dest, const void *src, unsigned int n);
#else
void * memcpy(void *dest, const void *src, size_t n);
#endif
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);
void *memchr(const void *buf, int ch, size_t count);

#endif
