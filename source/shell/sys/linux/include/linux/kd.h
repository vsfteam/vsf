#ifndef __VSF_LINUX_KD_H__
#define __VSF_LINUX_KD_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define K_RAW                   0
#define K_XLATE                 1
#define K_MEDIUMRAW             2
#define K_UNICODE               3
#define K_OFF                   4

#define KDSETMODE               (('K' << 8) | 0)
#   define KD_TEXT              0
#   define KD_GRAPHICS          1
#define KDGETMODE               (('K' << 8) | 1)
#define KDGKBMODE               (('K' << 8) | 2)
#define KDSKBMODE               (('K' << 8) | 3)
#define KDGETLED                (('K' << 8) | 4)
#   define LED_CAP              0x04
#   define LED_NUM              0x02
#   define LED_SCR              0x01
#define KDGKBENT                (('K' << 8) | 5)
#define KDSKBENT                (('K' << 8) | 6)
#define KDGKBTYPE               (('K' << 8) | 7)
#   define KB_84                1
#   define KB_101               2
#   define KB_OTHER             3

struct kbentry {
    unsigned char kb_table;
    unsigned char kb_index;
    unsigned short kb_value;
};

#ifdef __cplusplus
}
#endif

#endif
