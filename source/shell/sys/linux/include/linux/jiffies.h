#ifndef __VSF_LINUX_JIFFIES_H__
#define __VSF_LINUX_JIFFIES_H__

#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define jiffies                     vsf_systimer_get_tick()

#ifdef __cplusplus
}
#endif

#endif
