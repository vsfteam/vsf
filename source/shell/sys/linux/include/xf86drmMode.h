/*
 * libdrm compatible API - Mode setting functions
 *
 * This header provides a libdrm-compatible API for KMS (Kernel Mode Setting).
 */

#ifndef _XF86DRMMODE_H_
#define _XF86DRMMODE_H_

#include <stdint.h>
#include <drm/drm_mode.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Resources */
typedef struct _drmModeRes {
    int count_fbs;
    uint32_t *fbs;
    int count_crtcs;
    uint32_t *crtcs;
    int count_connectors;
    uint32_t *connectors;
    int count_encoders;
    uint32_t *encoders;
    uint32_t min_width;
    uint32_t min_height;
    uint32_t max_width;
    uint32_t max_height;
} drmModeRes, *drmModeResPtr;

/* Display mode */
typedef struct _drmModeModeInfo {
    uint32_t clock;
    uint16_t hdisplay;
    uint16_t hsync_start;
    uint16_t hsync_end;
    uint16_t htotal;
    uint16_t hskew;
    uint16_t vdisplay;
    uint16_t vsync_start;
    uint16_t vsync_end;
    uint16_t vtotal;
    uint16_t vscan;
    uint32_t vrefresh;
    uint32_t flags;
    uint32_t type;
    char name[DRM_DISPLAY_MODE_LEN];
} drmModeModeInfo, *drmModeModeInfoPtr;

/* Encoder */
typedef struct _drmModeEncoder {
    uint32_t encoder_id;
    uint32_t encoder_type;
    uint32_t crtc_id;
    uint32_t possible_crtcs;
    uint32_t possible_clones;
} drmModeEncoder, *drmModeEncoderPtr;

/* Connector */
typedef struct _drmModeConnector {
    uint32_t connector_id;
    uint32_t encoder_id;
    uint32_t connector_type;
    uint32_t connector_type_id;
    uint32_t connection;
    uint32_t mmWidth, mmHeight;
    uint32_t subpixel;
    int count_modes;
    drmModeModeInfoPtr modes;
    int count_props;
    uint32_t *props;
    uint64_t *prop_values;
    int count_encoders;
    uint32_t *encoders;
} drmModeConnector, *drmModeConnectorPtr;

/* CRTC */
typedef struct _drmModeCrtc {
    uint32_t crtc_id;
    uint32_t buffer_id;
    uint32_t x, y;
    uint32_t width, height;
    uint32_t gamma_size;
    drmModeModeInfo mode;
} drmModeCrtc, *drmModeCrtcPtr;

/* Framebuffer */
typedef struct _drmModeFB {
    uint32_t fb_id;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t depth;
    uint32_t handle;
} drmModeFB, *drmModeFBPtr;

/* Plane */
typedef struct _drmModePlane {
    uint32_t plane_id;
    uint32_t crtc_id;
    uint32_t fb_id;
    uint32_t possible_crtcs;
    uint32_t gamma_size;
    uint32_t count_formats;
    uint32_t *formats;
} drmModePlane, *drmModePlanePtr;

typedef struct _drmModePlaneRes {
    uint32_t count_planes;
    uint32_t *planes;
} drmModePlaneRes, *drmModePlaneResPtr;

/* Property */
typedef struct _drmModeProperty {
    uint32_t prop_id;
    uint32_t flags;
    char name[DRM_PROP_NAME_LEN];
    int count_values;
    uint64_t *values;
    int count_enums;
    struct drm_mode_property_enum *enums;
} drmModeProperty, *drmModePropertyPtr;

typedef struct _drmModeObjectProperties {
    uint32_t count_props;
    uint32_t *props;
    uint64_t *prop_values;
} drmModeObjectProperties, *drmModeObjectPropertiesPtr;

/* Atomic request */
typedef struct _drmModeAtomicReq drmModeAtomicReq, *drmModeAtomicReqPtr;

/* Resource management */
extern drmModeResPtr drmModeGetResources(int fd);
extern void drmModeFreeResources(drmModeResPtr ptr);

/* CRTC functions */
extern drmModeCrtcPtr drmModeGetCrtc(int fd, uint32_t crtcId);
extern int drmModeSetCrtc(int fd, uint32_t crtcId, uint32_t bufferId,
                        uint32_t x, uint32_t y, uint32_t *connectors, int count,
                        drmModeModeInfoPtr mode);
extern void drmModeFreeCrtc(drmModeCrtcPtr ptr);

/* Connector functions */
extern drmModeConnectorPtr drmModeGetConnector(int fd, uint32_t connectorId);
extern void drmModeFreeConnector(drmModeConnectorPtr ptr);
extern drmModeEncoderPtr drmModeGetEncoder(int fd, uint32_t encoderId);
extern void drmModeFreeEncoder(drmModeEncoderPtr ptr);

/* FB functions */
extern drmModeFBPtr drmModeGetFB(int fd, uint32_t fbId);
extern void drmModeFreeFB(drmModeFBPtr ptr);
extern int drmModeAddFB(int fd, uint32_t width, uint32_t height, uint8_t depth,
                        uint8_t bpp, uint32_t pitch, uint32_t bo, uint32_t *buf_id);
extern int drmModeAddFB2(int fd, uint32_t width, uint32_t height,
                        uint32_t pixel_format, const uint32_t bo_handles[4],
                        const uint32_t pitches[4], const uint32_t offsets[4],
                        uint32_t *buf_id, uint32_t flags);
extern int drmModeRmFB(int fd, uint32_t bufferId);

/* Page flip */
extern int drmModePageFlip(int fd, uint32_t crtc_id, uint32_t fb_id,
                        uint32_t flags, void *user_data);

/* Dumb buffer */
extern int drmModeCreateDumbBuffer(int fd, struct drm_mode_create_dumb *args);
extern int drmModeMapDumbBuffer(int fd, struct drm_mode_map_dumb *args);
extern int drmModeDestroyDumbBuffer(int fd, struct drm_mode_destroy_dumb *args);

/* Plane functions */
extern drmModePlaneResPtr drmModeGetPlaneResources(int fd);
extern void drmModeFreePlaneResources(drmModePlaneResPtr ptr);
extern drmModePlanePtr drmModeGetPlane(int fd, uint32_t plane_id);
extern void drmModeFreePlane(drmModePlanePtr ptr);
extern int drmModeSetPlane(int fd, uint32_t plane_id, uint32_t crtc_id,
                        uint32_t fb_id, uint32_t flags,
                        int32_t crtc_x, int32_t crtc_y,
                        uint32_t crtc_w, uint32_t crtc_h,
                        uint32_t src_x, uint32_t src_y,
                        uint32_t src_w, uint32_t src_h);

/* Cursor */
extern int drmModeSetCursor(int fd, uint32_t crtcId, uint32_t bo,
                        uint32_t width, uint32_t height);
extern int drmModeMoveCursor(int fd, uint32_t crtcId, int x, int y);

/* Property functions */
extern drmModePropertyPtr drmModeGetProperty(int fd, uint32_t propertyId);
extern void drmModeFreeProperty(drmModePropertyPtr ptr);

/* Gamma */
extern int drmModeCrtcSetGamma(int fd, uint32_t crtcId, uint32_t size,
                        uint16_t *red, uint16_t *green, uint16_t *blue);
extern int drmModeCrtcGetGamma(int fd, uint32_t crtcId, uint32_t size,
                        uint16_t *red, uint16_t *green, uint16_t *blue);

#ifdef __cplusplus
}
#endif

#endif /* _XF86DRMMODE_H_ */
