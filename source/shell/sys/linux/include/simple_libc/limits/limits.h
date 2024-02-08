#ifndef __SIMPLE_LIBC_LIMITS_H__
#define __SIMPLE_LIBC_LIMITS_H__

#define CHAR_BIT                        8
#define LONG_BIT                        32
#define WORD_BIT                        32

#define UCHAR_MAX                       ((unsigned char)(~0U))
#define SCHAR_MAX                       ((signed char)(UCHAR_MAX >> 1))
#define USHRT_MAX                       ((unsigned short)(~0U))
#define SHRT_MAX                        ((short)(USHRT_MAX >> 1))
#define UINT_MAX                        (~0U)
#define INT_MAX                         ((int)(~0U >> 1))
#define ULONG_MAX                       (~0UL)
#define LONG_MAX                        ((long)(~0UL >> 1))
#define ULLONG_MAX                      (~0ULL)
#define LLONG_MAX                       ((long long)(~0ULL >> 1))

#define CHAR_MIN                        SCHAR_MIN
#define CHAR_MAX                        SCHAR_MAX

#define SCHAR_MIN                       (-SCHAR_MAX - 1)
#define SHRT_MIN                        (-SHRT_MAX - 1)
#define INT_MIN                         (-INT_MAX - 1)
#define LONG_MIN                        (-LONG_MAX - 1)
#define LLONG_MIN                       (-LLONG_MAX - 1)

#define LONG_LONG_MAX                   LLONG_MAX
#define LONG_LONG_MIN                   LLONG_MIN
#define ULONG_LONG_MAX                  ULLONG_MAX

#define SSIZE_MAX                       _POSIX_SSIZE_MAX

#define MB_LEN_MAX                      16

// POSIX.1
#define _POSIX_AIO_LISTIO_MAX           2
#define _POSIX_AIO_MAX                  1
#define _POSIX_ARG_MAX                  4096
#define _POSIX_CHILD_MAX                6
#define _POSIX_DELAYTIMER_MAX           32
#define _POSIX_LINK_MAX                 8
#define _POSIX_MAX_CANON                255
#define _POSIX_MAX_INPUT                255
#define _POSIX_MQ_OPEN_MAX              8
#define _POSIX_MQ_PRIO_MAX              32
#define _POSIX_NAME_MAX                 14
#define _POSIX_NGROUPS_MAX              0
#define _POSIX_OPEN_MAX                 16
#define _POSIX_PATH_MAX                 255
#define _POSIX_PIPE_BUF                 512
#define _POSIX_RTSIG_MAX                8
#define _POSIX_SEM_NSEMS_MAX            256
#define _POSIX_SEM_VALUE_MAX            32767
#define _POSIX_SIGQUEUE_MAX             32
#define _POSIX_SSIZE_MAX                32767
#define _POSIX_STREAM_MAX               8
#define _POSIX_TIMER_MAX                32
#define _POSIX_TZNAME_MAX               3
#define _POSIX_LOGIN_NAME_MAX           9
#define _POSIX_THREAD_DESTRUCTOR_INTERATION 4
#define _POSIX_THREAD_KEYS_MAX          128
#define _POSIX_THREAD_THREADS_MAX       64
#define _POSIX_TTY_NAME_MAX             9

// POSIX.2

#define _POSIX2_BC_BASE_MAX             99
#define _POSIX2_BC_DIM_MAX              2048
#define _POSIX2_BC_SCALE_MAX            99
#define _POSIX2_BC_STRING_MAX           1000
#define _POSIX2_COLL_WEIGHTS_MAX        2
#define _POSIX2_EXPR_NEST_MAX           32
#define _POSIX2_LINE_MAX                2048
#define _POSIX2_RE_DUP_MAX              255
#define BC_BASE_MAX                     _POSIX2_BC_BASE_MAX
#define BC_DIM_MAX                      _POSIX2_BC_DIM_MAX
#define BC_SCALE_MAX                    _POSIX2_BC_SCALE_MAX
#define BC_STRING_MAX                   _POSIX2_BC_STRING_MAX
#define COLL_WEIGHTS_MAX                _POSIX2_COLL_WEIGHTS_MAX
#define EXPR_NEST_MAX                   _POSIX2_EXPR_NEST_MAX
#define LINE_MAX                        _POSIX2_LINE_MAX
#define RE_DUP_MAX                      _POSIX2_RE_DUP_MAX

#endif
