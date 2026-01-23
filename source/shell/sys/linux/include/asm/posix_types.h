#include "../sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __kernel_long_t
typedef long __kernel_long_t;
typedef unsigned long __kernel_ulong_t;
#endif

#ifndef __kernel_size_t
typedef size_t __kernel_size_t;
#endif

typedef off_t __kernel_off_t;
typedef ptrdiff_t __kernel_ptrdiff_t;
typedef time_t __kernel_time_t;
typedef time64_t __kernel_time64_t;
typedef clock_t __kernel_clock_t;
typedef clockid_t __kernel_clockid_t;
typedef timer_t __kernel_timer_t;

#ifdef __cplusplus
}
#endif
