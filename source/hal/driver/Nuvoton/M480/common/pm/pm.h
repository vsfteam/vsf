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

#ifndef __HAL_DRIVER_NUVOTON_M480_PM_H__
#define __HAL_DRIVER_NUVOTON_M480_PM_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"


/*============================ MACROS ========================================*/

#define VSF_PM_CFG_REIMPLEMENT_PLL_CFG              ENABLED
#define VSF_PM_CFG_REIMPLEMENT_PLL_SEL              ENABLED

#define VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER         ENABLED
#define VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER_MASK    ENABLED
#define VSF_PM_CFG_REIMPLEMENT_SLEEP_MODE           ENABLED
#define VSF_PM_CFG_REIMPLEMENT_PCLK_NUMBER          ENABLED
#define VSF_PM_CFG_REIMPLEMENT_PCLK_CFG             ENABLED
#define VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER          ENABLED
#define VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER_MASK     ENABLED
#define VSF_PM_CFG_REIMPLEMENT_SCLK_SEL             ENABLED
#define VSF_PM_CFG_REIMPLEMENT_MCLK_CFG             ENABLED
#define VSF_PM_CFG_REIMPLEMENT_MCLK_NO              ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __def_idx(__name, __no)     VSF_MCONNECT2(__name, _IDX) = (__no)
#define __def_msk(__name)           VSF_MCONNECT2(__name, _MSK) = VSF_BIT(VSF_MCONNECT2(__name, _IDX) & 0x1F)

// bit0 - bit13:    clksrc bitfield
// bit14- bit27:    clkdiv bitfield
// bit28- bit31:    clkdiv_remap
#define __def_pclk(__name, __bf_clksel, __bf_clkdiv, __clksel_map_idx)          \
            VSF_MCONNECT2(__name, _IDX) = ((__bf_clksel) << 0)                  \
                                |   ((__bf_clkdiv) << 14)                       \
                                |   ((__clksel_map_idx) << 28)

#define __def_sclk_idx(__name, __bus_idx, __bit_idx)                            \
            VSF_MCONNECT2(__name, _IDX) = ((__bit_idx) << 0) | ((__bus_idx) << 5)

#define __def_clk_src(__name, __value)                                          \
            __name = (__value)
/*============================ TYPES =========================================*/

//! power set index
typedef enum pm_power_cfg_no_t{
    __def_idx(POWER_HXT, 0),
    __def_idx(POWER_LXT, 1),
    __def_idx(POWER_HIRC, 2),
    __def_idx(POWER_LIRC, 3),
} pm_power_cfg_no_t;

//! power set mask
typedef enum pm_power_cfg_msk_t {
    __def_msk(POWER_HXT),
    __def_msk(POWER_LXT),
    __def_msk(POWER_HIRC),
    __def_msk(POWER_LIRC),
} pm_power_cfg_msk_t;

//! the lowpower mode
typedef enum pm_sleep_mode_t{
    PM_NPD              = 0,
    PM_LLPD             = 1,
    PM_FWPD             = 2,
    PM_SPD0             = 4,
    PM_SPD1             = 5,
    PM_DPD              = 6,

    PM_WAIT             = PM_NPD,
    PM_SLEEP            = PM_NPD,
    PM_DEEP_SLEEP       = PM_DPD,
    PM_POWER_OFF        = PM_DPD,
} pm_sleep_mode_t;

typedef enum pm_periph_clksrc_t {
    CLKSRC_HXT,
    CLKSRC_HXTD2,
    CLKSRC_LXT,
    CLKSRC_HIRC,
    CLKSRC_HIRCD2,
    CLKSRC_LIRC,

    CLKSRC_PLL,
    CLKSRC_HCLK,
    CLKSRC_HCLKD2,
    CLKSRC_HCLKD2K,
    CLKSRC_PCLK0,
    CLKSRC_PCLK1,

    CLKSRC_TM0_PIN,
    CLKSRC_TM1_PIN,
    CLKSRC_TM2_PIN,
    CLKSRC_TM3_PIN,
} pm_periph_clksrc_t;

