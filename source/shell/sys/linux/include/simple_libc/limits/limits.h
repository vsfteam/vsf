#ifndef __SIMPLE_LIBC_LIMITS_H__
#define __SIMPLE_LIBC_LIMITS_H__

#define CHAR_BIT                        (sizeof(char) * 8)
#define SCHAR_MAX                       127
#define SHRT_MAX                        32767
#define INT_MAX                         2147483647
#define LONG_MAX                        2147483647L
#define LLONG_MAX                       9223372036854775807LL

#define CHAR_MIN                        SCHAR_MIN
#define CHAR_MAX                        SCHAR_MAX

#define SCHAR_MIN                       (-SCHAR_MAX - 1)
#define SHRT_MIN                        (-SHRT_MAX - 1)
#define INT_MIN                         (-INT_MAX - 1)
#define LONG_MIN                        (-LONG_MAX - 1)
#define LLONG_MIN                       (-LLONG_MAX - 1)

#define UCHAR_MAX                       (SCHAR_MAX * 2 + 1)
#define UINT_MAX                        (INT_MAX * 2U + 1U)
#define ULONG_MAX                       (LONG_MAX * 2UL + 1UL)
#define ULLONG_MAX                      (LLONG_MAX * 2ULL + 1ULL)

#define LONG_LONG_MAX                   LLONG_MAX
#define LONG_LONG_MIN                   LLONG_MIN
#define ULONG_LONG_MAX                  ULLONG_MAX

#endif
