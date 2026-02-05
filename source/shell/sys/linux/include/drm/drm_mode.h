/*
 * DRM Mode Setting interface definitions
 *
 * This header defines the standard DRM/KMS (Kernel Mode Setting) ioctl
 * interface structures and constants for display mode configuration.
 */

#ifndef _DRM_MODE_H
#define _DRM_MODE_H

#include "drm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Display mode name length */
#define DRM_DISPLAY_MODE_LEN            32
#define DRM_PROP_NAME_LEN               32

/* Mode type flags */
#define DRM_MODE_TYPE_BUILTIN           (1 << 0)
#define DRM_MODE_TYPE_CLOCK_C           ((1 << 1) | DRM_MODE_TYPE_BUILTIN)
#define DRM_MODE_TYPE_CRTC_C            ((1 << 2) | DRM_MODE_TYPE_BUILTIN)
#define DRM_MODE_TYPE_PREFERRED         (1 << 3)
#define DRM_MODE_TYPE_DEFAULT           (1 << 4)
#define DRM_MODE_TYPE_USERDEF           (1 << 5)
#define DRM_MODE_TYPE_DRIVER            (1 << 6)

/* DPMS states */
#define DRM_MODE_DPMS_ON                0
#define DRM_MODE_DPMS_STANDBY           1
#define DRM_MODE_DPMS_SUSPEND           2
#define DRM_MODE_DPMS_OFF               3

/* Scaling modes */
#define DRM_MODE_SCALE_NONE             0
#define DRM_MODE_SCALE_FULLSCREEN       1
#define DRM_MODE_SCALE_CENTER           2
#define DRM_MODE_SCALE_ASPECT           3

/* Dithering modes */
#define DRM_MODE_DITHERING_OFF          0
#define DRM_MODE_DITHERING_ON           1
#define DRM_MODE_DITHERING_AUTO         2

/* Picture aspect ratio */
#define DRM_MODE_PICTURE_ASPECT_NONE    0
#define DRM_MODE_PICTURE_ASPECT_4_3     1
#define DRM_MODE_PICTURE_ASPECT_16_9    2

/* Content types */
#define DRM_MODE_CONTENT_TYPE_NO_DATA   0
#define DRM_MODE_CONTENT_TYPE_GRAPHICS  1
#define DRM_MODE_CONTENT_TYPE_PHOTO     2
#define DRM_MODE_CONTENT_TYPE_CINEMA    3
#define DRM_MODE_CONTENT_TYPE_GAME      4

/* Connector types */
#define DRM_MODE_CONNECTOR_UNKNOWN      0
#define DRM_MODE_CONNECTOR_VGA          1
#define DRM_MODE_CONNECTOR_DVII         2
#define DRM_MODE_CONNECTOR_DVID         3
#define DRM_MODE_CONNECTOR_DVIA         4
#define DRM_MODE_CONNECTOR_COMPOSITE    5
#define DRM_MODE_CONNECTOR_SVIDEO       6
#define DRM_MODE_CONNECTOR_LVDS         7
#define DRM_MODE_CONNECTOR_COMPONENT    8
#define DRM_MODE_CONNECTOR_9PINDIN      9
#define DRM_MODE_CONNECTOR_DISPLAYPORT  10
#define DRM_MODE_CONNECTOR_HDMIA        11
#define DRM_MODE_CONNECTOR_HDMIB        12
#define DRM_MODE_CONNECTOR_TV           13
#define DRM_MODE_CONNECTOR_EDP          14
#define DRM_MODE_CONNECTOR_VIRTUAL      15
#define DRM_MODE_CONNECTOR_DSI          16
#define DRM_MODE_CONNECTOR_DPI          17
#define DRM_MODE_CONNECTOR_WRITEBACK    18
#define DRM_MODE_CONNECTOR_SPI          19

/* Connector connection status */
#define DRM_MODE_CONNECTED              1
#define DRM_MODE_DISCONNECTED           2
#define DRM_MODE_UNKNOWNCONNECTION      3

