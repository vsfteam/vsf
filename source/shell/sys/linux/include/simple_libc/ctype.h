#ifndef __SIMPLE_LIBC_CTYPE_H__
#define __SIMPLE_LIBC_CTYPE_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#else
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

static inline int isalnum(int c)
{
    return ((__ismask(c) & (CTYPE_U|CTYPE_L|CTYPE_N)) != 0);
}

static inline int isalpha(int c)
{
    return ((__ismask(c) & (CTYPE_U|CTYPE_L)) != 0);
}
static inline int iscntrl(int c)
{
    return ((__ismask(c) & (CTYPE_C)) != 0);
}
static inline int isdigit(int c)
{
    return ((__ismask(c) & (CTYPE_N)) != 0);
}
static inline int isgraph(int c)
{
    return ((__ismask(c) & (CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_N)) != 0);
}
static inline int islower(int c)
{
    return ((__ismask(c) & (CTYPE_L)) != 0);
}
static inline int isprint(int c)
{
    return ((__ismask(c) & (CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_N|CTYPE_SP)) != 0);
}
static inline int ispunct(int c)
{
    return ((__ismask(c) & (CTYPE_P)) != 0);
}
static inline int isspace(int c)
{
    return ((__ismask(c) & (CTYPE_S)) != 0);
}
static inline int isupper(int c)
{
    return ((__ismask(c) & (CTYPE_U)) != 0);
}
static inline int isxdigit(int c)
{
    return ((__ismask(c) & (CTYPE_N|CTYPE_X)) != 0);
}
static inline int isblank(int c)
{
    return ((__ismask(c) & (CTYPE_B)) != 0);
}

static inline int isascii(int c)
{
    return (((unsigned char)(c))<=0x7f);
}
static inline int toascii(int c)
{
    return (((unsigned char)(c))&0x7f);
}

static inline int tolower(int c)
{
    if (isupper(c)) {
        c -= 'A'-'a';
    }
    return c;
}

static inline int toupper(int c)
{
    if (islower(c)) {
        c -= 'a'-'A';
    }
    return c;
}

#ifdef __WIN__
#define _CONTROL            CTYPE_C
#define _SPACE              CTYPE_S
#define _DIGIT              CTYPE_N
#define _LOWER              CTYPE_L
#define _PUNCT              CTYPE_P
#define _BLANK              CTYPE_B
#define _UPPER              CTYPE_U
#define _HEX                CTYPE_X
#endif

#ifdef __cplusplus
}
#endif

#endif
