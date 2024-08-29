#ifndef __VSF_LINUX_FB_H__
#define __VSF_LINUX_FB_H__

#include <linux/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// for ioctrl
#define FBIOGET_VSCREENINFO     (('F' << 8) | 0)
#define FBIOPUT_VSCREENINFO     (('F' << 8) | 1)
#define FBIOGET_FSCREENINFO     (('F' << 8) | 2)
#define FBIOGETCMAP             (('F' << 8) | 4)
#define FBIOPUTCMAP             (('F' << 8) | 5)
#define FBIOPAN_DISPLAY         (('F' << 8) | 6)
#define FBIOBLANK               (('F' << 8) | 17)
#define FBIO_WAITFORVSYNC       (('F' << 8) | 32)

// supported only by VSF for displays without framebuffer,
//  take an argument as pointer to vk_disp_area_t.
// Usage:
// No need to mmap the frame buffer, just set the area to update,
//  and then write the buffer of the area.
#define FBIOSET_AREA            (('F' << 8) | 64)

#define FB_NONSTD_HAM           1
#define FB_NONSTD_REV_PIX_IN_B  2

#define FB_ACTIVATE_NOW         0
#define FB_ACTIVATE_NXTOPEN     1
#define FB_ACTIVATE_TEST        2
#define FB_ACTIVATE_MASK        15
#define FB_ACTIVATE_VBL         16
#define FB_CHANGE_CMAP_VBL      32
#define FB_ACTIVATE_ALL         64
#define FB_ACTIVATE_FORCE       128
#define FB_ACTIVATE_INV_MODE    256
#define FB_ACTIVATE_KD_TEXT     512

#define FB_VISUAL_MONO01        0
#define FB_VISUAL_MONO10        1
#define FB_VISUAL_TRUECOLOR     2
#define FB_VISUAL_PSEUDOCOLOR   3
#define FB_VISUAL_DIRECTCOLOR   4
#define FB_VISUAL_STATIC_PSEUDOCOLOR    5
#define FB_VISUAL_FOURCC        6

#define FB_TYPE_PACKED_PIXELS   0
#define FB_TYPE_PLANES          1
#define FB_TYPE_INTERLEAVED_PLANES  2
#define FB_TYPE_TEXT            3
#define FB_TYPE_VGA_PLANES      4
#define FB_TYPE_FOURCC          5

#define FB_SYNC_HOR_HIGH_ACT    1
#define FB_SYNC_VERT_HIGH_ACT   2
#define FB_SYNC_EXT             4
#define FB_SYNC_COMP_HIGH_ACT   8
#define FB_SYNC_BROADCAST       16
#define FB_SYNC_ON_GREEN        32

#define FB_VMODE_NONINTERLACED  0
#define FB_VMODE_INTERLACED     1
#define FB_VMODE_DOUBLE         2
#define FB_VMODE_ODD_FLD_FIRST  4
#define FB_VMODE_MASK           255

#define FB_VMODE_YWRAP          256
#define FB_VMODE_SMOOTH_XPAN    512
#define FB_VMODE_CONUPDATE      512

#define FB_ROTATE_UR            0
#define FB_ROTATE_CW            1
#define FB_ROTATE_UD            2
#define FB_ROTATE_CCW           3

#define VESA_NO_BLANKING        0
#define VESA_VSYNC_SUSPEND      1
#define VESA_HSYNC_SUSPEND      2
#define VESA_POWERDOWN          3

#define PICOS2KHZ(a)            (1000000000UL / (a))
#define KHZ2PICOS(a)            (1000000000UL / (a))

struct fb_fix_screeninfo {
    char id[16];

    uintptr_t smem_start;
    __u32 smem_len;
    __u32 line_length;
    __u32 type;
    __u32 visual;
};

struct fb_bitfield {
    __u32 offset;
    __u32 length;
    __u32 msb_right;
};

struct fb_var_screeninfo {
    __u32 xres;
    __u32 yres;
    __u32 xres_virtual;
    __u32 yres_virtual;
    __u32 xoffset;
    __u32 yoffset;

    __u32 bits_per_pixel;
    __u32 grayscale;
    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;
    struct fb_bitfield transp;

    __u32 nonstd;
    __u32 activate;
    __u32 height;
    __u32 width;
    __u32 fps;

    // timing
    __u32 pixclock;
    __u32 left_margin;
    __u32 right_margin;
    __u32 upper_margin;
    __u32 lower_margin;
    __u32 hsync_len;
    __u32 vsync_len;
    __u32 sync;
    __u32 vmode;
    __u32 rotate;
    __u32 colorspace;
};

struct fb_cmap {
    __u32 start;
    __u32 len;
    __u16 *red;
    __u16 *green;
    __u16 *blue;
    __u16 *transp;
};

#ifdef __cplusplus
}
#endif

#endif
