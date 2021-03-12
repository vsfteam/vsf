#ifndef __VSF_SPI_H__
#define __VSF_SPI_H__

#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"
#include "hal/interface/vsf_interface_spi.h"

enum em_spi_mode_t {
    SPI_EN                      = 0x0040,
    SPI_NSS_SET                 = 0x0100,
    
    SPI_MODE_SLAVE              = 0x0000,
    SPI_MODE_MASTER             = 0x0004,
    
    SPI_DIRECTION_2LINES        = 0x0000,
    SPI_DIRECTION_2LINES_RXONLY = 0x0400,
    SPI_DIRECTION_1LINE         = 0x8000,
    
    SPI_POLARITY_LOW            = 0x0000,
    SPI_POLARITY_HIGH           = 0x0002,
    
    SPI_PHASE_1EDGE             = 0x0000,
    SPI_PHASE_2EDGE             = 0x0001,
    
    SPI_NSS_SOFT                = 0x0200,
    SPI_NSS_HARD_INPUT          = 0x0000,
    SPI_NSS_HARD_OUTPUT         = 0x0004,
    
    SPI_FIRSTBIT_MSB            = 0x0000,
    SPI_FIRSTBIT_LSB            = 0x0008,
    
    SPI_CRCCALCULATION_DISABLE  = 0x0000,
    SPI_CRCCALCULATION_ENABLE   = 0x2000,
};

typedef enum {
    SPI_DATASIZE_8BIT           = 0x0000,
    SPI_DATASIZE_16BIT          = 0x0800,
}em_spi_data_size_t;

typedef enum {
    SPI_BAUDRATEPRESCALER_2     = 0x0000,
    SPI_BAUDRATEPRESCALER_4     = 0x0008,
    SPI_BAUDRATEPRESCALER_8     = 0x0010,
    SPI_BAUDRATEPRESCALER_16    = 0x0018,
    SPI_BAUDRATEPRESCALER_32    = 0x0020,
    SPI_BAUDRATEPRESCALER_64    = 0x0028,
    SPI_BAUDRATEPRESCALER_128   = 0x0030,
    SPI_BAUDRATEPRESCALER_256   = 0x0038,
}em_spi_clock_div_t;

typedef enum {
    SPI_IT_TXE                  = 0x0080,
    SPI_IT_RXNE                 = 0x0040,
    SPI_IT_ERR                  = 0x0020,
}em_spi_it_t;

typedef enum {
    SPI_SR_TXE_IS_TRUE          = 0x0002,
    SPI_SR_RXNE_IS_TRUE         = 0x0001,
}em_spi_sr_t;

typedef enum {
    SPI1_CLK_EN                 = 0x1000,
    SPI1_GPIO_CLK_EN            = 0x0004,
    
    SPI2_CLK_EN                 = 0x4000,
    SPI2_GPIO_CLK_EN            = 0x0008,
    
    SPI1_GPIO_CLEAR             = 0x000FFFFFU,
    SPI1_GPIO_MODE              = 0xB4B00000U,
    
    SPI2_GPIO_CLEAR             = 0x000FFFFFU,
    SPI2_GPIO_MODE              = 0x4B400000U,
}en_spi_clk_gpio_t;

struct spi_status_t {
    uint8_t SPI_RXNE   : 1;
    uint8_t SPI_TXE    : 1;
    uint8_t SPI_CHSIDE : 1;
    uint8_t SPI_UDR    : 1;
    uint8_t SPI_CRCERR : 1;
    uint8_t SPI_MODF   : 1;
    uint8_t SPI_OVR    : 1;
    uint8_t SPI_BSY    : 1;
};

struct spi_capability_t {
    implement(peripheral_capability_t);
};

struct vsf_spi_t {
    SPI_TypeDef *hspi;
    uint8_t     *output;
    uint8_t     *input;
    uint32_t    data_size;
    uint32_t    rx_count;
    uint32_t    tx_count;
    bool        data_exchange;
};

extern vsf_spi_t vsf_spi[SPI_COUNT];

extern spi_status_t     vsf_spi_get_status(vsf_spi_t *spi_obj);
extern spi_capability_t vsf_spi_get_capability(void);
extern vsf_err_t        vsf_spi_init(vsf_spi_t *spi_obj, spi_cfg_t *cfg_ptr);
extern uintalu_t        vsf_spi_polarity_set(vsf_spi_t *spi_obj, uintalu_t polarity);
extern uintalu_t        vsf_spi_polarity_get(vsf_spi_t *spi_obj);
extern fsm_rt_t         vsf_spi_exchange(vsf_spi_t *spi_obj, uintalu_t output, void *input);
extern fsm_rt_t         vsf_spi_request_exchange(vsf_spi_t *spi_obj, void *output, void *input, uint_fast32_t size);

#endif