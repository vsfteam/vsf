#ifndef __VSF_LINUX_ERR_H__
#define __VSF_LINUX_ERR_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static inline void vwarnx(const char *fmt, va_list args)
{
    if (fmt != NULL) {
        vfprintf(stderr, fmt, args);
    }
}

static inline void warnx(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
        vwarnx(fmt, args);
    va_end(args);
}

static inline void vwarn(const char *fmt, va_list args)
{
    vwarnx(fmt, args);
}

static inline void warn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
        vwarn(fmt, args);
    va_end(args);
}

static inline void verr(int eval, const char *fmt, va_list args)
{
    vwarn(fmt, args);
    exit(eval);
}

static inline void verrx(int eval, const char *fmt, va_list args)
{
    vwarnx(fmt, args);
    exit(eval);
}

static inline void err(int eval, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
        verr(eval, fmt, args);
    va_end(args);
}

static inline void errx(int eval, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
        verrx(eval, fmt, args);
    va_end(args);
}

#endif      // __VSF_LINUX_ERR_H__