typedef enum pm_periph_clksel_t {
    SDH_CLKSEL_MAP_IDX  = 0,
    SDH_CLKSEL_MAP      = (CLKSRC_HXT << 0) | (CLKSRC_PLL << 4) | (CLKSRC_HCLK << 8) | (CLKSRC_HIRC << 12),
    STCLK_CLKSEL_MAP_IDX= 1,
    STCLK_CLKSEL_MAP    = (CLKSRC_HXT << 0) | (CLKSRC_LXT << 4) | (CLKSRC_HXTD2 << 8) | (CLKSRC_HCLKD2 << 12) | (CLKSRC_HIRCD2 << 28),
    HCLK_CLKSEL_MAP_IDX = 2,
    HCLK_CLKSEL_MAP     = (CLKSRC_HXT << 0) | (CLKSRC_LXT << 4) | (CLKSRC_PLL << 8) | (CLKSRC_LIRC << 12) | (CLKSRC_HIRC << 28),
    WWDT_CLKSEL_MAP_IDX = 3,
    WWDT_CLKSEL_MAP     = (CLKSRC_HCLKD2K << 8) | (CLKSRC_LIRC << 12),
    CLKO_CLKSEL_MAP_IDX = 4,
    CLKO_CLKSEL_MAP     = (CLKSRC_HXT << 0) | (CLKSRC_LXT << 4) | (CLKSRC_HCLK << 8) | (CLKSRC_HIRC << 12),
    UART_CLKSEL_MAP_IDX = 5,
    UART_CLKSEL_MAP     = (CLKSRC_HXT << 0) | (CLKSRC_PLL << 4) | (CLKSRC_LXT << 8) | (CLKSRC_HIRC << 12),
    TMR3_CLKSEL_MAP_IDX = 6,
    TMR3_CLKSEL_MAP     = (CLKSRC_HXT << 0) | (CLKSRC_LXT << 4) | (CLKSRC_PCLK1 << 8) | (CLKSRC_TM3_PIN << 12) | (CLKSRC_LIRC << 20) | (CLKSRC_HIRC << 28),
    TMR2_CLKSEL_MAP_IDX = 7,
    TMR2_CLKSEL_MAP     = (CLKSRC_HXT << 0) | (CLKSRC_LXT << 4) | (CLKSRC_PCLK1 << 8) | (CLKSRC_TM2_PIN << 12) | (CLKSRC_LIRC << 20) | (CLKSRC_HIRC << 28),
    TMR1_CLKSEL_MAP_IDX = 8,
    TMR1_CLKSEL_MAP     = (CLKSRC_HXT << 0) | (CLKSRC_LXT << 4) | (CLKSRC_PCLK0 << 8) | (CLKSRC_TM1_PIN << 12) | (CLKSRC_LIRC << 20) | (CLKSRC_HIRC << 28),
    TMR0_CLKSEL_MAP_IDX = 9,
    TMR0_CLKSEL_MAP     = (CLKSRC_HXT << 0) | (CLKSRC_LXT << 4) | (CLKSRC_PCLK0 << 8) | (CLKSRC_TM0_PIN << 12) | (CLKSRC_LIRC << 20) | (CLKSRC_HIRC << 28),
    WDT_CLKSEL_MAP_IDX  = 10,
    WDT_CLKSEL_MAP      = (CLKSRC_LXT << 4) | (CLKSRC_HCLKD2K << 8) | (CLKSRC_LIRC << 12),
    SPI13_CLKSEL_MAP_IDX= 11,
    SPI13_CLKSEL_MAP    = (CLKSRC_HXT << 0) | (CLKSRC_PLL << 4) | (CLKSRC_PCLK0 << 8) | (CLKSRC_HIRC << 12),
    SPI02_CLKSEL_MAP_IDX= 12,
    SPI02_CLKSEL_MAP    = (CLKSRC_HXT << 0) | (CLKSRC_PLL << 4) | (CLKSRC_PCLK1 << 8) | (CLKSRC_HIRC << 12),
    BPWM1_CLKSEL_MAP_IDX= 13,
    BPWM1_CLKSEL_MAP    = (CLKSRC_PLL << 0) | (CLKSRC_PCLK1 << 4),
    BPWM0_CLKSEL_MAP_IDX= 14,
    BPWM0_CLKSEL_MAP    = (CLKSRC_PLL << 0) | (CLKSRC_PCLK0 << 4),
    RTC_CLKSEL_MAP_IDX  = 15,
    RTC_CLKSEL_MAP      = (CLKSRC_LXT << 0) | (CLKSRC_LIRC << 4),

    SDH0_CLKSEL_MAP_IDX = SDH_CLKSEL_MAP_IDX,
    SDH1_CLKSEL_MAP_IDX = SDH_CLKSEL_MAP_IDX,
    UART0_CLKSEL_MAP_IDX= UART_CLKSEL_MAP_IDX,
    UART1_CLKSEL_MAP_IDX= UART_CLKSEL_MAP_IDX,
    UART2_CLKSEL_MAP_IDX= UART_CLKSEL_MAP_IDX,
    UART3_CLKSEL_MAP_IDX= UART_CLKSEL_MAP_IDX,
    UART4_CLKSEL_MAP_IDX= UART_CLKSEL_MAP_IDX,
    UART5_CLKSEL_MAP_IDX= UART_CLKSEL_MAP_IDX,
    SPI0_CLKSEL_MAP_IDX = SPI02_CLKSEL_MAP_IDX,
    SPI1_CLKSEL_MAP_IDX = SPI13_CLKSEL_MAP_IDX,
    SPI2_CLKSEL_MAP_IDX = SPI02_CLKSEL_MAP_IDX,
    SPI3_CLKSEL_MAP_IDX = SPI13_CLKSEL_MAP_IDX,
    QSPI0_CLKSEL_MAP_IDX= SPI13_CLKSEL_MAP_IDX,
    EPWM1_CLKSEL_MAP_IDX= BPWM1_CLKSEL_MAP_IDX,
    EPWM0_CLKSEL_MAP_IDX= BPWM0_CLKSEL_MAP_IDX,
    I2S0_CLKSEL_MAP_IDX = SPI13_CLKSEL_MAP_IDX,
    SC0_CLKSEL_MAP_IDX  = SPI13_CLKSEL_MAP_IDX,
    SC1_CLKSEL_MAP_IDX  = SPI02_CLKSEL_MAP_IDX,
    SC2_CLKSEL_MAP_IDX  = SPI13_CLKSEL_MAP_IDX,
} pm_periph_clksel_t;

