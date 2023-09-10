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

#define VESA_NO_BLANKING        0
#define VESA_VSYNC_SUSPEND      1
#define VESA_HSYNC_SUSPEND      2
#define VESA_POWERDOWN          3

struct fb_fix_screeninfo {
    char id[16];

    uintptr_t smem_start;
    __u32 smem_len;
    __u32 line_length;
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
    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;
    struct fb_bitfield transp;

    __u32 height;
    __u32 width;
    __u32 fps;
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
