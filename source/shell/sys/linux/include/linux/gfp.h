#ifndef __VSF_LINUX_GFP_H__
#define __VSF_LINUX_GFP_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int        gfp_t;

#define __GFP_ZERO          (1 << 8)
#define __GFP_ATOMIC        (1 << 9)

#define GFP_ATOMIC          (__GFP_ATOMIC)
#define GFP_KERNEL          0
#define GFP_NOWAIT          0
#define GFP_NOIO            0
#define GFP_NOFS            0
#define GFP_USER            0
#define GFP_DMA             0

#ifdef __cplusplus
}
#endif

#endif
