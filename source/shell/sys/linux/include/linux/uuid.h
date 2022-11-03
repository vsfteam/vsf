#ifndef __VSF_LINUX_UUID_H__
#define __VSF_LINUX_UUID_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __u8 b[16];
} guid_t;

#define GUID_INIT(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7)                      \
((guid_t){{                                                                     \
    (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff,      \
    (b) & 0xff, ((b) >> 8) & 0xff,                                              \
    (c) & 0xff, ((c) >> 8) & 0xff,                                              \
    (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7),                             \
}})

#ifdef __cplusplus
}
#endif

#endif
