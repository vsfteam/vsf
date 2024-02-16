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
} PACKED;

#ifdef __cplusplus
}
#endif

#endif
