#ifndef __VSF_LINUX_FB_H__
#define __VSF_LINUX_FB_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// for ioctrl
#define FBIOGET_VSCREENINFO     (('f' << 8) | 0)
#define FBIOPUT_VSCREENINFO     (('f' << 8) | 1)
#define FBIOGET_FSCREENINFO     (('f' << 8) | 2)
#define FBIOPAN_DISPLAY         (('f' << 8) | 3)
#define FBIO_WAITFORVSYNC       (('f' << 8) | 4)

struct fb_fix_screeninfo {
    char id[16];

    uintptr_t smem_start;
    uint32_t smem_len;
};

struct fb_bitfield {
    uint32_t offset;
    uint32_t length;
    uint32_t msb_right;
};

struct fb_var_screeninfo {
    uint32_t xres;
    uint32_t yres;
    uint32_t xres_virtual;
    uint32_t yres_virtual;
    uint32_t xoffset;
    uint32_t yoffset;

    uint32_t bits_per_pixel;
    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;
    struct fb_bitfield transp;
};

#ifdef __cplusplus
}
#endif

#endif
