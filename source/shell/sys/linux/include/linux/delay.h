#ifndef __VSF_LINUX_DELAY_H__
#define __VSF_LINUX_DELAY_H__

#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline msleep(unsigned int msecs)
{
    vsf_thread_delay_ms(msecs);
}

static inline usleep_range(unsigned long min, unsigned long max)
{
    vsf_thread_delay_us(min);
}

#ifdef __cplusplus
}
#endif

#endif