/* Subpixel order */
#define DRM_MODE_SUBPIXEL_UNKNOWN       0
#define DRM_MODE_SUBPIXEL_HORIZONTAL_RGB 1
#define DRM_MODE_SUBPIXEL_HORIZONTAL_BGR 2
#define DRM_MODE_SUBPIXEL_VERTICAL_RGB  3
#define DRM_MODE_SUBPIXEL_VERTICAL_BGR  4
#define DRM_MODE_SUBPIXEL_NONE          5

/* Encoder types */
#define DRM_MODE_ENCODER_NONE           0
#define DRM_MODE_ENCODER_DAC            1
#define DRM_MODE_ENCODER_TMDS           2
#define DRM_MODE_ENCODER_LVDS           3
#define DRM_MODE_ENCODER_TVDAC          4
#define DRM_MODE_ENCODER_VIRTUAL        5
#define DRM_MODE_ENCODER_DSI            6
#define DRM_MODE_ENCODER_DPMST          7
#define DRM_MODE_ENCODER_DPI            8

/* Display timing mode info */
struct drm_mode_modeinfo {
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
};

/* Card resources */
struct drm_mode_card_res {
    uint64_t fb_id_ptr;
    uint64_t crtc_id_ptr;
    uint64_t connector_id_ptr;
    uint64_t encoder_id_ptr;
    uint32_t count_fbs;
    uint32_t count_crtcs;
    uint32_t count_connectors;
    uint32_t count_encoders;
    uint32_t min_width;
    uint32_t min_height;
    uint32_t max_width;
    uint32_t max_height;
};

/* CRTC configuration */
struct drm_mode_crtc {
    uint64_t set_connectors_ptr;
    uint32_t crtc_id;
    uint32_t fb_id;
    uint32_t x;
    uint32_t y;
    uint32_t gamma_size;
    uint32_t mode_valid;
    uint32_t count_connectors;
    struct drm_mode_modeinfo mode;
};

/* Cursor control */
#define DRM_MODE_CURSOR_BO              0x01
#define DRM_MODE_CURSOR_MOVE            0x02
#define DRM_MODE_CURSOR_FLAGS_MASK      0x03

struct drm_mode_cursor {
    uint32_t flags;
    uint32_t crtc_id;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t handle;
};

struct drm_mode_cursor2 {
    uint32_t flags;
    uint32_t crtc_id;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t handle;
    int32_t hot_x;
    int32_t hot_y;
};

/* Gamma LUT */
struct drm_mode_crtc_lut {
    uint32_t crtc_id;
    uint32_t gamma_size;
    uint64_t red;
    uint64_t green;
    uint64_t blue;
};

/* Color transformation matrix */
struct drm_color_ctm {
    uint64_t matrix[9];
};

/* Color LUT entry */
struct drm_color_lut {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t reserved;
};

/* Encoder information */
struct drm_mode_get_encoder {
    uint32_t encoder_id;
    uint32_t encoder_type;
    uint32_t crtc_id;
    uint32_t possible_crtcs;
    uint32_t possible_clones;
};

/* Connector information */
struct drm_mode_get_connector {
    uint64_t encoders_ptr;
    uint64_t modes_ptr;
    uint64_t props_ptr;
    uint64_t prop_values_ptr;
    uint32_t count_modes;
    uint32_t count_props;
    uint32_t count_encoders;
    uint32_t encoder_id;
    uint32_t connector_id;
    uint32_t connector_type;
    uint32_t connector_type_id;
    uint32_t connection;
    uint32_t mm_width;
    uint32_t mm_height;
    uint32_t subpixel;
};

/* Property flags */
#define DRM_MODE_PROP_PENDING           (1 << 0)
#define DRM_MODE_PROP_RANGE             (1 << 1)
#define DRM_MODE_PROP_IMMUTABLE         (1 << 2)
#define DRM_MODE_PROP_ENUM              (1 << 3)
#define DRM_MODE_PROP_BLOB              (1 << 4)
#define DRM_MODE_PROP_BITMASK           (1 << 5)
#define DRM_MODE_PROP_TYPE              (1 << 6)
#define DRM_MODE_PROP_OBJECT            (1 << 7)
#define DRM_MODE_PROP_SIGNED_RANGE      (1 << 8)

