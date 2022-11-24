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

#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

/*\note first define basic info for arch. */
//! arch info
#   define VSF_ARCH_PRI_NUM         16
#   define VSF_ARCH_PRI_BIT         4

#else

#ifndef __HAL_DEVICE_MT071_H__
#define __HAL_DEVICE_MT071_H__

/*============================ INCLUDES ======================================*/

#include "./common.h"
#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/

// TODO: implement pm
#define __SYSTEM_FREQ               VSF_SYSTIMER_FREQ
#define __PLL_FREQ_HZ               __SYSTEM_FREQ

#define GPIO_COUNT                  7
#define PIO0
#define PIO0_PIN_NUM                8
#define PIO1
#define PIO1_PIN_NUM                8
#define PIO2
#define PIO2_PIN_NUM                8
#define PIO3
#define PIO3_PIN_NUM                8
#define PIO4
#define PIO4_PIN_NUM                8
#define PIO5
#define PIO5_PIN_NUM                8
#define PIO6
#define PIO6_PIN_NUM                8

#define DMA_COUNT                   1
#   define DMA_CHANNEL_COUNT        4
#   define DMA_CHANNEL_MAX_TRANS_SIZES                                          \
                                    1023, 511, 255, 255

#define USART_MAX_PORT              1
#   define USART0_TX_DMA            0
#   define USART0_RX_DMA            1
#   define USART1_TX_DMA            2
#   define USART1_RX_DMA            3

#define USB_OTG_COUNT               1
#define USB_OTG0_IRQHandler         USB_IRQHandler
#define USB_OTG0_EP_NUMBER          8
#define USB_OTG0_CONFIG                                                         \
    .reg                            = (void *)USB_Common_Base,                  \
    .sclk                           = SCLK_USB_IDX,                             \
    .pclk                           = PCLK_USB_IDX,                             \
    .irq                            = USB_IRQn,

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __def_idx(__name, __no)     VSF_MCONNECT2(__name, _IDX) = (__no)
#define __def_msk(__name)           VSF_MCONNECT2(__name, _MSK) = BIT(VSF_MCONNECT2(__name, _IDX) & 0x1F)

// bit0  - bit7 :   clksel
// bit8  - bit15:   clkdiv
// bit16 - bit23:   clkref
#define __def_clk_idx(__name, __clksel, __clkdiv, __clkref)                     \
        VSF_MCONNECT2(__name, _IDX) = ((__clksel) << 0)                         \
                            |   ((__clkdiv) << 8)                               \
                            |   ((__clkref) << 16)

/*============================ TYPES =========================================*/

//! \name power set index
//! @{
enum vsf_pm_power_cfg_no_t {
    __def_idx(POWER_12MIRC, 0),
    __def_idx(POWER_12MOSC, 2),
    __def_idx(POWER_10KIRC, 3),
};
//! @}

//! \name power set mask
//! @{
enum vsf_pm_power_cfg_msk_t {
    __def_msk(POWER_12MIRC),
    __def_msk(POWER_12MOSC),
    __def_msk(POWER_10KIRC),
};
//! @}

//! \name the lowpower mode
//! @{
enum vsf_pm_sleep_mode_t {
    VSF_PM_SLEEP        = 1 << 8,
    VSF_PM_DPD          = 1 << 1,

    VSF_PM_WAIT         = VSF_PM_SLEEP,
    VSF_PM_DEEP_SLEEP   = VSF_PM_DPD,
    VSF_PM_POWER_OFF    = VSF_PM_DPD,
} ;
//! @}

//! \name peripheral async clock index
//! @{
enum vsf_pm_pclk_no_t {
                    // NAME         CLKSEL,         CLKDIV,         CLKREF
    __def_clk_idx(  PCLK_USB,       0x18,           0,              0x0F),
    __def_clk_idx(  PCLK_USART0,    0x16,           0x24,           0),
    __def_clk_idx(  PCLK_USART1,    0x16,           0x25,           0),
    __def_clk_idx(  PCLK_OUTCLK,    0x1E,           0x2B,           0),
    __def_clk_idx(  PCLK_SYSTICK,   0,              0x28,           0),
    __def_clk_idx(  PCLK_SPI0,      0,              0x29,           0),
    __def_clk_idx(  PCLK_ADC,       0,              0x2C,           0),
    __def_clk_idx(  PCLK_SPI1,      0,              0x34,           0),
    __def_clk_idx(  PCLK_DAC,       0,              0x36,           0),
};
//! @}

