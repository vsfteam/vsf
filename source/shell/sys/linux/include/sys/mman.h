#ifndef __VSF_LINUX_MMAN_H__
#define __VSF_LINUX_MMAN_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define mmap                VSF_LINUX_WRAPPER(mmap)
#define munmap              VSF_LINUX_WRAPPER(munmap)
#define mprotect            VSF_LINUX_WRAPPER(mprotect)
#endif

#define PROT_NONE       0
#define PROT_READ       (1 << 0)
#define PROT_WRITE      (1 << 1)
#define PROT_EXEC       (1 << 2)
#define MAP_32BIT       (1 << 3)

#define MAP_FILE        0
#define MAP_SHARED      1
#define MAP_PRIVATE     2
#define MAP_TYPE        0xf
#define MAP_FIXED       0x10
#define MAP_ANONYMOUS   0x20
#define MAP_ANON        MAP_ANONYMOUS

#define MAP_FAILED      ((void *)-1)

#define MS_ASYNC        1
#define MS_SYNC         2
#define MS_INVALIDATE   4

void * mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
int munmap(void *addr, size_t len);
int mprotect(void *addr, size_t len, int prot);
int msync(void *addr, size_t len, int flags);
int mlock(const void *addr, size_t len);
int munlock(const void *addr, size_t len);

#ifdef __cplusplus
}
#endif

#endif