/* Property enum definition */
struct drm_mode_property_enum {
    uint64_t value;
    char name[DRM_PROP_NAME_LEN];
};

/* Property information */
struct drm_mode_get_property {
    uint64_t values_ptr;
    uint64_t enum_blob_ptr;
    uint32_t prop_id;
    uint32_t flags;
    char name[DRM_PROP_NAME_LEN];
    uint32_t count_values;
    uint32_t count_enum_blobs;
};

/* Connector property setting */
struct drm_mode_connector_set_property {
    uint64_t value;
    uint32_t prop_id;
    uint32_t connector_id;
};

/* Object properties */
struct drm_mode_obj_get_properties {
    uint64_t props_ptr;
    uint64_t prop_values_ptr;
    uint32_t count_props;
    uint32_t obj_id;
    uint32_t obj_type;
};

struct drm_mode_obj_set_property {
    uint64_t value;
    uint32_t prop_id;
    uint32_t obj_id;
    uint32_t obj_type;
};

/* Property blob */
struct drm_mode_get_blob {
    uint32_t blob_id;
    uint32_t length;
    uint64_t data;
};

/* Framebuffer (legacy) */
struct drm_mode_fb_cmd {
    uint32_t fb_id;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t depth;
    uint32_t handle;
};

/* Framebuffer flags */
#define DRM_MODE_FB_INTERLACED          (1 << 0)
#define DRM_MODE_FB_MODIFIERS           (1 << 1)

/* Framebuffer (modern) */
struct drm_mode_fb_cmd2 {
    uint32_t fb_id;
    uint32_t width;
    uint32_t height;
    uint32_t pixel_format;
    uint32_t flags;
    uint32_t handles[4];
    uint32_t pitches[4];
    uint32_t offsets[4];
    uint64_t modifiers[4];
};

/* Dirty framebuffer */
#define DRM_MODE_FB_DIRTY_ANNOTATE_COPY 0x01
#define DRM_MODE_FB_DIRTY_ANNOTATE_FILL 0x02
#define DRM_MODE_FB_DIRTY_MAX_CLIPS     256

struct drm_mode_fb_dirty_cmd {
    uint32_t fb_id;
    uint32_t flags;
    uint32_t color;
    uint32_t num_clips;
    uint64_t clips_ptr;
};

/* Mode command */
struct drm_mode_mode_cmd {
    uint32_t connector_id;
    struct drm_mode_modeinfo mode;
};

/* Page flip */
#define DRM_MODE_PAGE_FLIP_EVENT        0x01
#define DRM_MODE_PAGE_FLIP_ASYNC        0x02
#define DRM_MODE_PAGE_FLIP_TARGET_ABSOLUTE  0x04
#define DRM_MODE_PAGE_FLIP_TARGET_RELATIVE  0x08

struct drm_mode_crtc_page_flip {
    uint32_t crtc_id;
    uint32_t fb_id;
    uint32_t flags;
    uint32_t reserved;
    uint64_t user_data;
};

/* Dumb buffer - create */
struct drm_mode_create_dumb {
    uint32_t height;
    uint32_t width;
    uint32_t bpp;
    uint32_t flags;
    uint32_t handle;
    uint32_t pitch;
    uint64_t size;
};

/* Dumb buffer - map */
struct drm_mode_map_dumb {
    uint32_t handle;
    uint32_t pad;
    uint64_t offset;
};

/* Dumb buffer - destroy */
struct drm_mode_destroy_dumb {
    uint32_t handle;
};

/* Atomic commit flags */
#define DRM_MODE_ATOMIC_TEST_ONLY       0x0100
#define DRM_MODE_ATOMIC_NONBLOCK        0x0200
#define DRM_MODE_ATOMIC_PAGE_FLIP_EVENT 0x0400
#define DRM_MODE_ATOMIC_PAGE_FLIP_ASYNC 0x0800

/* Plane types */
#define DRM_PLANE_TYPE_OVERLAY          0
#define DRM_PLANE_TYPE_PRIMARY          1
#define DRM_PLANE_TYPE_CURSOR           2

