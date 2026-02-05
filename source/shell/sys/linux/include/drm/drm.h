/*
 * DRM (Direct Rendering Manager) interface definitions
 * 
 * This header defines the standard DRM ioctl interface structures and constants
 * used for display management in Linux systems.
 */

#ifndef _DRM_H_
#define _DRM_H_

#include <stdint.h>
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DRM device types */
#define DRM_NAME                    "drm"
#define DRM_IOCTL_BASE              'd'

/* DRM lock flags */
#define _DRM_LOCK_HELD              0x80000000U
#define _DRM_LOCK_CONT              0x40000000U

/* DRM handle types */
typedef uint32_t drm_handle_t;
typedef uint32_t drm_context_t;
typedef uint32_t drm_drawable_t;
typedef uint32_t drm_magic_t;

/* DRM memory mapping types */
enum drm_map_type {
    DRM_FRAME_BUFFER = 0,
    DRM_REGISTERS = 1,
    DRM_SHM = 2,
    DRM_AGP = 3,
    DRM_SCATTER_GATHER = 4,
    DRM_CONSISTENT = 5,
    DRM_GEM = 6
};

/* DRM memory mapping flags */
enum drm_map_flags {
    DRM_RESTRICTED          = 0x01,
    DRM_READ_ONLY           = 0x02,
    DRM_LOCKED              = 0x04,
    DRM_KERNEL              = 0x08,
    DRM_WRITE_COMBINING     = 0x10,
    DRM_CONTAINS_LOCK       = 0x20,
    DRM_REMOVABLE           = 0x40
};

/* DRM lock structure */
struct drm_lock {
    int context;
    enum {
        DRM_LOCK_READY = 0,
        DRM_LOCK_QUIESCENT = 1,
        DRM_LOCK_FLUSH = 2,
        DRM_LOCK_FLUSH_ALL = 3,
        DRM_KERNEL_CONTEXT = 4
    } lock;
};

/* DRM version information */
struct drm_version {
    int version_major;
    int version_minor;
    int version_patchlevel;
    size_t name_len;
    char *name;
    size_t date_len;
    char *date;
    size_t desc_len;
    char *desc;
};

/* DRM unique identification */
struct drm_unique {
    size_t unique_len;
    char *unique;
};

/* DRM memory map */
struct drm_map {
    unsigned long offset;
    unsigned long size;
    enum drm_map_type type;
    enum drm_map_flags flags;
    drm_handle_t handle;
    int mtrr;
};

/* DRM client information */
struct drm_client {
    int idx;
    int auth;
    unsigned long pid;
    unsigned long uid;
    unsigned long magic;
    unsigned long iocs;
};

/* DRM statistics */
struct drm_stats {
    unsigned long count;
    struct {
        unsigned long value;
        enum {
            DRM_STAT_LOCK,
            DRM_STAT_OPENS,
            DRM_STAT_CLOSES,
            DRM_STAT_IOCTLS,
            DRM_STAT_LOCKS,
            DRM_STAT_UNLOCKS,
            DRM_STAT_IRQ,
            DRM_STAT_PRIMARY,
            DRM_STAT_SECONDARY,
            DRM_STAT_DMA,
            DRM_STAT_SPECIAL,
            DRM_STAT_MISSED
        } type;
    } data[15];
};

/* DRM version setting */
struct drm_set_version {
    int drm_di_major;
    int drm_di_minor;
    int drm_dd_major;
    int drm_dd_minor;
};

/* DRM authentication */
struct drm_auth {
    drm_magic_t magic;
};

/* DRM context */
enum drm_ctx_flags {
    DRM_CONTEXT_PRESERVED = 0x01,
    DRM_CONTEXT_2DONLY    = 0x02
};

struct drm_ctx {
    enum drm_ctx_flags flags;
    int handle;
};

/* DRM DMA flags */
enum drm_dma_flags {
    DRM_DMA_BLOCK           = 0x01,
    DRM_DMA_WHILE_LOCKED    = 0x02,
    DRM_DMA_PRIORITY        = 0x04,
    DRM_DMA_WAIT            = 0x10,
    DRM_DMA_SMALLER_OK      = 0x20,
    DRM_DMA_LARGER_OK       = 0x40
};

/* GEM (Graphics Execution Manager) objects */
struct drm_gem_close {
    uint32_t handle;
    uint32_t pad;
};

struct drm_gem_flink {
    uint32_t handle;
    uint32_t name;
};

