#ifndef __SIMPLE_LIBC_CTYPE_H__
#define __SIMPLE_LIBC_CTYPE_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../errno.h"
#else
#   include <errno.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CTYPE_U             0x01        // upper
#define CTYPE_L             0x02        // lower
#define CTYPE_N             0x04        // numberic
#define CTYPE_S             0x08        // white space(space/lf/tab)
#define CTYPE_P             0x10        // punct
#define CTYPE_C             0x20        // control
#define CTYPE_X             0x40        // hex
#define CTYPE_B             0x80        // space
#define CTYPE_SP            (CTYPE_S | CTYPE_P)

extern const char __vsf_linux_ctype[1 + 256];
#define __ismask(__x)       (__vsf_linux_ctype[(int)(unsigned char)(__x) + 1])

static inline unsigned char isalnum(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_U|CTYPE_L|CTYPE_N)) != 0);
}

static inline unsigned char isalpha(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_U|CTYPE_L)) != 0);
}
static inline unsigned char iscntrl(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_C)) != 0);
}
static inline unsigned char isdigit(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_N)) != 0);
}
static inline unsigned char isgraph(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_N)) != 0);
}
static inline unsigned char islower(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_L)) != 0);
}
static inline unsigned char isprint(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_N|CTYPE_SP)) != 0);
}
static inline unsigned char ispunct(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_P)) != 0);
}
static inline unsigned char isspace(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_S)) != 0);
}
static inline unsigned char isupper(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_U)) != 0);
}
static inline unsigned char isxdigit(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_N|CTYPE_X)) != 0);
}
static inline unsigned char isblank(unsigned char c)
{
    return ((__ismask(c) & (CTYPE_B)) != 0);
}

static inline unsigned char isascii(unsigned char c)
{
    return (((unsigned char)(c))<=0x7f);
}
static inline unsigned char toascii(unsigned char c)
{
    return (((unsigned char)(c))&0x7f);
}

static inline unsigned char tolower(unsigned char c)
{
    if (isupper(c)) {
        c -= 'A'-'a';
    }
    return c;
}

static inline unsigned char toupper(unsigned char c)
{
    if (islower(c)) {
        c -= 'a'-'A';
    }
    return c;
}

#ifdef __cplusplus
}
#endif

#endif