//! peripheral clock index
typedef enum pm_pclk_no_t{
                        // NAME         BF_CLKSEL,      BF_CLKDIV,      CLKSEL_MAP_IDX
    M480_BIT_FIELD(     HCLK_CLKSEL,    0,  3,  true),
    M480_BIT_FIELD(     SYSTICK_CLKSEL, 3,  3,  true),

    M480_BIT_FIELD(     WWDT_CLKSEL,    62, 2,  false),
    __def_pclk(         PCLK_WWDT,      WWDT_CLKSEL,    0,              WWDT_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     USB_CLKDIV,     4,  4,  false),
    __def_pclk(         PCLK_USB,       0,              USB_CLKDIV,     0),
    // AHB
    M480_BIT_FIELD(     EMAC_CLKDIV,    48, 8,  false),
    __def_pclk(         PCLK_EMAC,      0,              EMAC_CLKDIV,    0),
    M480_BIT_FIELD(     SDH0_CLKSEL,    20, 2,  true),
    M480_BIT_FIELD(     SDH0_CLKDIV,    24, 8,  false),
    __def_pclk(         PCLK_SDH0,      SDH0_CLKSEL,    SDH0_CLKDIV,    SDH0_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     SDH1_CLKSEL,    22, 2,  true),
    M480_BIT_FIELD(     SDH1_CLKDIV,    56, 8,  false),
    __def_pclk(         PCLK_SDH1,      SDH1_CLKSEL,    0,              SDH1_CLKSEL_MAP_IDX),

    // APB0
    M480_BIT_FIELD(     WDT_CLKSEL,     32, 2,  true),
    __def_pclk(         PCLK_WDT,       WDT_CLKSEL,     0,              WDT_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     RTC_CLKSEL,     104,1,  false),
    __def_pclk(         PCLK_RTC,       RTC_CLKSEL,     0,              RTC_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     TMR0_CLKSEL,    40, 3,  false),
    __def_pclk(         PCLK_TMR0,      TMR0_CLKSEL,    0,              TMR0_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     TMR1_CLKSEL,    44, 3,  false),
    __def_pclk(         PCLK_TMR1,      TMR1_CLKSEL,    0,              TMR1_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     TMR2_CLKSEL,    48, 3,  false),
    __def_pclk(         PCLK_TMR2,      TMR2_CLKSEL,    0,              TMR2_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     TMR3_CLKSEL,    52, 3,  false),
    __def_pclk(         PCLK_TMR3,      TMR3_CLKSEL,    0,              TMR3_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     CLKO_CLKSEL,    60, 2,  false),
    __def_pclk(         PCLK_CLKO,      CLKO_CLKSEL,    0,              CLKO_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     QSPI0_CLKSEL,   66, 2,  false),
    __def_pclk(         PCLK_QSPI0,     QSPI0_CLKSEL,   0,              QSPI0_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     SPI0_CLKSEL,    68, 2,  false),
    __def_pclk(         PCLK_SPI0,      SPI0_CLKSEL,    0,              SPI0_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     SPI1_CLKSEL,    70, 2,  false),
    __def_pclk(         PCLK_SPI1,      SPI1_CLKSEL,    0,              SPI1_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     SPI2_CLKSEL,    74, 2,  false),
    __def_pclk(         PCLK_SPI2,      SPI2_CLKSEL,    0,              SPI2_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     UART0_CLKSEL,   56, 2,  false),
    M480_BIT_FIELD(     UART0_CLKDIV,   8,  4,  false),
    __def_pclk(         PCLK_UART0,     UART0_CLKSEL,   UART0_CLKDIV,   UART0_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     UART1_CLKSEL,   58, 2,  false),
    M480_BIT_FIELD(     UART1_CLKDIV,   12, 4,  false),
    __def_pclk(         PCLK_UART1,     UART1_CLKSEL,   UART1_CLKDIV,   UART1_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     UART2_CLKSEL,   120,2,  false),
    M480_BIT_FIELD(     UART2_CLKDIV,   64, 4,  false),
    __def_pclk(         PCLK_UART2,     UART2_CLKSEL,   UART2_CLKDIV,   UART2_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     UART3_CLKSEL,   122,2,  false),
    M480_BIT_FIELD(     UART3_CLKDIV,   68, 4,  false),
    __def_pclk(         PCLK_UART3,     UART3_CLKSEL,   UART3_CLKDIV,   UART3_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     UART4_CLKSEL,   124,2,  false),
    M480_BIT_FIELD(     UART4_CLKDIV,   72, 4,  false),
    __def_pclk(         PCLK_UART4,     UART4_CLKSEL,   UART4_CLKDIV,   UART4_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     UART5_CLKSEL,   126,2,  false),
    M480_BIT_FIELD(     UART5_CLKDIV,   76, 4,  false),
    __def_pclk(         PCLK_UART5,     UART5_CLKSEL,   UART5_CLKDIV,   UART5_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     EADC_CLKDIV,    16, 8,  false),
    __def_pclk(         PCLK_EADC,      0,              EADC_CLKDIV,    0),
    M480_BIT_FIELD(     I2S0_CLKSEL,    112,2,  false),
    __def_pclk(         PCLK_I2S0,      I2S0_CLKSEL,    0,              I2S0_CLKSEL_MAP_IDX),

    // APB1
    M480_BIT_FIELD(     SC0_CLKSEL,     96, 2,  false),
    __def_pclk(         PCLK_SC0,       SC0_CLKSEL,     0,              SC0_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     SC1_CLKSEL,     98, 2,  false),
    __def_pclk(         PCLK_SC1,       SC1_CLKSEL,     0,              SC1_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     SC2_CLKSEL,     100,2,  false),
    __def_pclk(         PCLK_SC2,       SC2_CLKSEL,     0,              SC2_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     SPI3_CLKSEL,    76, 2,  false),
    __def_pclk(         PCLK_SPI3,      SPI3_CLKSEL,    0,              SPI3_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     EPWM0_CLKSEL,   64, 1,  false),
    __def_pclk(         PCLK_EPWM0,     EPWM0_CLKSEL,   0,              EPWM0_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     EPWM1_CLKSEL,   65, 1,  false),
    __def_pclk(         PCLK_EPWM1,     EPWM1_CLKSEL,   0,              EPWM1_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     BPWM0_CLKSEL,   72, 1,  false),
    __def_pclk(         PCLK_BPWM0,     BPWM0_CLKSEL,   0,              BPWM0_CLKSEL_MAP_IDX),
    M480_BIT_FIELD(     BPWM1_CLKSEL,   73, 1,  false),
    __def_pclk(         PCLK_BPWM1,     BPWM1_CLKSEL,   0,              BPWM1_CLKSEL_MAP_IDX),
} pm_pclk_no_t;

