#ifndef __VSF_LINUX_FIRMWARE_H__
#define __VSF_LINUX_FIRMWARE_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct firmware {
    size_t size;
    const u8 *data;
};

extern int request_firmware(const struct firmware **fw, const char *name, struct device *device);
extern void release_firmware(const struct firmware *fw);

#ifdef __cplusplus
}
#endif

#endif