struct drm_gem_open {
    uint32_t name;
    uint32_t handle;
    uint64_t size;
};

/* DRM PRIME (DMA-BUF) */
struct drm_prime_handle {
    uint32_t handle;
    uint32_t flags;
    int32_t fd;
    uint32_t pad;
};

/* DRM capabilities */
struct drm_get_cap {
    uint64_t capability;
    uint64_t value;
};

#define DRM_CAP_DUMB_BUFFER             0x1
#define DRM_CAP_VBLANK_HIGH_CRTC        0x2
#define DRM_CAP_DUMB_PREFERRED_DEPTH    0x3
#define DRM_CAP_DUMB_PREFER_SHADOW      0x4
#define DRM_CAP_PRIME                   0x5
#define DRM_PRIME_CAP_IMPORT            0x1
#define DRM_PRIME_CAP_EXPORT            0x2
#define DRM_CAP_TIMESTAMP_MONOTONIC     0x6
#define DRM_CAP_ASYNC_PAGE_FLIP         0x7
#define DRM_CAP_CURSOR_WIDTH            0x8
#define DRM_CAP_CURSOR_HEIGHT           0x9
#define DRM_CAP_ADDFB2_MODIFIERS        0x10
#define DRM_CAP_PAGE_FLIP_TARGET        0x11
#define DRM_CAP_CRTC_IN_VBLANK_EVENT    0x12
#define DRM_CAP_SYNCOBJ                 0x13
#define DRM_CAP_SYNCOBJ_TIMELINE        0x14

/* Client capabilities */
struct drm_set_client_cap {
    uint64_t capability;
    uint64_t value;
};

#define DRM_CLIENT_CAP_STEREO_3D        1
#define DRM_CLIENT_CAP_UNIVERSAL_PLANES 2
#define DRM_CLIENT_CAP_ATOMIC           3
#define DRM_CLIENT_CAP_ASPECT_RATIO     4
#define DRM_CLIENT_CAP_WRITEBACK_CONNECTORS 5

/* Ioctl helper macros */
#define DRM_IO(nr)                      _IO(DRM_IOCTL_BASE, nr)
#define DRM_IOR(nr, type)               _IOR(DRM_IOCTL_BASE, nr, type)
#define DRM_IOW(nr, type)               _IOW(DRM_IOCTL_BASE, nr, type)
#define DRM_IOWR(nr, type)              _IOWR(DRM_IOCTL_BASE, nr, type)

/* DRM ioctls */
#define DRM_IOCTL_VERSION               DRM_IOWR(0x00, struct drm_version)
#define DRM_IOCTL_GET_UNIQUE            DRM_IOWR(0x01, struct drm_unique)
#define DRM_IOCTL_GET_MAGIC             DRM_IOR(0x02, struct drm_auth)
#define DRM_IOCTL_IRQ_BUSID             DRM_IOWR(0x03, struct drm_irq_busid)
#define DRM_IOCTL_GET_MAP               DRM_IOWR(0x04, struct drm_map)
#define DRM_IOCTL_GET_CLIENT            DRM_IOWR(0x05, struct drm_client)
#define DRM_IOCTL_GET_STATS             DRM_IOR(0x06, struct drm_stats)
#define DRM_IOCTL_SET_VERSION           DRM_IOWR(0x07, struct drm_set_version)
#define DRM_IOCTL_MODESET_CTL           DRM_IOW(0x08, struct drm_modeset_ctl)
#define DRM_IOCTL_GEM_CLOSE             DRM_IOW(0x09, struct drm_gem_close)
#define DRM_IOCTL_GEM_FLINK             DRM_IOWR(0x0a, struct drm_gem_flink)
#define DRM_IOCTL_GEM_OPEN              DRM_IOWR(0x0b, struct drm_gem_open)
#define DRM_IOCTL_GET_CAP               DRM_IOWR(0x0c, struct drm_get_cap)
#define DRM_IOCTL_SET_CLIENT_CAP        DRM_IOW(0x0d, struct drm_set_client_cap)

#define DRM_IOCTL_SET_MASTER            DRM_IO(0x0e)
#define DRM_IOCTL_DROP_MASTER           DRM_IO(0x0f)

#define DRM_IOCTL_PRIME_HANDLE_TO_FD    DRM_IOWR(0x12, struct drm_prime_handle)
#define DRM_IOCTL_PRIME_FD_TO_HANDLE    DRM_IOWR(0x13, struct drm_prime_handle)

#ifdef __cplusplus
}
#endif

#endif /* _DRM_H_ */
