/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/
#ifndef __HAL_DRIVER_NUVOTON_M480_DMA_H__
#define __HAL_DRIVER_NUVOTON_M480_DMA_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"
/*============================ MACROS ========================================*/
 #define M484_DMA_REQUEST_MAX_SIZE    65536

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct m484_dma_cfg_t {
    vsf_arch_prio_t prio;
} m484_dma_cfg_t;

typedef enum m484_dma_transfer_mode_t {
    M484_DMA_MEM                        = 0UL,
    M484_DMA_USB_TX                     = 2UL,
    M484_DMA_USB_RX                     = 3UL,
    M484_DMA_UART0_TX                   = 4UL,
    M484_DMA_UART0_RX                   = 5UL,
    M484_DMA_UART1_TX                   = 6UL,
    M484_DMA_UART1_RX                   = 7UL,
    M484_DMA_UART2_TX                   = 8UL,
    M484_DMA_UART2_RX                   = 9UL,
    M484_DMA_UART3_TX                   = 10UL,
    M484_DMA_UART3_RX                   = 11UL,
    M484_DMA_UART4_TX                   = 12UL,
    M484_DMA_UART4_RX                   = 13UL,
    M484_DMA_UART5_TX                   = 14UL,
    M484_DMA_UART5_RX                   = 15UL,
    M484_DMA_USCI0_TX                   = 16UL,
    M484_DMA_USCI0_RX                   = 17UL,
    M484_DMA_USCI1_TX                   = 18UL,
    M484_DMA_USCI1_RX                   = 19UL,
    M484_DMA_QSPI0_TX                   = 20UL,
    M484_DMA_QSPI0_RX                   = 21UL,
    M484_DMA_SPI0_TX                    = 22UL,
    M484_DMA_SPI0_RX                    = 23UL,
    M484_DMA_SPI1_TX                    = 24UL,
    M484_DMA_SPI1_RX                    = 25UL,
    M484_DMA_SPI2_TX                    = 26UL,
    M484_DMA_SPI2_RX                    = 27UL,
    M484_DMA_SPI3_TX                    = 28UL,
    M484_DMA_SPI3_RX                    = 29UL,
    M484_DMA_QSPI1_TX                   = 30UL,
    M484_DMA_QSPI1_RX                   = 31UL,
    M484_DMA_EPWM0_P1_RX                = 32UL,
    M484_DMA_EPWM0_P2_RX                = 33UL,
    M484_DMA_EPWM0_P3_RX                = 34UL,
    M484_DMA_EPWM1_P1_RX                = 35UL,
    M484_DMA_EPWM1_P2_RX                = 36UL,
    M484_DMA_EPWM1_P3_RX                = 37UL,
    M484_DMA_I2C0_TX                    = 38UL,
    M484_DMA_I2C0_RX                    = 39UL,
    M484_DMA_I2C1_TX                    = 40UL,
    M484_DMA_I2C1_RX                    = 41UL,
    M484_DMA_I2C2_TX                    = 42UL,
    M484_DMA_I2C2_RX                    = 43UL,
    M484_DMA_I2S0_TX                    = 44UL,
    M484_DMA_I2S0_RX                    = 45UL,
    M484_DMA_TMR0                       = 46UL,
    M484_DMA_TMR1                       = 47UL,
    M484_DMA_TMR2                       = 48UL,
    M484_DMA_TMR3                       = 49UL,
    M484_DMA_EADC0_RX                   = 50UL,
    M484_DMA_DAC0_TX                    = 51UL,
    M484_DMA_DAC1_TX                    = 52UL,
    M484_DMA_EPWM0_CH0_TX               = 53UL,
    M484_DMA_EPWM0_CH1_TX               = 54UL,
    M484_DMA_EPWM0_CH2_TX               = 55UL,
    M484_DMA_EPWM0_CH3_TX               = 56UL,
    M484_DMA_EPWM0_CH4_TX               = 57UL,
    M484_DMA_EPWM0_CH5_TX               = 58UL,
    M484_DMA_EPWM1_CH0_TX               = 59UL,
    M484_DMA_EPWM1_CH1_TX               = 60UL,
    M484_DMA_EPWM1_CH2_TX               = 61UL,
    M484_DMA_EPWM1_CH3_TX               = 62UL,
    M484_DMA_EPWM1_CH4_TX               = 63UL,
    M484_DMA_EPWM1_CH5_TX               = 64UL,
    M484_DMA_UART6_TX                   = 66UL,
    M484_DMA_UART6_RX                   = 67UL,
    M484_DMA_UART7_TX                   = 68UL,
    M484_DMA_UART7_RX                   = 69UL,
    M484_DMA_EADC1_RX                   = 70UL,

    M484_DMA_TRANSFER_WIDTH_8_BIT       = 0 << PDMA_DSCT_CTL_TXWIDTH_Pos,
    M484_DMA_TRANSFER_WIDTH_16_BIT      = 1 << PDMA_DSCT_CTL_TXWIDTH_Pos,
    M484_DMA_TRANSFER_WIDTH_32_BIT      = 2 << PDMA_DSCT_CTL_TXWIDTH_Pos,

    M484_DMA_TRANSFER_SOURCE_INC        = 0 << PDMA_DSCT_CTL_SAINC_Pos,
    M484_DMA_TRANSFER_SOURCE_FIXED      = 3 << PDMA_DSCT_CTL_SAINC_Pos,

    M484_DMA_TRANSFER_DESTINATION_INC   = 0 << PDMA_DSCT_CTL_DAINC_Pos,
    M484_DMA_TRANSFER_DESTINATION_FIXED = 3 << PDMA_DSCT_CTL_DAINC_Pos,



    // TODO: support brust mode; In M484, brust only work for MEM to MEM
} m484_dma_transfer_mode_t;

typedef enum m484_dma_irq_mask_t {
    M484_DMA_TRANSFER_DONE_INTERRUPT    = 1 << 0,
    /*
    TODO: add talbe and timeout interrupt support
    M484_DMA_TABLE_EMPTY_INTERRUPT      = 1 << 1,
    M484_DMA_TIMEOUT_INTERRUPT          = 1 << 2,
    */
} m484_dma_irq_mask_t;

typedef void m484_dma_isr_handler_t(void *target_ptr, uint32_t irq_mask);

typedef struct m484_dma_isr_t {
    m484_dma_isr_handler_t      *handler_fn;
    void                        *target_ptr;
} m484_dma_isr_t;

typedef struct m484_dma_channel_cfg_t {
    uint32_t                 mode;
    m484_dma_isr_t           isr;
    void                     *src_address;
    void                     *dst_address;
    uint32_t                 count;
} m484_dma_channel_cfg_t;

typedef struct m484_dma_channel_t {
    uint16_t count;
    m484_dma_isr_t isr;
    m484_dma_irq_mask_t irq_mask;
} m484_dma_channel_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

vsf_err_t m484_dma_init(m484_dma_cfg_t *cfg_ptr);

vsf_err_t m484_dma_channel_config(uint8_t channel, m484_dma_channel_cfg_t *cfg_ptr);
bool m484_dma_channel_is_done(uint8_t channel);
vsf_err_t m484_dma_cancel_transfer(int8_t channel);
int_fast32_t m484_dma_get_transfered_count(int8_t channel);

vsf_err_t m484_dma_channel_irq_enable(uint8_t channel, uint32_t irq_mask);
vsf_err_t m484_dma_channel_irq_disable(uint8_t channel, uint32_t irq_mask);




#endif
