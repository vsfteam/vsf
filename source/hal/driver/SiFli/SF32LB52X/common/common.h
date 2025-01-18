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

#ifndef __HAL_DRIVER_SIFLI_SF32LB52X_COMMON_H__
#define __HAL_DRIVER_SIFLI_SF32LB52X_COMMON_H__

/* \note common.h should only be included by device.h */

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/

#define SWI0_IRQn                   Interrupt23_IRQn
#define SWI1_IRQn                   Interrupt24_IRQn
#define SWI2_IRQn                   Interrupt25_IRQn
#define SWI3_IRQn                   Interrupt26_IRQn
#define SWI4_IRQn                   Interrupt27_IRQn
#define SWI5_IRQn                   Interrupt28_IRQn
#define SWI6_IRQn                   Interrupt29_IRQn
#define SWI7_IRQn                   Interrupt30_IRQn
#define SWI8_IRQn                   Interrupt31_IRQn
#define SWI9_IRQn                   Interrupt32_IRQn
#define SWI10_IRQn                  Interrupt33_IRQn
#define SWI11_IRQn                  Interrupt34_IRQn
#define SWI12_IRQn                  Interrupt35_IRQn
#define SWI13_IRQn                  Interrupt36_IRQn
#define SWI14_IRQn                  Interrupt37_IRQn
#define SWI15_IRQn                  Interrupt38_IRQn
#define SWI16_IRQn                  Interrupt39_IRQn
#define SWI17_IRQn                  Interrupt40_IRQn
#define SWI18_IRQn                  Interrupt41_IRQn
#define SWI19_IRQn                  Interrupt42_IRQn
#define SWI20_IRQn                  Interrupt43_IRQn
#define SWI21_IRQn                  Interrupt44_IRQn
#define SWI22_IRQn                  Interrupt45_IRQn
#define SWI23_IRQn                  Interrupt80_IRQn
#define SWI24_IRQn                  Interrupt81_IRQn
#define SWI25_IRQn                  Interrupt83_IRQn
#define SWI26_IRQn                  Interrupt87_IRQn
#define SWI27_IRQn                  Interrupt88_IRQn
#define SWI28_IRQn                  Interrupt97_IRQn

// CLK & RST REGION

#define VSF_HW_CLKRST_REGION(__WORD_OFFSET, __BIT_OFFSET, __BIT_LENGTH)         \
            (((__WORD_OFFSET) << 16) | ((__BIT_LENGTH) << 8) | ((__BIT_OFFSET) << 0))

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_hw_peripheral_rst_set       vsf_hw_clkrst_region_set_bit
#define vsf_hw_peripheral_rst_clear     vsf_hw_clkrst_region_clear_bit
#define vsf_hw_peripheral_rst_get       vsf_hw_clkrst_region_get_bit

#define vsf_hw_peripheral_enable        vsf_hw_clkrst_region_set_bit
#define vsf_hw_peripheral_disable       vsf_hw_clkrst_region_clear_bit

/*============================ TYPES =========================================*/

