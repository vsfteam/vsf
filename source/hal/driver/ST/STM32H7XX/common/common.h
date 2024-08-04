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

#ifndef __HAL_DRIVER_ST_STM32H7XX_COMMON_H__
#define __HAL_DRIVER_ST_STM32H7XX_COMMON_H__

/* \note __common.h should only be included by device.h */

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/

// CLOCK

#define VSF_HW_REG32_REGION(__WORD_OFFSET, __BIT_OFFSET, __BIT_LENGTH)          \
            (((__WORD_OFFSET) << 16) | ((__BIT_LENGTH) << 8) | ((__BIT_OFFSET) << 0))

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_hw_peripheral_clk_set       vsf_hw_reg_region_set
#define vsf_hw_peripheral_clk_get       vsf_hw_reg_region_get

#define vsf_hw_peripheral_rst_set       vsf_hw_reg_region_set_bit
#define vsf_hw_peripheral_rst_clear     vsf_hw_reg_region_clear_bit
#define vsf_hw_peripheral_rst_get       vsf_hw_reg_region_get_bit

#define vsf_hw_peripheral_enable        vsf_hw_reg_region_set_bit
#define vsf_hw_peripheral_disable       vsf_hw_reg_region_clear_bit

/*============================ TYPES =========================================*/

typedef enum vsf_hw_peripheral_clk_t {
    VSF_HW_CLK_USART16                  = VSF_HW_REG32_REGION(0x15, 3, 3),  // USART16SEL IN RCC.D2CCIP2R
    VSF_HW_CLK_USART234578              = VSF_HW_REG32_REGION(0x15, 0, 3),  // USART234578SEL IN RCC.D2CCIP2R
} vsf_hw_peripheral_clk_t;

typedef enum vsf_hw_peripheral_rst_t {
    VSF_HW_RST_USART1                   = VSF_HW_REG32_REGION(0x26, 4, 1),  // USART1RST IN RCC.APB2RSTR
    VSF_HW_RST_USART2                   = VSF_HW_REG32_REGION(0x24, 17, 1), // USART2RST IN RCC.APB1LRSTR
    VSF_HW_RST_USART3                   = VSF_HW_REG32_REGION(0x24, 18, 1), // USART3RST IN RCC.APB1LRSTR
    VSF_HW_RST_USART4                   = VSF_HW_REG32_REGION(0x24, 19, 1), // USART4RST IN RCC.APB1LRSTR
    VSF_HW_RST_USART5                   = VSF_HW_REG32_REGION(0x24, 20, 1), // USART5RST IN RCC.APB1LRSTR
    VSF_HW_RST_USART6                   = VSF_HW_REG32_REGION(0x26, 5, 1),  // USART6RST IN RCC.APB2RSTR
    VSF_HW_RST_USART7                   = VSF_HW_REG32_REGION(0x24, 30, 1), // USART7RST IN RCC.APB1LRSTR
    VSF_HW_RST_USART8                   = VSF_HW_REG32_REGION(0x24, 31, 1), // USART8RST IN RCC.APB1LRSTR
} vsf_hw_peripheral_rst_t;

typedef enum vsf_hw_peripheral_en_t {
    VSF_HW_EN_GPIOA                     = VSF_HW_REG32_REGION(0x38, 0, 1),  // GPIOAEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOB                     = VSF_HW_REG32_REGION(0x38, 1, 1),  // GPIOBEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOC                     = VSF_HW_REG32_REGION(0x38, 2, 1),  // GPIOCEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOD                     = VSF_HW_REG32_REGION(0x38, 3, 1),  // GPIODEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOE                     = VSF_HW_REG32_REGION(0x38, 4, 1),  // GPIOEEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOF                     = VSF_HW_REG32_REGION(0x38, 5, 1),  // GPIOFEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOG                     = VSF_HW_REG32_REGION(0x38, 6, 1),  // GPIOGEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOH                     = VSF_HW_REG32_REGION(0x38, 7, 1),  // GPIOHEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOI                     = VSF_HW_REG32_REGION(0x38, 8, 1),  // GPIOIEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOJ                     = VSF_HW_REG32_REGION(0x38, 9, 1),  // GPIOJEN IN RCC.AHB4ENR
    VSF_HW_EN_GPIOK                     = VSF_HW_REG32_REGION(0x38, 10, 1), // GPIOKEN IN RCC.AHB4ENR

    VSF_HW_EN_USART1                    = VSF_HW_REG32_REGION(0x3C, 4, 1),  // USART1EN IN RCC.APB2ENR
    VSF_HW_EN_USART2                    = VSF_HW_REG32_REGION(0x3A, 17, 1), // USART2EN IN RCC.APB1LENR
    VSF_HW_EN_USART3                    = VSF_HW_REG32_REGION(0x3A, 18, 1), // USART3EN IN RCC.APB1LENR
    VSF_HW_EN_USART4                    = VSF_HW_REG32_REGION(0x3A, 19, 1), // USART4EN IN RCC.APB1LENR
    VSF_HW_EN_USART5                    = VSF_HW_REG32_REGION(0x3A, 20, 1), // USART5EN IN RCC.APB1LENR
    VSF_HW_EN_USART6                    = VSF_HW_REG32_REGION(0x3C, 5, 1),  // USART6EN IN RCC.APB2ENR
    VSF_HW_EN_USART7                    = VSF_HW_REG32_REGION(0x3A, 30, 1), // USART7EN IN RCC.APB1LENR
    VSF_HW_EN_USART8                    = VSF_HW_REG32_REGION(0x3A, 31, 1), // USART8EN IN RCC.APB1LENR
} vsf_hw_peripheral_en_t;

