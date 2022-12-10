#ifndef __VSF_LINUX_I2C_DEV_H__
#define __VSF_LINUX_I2C_DEV_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// for ioctrl
#define I2C_SLAVE               (('i' << 8) | 0)
#define I2C_SLAVE_FORCE         I2C_SLAVE
#define I2C_TENBIT              (('i' << 8) | 1)
#define I2C_FUNCS               (('i' << 8) | 2)
#define I2C_RDWR                (('i' << 8) | 3)
#define I2C_SMBUS               (('i' << 8) | 4)

struct i2c_smbus_ioctl_data {
    __u8 read_write;
    __u8 command;
    __u32 size;
    union i2c_smbus_data *data;
};

struct i2c_rdwr_ioctl_data {
    struct i2c_msg *msgs;
    __u32 nmsgs;
};

#ifdef __cplusplus
}
#endif

#endif