typedef enum vsf_hw_peripheral_rst_t {
#ifdef SOC_BF0_HCPU
    // RCC.RSTR1
    VSF_HW_RST_USART1                   = VSF_HW_CLKRST_REGION(0x00, 1, 1),
    VSF_HW_RST_USART2                   = VSF_HW_CLKRST_REGION(0x00, 2, 1),
    VSF_HW_RST_USART3                   = VSF_HW_CLKRST_REGION(0x00, 3, 1),
    VSF_HW_RST_USART4                   = VSF_HW_CLKRST_REGION(0x00, 4, 1),
    VSF_HW_RST_SPI1                     = VSF_HW_CLKRST_REGION(0x00, 5, 1),
    VSF_HW_RST_SPI2                     = VSF_HW_CLKRST_REGION(0x00, 6, 1),
    VSF_HW_RST_SPI3                     = VSF_HW_CLKRST_REGION(0x00, 7, 1),
    VSF_HW_RST_I2C1                     = VSF_HW_CLKRST_REGION(0x00, 8, 1),
    VSF_HW_RST_I2C2                     = VSF_HW_CLKRST_REGION(0x00, 9, 1),
    VSF_HW_RST_I2C3                     = VSF_HW_CLKRST_REGION(0x00, 10, 1),
    VSF_HW_RST_GPTIM1                   = VSF_HW_CLKRST_REGION(0x00, 11, 1),
    VSF_HW_RST_GPTIM2                   = VSF_HW_CLKRST_REGION(0x00, 12, 1),
    VSF_HW_RST_GPTIM3                   = VSF_HW_CLKRST_REGION(0x00, 13, 1),
    VSF_HW_RST_GPIO                     = VSF_HW_CLKRST_REGION(0x00, 14, 1),
    VSF_HW_RST_DMAC1                    = VSF_HW_CLKRST_REGION(0x00, 15, 1),
    VSF_HW_RST_GPDAC1                   = VSF_HW_CLKRST_REGION(0x00, 16, 1),
    VSF_HW_RST_GPDAC2                   = VSF_HW_CLKRST_REGION(0x00, 17, 1),
    VSF_HW_RST_BTIM1                    = VSF_HW_CLKRST_REGION(0x00, 18, 1),
    VSF_HW_RST_BTIM2                    = VSF_HW_CLKRST_REGION(0x00, 19, 1),
    VSF_HW_RST_LPTIM                    = VSF_HW_CLKRST_REGION(0x00, 20, 1),
    VSF_HW_RST_LPUART                   = VSF_HW_CLKRST_REGION(0x00, 21, 1),

    // RCC.RSTR2
    VSF_HW_RST_LCDC                     = VSF_HW_CLKRST_REGION(0x01, 0, 1),
    VSF_HW_RST_PSRAMC                   = VSF_HW_CLKRST_REGION(0x01, 1, 1),
    VSF_HW_RST_TRNG                     = VSF_HW_CLKRST_REGION(0x01, 2, 1),
    VSF_HW_RST_AES                      = VSF_HW_CLKRST_REGION(0x01, 3, 1),
    VSF_HW_RST_CRC                      = VSF_HW_CLKRST_REGION(0x01, 4, 1),
    VSF_HW_RST_WDT1                     = VSF_HW_CLKRST_REGION(0x01, 6, 1),
    VSF_HW_RST_I2S                      = VSF_HW_CLKRST_REGION(0x01, 7, 1),
    VSF_HW_RST_EPIC                     = VSF_HW_CLKRST_REGION(0x01, 8, 1),
    VSF_HW_RST_KEYPAD                   = VSF_HW_CLKRST_REGION(0x01, 9, 1),
    VSF_HW_RST_USBC                     = VSF_HW_CLKRST_REGION(0x01, 10, 1),
    VSF_HW_RST_GPDAC                    = VSF_HW_CLKRST_REGION(0x01, 11, 1),
    VSF_HW_RST_MAILBOX                  = VSF_HW_CLKRST_REGION(0x01, 12, 1),
    VSF_HW_RST_SYSCFG                   = VSF_HW_CLKRST_REGION(0x01, 13, 1),
    VSF_HW_RST_FLASHC                   = VSF_HW_CLKRST_REGION(0x01, 14, 1),
    VSF_HW_RST_DMAC2                    = VSF_HW_CLKRST_REGION(0x01, 15, 1),
    VSF_HW_RST_ATIM1                    = VSF_HW_CLKRST_REGION(0x01, 16, 1),
    VSF_HW_RST_ATIM2                    = VSF_HW_CLKRST_REGION(0x01, 17, 1),
    VSF_HW_RST_BTIM3                    = VSF_HW_CLKRST_REGION(0x01, 18, 1),
    VSF_HW_RST_BTIM4                    = VSF_HW_CLKRST_REGION(0x01, 19, 1),
    VSF_HW_RST_GPTIM4                   = VSF_HW_CLKRST_REGION(0x01, 20, 1),
    VSF_HW_RST_GPTIM5                   = VSF_HW_CLKRST_REGION(0x01, 21, 1),
    VSF_HW_RST_EXTDMA                   = VSF_HW_CLKRST_REGION(0x01, 22, 1),
    VSF_HW_RST_NNACC                    = VSF_HW_CLKRST_REGION(0x01, 28, 1),
    VSF_HW_RST_GAU                      = VSF_HW_CLKRST_REGION(0x01, 29, 1),
    VSF_HW_RST_WDT2                     = VSF_HW_CLKRST_REGION(0x01, 30, 1),
    VSF_HW_RST_ACPU                     = VSF_HW_CLKRST_REGION(0x01, 31, 1),
#endif
} vsf_hw_peripheral_rst_t;

