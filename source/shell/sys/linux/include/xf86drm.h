/*
 * libdrm compatible API - Core DRM functions
 *
 * This header provides a libdrm-compatible API for user-space DRM access.
 */

#ifndef _XF86DRM_H_
#define _XF86DRM_H_

#include <stdint.h>
#include <sys/types.h>
#include <drm/drm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Device paths */
#ifndef DRM_DEV_NAME
#define DRM_DEV_NAME            "%s/dri/card%%d"
#endif

#ifndef DRM_DIR_NAME
#define DRM_DIR_NAME            "/dev"
#endif

#define DRM_MAX_MINOR           16

/* Version information */
typedef struct _drmVersion {
    int version_major;
    int version_minor;
    int version_patchlevel;
    int name_len;
    char *name;
    int date_len;
    char *date;
    int desc_len;
    char *desc;
} drmVersion, *drmVersionPtr;

/* Device functions */
extern int drmOpen(const char *name, const char *busid);
extern int drmClose(int fd);

/* Version functions */
extern drmVersionPtr drmGetVersion(int fd);
extern void drmFreeVersion(drmVersionPtr);

/* Capabilities */
extern int drmGetCap(int fd, uint64_t capability, uint64_t *value);

/* Authentication */
extern int drmGetMagic(int fd, drm_magic_t *magic);
extern int drmAuthMagic(int fd, drm_magic_t magic);

/* Master */
extern int drmSetMaster(int fd);
extern int drmDropMaster(int fd);

/* PRIME */
extern int drmPrimeHandleToFD(int fd, uint32_t handle, uint32_t flags, int *prime_fd);
extern int drmPrimeFDToHandle(int fd, int prime_fd, uint32_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* _XF86DRM_H_ */
