#ifndef __VSF_LINUX_FUTEX_H__
#define __VSF_LINUX_FUTEX_H__

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FUTEX_WAIT              0
#define FUTEX_WAKE              1
#define FUTEX_FD                2
#define FUTEX_REQUEUE           3
#define FUTEX_CMP_REQUEUE       4
#define FUTEX_WAKE_OP           5
#define FUTEX_LOCK_PI           6
#define FUTEX_UNLOCK_PI         7
#define FUTEX_TRYLOCK_PI        8
#define FUTEX_WAIT_BITSET       9
#define FUTEX_WAKE_BITSET       10
#define FUTEX_WAIT_REQUEUE_PI   11
#define FUTEX_CMP_REQUEUE_PI    12

#define FUTEX_PRIVATE_FLAG      128
#define FUTEX_CLOCK_REALTIME    256
#define FUTEX_CMD_MASK          ~(FUTEX_PRIVATE_FLAG | FUTEX_CLOCK_REALTIME)

#define FUTEX_BITSET_MATCH_ANY	0xffffffff

long vsf_linux_sys_futex(uint32_t *futex, int futex_op, uint32_t val, const struct timespec *spec, uint32_t *futex2, uint32_t val3);
#define __NR_futex(__p0, __p1, __p2, __p3, __p4, __p5)                          \
            vsf_linux_sys_futex((uint32_t *)(__p0), (__p1), (__p2), (__p3), (uint32_t *)(__p4), (__p5))
#define __NR_futex_time64       __NR_futex
#define SYS_futex               __NR_futex

#ifdef __cplusplus
}
#endif

#endif