typedef enum vsf_hw_peripheral_en_t {
#ifdef SOC_BF0_HCPU
    // RCC.ENR1
    VSF_HW_EN_USART1                    = VSF_HW_CLKRST_REGION(0x00, 1, 1),
    VSF_HW_EN_USART2                    = VSF_HW_CLKRST_REGION(0x00, 2, 1),
    VSF_HW_EN_USART3                    = VSF_HW_CLKRST_REGION(0x00, 3, 1),
    VSF_HW_EN_USART4                    = VSF_HW_CLKRST_REGION(0x00, 4, 1),
    VSF_HW_EN_SPI1                      = VSF_HW_CLKRST_REGION(0x00, 5, 1),
    VSF_HW_EN_SPI2                      = VSF_HW_CLKRST_REGION(0x00, 6, 1),
    VSF_HW_EN_SPI3                      = VSF_HW_CLKRST_REGION(0x00, 7, 1),
    VSF_HW_EN_I2C1                      = VSF_HW_CLKRST_REGION(0x00, 8, 1),
    VSF_HW_EN_I2C2                      = VSF_HW_CLKRST_REGION(0x00, 9, 1),
    VSF_HW_EN_I2C3                      = VSF_HW_CLKRST_REGION(0x00, 10, 1),
    VSF_HW_EN_GPTIM1                    = VSF_HW_CLKRST_REGION(0x00, 11, 1),
    VSF_HW_EN_GPTIM2                    = VSF_HW_CLKRST_REGION(0x00, 12, 1),
    VSF_HW_EN_GPTIM3                    = VSF_HW_CLKRST_REGION(0x00, 13, 1),
    VSF_HW_EN_GPIO                      = VSF_HW_CLKRST_REGION(0x00, 14, 1),
    VSF_HW_EN_DMAC1                     = VSF_HW_CLKRST_REGION(0x00, 15, 1),
    VSF_HW_EN_GPDAC1                    = VSF_HW_CLKRST_REGION(0x00, 16, 1),
    VSF_HW_EN_GPDAC2                    = VSF_HW_CLKRST_REGION(0x00, 17, 1),
    VSF_HW_EN_BTIM1                     = VSF_HW_CLKRST_REGION(0x00, 18, 1),
    VSF_HW_EN_BTIM2                     = VSF_HW_CLKRST_REGION(0x00, 19, 1),
    VSF_HW_EN_LPTIM                     = VSF_HW_CLKRST_REGION(0x00, 20, 1),
    VSF_HW_EN_LPUART                    = VSF_HW_CLKRST_REGION(0x00, 21, 1),

    // RCC.ENR2
    VSF_HW_EN_LCDC                      = VSF_HW_CLKRST_REGION(0x01, 0, 1),
    VSF_HW_EN_PSRAMC                    = VSF_HW_CLKRST_REGION(0x01, 1, 1),
    VSF_HW_EN_TRNG                      = VSF_HW_CLKRST_REGION(0x01, 2, 1),
    VSF_HW_EN_AES                       = VSF_HW_CLKRST_REGION(0x01, 3, 1),
    VSF_HW_EN_CRC                       = VSF_HW_CLKRST_REGION(0x01, 4, 1),
    VSF_HW_EN_WDT1                      = VSF_HW_CLKRST_REGION(0x01, 6, 1),
    VSF_HW_EN_I2S                       = VSF_HW_CLKRST_REGION(0x01, 7, 1),
    VSF_HW_EN_EPIC                      = VSF_HW_CLKRST_REGION(0x01, 8, 1),
    VSF_HW_EN_KEYPAD                    = VSF_HW_CLKRST_REGION(0x01, 9, 1),
    VSF_HW_EN_USBC                      = VSF_HW_CLKRST_REGION(0x01, 10, 1),
    VSF_HW_EN_GPDAC                     = VSF_HW_CLKRST_REGION(0x01, 11, 1),
    VSF_HW_EN_MAILBOX                   = VSF_HW_CLKRST_REGION(0x01, 12, 1),
    VSF_HW_EN_SYSCFG                    = VSF_HW_CLKRST_REGION(0x01, 13, 1),
    VSF_HW_EN_FLASHC                    = VSF_HW_CLKRST_REGION(0x01, 14, 1),
    VSF_HW_EN_DMAC2                     = VSF_HW_CLKRST_REGION(0x01, 15, 1),
    VSF_HW_EN_ATIM1                     = VSF_HW_CLKRST_REGION(0x01, 16, 1),
    VSF_HW_EN_ATIM2                     = VSF_HW_CLKRST_REGION(0x01, 17, 1),
    VSF_HW_EN_BTIM3                     = VSF_HW_CLKRST_REGION(0x01, 18, 1),
    VSF_HW_EN_BTIM4                     = VSF_HW_CLKRST_REGION(0x01, 19, 1),
    VSF_HW_EN_GPTIM4                    = VSF_HW_CLKRST_REGION(0x01, 20, 1),
    VSF_HW_EN_GPTIM5                    = VSF_HW_CLKRST_REGION(0x01, 21, 1),
    VSF_HW_EN_EXTDMA                    = VSF_HW_CLKRST_REGION(0x01, 22, 1),
    VSF_HW_EN_NNACC                     = VSF_HW_CLKRST_REGION(0x01, 28, 1),
    VSF_HW_EN_GAU                       = VSF_HW_CLKRST_REGION(0x01, 29, 1),
    VSF_HW_EN_WDT2                      = VSF_HW_CLKRST_REGION(0x01, 30, 1),
    VSF_HW_EN_ACPU                      = VSF_HW_CLKRST_REGION(0x01, 31, 1),
#endif
} vsf_hw_peripheral_en_t;

typedef struct vsf_hw_clk_t vsf_hw_clk_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_hw_clkrst_region_set_bit(uint32_t region);
extern void vsf_hw_clkrst_region_clear_bit(uint32_t region);
extern uint_fast8_t vsf_hw_clkrst_region_get_bit(uint32_t region);

#endif
/* EOF */
