#ifndef __VSF_LINUX_KD_H__
#define __VSF_LINUX_KD_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define K_RAW                   0
#define K_XLATE                 1
#define K_MEDIUMRAW             2
#define K_UNICODE               3
#define K_OFF                   4

#define KDSETMODE               (('K' << 8) | 0)
#   define KD_TEXT              0
#   define KD_GRAPHICS          1
#define KDGETMODE               (('K' << 8) | 1)
#define KDGKBMODE               (('K' << 8) | 2)
#define KDSKBMODE               (('K' << 8) | 3)

#ifdef __cplusplus
}
#endif

#endif