/* Plane resources */
struct drm_mode_get_plane_res {
    uint64_t plane_id_ptr;
    uint32_t count_planes;
};

/* Plane information */
struct drm_mode_get_plane {
    uint32_t plane_id;
    uint32_t crtc_id;
    uint32_t fb_id;
    uint32_t possible_crtcs;
    uint32_t gamma_size;
    uint32_t count_format_types;
    uint64_t format_type_ptr;
};

/* Plane configuration */
struct drm_mode_set_plane {
    uint32_t plane_id;
    uint32_t crtc_id;
    uint32_t fb_id;
    uint32_t flags;
    int32_t crtc_x;
    int32_t crtc_y;
    uint32_t crtc_w;
    uint32_t crtc_h;
    uint32_t src_x;
    uint32_t src_y;
    uint32_t src_h;
    uint32_t src_w;
};

/* Atomic commit */
struct drm_mode_atomic {
    uint32_t flags;
    uint32_t count_objs;
    uint64_t objs_ptr;
    uint64_t count_props_ptr;
    uint64_t props_ptr;
    uint64_t prop_values_ptr;
    uint64_t reserved;
    uint64_t user_data;
};

/* Object types for atomic commit */
#define DRM_MODE_OBJECT_CRTC            0xcccccccc
#define DRM_MODE_OBJECT_CONNECTOR       0xc0c0c0c0
#define DRM_MODE_OBJECT_ENCODER         0xe0e0e0e0
#define DRM_MODE_OBJECT_MODE            0xdededede
#define DRM_MODE_OBJECT_PROPERTY        0xb0b0b0b0
#define DRM_MODE_OBJECT_FB              0xfbfbfbfb
#define DRM_MODE_OBJECT_BLOB            0xbbbbbbbb
#define DRM_MODE_OBJECT_PLANE           0xeeeeeeee
#define DRM_MODE_OBJECT_ANY             0

