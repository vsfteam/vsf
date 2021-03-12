#ifndef __SIMPLE_LIBC_CTYPE_H__
#define __SIMPLE_LIBC_CTYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CTYPE_U         0x01        // upper
#define CTYPE_L         0x02        // lower
#define CTYPE_N         0x04        // numberic
#define CTYPE_S         0x08        // white space(space/lf/tab)
#define CTYPE_P         0x10        // punct
#define CTYPE_C         0x20        // control
#define CTYPE_X         0x40        // hex
#define CTYPE_B         0x80        // space
#define CTYPE_SP        (CTYPE_S | CTYPE_P)

extern const char __vsf_linux_ctype[1 + 256];
#define __ismask(__x)   (__vsf_linux_ctype[(int)(unsigned char)(__x) + 1])

#define isalnum(__c)    ((__ismask(__c) & (CTYPE_U|CTYPE_L|CTYPE_N)) != 0)
#define isalpha(__c)    ((__ismask(__c) & (CTYPE_U|CTYPE_L)) != 0)
#define iscntrl(__c)    ((__ismask(__c) & (CTYPE_C)) != 0)
#define isdigit(__c)    ((__ismask(__c) & (CTYPE_N)) != 0)
#define isgraph(__c)    ((__ismask(__c) & (CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_N)) != 0)
#define islower(__c)    ((__ismask(__c) & (CTYPE_L)) != 0)
#define isprint(__c)    ((__ismask(__c) & (CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_N|CTYPE_SP)) != 0)
#define ispunct(__c)    ((__ismask(__c) & (CTYPE_P)) != 0)
/* Note: isspace() must return false for %NUL-terminator */
#define isspace(__c)    ((__ismask(__c) & (CTYPE_S)) != 0)
#define isupper(__c)    ((__ismask(__c) & (CTYPE_U)) != 0)
#define isxdigit(__c)   ((__ismask(__c) & (CTYPE_N|CTYPE_X)) != 0)

#define isascii(__c)    (((unsigned char)(c))<=0x7f)
#define toascii(__c)    (((unsigned char)(c))&0x7f)

#define tolower(__c)    __tolower(__c)
#define toupper(__c)    __toupper(__c)

static inline unsigned char __tolower(unsigned char c)
{
    if (isupper(c)) {
        c -= 'A'-'a';
    }
    return c;
}

static inline unsigned char __toupper(unsigned char c)
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