enum {
    VSF_HW_CLK_PRESCALER_DIV,
    VSF_HW_CLK_PRESCALER_ADD1_DIV,
    VSF_HW_CLK_PRESCALER_SFT,
    VSF_HW_CLK_PRESCALER_FUNC,
};
enum {
    VSF_HW_CLK_TYPE_CONST,
    VSF_HW_CLK_TYPE_CLK,
    VSF_HW_CLK_TYPE_SEL,
};

typedef struct vsf_hw_clk_t vsf_hw_clk_t;
struct vsf_hw_clk_t {
    uint32_t clksel_region;
    uint32_t clkprescaler_region;
    uint32_t clken_region;
    uint32_t clkrdy_region;

    union {
        const vsf_hw_clk_t **clksel_mapper;
        uint32_t clk_freq_hz;
        const vsf_hw_clk_t *clksrc;
    };
    union {
        const uint8_t *clkprescaler_mapper;
        uint32_t (*getclk)(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler);
    };

    uint8_t clktype;
    uint8_t clkprescaler_type;
    uint8_t clkprescaler_min;
    uint8_t clkprescaler_max;
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_hw_clk_t VSF_HW_CLK_HSI;
extern const vsf_hw_clk_t VSF_HW_CLK_HSI_KER;
extern const vsf_hw_clk_t VSF_HW_CLK_HSI48;
extern const vsf_hw_clk_t VSF_HW_CLK_LSI;
extern const vsf_hw_clk_t VSF_HW_CLK_CSI;
extern const vsf_hw_clk_t VSF_HW_CLK_CSI_KER;
extern const vsf_hw_clk_t VSF_HW_CLK_HSE;
extern const vsf_hw_clk_t VSF_HW_CLK_LSE;

extern const vsf_hw_clk_t VSF_HW_CLK_PLL1_P;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL1_Q;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL1_R;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_P;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_Q;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL2_R;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3_P;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3_Q;
extern const vsf_hw_clk_t VSF_HW_CLK_PLL3_R;

extern const vsf_hw_clk_t VSF_HW_CLK_SYS;
extern const vsf_hw_clk_t VSF_HW_CLK_SYSD1;
extern const vsf_hw_clk_t VSF_HW_CLK_HCLK;
extern const vsf_hw_clk_t VSF_HW_CLK_PER;

extern const vsf_hw_clk_t VSF_HW_CLK_PCLK1;
extern const vsf_hw_clk_t VSF_HW_CLK_PCLK2;
extern const vsf_hw_clk_t VSF_HW_CLK_PCLK3;
extern const vsf_hw_clk_t VSF_HW_CLK_PCLK4;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern uint32_t vsf_hw_clk_get(const vsf_hw_clk_t *clk);

extern void vsf_hw_reg_region_set(uint32_t region, uint_fast8_t value);
extern uint_fast8_t vsf_hw_reg_region_get(uint32_t region);

extern void vsf_hw_reg_region_set_bit(uint32_t region);
extern void vsf_hw_reg_region_clear_bit(uint32_t region);
extern uint_fast8_t vsf_hw_reg_region_get_bit(uint32_t region);

#endif
/* EOF */
