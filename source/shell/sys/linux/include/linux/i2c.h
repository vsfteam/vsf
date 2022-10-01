#ifndef __VSF_LINUX_I2C_H__
#define __VSF_LINUX_I2C_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// I2C functions
#define I2C_FUNC_I2C            (1UL << 0)
#define I2C_FUNC_10BIT_ADDR     (1UL << 1)
#define I2C_FUNC_NOSTART        (1UL << 4)

struct i2c_msg {
    uint16_t addr;
    uint16_t flags;
#define I2C_M_RD                0x0001
#define I2C_M_TEN               0x0010
#define I2C_M_NOSTART           0x4000
    uint16_t len;
    uint8_t *buf;
};

#ifdef __cplusplus
}
#endif

#endif