typedef enum pm_mclk_no_t {
    MCLK_CORE_IDX = 0,
} pm_mclk_no_t;
//! Peripheral AHB Clock Macros
typedef enum pm_sclk_no_t {
                        // NAME         BUS_IDX,    BIT_IDX
    // AHB
    __def_sclk_idx(     SCLK_DMA,       0,          1   ),
    __def_sclk_idx(     SCLK_ISP,       0,          2   ),
    __def_sclk_idx(     SCLK_EBI,       0,          3   ),
    __def_sclk_idx(     SCLK_EMAC,      0,          5   ),
    __def_sclk_idx(     SCLK_SDH0,      0,          6   ),
    __def_sclk_idx(     SCLK_CRC,       0,          7   ),
    __def_sclk_idx(     SCLK_HSUSB,     0,          10  ),
    __def_sclk_idx(     SCLK_CRYPTO,    0,          12  ),
    __def_sclk_idx(     SCLK_SPIM,      0,          14  ),
    __def_sclk_idx(     SCLK_FLASH,     0,          15  ),
    __def_sclk_idx(     SCLK_USBH,      0,          16  ),
    __def_sclk_idx(     SCLK_SDH1,      0,          17  ),

    // APB0
    __def_sclk_idx(     SCLK_WDT,       1,          0   ),
    __def_sclk_idx(     SCLK_RTC,       1,          1   ),
    __def_sclk_idx(     SCLK_TMR0,      1,          2   ),
    __def_sclk_idx(     SCLK_TMR1,      1,          3   ),
    __def_sclk_idx(     SCLK_TMR2,      1,          4   ),
    __def_sclk_idx(     SCLK_TMR3,      1,          5   ),
    __def_sclk_idx(     SCLK_CLKO,      1,          6   ),
    __def_sclk_idx(     SCLK_ACMP,      1,          7   ),
    __def_sclk_idx(     SCLK_I2C0,      1,          8   ),
    __def_sclk_idx(     SCLK_I2C1,      1,          9   ),
    __def_sclk_idx(     SCLK_I2C2,      1,          10  ),
    __def_sclk_idx(     SCLK_QSPI0,     1,          12  ),
    __def_sclk_idx(     SCLK_SPI0,      1,          13  ),
    __def_sclk_idx(     SCLK_SPI1,      1,          14  ),
    __def_sclk_idx(     SCLK_SPI2,      1,          15  ),
    __def_sclk_idx(     SCLK_UART0,     1,          16  ),
    __def_sclk_idx(     SCLK_UART1,     1,          17  ),
    __def_sclk_idx(     SCLK_UART2,     1,          18  ),
    __def_sclk_idx(     SCLK_UART3,     1,          19  ),
    __def_sclk_idx(     SCLK_UART4,     1,          20  ),
    __def_sclk_idx(     SCLK_UART5,     1,          21  ),
    __def_sclk_idx(     SCLK_CAN0,      1,          24  ),
    __def_sclk_idx(     SCLK_CAN1,      1,          25  ),
    __def_sclk_idx(     SCLK_OTG,       1,          26  ),
    __def_sclk_idx(     SCLK_USBD,      1,          27  ),
    __def_sclk_idx(     SCLK_EADC,      1,          28  ),
    __def_sclk_idx(     SCLK_I2S0,      1,          29  ),
    __def_sclk_idx(     SCLK_HSOTG,     1,          30  ),

    // APB1
    __def_sclk_idx(     SCLK_SC0,       2,          0   ),
    __def_sclk_idx(     SCLK_SC1,       2,          1   ),
    __def_sclk_idx(     SCLK_SC2,       2,          2   ),
    __def_sclk_idx(     SCLK_SPI3,      2,          6   ),
    __def_sclk_idx(     SCLK_USCI0,     2,          8   ),
    __def_sclk_idx(     SCLK_USCI1,     2,          9   ),
    __def_sclk_idx(     SCLK_DAC,       2,          12  ),
    __def_sclk_idx(     SCLK_EPWM0,     2,          16  ),
    __def_sclk_idx(     SCLK_EPWM1,     2,          17  ),
    __def_sclk_idx(     SCLK_BPWM0,     2,          18  ),
    __def_sclk_idx(     SCLK_BPWM1,     2,          19  ),
    __def_sclk_idx(     SCLK_QEI0,      2,          22  ),
    __def_sclk_idx(     SCLK_QEI1,      2,          23  ),
    __def_sclk_idx(     SCLK_CAP0,      2,          26  ),
    __def_sclk_idx(     SCLK_CAP1,      2,          27  ),
    __def_sclk_idx(     SCLK_OP,        2,          30  ),
} pm_sclk_no_t;

