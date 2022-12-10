#ifndef __VSF_LINUX_I2C_H__
#define __VSF_LINUX_I2C_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// I2C functions
#define I2C_FUNC_I2C                    (1UL << 0)
#define I2C_FUNC_10BIT_ADDR             (1UL << 1)
#define I2C_FUNC_SMBUS_PEC              (1UL << 3)
#define I2C_FUNC_NOSTART                (1UL << 4)

#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL  (1UL << 15)
#define I2C_FUNC_SMBUS_QUICK            (1UL << 16)
#define I2C_FUNC_SMBUS_READ_BYTE        (1UL << 17)
#define I2C_FUNC_SMBUS_WRITE_BYTE       (1UL << 18)
#define I2C_FUNC_SMBUS_READ_BYTE_DATA   (1UL << 19)
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA  (1UL << 20)
#define I2C_FUNC_SMBUS_READ_WORD_DATA   (1UL << 21)
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA  (1UL << 22)
#define I2C_FUNC_SMBUS_PROC_CALL        (1UL << 23)
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA  (1UL << 24)
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA (1UL << 25)
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK   (1UL << 26)
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK  (1UL << 27)

#define I2C_FUNC_SMBUS_BYTE             (I2C_FUNC_SMBUS_READ_BYTE | I2C_FUNC_SMBUS_WRITE_BYTE)
#define I2C_FUNC_SMBUS_BYTE_DATA        (I2C_FUNC_SMBUS_READ_BYTE_DATA | I2C_FUNC_SMBUS_WRITE_BYTE_DATA)
#define I2C_FUNC_SMBUS_WORD_DATA        (I2C_FUNC_SMBUS_READ_WORD_DATA | I2C_FUNC_SMBUS_WRITE_WORD_DATA)
#define I2C_FUNC_SMBUS_BLOCK_DATA       (I2C_FUNC_SMBUS_READ_BLOCK_DATA | I2C_FUNC_SMBUS_WRITE_BLOCK_DATA)
#define I2C_FUNC_SMBUS_I2C_BLOCK        (I2C_FUNC_SMBUS_READ_I2C_BLOCK | I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)

#define I2C_FUNC_SMBUS_EMUL             ( I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE\
                                        | I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA\
                                        | I2C_FUNC_SMBUS_PROC_CALL | I2C_FUNC_SMBUS_WRITE_BLOCK_DATA\
                                        | I2C_FUNC_SMBUS_I2C_BLOCK | I2C_FUNC_SMBUS_PEC)

#define I2C_SMBUS_BLOCK_MAX             32
union i2c_smbus_data {
    __u8 byte;
    __u16 word;
    __u8 block[I2C_SMBUS_BLOCK_MAX + 2];
};

#define I2C_SMBUS_READ                  1
#define I2C_SMBUS_WRITE                 0

#define I2C_SMBUS_QUICK                 0
#define I2C_SMBUS_BYTE                  1
#define I2C_SMBUS_BYTE_DATA             2
#define I2C_SMBUS_WORD_DATA             3
#define I2C_SMBUS_PROC_CALL             4
#define I2C_SMBUS_BLOCK_DATA            5
#define I2C_SMBUS_I2C_BLOCK_BROKEN      6
#define I2C_SMBUS_BLOCK_PROC_CALL       7
#define I2C_SMBUS_I2C_BLOCK_DATA        8

struct i2c_msg {
    uint16_t addr;
    uint16_t flags;
#define I2C_M_RD                        0x0001
#define I2C_M_TEN                       0x0010
#define I2C_M_NOSTART                   0x4000
    uint16_t len;
    uint8_t *buf;
};

#ifdef __cplusplus
}
#endif

#endif
