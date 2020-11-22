#ifndef __SIMPLE_LIBC_CTYPE_H__
#define __SIMPLE_LIBC_CTYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define _U              0x01        // upper
#define _L              0x02        // lower
#define _N              0x04        // numberic
#define _S              0x08        // white space(space/lf/tab)
#define _P              0x10        // punct
#define _C              0x20        // control
#define _X              0x40        // hex
#define _B              0x80        // space

extern const char __vsf_linux_ctype[1 + 256];
#define __ismask(__x)   (__vsf_linux_ctype[(int)(unsigned char)(__x) + 1])

#define isalnum(__c)    ((__ismask(__c) & (_U|_L|_N)) != 0)
#define isalpha(__c)    ((__ismask(__c) & (_U|_L)) != 0)
#define iscntrl(__c)    ((__ismask(__c) & (_C)) != 0)
#define isdigit(__c)    ((__ismask(__c) & (_N)) != 0)
#define isgraph(__c)    ((__ismask(__c) & (_P|_U|_L|_N)) != 0)
#define islower(__c)    ((__ismask(__c) & (_L)) != 0)
#define isprint(__c)    ((__ismask(__c) & (_P|_U|_L|_N|_SP)) != 0)
#define ispunct(__c)    ((__ismask(__c) & (_P)) != 0)
/* Note: isspace() must return false for %NUL-terminator */
#define isspace(__c)    ((__ismask(__c) & (_S)) != 0)
#define isupper(__c)    ((__ismask(__c) & (_U)) != 0)
#define isxdigit(__c)   ((__ismask(__c) & (_N|_X)) != 0)

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