//! \name Peripheral Sync Clock Macros
//! @{
enum vsf_pm_sclk_no_t {
    __def_idx(  SCLK_FLASH,         0),
    __def_idx(  SCLK_RAM,           1),
    __def_idx(  SCLK_GPIO,          2),
    __def_idx(  SCLK_DMA,           3),
    __def_idx(  SCLK_USART0,        4),
    __def_idx(  SCLK_USART1,        5),
    __def_idx(  SCLK_ROM,           6),
    __def_idx(  SCLK_I2C0,          7),
    __def_idx(  SCLK_ADC,           8),
    __def_idx(  SCLK_IOCFG,         9),
    __def_idx(  SCLK_WDT,           10),
    __def_idx(  SCLK_WPWM,          12),
    __def_idx(  SCLK_SPI0,          13),
    __def_idx(  SCLK_TIMER0,        14),
    __def_idx(  SCLK_TIMER1,        15),
    __def_idx(  SCLK_ACMP,          16),
    __def_idx(  SCLK_TIMER2,        18),
    __def_idx(  SCLK_IWDG,          19),
    __def_idx(  SCLK_SPI1,          22),
    __def_idx(  SCLK_I2C1,          24),
    __def_idx(  SCLK_TIMER3,        25),
    __def_idx(  SCLK_RTC,           30),
    __def_idx(  SCLK_USB,           31),
};

enum vsf_pm_sclk_msk_t {
    __def_msk(SCLK_FLASH),
    __def_msk(SCLK_RAM),
    __def_msk(SCLK_GPIO),
    __def_msk(SCLK_DMA),
    __def_msk(SCLK_USART0),
    __def_msk(SCLK_USART1),
    __def_msk(SCLK_ROM),
    __def_msk(SCLK_I2C0),
    __def_msk(SCLK_ADC),
    __def_msk(SCLK_IOCFG),
    __def_msk(SCLK_WDT),
    __def_msk(SCLK_WPWM),
    __def_msk(SCLK_SPI0),
    __def_msk(SCLK_TIMER0),
    __def_msk(SCLK_TIMER1),
    __def_msk(SCLK_ACMP),
    __def_msk(SCLK_TIMER2),
    __def_msk(SCLK_IWDG),
    __def_msk(SCLK_SPI1),
    __def_msk(SCLK_I2C1),
    __def_msk(SCLK_TIMER3),
    __def_msk(SCLK_RTC),
    __def_msk(SCLK_USB),
};
//! @}

//! @{
enum vsf_pm_clk_src_sel_t {
    CLKSRC_12MIRC           = 0,
    CLKSRC_PLL              = 1,
    CLKSRC_12MOSC           = 2,
    CLKSRC_10KIRC           = 3,

    CLKREF_12MIRC           = 0,
    CLKREF_12MOSC           = 1,

    MAINCLK_CLKSRC_12MIRC   = CLKSRC_12MIRC,
    MAINCLK_CLKSRC_PLL      = CLKSRC_PLL,
    MAINCLK_CLKSRC_12MOSC   = CLKSRC_12MOSC,
    MAINCLK_CLKSRC_10KIRC   = CLKSRC_10KIRC,

    USART_CLKSRC_12MIRC     = CLKSRC_12MIRC,
    USART_CLKSRC_PLL        = CLKSRC_PLL,
    USART_CLKSRC_12MOSC     = CLKSRC_12MOSC,
    USART_CLKSRC_10KIRC     = CLKSRC_10KIRC,

    USB_CLKSRC_12MIRC       = CLKREF_12MIRC,
    USB_CLKSRC_12MOSC       = CLKREF_12MOSC,

    OUTCLK_CLKSRC_12MIRC    = 0,
    OUTCLK_CLKSRC_PLL       = 1,
    OUTCLK_CLKSRC_10KIRC    = 2,
    OUTCLK_CLKSRC_12MOSC    = 3,
    OUTCLK_CLKSRC_32KOSC    = 4,
    OUTCLK_CLKSRC_USB_PLL   = 8,
};
//! @}

enum vsf_pm_pll_sel_t {
    PLL_IDX,
    PLL_USB_IDX,
};

struct io_wakeup_cfg_t {
    uint32_t dummy;
};
typedef struct io_wakeup_cfg_t io_wakeup_cfg_t;

typedef enum vsf_io_port_no_t vsf_io_port_no_t;
typedef enum vsf_pm_clk_src_sel_t vsf_pm_clk_src_sel_t;

def_interface( i_pm_wakeup_t )
    struct {
        vsf_err_t (*Enable)(io_wakeup_cfg_t *pcfg, uint_fast8_t size);
        vsf_err_t (*Disable)(vsf_io_port_no_t port, uint_fast32_t msk);
    }UseIO;
end_def_interface( i_pm_wakeup_t )

struct vsf_pm_pclk_cfg_t {
    vsf_pm_clk_src_sel_t    clk_src;
    uint16_t            div;
};

//! \name main clock config sturct
//! @{
struct vsf_pm_mclk_cfg_t {
    vsf_pm_clk_src_sel_t    clk_src;                //!< main clock source
    uint32_t            freq;                   //!< system oscilator frequency
    uint16_t            core_div[1];            //!< system core clock divider
    uint16_t            sclk_div[1];            //!< system sync clock divider
};
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_MT071_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