/* Mode setting ioctls */
#define DRM_IOCTL_MODE_GETRESOURCES     DRM_IOWR(0xA0, struct drm_mode_card_res)
#define DRM_IOCTL_MODE_GETCRTC          DRM_IOWR(0xA1, struct drm_mode_crtc)
#define DRM_IOCTL_MODE_SETCRTC          DRM_IOWR(0xA2, struct drm_mode_crtc)
#define DRM_IOCTL_MODE_CURSOR           DRM_IOWR(0xA3, struct drm_mode_cursor)
#define DRM_IOCTL_MODE_GETGAMMA         DRM_IOWR(0xA4, struct drm_mode_crtc_lut)
#define DRM_IOCTL_MODE_SETGAMMA         DRM_IOWR(0xA5, struct drm_mode_crtc_lut)
#define DRM_IOCTL_MODE_GETENCODER       DRM_IOWR(0xA6, struct drm_mode_get_encoder)
#define DRM_IOCTL_MODE_GETCONNECTOR     DRM_IOWR(0xA7, struct drm_mode_get_connector)
#define DRM_IOCTL_MODE_ATTACHMODE       DRM_IOWR(0xA8, struct drm_mode_mode_cmd)
#define DRM_IOCTL_MODE_DETACHMODE       DRM_IOWR(0xA9, struct drm_mode_mode_cmd)
#define DRM_IOCTL_MODE_GETPROPERTY      DRM_IOWR(0xAA, struct drm_mode_get_property)
#define DRM_IOCTL_MODE_SETPROPERTY      DRM_IOWR(0xAB, struct drm_mode_connector_set_property)
#define DRM_IOCTL_MODE_GETPROPBLOB      DRM_IOWR(0xAC, struct drm_mode_get_blob)
#define DRM_IOCTL_MODE_GETFB            DRM_IOWR(0xAD, struct drm_mode_fb_cmd)
#define DRM_IOCTL_MODE_ADDFB            DRM_IOWR(0xAE, struct drm_mode_fb_cmd)
#define DRM_IOCTL_MODE_RMFB             DRM_IOWR(0xAF, unsigned int)
#define DRM_IOCTL_MODE_PAGE_FLIP        DRM_IOWR(0xB0, struct drm_mode_crtc_page_flip)
#define DRM_IOCTL_MODE_DIRTYFB          DRM_IOWR(0xB1, struct drm_mode_fb_dirty_cmd)
#define DRM_IOCTL_MODE_CREATE_DUMB      DRM_IOWR(0xB2, struct drm_mode_create_dumb)
#define DRM_IOCTL_MODE_MAP_DUMB         DRM_IOWR(0xB3, struct drm_mode_map_dumb)
#define DRM_IOCTL_MODE_DESTROY_DUMB     DRM_IOWR(0xB4, struct drm_mode_destroy_dumb)
#define DRM_IOCTL_MODE_GETPLANERESOURCES DRM_IOWR(0xB5, struct drm_mode_get_plane_res)
#define DRM_IOCTL_MODE_GETPLANE         DRM_IOWR(0xB6, struct drm_mode_get_plane)
#define DRM_IOCTL_MODE_SETPLANE         DRM_IOWR(0xB7, struct drm_mode_set_plane)
#define DRM_IOCTL_MODE_ADDFB2           DRM_IOWR(0xB8, struct drm_mode_fb_cmd2)
#define DRM_IOCTL_MODE_OBJ_GETPROPERTIES DRM_IOWR(0xB9, struct drm_mode_obj_get_properties)
#define DRM_IOCTL_MODE_OBJ_SETPROPERTY  DRM_IOWR(0xBA, struct drm_mode_obj_set_property)
#define DRM_IOCTL_MODE_CURSOR2          DRM_IOWR(0xBB, struct drm_mode_cursor2)
#define DRM_IOCTL_MODE_ATOMIC           DRM_IOWR(0xBC, struct drm_mode_atomic)
#define DRM_IOCTL_MODE_CREATEPROPBLOB   DRM_IOWR(0xBD, struct drm_mode_create_blob)
#define DRM_IOCTL_MODE_DESTROYPROPBLOB  DRM_IOWR(0xBE, struct drm_mode_destroy_blob)

/* Pixel format helpers */
#ifndef fourcc_code
#define fourcc_code(a, b, c, d) \
    ((uint32_t)(a) | ((uint32_t)(b) << 8) | \
     ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))
#endif

#define DRM_FORMAT_BIG_ENDIAN           (1U << 31)
#define DRM_FORMAT_INVALID              0

/* 8 bpp */
#define DRM_FORMAT_C8                   fourcc_code('C', '8', ' ', ' ')

/* 16 bpp */
#define DRM_FORMAT_RGB565               fourcc_code('R', 'G', '1', '6')
#define DRM_FORMAT_BGR565               fourcc_code('B', 'G', '1', '6')
#define DRM_FORMAT_RGB555               fourcc_code('R', 'G', '1', '5')
#define DRM_FORMAT_BGR555               fourcc_code('B', 'G', '1', '5')

/* 24 bpp */
#define DRM_FORMAT_RGB888               fourcc_code('R', 'G', '2', '4')
#define DRM_FORMAT_BGR888               fourcc_code('B', 'G', '2', '4')

/* 32 bpp */
#define DRM_FORMAT_XRGB8888             fourcc_code('X', 'R', '2', '4')
#define DRM_FORMAT_XBGR8888             fourcc_code('X', 'B', '2', '4')
#define DRM_FORMAT_RGBX8888             fourcc_code('R', 'X', '2', '4')
#define DRM_FORMAT_BGRX8888             fourcc_code('B', 'X', '2', '4')
#define DRM_FORMAT_ARGB8888             fourcc_code('A', 'R', '2', '4')
#define DRM_FORMAT_ABGR8888             fourcc_code('A', 'B', '2', '4')
#define DRM_FORMAT_RGBA8888             fourcc_code('R', 'A', '2', '4')
#define DRM_FORMAT_BGRA8888             fourcc_code('B', 'A', '2', '4')

#ifdef __cplusplus
}
#endif

#endif /* _DRM_MODE_H */