typedef enum pm_sclk_msk_t {
    // AHB
    __def_msk(SCLK_DMA),
    __def_msk(SCLK_ISP),
    __def_msk(SCLK_EBI),
    __def_msk(SCLK_EMAC),
    __def_msk(SCLK_SDH0),
    __def_msk(SCLK_CRC),
    __def_msk(SCLK_HSUSB),
    __def_msk(SCLK_CRYPTO),
    __def_msk(SCLK_SPIM),
    __def_msk(SCLK_FLASH),
    __def_msk(SCLK_USBH),
    __def_msk(SCLK_SDH1),

    // APB0
    __def_msk(SCLK_WDT),
    __def_msk(SCLK_RTC),
    __def_msk(SCLK_TMR0),
    __def_msk(SCLK_TMR1),
    __def_msk(SCLK_TMR2),
    __def_msk(SCLK_TMR3),
    __def_msk(SCLK_CLKO),
    __def_msk(SCLK_ACMP),
    __def_msk(SCLK_I2C0),
    __def_msk(SCLK_I2C1),
    __def_msk(SCLK_I2C2),
    __def_msk(SCLK_QSPI0),
    __def_msk(SCLK_SPI0),
    __def_msk(SCLK_SPI1),
    __def_msk(SCLK_SPI2),
    __def_msk(SCLK_UART0),
    __def_msk(SCLK_UART1),
    __def_msk(SCLK_UART2),
    __def_msk(SCLK_UART3),
    __def_msk(SCLK_UART4),
    __def_msk(SCLK_UART5),
    __def_msk(SCLK_CAN0),
    __def_msk(SCLK_CAN1),
    __def_msk(SCLK_OTG),
    __def_msk(SCLK_USBD),
    __def_msk(SCLK_EADC),
    __def_msk(SCLK_I2S0),
    __def_msk(SCLK_HSOTG),

    // APB1
    __def_msk(SCLK_SC0),
    __def_msk(SCLK_SC1),
    __def_msk(SCLK_SC2),
    __def_msk(SCLK_SPI3),
    __def_msk(SCLK_USCI0),
    __def_msk(SCLK_USCI1),
    __def_msk(SCLK_DAC),
    __def_msk(SCLK_EPWM0),
    __def_msk(SCLK_EPWM1),
    __def_msk(SCLK_BPWM0),
    __def_msk(SCLK_BPWM1),
    __def_msk(SCLK_QEI0),
    __def_msk(SCLK_QEI1),
    __def_msk(SCLK_CAP0),
    __def_msk(SCLK_CAP1),
    __def_msk(SCLK_OP),
} pm_sclk_msk_t;

