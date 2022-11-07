#ifndef __VSF_LINUX_JIFFIES_H__
#define __VSF_LINUX_JIFFIES_H__

#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define jiffies                     vsf_systimer_get_tick()
#define msecs_to_jiffies(__msec)    vsf_systimer_ms_to_tick(__msec)
#define usecs_to_jiffies(__msec)    vsf_systimer_us_to_tick(__msec)

#ifdef __cplusplus
}
#endif

#endif
