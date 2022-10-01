#ifndef __VSF_LINUX_SPI_H__
#define __VSF_LINUX_SPI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_CPHA                    0x01
#define SPI_CPOL                    0x02

#define SPI_MODE_0                  (0 | 0)
#define SPI_MODE_1                  (0 | SPI_CPHA)
#define SPI_MODE_2                  (SPI_CPOL | 0)
#define SPI_MODE_3                  (SPI_CPOL | SPI_CPHA)

#define SPI_CS_HIGH                 0x04
#define SPI_LSB_FIRST               0x08
#define SPI_NO_CS                   0x40
#define SPI_READY                   0x80

// IOCTL
#define SPI_IOC_MESSAGE(__N)        (('s' << 8) | (0x10 + (__N)))

#define SPI_IOC_RD_MODE             (('s' << 8) | 1)
#define SPI_IOC_WR_MODE             (('s' << 8) | 1)

#define SPI_IOC_RD_LSB_FIRST        (('s' << 8) | 2)
#define SPI_IOC_WR_LSB_FIRST        (('s' << 8) | 2)

#define SPI_IOC_RD_BITS_PER_WORD    (('s' << 8) | 3)
#define SPI_IOC_WR_BITS_PER_WORD    (('s' << 8) | 3)

#define SPI_IOC_RD_MAX_SPEED_HZ     (('s' << 8) | 4)
#define SPI_IOC_WR_MAX_SPEED_HZ     (('s' << 8) | 4)

struct spi_ioc_transfer {
    uintptr_t tx_buf;
    uintptr_t rx_buf;
    uint32_t len;
    uint32_t speed_hz;
    uint16_t delay_usecs;
    uint8_t bits_per_word;
    uint8_t cs_change;
};

#ifdef __cplusplus
}
#endif

#endif