typedef enum pm_clk_src_sel_t {
    // CLK->CLKSEL0
    __def_clk_src(  HCLK_CLKSRC_HXT,        0),
    __def_clk_src(  HCLK_CLKSRC_LXT,        1),
    __def_clk_src(  HCLK_CLKSRC_PLL,        2),
    __def_clk_src(  HCLK_CLKSRC_LIRC,       3),
    __def_clk_src(  HCLK_CLKSRC_HIRC,       7),

    __def_clk_src(  SYSTICK_CLKSRC_HXT,     0),
    __def_clk_src(  SYSTICK_CLKSRC_LXT,     1),
    __def_clk_src(  SYSTICK_CLKSRC_HXTD2,   2),
    __def_clk_src(  SYSTICK_CLKSRC_HCLKD2,  3),
    __def_clk_src(  SYSTICK_CLKSRC_HIRCD2,  7),

    __def_clk_src(  SDH0_CLKSRC_HXT,        0),
    __def_clk_src(  SDH0_CLKSRC_PLL,        1),
    __def_clk_src(  SDH0_CLKSRC_HCLK,       2),
    __def_clk_src(  SDH0_CLKSRC_HIRC,       3),

    __def_clk_src(  SDH1_CLKSRC_HXT,        0),
    __def_clk_src(  SDH1_CLKSRC_PLL,        1),
    __def_clk_src(  SDH1_CLKSRC_HCLK,       2),
    __def_clk_src(  SDH1_CLKSRC_HIRC,       3),

    // CLK->CLKSEL1
    __def_clk_src(  WDT_CLKSRC_LXT,         1),
    __def_clk_src(  WDT_CLKSRC_HCLKD2K,     2),
    __def_clk_src(  WDT_CLKSRC_LIRC,        3),

    __def_clk_src(  TMR0_CLKSRC_HXT,        0),
    __def_clk_src(  TMR0_CLKSRC_LXT,        1),
    __def_clk_src(  TMR0_CLKSRC_PCLK0,      2),
    __def_clk_src(  TMR0_CLKSRC_TM0EXT,     3),
    __def_clk_src(  TMR0_CLKSRC_LIRC,       5),
    __def_clk_src(  TMR0_CLKSRC_HIRC,       7),

    __def_clk_src(  TMR1_CLKSRC_HXT,        0),
    __def_clk_src(  TMR1_CLKSRC_LXT,        1),
    __def_clk_src(  TMR1_CLKSRC_PCLK0,      2),
    __def_clk_src(  TMR1_CLKSRC_TM1EXT,     3),
    __def_clk_src(  TMR1_CLKSRC_LIRC,       5),
    __def_clk_src(  TMR1_CLKSRC_HIRC,       7),

    __def_clk_src(  TMR2_CLKSRC_HXT,        0),
    __def_clk_src(  TMR2_CLKSRC_LXT,        1),
    __def_clk_src(  TMR2_CLKSRC_PCLK1,      2),
    __def_clk_src(  TMR2_CLKSRC_TM2EXT,     3),
    __def_clk_src(  TMR2_CLKSRC_LIRC,       5),
    __def_clk_src(  TMR2_CLKSRC_HIRC,       7),

    __def_clk_src(  TMR3_CLKSRC_HXT,        0),
    __def_clk_src(  TMR3_CLKSRC_LXT,        1),
    __def_clk_src(  TMR3_CLKSRC_PCLK1,      2),
    __def_clk_src(  TMR3_CLKSRC_TM3EXT,     3),
    __def_clk_src(  TMR3_CLKSRC_LIRC,       5),
    __def_clk_src(  TMR3_CLKSRC_HIRC,       7),

    __def_clk_src(  UART0_CLKSRC_HXT,       0),
    __def_clk_src(  UART0_CLKSRC_PLL,       1),
    __def_clk_src(  UART0_CLKSRC_LXT,       2),
    __def_clk_src(  UART0_CLKSRC_HIRC,      3),

    __def_clk_src(  UART1_CLKSRC_HXT,       0),
    __def_clk_src(  UART1_CLKSRC_PLL,       1),
    __def_clk_src(  UART1_CLKSRC_LXT,       2),
    __def_clk_src(  UART1_CLKSRC_HIRC,      3),

    __def_clk_src(  CLKO_CLKSRC_HXT,        0),
    __def_clk_src(  CLKO_CLKSRC_LXT,        1),
    __def_clk_src(  CLKO_CLKSRC_HCLK,       2),
    __def_clk_src(  CLKO_CLKSRC_HIRC,       3),

    __def_clk_src(  WWDT_CLKSRC_HCLKD2K,    2),
    __def_clk_src(  WWDT_CLKSRC_LIRC,       3),

    // CLK->CLKSEL2
    __def_clk_src(  EPWM0_CLKSRC_PLL,       0),
    __def_clk_src(  EPWM0_CLKSRC_PCLK0,     1),

    __def_clk_src(  EPWM1_CLKSRC_PLL,       0),
    __def_clk_src(  EPWM1_CLKSRC_PCLK0,     1),

    __def_clk_src(  QSPI0_CLKSRC_HXT,       0),
    __def_clk_src(  QSPI0_CLKSRC_PLL,       1),
    __def_clk_src(  QSPI0_CLKSRC_PCLK0,     2),
    __def_clk_src(  QSPI0_CLKSRC_HIRC,      3),

    __def_clk_src(  SPI0_CLKSRC_HXT,        0),
    __def_clk_src(  SPI0_CLKSRC_PLL,        1),
    __def_clk_src(  SPI0_CLKSRC_PCLK1,      2),
    __def_clk_src(  SPI0_CLKSRC_HIRC,       3),

    __def_clk_src(  SPI1_CLKSRC_HXT,        0),
    __def_clk_src(  SPI1_CLKSRC_PLL,        1),
    __def_clk_src(  SPI1_CLKSRC_PCLK0,      2),
    __def_clk_src(  SPI1_CLKSRC_HIRC,       3),

    __def_clk_src(  BPWM0_CLKSRC_PLL,       0),
    __def_clk_src(  BPWM0_CLKSRC_PCLK0,     1),

    __def_clk_src(  BPWM1_CLKSRC_PLL,       0),
    __def_clk_src(  BPWM1_CLKSRC_PCLK0,     1),

    __def_clk_src(  SPI2_CLKSRC_HXT,        0),
    __def_clk_src(  SPI2_CLKSRC_PLL,        1),
    __def_clk_src(  SPI2_CLKSRC_PCLK1,      2),
    __def_clk_src(  SPI2_CLKSRC_HIRC,       3),

    __def_clk_src(  SPI3_CLKSRC_HXT,        0),
    __def_clk_src(  SPI3_CLKSRC_PLL,        1),
    __def_clk_src(  SPI3_CLKSRC_PCLK1,      2),
    __def_clk_src(  SPI3_CLKSRC_HIRC,       3),

    // CLK->CLKSEL3
    __def_clk_src(  SC0_CLKSRC_HXT,         0),
    __def_clk_src(  SC0_CLKSRC_PLL,         1),
    __def_clk_src(  SC0_CLKSRC_PCLK0,       2),
    __def_clk_src(  SC0_CLKSRC_HIRC,        3),

    __def_clk_src(  SC1_CLKSRC_HXT,         0),
    __def_clk_src(  SC1_CLKSRC_PLL,         1),
    __def_clk_src(  SC1_CLKSRC_PCLK1,       2),
    __def_clk_src(  SC1_CLKSRC_HIRC,        3),

    __def_clk_src(  SC2_CLKSRC_HXT,         0),
    __def_clk_src(  SC2_CLKSRC_PLL,         1),
    __def_clk_src(  SC2_CLKSRC_PCLK0,       2),
    __def_clk_src(  SC2_CLKSRC_HIRC,        3),

    __def_clk_src(  RTC_CLKSRC_LXT,         0),
    __def_clk_src(  RTC_CLKSRC_LIRC,        1),

    __def_clk_src(  I2S0_CLKSRC_HXT,        0),
    __def_clk_src(  I2S0_CLKSRC_PLL,        1),
    __def_clk_src(  I2S0_CLKSRC_PCLK0,      2),
    __def_clk_src(  I2S0_CLKSRC_HIRC,       3),

    __def_clk_src(  UART2_CLKSRC_HXT,       0),
    __def_clk_src(  UART2_CLKSRC_PLL,       1),
    __def_clk_src(  UART2_CLKSRC_LXT,       2),
    __def_clk_src(  UART2_CLKSRC_HIRC,      3),

    __def_clk_src(  UART3_CLKSRC_HXT,       0),
    __def_clk_src(  UART3_CLKSRC_PLL,       1),
    __def_clk_src(  UART3_CLKSRC_LXT,       2),
    __def_clk_src(  UART3_CLKSRC_HIRC,      3),

    __def_clk_src(  UART4_CLKSRC_HXT,       0),
    __def_clk_src(  UART4_CLKSRC_PLL,       1),
    __def_clk_src(  UART4_CLKSRC_LXT,       2),
    __def_clk_src(  UART4_CLKSRC_HIRC,      3),

    __def_clk_src(  UART5_CLKSRC_HXT,       0),
    __def_clk_src(  UART5_CLKSRC_PLL,       1),
    __def_clk_src(  UART5_CLKSRC_LXT,       2),
    __def_clk_src(  UART5_CLKSRC_HIRC,      3),
} pm_clk_src_sel_t;

