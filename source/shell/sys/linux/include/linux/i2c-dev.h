#ifndef __VSF_LINUX_I2C_DEV_H__
#define __VSF_LINUX_I2C_DEV_H__

#ifdef __cplusplus
extern "C" {
#endif

// for ioctrl
#define I2C_SLAVE               (('i' << 8) | 0)
#define I2C_SLAVE_FORCE         I2C_SLAVE
#define I2C_TENBIT              (('i' << 8) | 1)
#define I2C_FUNCS               (('i' << 8) | 2)
#define I2C_RDWR                (('i' << 8) | 3)

#ifdef __cplusplus
}
#endif

#endif
