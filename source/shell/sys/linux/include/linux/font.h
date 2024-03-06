#ifndef __VSF_LINUX_FONT_H__
#define __VSF_LINUX_FONT_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FONT_EXTRA_WORDS            4

#define VGA8x8_IDX                  0
#define VGA8x16_IDX                 1

struct font_desc {
    int idx;
    const char *name;
    unsigned int width, height, charcount;
    const void *data;
    int pref;
};

struct font_data {
    unsigned int extra[FONT_EXTRA_WORDS];
    const unsigned char data[];
} VSF_CAL_PACKED;

typedef struct vsf_linux_font_vplt_t {
    vsf_vplt_info_t info;

    // self
    VSF_APPLET_VPLT_ENTRY_MOD_DEF(linux_font);

    VSF_APPLET_VPLT_ENTRY_VAR_DEF(font_vga_8x8);
    VSF_APPLET_VPLT_ENTRY_VAR_DEF(font_vga_8x16);
} vsf_linux_font_vplt_t;

#ifdef __cplusplus
}
#endif

#endif