typedef enum pm_pll_sel_t {
    PLL0_IDX,
} pm_pll_sel_t;

typedef struct io_wakeup_cfg_t {
    uint32_t dummy;
} io_wakeup_cfg_t;

////typedef enum io_port_no_t io_port_no_t;
//
//def_interface( i_pm_wakeup_t )
//    struct {
//        vsf_err_t (*Enable)(io_wakeup_cfg_t *pcfg, uint_fast8_t size);
//        //vsf_err_t (*Disable)(io_port_no_t port, uint_fast32_t msk);
//    } UseIO;
//end_def_interface( i_pm_wakeup_t )

typedef struct pm_pclk_cfg_t {
    pm_clk_src_sel_t    clk_src;
    uint16_t            div;
} pm_pclk_cfg_t;

//! main clock config sturct
typedef struct pm_mclk_cfg_t {
    pm_clk_src_sel_t    clk_src;                //!< main clock source
    uint32_t            freq;                   //!< system oscilator frequency
    uint16_t            core_div[1];            //!< system core clock divider
    uint16_t            ahb_div[1];             //!< system AHB clock divider
    uint16_t            apb_div[2];             //!< system APB clock divider
} pm_mclk_cfg_t;

//! pll config struct
typedef struct pm_pll_cfg_t {
    pm_clk_src_sel_t            pll_clk_src;    //!< pll clock source
    uint32_t                    freq;           //!< system oscilator frequency
    uint8_t                     Msel;           //!< PLL Feedback divider value
    uint8_t                     Psel;           //!< pll Feedback divider value
} pm_pll_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/

// TODO : update template path
#include "hal/driver/common/template/vsf_template_pm.h"

/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
