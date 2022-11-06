#ifndef __VSF_LINUX_KSTRTOX_H__
#define __VSF_LINUX_KSTRTOX_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int kstrtou16(const char *s, unsigned int base, u16 *res);
extern int kstrtos16(const char *s, unsigned int base, s16 *res);
extern int kstrtou8(const char *s, unsigned int base, u8 *res);
extern int kstrtos8(const char *s, unsigned int base, s8 *res);

#ifdef __cplusplus
}
#endif

#endif
