#ifndef __VSF_LINUX_FS_H__
#define __VSF_LINUX_FS_H__

#ifdef __cplusplus
extern "C" {
#endif

// for ioctrl
#define BLKBSZGET               (('b' << 8) | 0)
#define BLKBSZSET               (('b' << 8) | 1)
#define BLKGETSIZE64            (('b' << 8) | 2)

#ifdef __cplusplus
}
#endif

#endif
