/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __F1C100S_REG_H__
#define __F1C100S_REG_H__

/*============================ INCLUDES ======================================*/

/*! \note i_reg_xxxx.h header files must be standalong and assume following 
 *!       conditions: 
 *!       a. stdint.h exists
 *!       b. anonymouse structures and unions are supported
 */
#include <stdint.h>

/*============================ MACROS ========================================*/

// System Controller
#define SYSCON_BASE                                     ((syscon_reg_t *)0x01c00000)
#   define SYSCON_USB_CTRL                              0x004
#       define USB_FIFO_MODE                            (3UL << 0)
#       define USB_FIFO_MODE_8KB                        (1UL << 0)

// CCU
#define CCU_BASE                                        ((ccu_reg_t *)0x01c20000)
#   define CCU_PLL_CPU_CTRL                             0x000
#       define PLL_CPU_CTRL_PLL_ENABLE                  (1UL << 31)
#       define PLL_CPU_CTRL_LOCK                        (1UL << 28)
#       define __PLL_CPU_CTRL_PLL_OUT_EVT_DIV_P(__P)    (((__P) >> 1) << 16)
#       define __PLL_CPU_CTRL_PLL_FACTOR_N(__N)         (((__N) - 1) << 8)
#       define __PLL_CPU_CTRL_PLL_FACTOR_K(__K)         (((__K) - 1) << 4)
#       define __PLL_CPU_CTRL_PLL_FACTOR_M(__M)         (((__M) - 1) << 0)
//      PLL = (24MHz * N * K) / (M * P), range [200MHz, 2.6GHz]
//      P in [1, 2, 4]
#       define PLL_CPU_CTRL_PLL_OUT_EVT_DIV_P(...)      __PLL_CPU_CTRL_PLL_OUT_EVT_DIV_P((6UL, ##__VA_ARGS__))
//      N in [1 .. 32]
#       define PLL_CPU_CTRL_PLL_FACTOR_N(...)           __PLL_CPU_CTRL_PLL_FACTOR_N((32UL, ##__VA_ARGS__))
//      K in [1 .. 4]
#       define PLL_CPU_CTRL_PLL_FACTOR_K(...)           __PLL_CPU_CTRL_PLL_FACTOR_K((4UL, ##__VA_ARGS__))
//      M in [1 .. 4]
#       define PLL_CPU_CTRL_PLL_FACTOR_M(...)           __PLL_CPU_CTRL_PLL_FACTOR_M((4UL, ##__VA_ARGS__))
#   define CCU_PLL_AUDIO_CTRL                           0x008
#       define PLL_AUDIO_CTRL_PLL_ENABLE                (1UL << 31)
#       define PLL_AUDIO_CTRL_LOCK                      (1UL << 28)
#       define PLL_AUDIO_CTRL_PLL_SDM_EN                (1UL << 24)
#       define __PLL_AUDIO_CTRL_PLL_FACTOR_N(__N)       (((__N) - 1) << 8)
#       define __PLL_AUDIO_CTRL_PLL_PREDIV_M(__M)       (((__M) - 1) << 0)
//      PLL = (24MHz * N * 2) / M, range [20MHz, 200MHz]
//      N in [1 .. 128], if PLL_AUDIO_CTRL_PLL_SDM_EN is enabled, N in [1 .. 16]
#       define PLL_AUDIO_CTRL_PLL_FACTOR_N(...)         __PLL_AUDIO_CTRL_PLL_FACTOR_N((128UL, ##__VA_ARGS__))
//      M in [1 .. 32]
#       define PLL_AUDIO_CTRL_PLL_PREDIV_M(...)         __PLL_AUDIO_CTRL_PLL_PREDIV_M((32UL, ##__VA_ARGS__))
#   define CCU_PLL_VIDEO_CTRL                           0x010
#       define PLL_VIDEO_CTRL_PLL_ENABLE                (1UL << 31)
#       define PLL_VIDEO_CTRL_PLL_MODE                  (1UL << 30)
#       define PLL_VIDEO_CTRL_PLL_MODE_AUTO             PLL_VIDEO_CTRL_PLL_MODE
#       define PLL_VIDEO_CTRL_PLL_MODE_MANUAL           0
#       define PLL_VIDEO_CTRL_LOCK                      (1UL << 28)
#       define PLL_VIDEO_CTRL_FRAC_CLK_OUT              (1UL << 25)
#       define PLL_VIDEO_CTRL_PLL_MODE_SEL              (1UL << 24)
#       define PLL_VIDEO_CTRL_PLL_MODE_INTEGER          PLL_VIDEO_CTRL_PLL_MODE_SEL
#       define PLL_VIDEO_CTRL_PLL_MODE_FRACTIONAL       0
#       define PLL_VIDEO_CTRL_PLL_SDM_EN                (1UL << 20)
#       define __PLL_VIDEO_CTRL_PLL_FACTOR_N(__N)       (((__N) - 1) << 8)
#       define __PLL_VIDEO_CTRL_PLL_PREDIV_M(__M)       (((__M) - 1) << 0)
//      Integer mode:
//          PLL = (24MHz * N) / M, range [30MHz, 600MHz]
//      N in [1 .. 128]
#       define PLL_VIDEO_CTRL_PLL_FACTOR_N(...)         __PLL_VIDEO_CTRL_PLL_FACTOR_N((128UL, ##__VA_ARGS__))
//      M in [1 .. 16]
#       define PLL_VIDEO_CTRL_PLL_PREDIV_M(...)         __PLL_VIDEO_CTRL_PLL_PREDIV_M((16UL, ##__VA_ARGS__))
#   define CCU_PLL_VE_CTRL                              0x018
#       define PLL_VE_CTRL_PLL_ENABLE                   (1UL << 31)
#       define PLL_VE_CTRL_LOCK                         (1UL << 28)
#       define PLL_VE_CTRL_FRAC_CLK_OUT                 (1UL << 25)
#       define PLL_VE_CTRL_PLL_MODE_SEL                 (1UL << 24)
#       define PLL_VE_CTRL_PLL_MODE_FRACTIONAL          (0UL << 24)
#       define PLL_VE_CTRL_PLL_MODE_INTEGER             (1UL << 24)
#       define __PLL_VE_CTRL_PLL_FACTOR_N(__N)          (((__N) - 1) << 8)
#       define __PLL_VE_CTRL_PLL_PREDIV_M(__M)          (((__M) - 1) << 0)
//      Integer mode:
//          PLL = (24MHz * N) / M, range [30MHz, 600MHz]
//      N in [1 .. 128]
#       define PLL_VE_CTRL_PLL_FACTOR_N(...)            __PLL_VE_CTRL_PLL_FACTOR_N((128UL, ##__VA_ARGS__))
//      M in [1 .. 16]
#       define PLL_VE_CTRL_PLL_PREDIV_M(...)            __PLL_VE_CTRL_PLL_PREDIV_M((16UL, ##__VA_ARGS__))
#   define CCU_PLL_DDR_CTRL                             0x020
#       define PLL_DDR_CTRL_PLL_ENABLE                  (1UL << 31)
#       define PLL_DDR_CTRL_LOCK                        (1UL << 28)
#       define PLL_DDR_CTRL_SDRAM_SIGMA_DELTA_EN        (1UL << 24)
#       define PLL_DDR_CTRL_PLL_DDR_CFG_UPDATE          (1UL << 20)
#       define __PLL_DDR_CTRL_PLL_FACTOR_N(__N)         (((__N) - 1) << 8)
#       define __PLL_DDR_CTRL_PLL_FACTOR_K(__K)         (((__K) - 1) << 4)
#       define __PLL_DDR_CTRL_PLL_FACTOR_M(__M)         (((__M) - 1) << 0)
//      PLL = (24MHz * N * K) / M
//      N in [1 .. 32]
#       define PLL_DDR_CTRL_PLL_FACTOR_N(...)           __PLL_DDR_CTRL_PLL_FACTOR_N((32UL, ##__VA_ARGS__))
//      K in [1 .. 4]
#       define PLL_DDR_CTRL_PLL_FACTOR_K(...)           __PLL_DDR_CTRL_PLL_FACTOR_K((4UL, ##__VA_ARGS__))
//      M in [1 .. 4]
#       define PLL_DDR_CTRL_PLL_FACTOR_M(...)           __PLL_DDR_CTRL_PLL_FACTOR_M((4UL, ##__VA_ARGS__))
#   define CCU_PLL_PERIPH_CTRL                          0x028
#       define PLL_PERIPH_CTRL_PLL_ENABLE               (1UL << 31)
#       define PLL_PERIPH_CTRL_LOCK                     (1UL << 28)
#       define PLL_PERIPH_CTRL_PLL_24M_OUT_EN           (1UL << 18)
#       define __PLL_PERIPH_CTRL_PLL_FACTOR_N(__N)      (((__N) - 1) << 8)
#       define __PLL_PERIPH_CTRL_PLL_FACTOR_K(__K)      (((__K) - 1) << 4)
//      PLL = 24MHz * N * K
//      N in [1 .. 32]
#       define PLL_PERIPH_CTRL_PLL_FACTOR_N(...)        __PLL_PERIPH_CTRL_PLL_FACTOR_N((32UL, ##__VA_ARGS__))
//      K in [1 .. 4]
#       define PLL_PERIPH_CTRL_PLL_FACTOR_K(...)        __PLL_PERIPH_CTRL_PLL_FACTOR_K((4UL, ##__VA_ARGS__))
#   define CCU_CPU_CLK_SRC                              0x050
#       define CPU_CLK_SRC_SEL                          (3UL << 16)
#       define CPU_CLK_SRC_SEL_LOSC                     (0UL << 16)
#       define CPU_CLK_SRC_SEL_OSC24M                   (1UL << 16)
#       define CPU_CLK_SRC_SEL_PLL_CPU                  (2UL << 16)
#   define CCU_AHB_APB_HCLKC_CFG                        0x054
#       define __AHB_APB_HCLKC_CFG_HCLKC_DIV(__DIV)     (((__DIV) - 1) << 16)
//      HCLKC_DIV in [1 .. 4]
#       define AHB_APB_HCLKC_CFG_HCLKC_DIV(...)         __AHB_APB_HCLKC_CFG_HCLKC_DIV((4UL, ##__VA_ARGS__))
#       define AHB_APB_HCLKC_CFG_AHB_CLK_SRC_SEL        (3UL << 12)
#       define AHB_APB_HCLKC_CFG_AHB_CLK_SRC_SEL_LOSC   (0UL << 12)
#       define AHB_APB_HCLKC_CFG_AHB_CLK_SRC_SEL_OSC24M (1UL << 12)
#       define AHB_APB_HCLKC_CFG_AHB_CLK_SRC_SEL_CPUCLK (2UL << 12)
#       define AHB_APB_HCLKC_CFG_AHB_CLK_SRC_SEL_PERIPH (3UL << 12)     // PLL_PERIPH/AHB_PRE_DIV
#       define __AHB_APB_HCLKC_CFG_APB_CLK_RATIO(__R)   ((__R) << 8)
#       define __AHB_APB_HCLKC_CFG_AHB_PRE_DIV(__DIV)   (((__DIV) - 1) << 6)
#       define __AHB_APB_HCLKC_CFG_AHB_CLK_DIV_RATIO(__R)   ((__R) << 4)
//      R in [1, 2, 3], divider is 2 ^ R
#       define AHB_APB_HCLKC_CFG_APB_CLK_RATIO(...)     __AHB_APB_HCLKC_CFG_APB_CLK_RATIO((3UL, ##__VA_ARGS__))
//      AHB_PRE_DIV in [1 .. 4]
#       define AHB_APB_HCLKC_CFG_AHB_PRE_DIV(...)       __AHB_APB_HCLKC_CFG_AHB_PRE_DIV((4UL, ##__VA_ARGS__))
//      R in [0 .. 3], divider is 2 ^ R
#       define AHB_APB_HCLKC_CFG_AHB_CLK_DIV_RATIO(...) __AHB_APB_HCLKC_CFG_AHB_CLK_DIV_RATIO((3UL, ##__VA_ARGS__))
#   define CCU_BUS_CLK_GATINT0                          0x060
#       define BUS_CLK_GATING0_USB_OTG_GATING           (1UL << 24)
#       define BUS_CLK_GATING0_SPI1_GATING              (1UL << 21)
#       define BUS_CLK_GATING0_SPI0_GATING              (1UL << 20)
#       define BUS_CLK_GATING0_SDRAM_GATING             (1UL << 14)
#       define BUS_CLK_GATING0_SD1_GATING               (1UL << 9)
#       define BUS_CLK_GATING0_SD0_GATING               (1UL << 8)
#       define BUS_CLK_GATING0_DMA_GATING               (1UL << 6)
#   define CCU_BUS_CLK_GATE1                            0x064
#       define BUS_CLK_GATING1_DEFE_GATING              (1UL << 14)
#       define BUS_CLK_GATING1_DEBE_GATING              (1UL << 12)
#       define BUS_CLK_GATING1_TVE_GATING               (1UL << 10)
#       define BUS_CLK_GATING1_TBD_GATING               (1UL << 9)
#       define BUS_CLK_GATING1_CSI_GATING               (1UL << 8)
#       define BUS_CLK_GATING1_DEINTERLACE_GATING       (1UL << 5)
#       define BUS_CLK_GATING1_LCD_GATING               (1UL << 4)
#       define BUS_CLK_GATING1_VE_GATING                (1UL << 0)
#   define CCU_BUS_CLK_GATE2                            0x068
#       define BUS_CLK_GATING2_UART2_GATING             (1UL << 22)
#       define BUS_CLK_GATING2_UART1_GATING             (1UL << 21)
#       define BUS_CLK_GATING2_UART0_GATING             (1UL << 20)
#       define BUS_CLK_GATING2_TWI2_GATING              (1UL << 18)
#       define BUS_CLK_GATING2_TWI1_GATING              (1UL << 17)
#       define BUS_CLK_GATING2_TWI0_GATING              (1UL << 16)
#       define BUS_CLK_GATING2_RSB_GATING               (1UL << 3)
#       define BUS_CLK_GATING2_CIR_GATING               (1UL << 2)
#       define BUS_CLK_GATING2_OWA_GATING               (1UL << 1)
#       define BUS_CLK_GATING2_AUDIO_CODEC_GATING       (1UL << 0)
#   define CCU_SDMMC0_CLK                               0x088
#       define SDMMC0_CLK_SCLK_GATING                   (1UL << 31)
#       define SDMMC0_CLK_CLK_SRC_SEL                   (3UL << 24)
#       define SDMMC0_CLK_CLK_SRC_SEL_OSC24M            (0UL << 24)
#       define SDMMC0_CLK_CLK_SRC_SEL_PLL_PERIPH        (1UL << 24)
#       define __SDMMC0_CLK_SAMPLE_CLK_PHASE_CTR(__DLY) ((__DLY) << 20)
#       define __SDMMC0_CLK_CLK_DIV_RATIO_N(__N)        ((__N) << 16)
#       define __SDMMC0_CLK_OUTPUT_CLK_PHASE_CTR(__DLY) ((__DLY) << 8)
#       define __SDMMC0_CLK_CLK_DIV_RATIO_M(__M)        (((__M) - 1) << 0)
//      DLY in [0 .. 7]
#       define SDMMC0_CLK_SAMPLE_CLK_PHASE_CTR(...)     __SDMMC0_CLK_SAMPLE_CLK_PHASE_CTR((7UL, ##__VA_ARGS__))
//      N in [0 .. 3], divider is 2 ^ N
#       define SDMMC0_CLK_CLK_DIV_RATIO_N(...)          __SDMMC0_CLK_CLK_DIV_RATIO_N((3UL, ##__VA_ARGS__))
//      DLY in [0 .. 7]
#       define SDMMC0_CLK_OUTPUT_CLK_PHASE_CTR(...)     __SDMMC0_CLK_OUTPUT_CLK_PHASE_CTR((7UL, ##__VA_ARGS__))
//      M in [1 .. 16]
#       define SDMMC0_CLK_CLK_DIV_RATIO_M(...)          __SDMMC0_CLK_CLK_DIV_RATIO_M((16UL, ##__VA_ARGS__))
#   define CCU_SDMMC1_CLK                               0x08c
#       define SDMMC1_CLK_SCLK_GATING                   (1UL << 31)
#       define SDMMC1_CLK_CLK_SRC_SEL                   (3UL << 24)
#       define SDMMC1_CLK_CLK_SRC_SEL_OSC24M            (0UL << 24)
#       define SDMMC1_CLK_CLK_SRC_SEL_PLL_PERIPH        (1UL << 24)
#       define __SDMMC1_CLK_SAMPLE_CLK_PHASE_CTR(__DLY) ((__DLY) << 20)
#       define __SDMMC1_CLK_CLK_DIV_RATIO_N(__N)        ((__N) << 16)
#       define __SDMMC1_CLK_OUTPUT_CLK_PHASE_CTR(__DLY) ((__DLY) << 8)
#       define __SDMMC1_CLK_CLK_DIV_RATIO_M(__M)        (((__M) - 1) << 0)
//      DLY in [0 .. 7]
#       define SDMMC1_CLK_SAMPLE_CLK_PHASE_CTR(...)     __SDMMC1_CLK_SAMPLE_CLK_PHASE_CTR((7UL, ##__VA_ARGS__))
//      N in [0 .. 3], divider is 2 ^ N
#       define SDMMC1_CLK_CLK_DIV_RATIO_N(...)          __SDMMC1_CLK_CLK_DIV_RATIO_N((3UL, ##__VA_ARGS__))
//      DLY in [0 .. 7]
#       define SDMMC1_CLK_OUTPUT_CLK_PHASE_CTR(...)     __SDMMC1_CLK_OUTPUT_CLK_PHASE_CTR((7UL, ##__VA_ARGS__))
//      M in [1 .. 16]
#       define SDMMC1_CLK_CLK_DIV_RATIO_M(...)          __SDMMC1_CLK_CLK_DIV_RATIO_M((16UL, ##__VA_ARGS__))
#   define CCU_DAUDIO_CLK                               0x0b0
#       define DAUDIO_CLK_SCLK_GATING                   (1UL << 31)
#       define DAUTIO_CLK_CLK_SRC_SEL                   (3UL << 16)
#       define DAUTIO_CLK_CLK_SRC_SEL_PLL_AUDIO_8X      (0UL << 16)
#       define DAUTIO_CLK_CLK_SRC_SEL_PLL_AUDIO_8XD2    (1UL << 16)
#       define DAUTIO_CLK_CLK_SRC_SEL_PLL_AUDIO_8XD4    (2UL << 16)
#       define DAUTIO_CLK_CLK_SRC_SEL_PLL_AUDIO_8XD8    (3UL << 16)
#   define CCU_OWA_CLK                                  0x0b4
#       define OWA_CLK_SCLK_GATING                      (1UL << 31)
#       define OWA_CLK_CLK_SRC_SEL                      (3UL << 16)
#       define OWA_CLK_CLK_SRC_SEL_PLL2                 (0UL << 16)
#       define OWA_CLK_CLK_SRC_SEL_PLL2D2               (1UL << 16)
#       define OWA_CLK_CLK_SRC_SEL_PLL2D4               (2UL << 16)
#       define OWA_CLK_CLK_SRC_SEL_PLL2D8               (3UL << 16)
#   define CCU_CIR_CLK                                  0x0b8
#       define CIR_CLK_SCLK_GATING                      (1UL << 31)
#       define CIR_CLK_CLK_SRC_SEL                      (3UL << 24)
#       define CIR_CLK_CLK_SRC_SEL_LOSC                 (0UL << 24)
#       define CIR_CLK_CLK_SRC_SEL_OSC24M               (1UL << 24)
#       define __CIR_CLK_CLK_DIV_RATION_N(__N)          ((__N) << 16)
#       define __CIR_CLK_CLK_DIV_RATION_M(__M)          (((__M) - 1) << 0)
//      N in [0 .. 3], divider is 2 ^ N
#       define CIR_CLK_CLK_DIV_RATION_N(...)            __CIR_CLK_CLK_DIV_RATION_N((3UL, ##__VA_ARGS__))
//      M in [1 .. 16]
#       define CIR_CLK_CLK_DIV_RATION_M(...)            __CIR_CLK_CLK_DIV_RATION_M((16UL, ##__VA_ARGS__))
#   define CCU_USBPHY_CLK                               0x0cc
#       define USBPHY_CLK_SCLK_GATING                   (1UL << 1)
#       define USBPHY_CLK_USBPHY_RST                    (1UL << 0)
#   define CCU_DRAM_GATING                              0x100
#       define DRAM_GATING_BE_DCLK_GATING               (1UL << 26)
#       define DRAM_GATING_FE_DCLK_GATING               (1UL << 24)
#       define DRAM_GATING_TVD_DCLK_GATING              (1UL << 3)
#       define DRAM_GATING_DEINTERLACE_DCLK_GATING      (1UL << 2)
#       define DRAM_GATING_CSI_DCLK_GATING              (1UL << 1)
#       define DRAM_GATING_VE_DCLK_GATING               (1UL << 0)
#   define CCU_BE_CLK                                   0x104
#       define BE_CLK_SCLK_GATING                       (1UL << 31)
#       define BE_CLK_CLK_SRC_SEL                       (3UL << 24)
#       define BE_CLK_CLK_SRC_SEL_PLL_VIDEO             (0UL << 24)
#       define BE_CLK_CLK_SRC_SEL_PLL_PERIPH            (2UL << 24)
#       define __BE_CLK_CLK_DIV_RATIO_M(__M)            (((__M) - 1) << 0)
//      M in [1 .. 16]
#       define BE_CLK_CLK_DIV_RATIO_M(...)              __BE_CLK_CLK_DIV_RATIO_M((16, ##__VA_ARGS__))
#   define CCU_FE_CLK                                   0x10c
#       define FE_CLK_SCLK_GATING                       (1UL << 31)
#       define FE_CLK_CLK_SRC_SEL                       (3UL << 24)
#       define FE_CLK_CLK_SRC_SEL_PLL_VIDEO             (0UL << 24)
#       define FE_CLK_CLK_SRC_SEL_PLL_PERIPH            (2UL << 24)
#       define __FE_CLK_CLK_DIV_RATIO_M(__M)            (((__M) - 1) << 0)
//      M in [1 .. 16]
#       define FE_CLK_CLK_DIV_RATIO_M(...)              __FE_CLK_CLK_DIV_RATIO_M((16, ##__VA_ARGS__))
#   define CCU_TCON_CLK                                 0x118
#       define TCON_CLK_SCLK_GATING                     (1UL << 31)
#       define TCON_CLK_CLK_SRC_SEL                     (7UL << 24)
#       define TCON_CLK_CLK_SRC_SEL_PLL_VIDEO_1X        (0UL << 24)
#       define TCON_CLK_CLK_SRC_SEL_PLL_VIDEO_2X        (2UL << 24)
#   define CCU_DI_CLK                                   0x11c
#       define DI_CLK_SCLK_GATING                       (1UL << 31)
#       define DI_CLK_CLK_SRC_SEL                       (7UL << 24)
#       define DI_CLK_CLK_SRC_SEL_PLL_VIDEO_1X          (0UL << 24)
#       define DI_CLK_CLK_SRC_SEL_PLL_VIDEO_2X          (2UL << 24)
#       define __DI_CLK_CLK_DIV_RATIO_M(__M)            (((__M) - 1) << 0)
//      M in [1 .. 16]
#       define DI_CLK_CLK_DIV_RATIO_M(...)              __DI_CLK_CLK_DIV_RATIO_M((16UL, ##__VA_ARGS__))
#   define CCU_TVE_CLK                                  0x120
#       define TVE_CLK_SCLK2_GATING                     (1UL << 31)
#       define TVE_CLK_SCLK2_SRC_SEL                    (7UL << 24)
#       define TVE_CLK_SCLK2_SRC_SEL_PLL_VIDEO_1X       (0UL << 24)
#       define TVE_CLK_SCLK2_SRC_SEL_PLL_VIDEO_2X       (2UL << 24)
#       define TVE_CLK_SCLK1_GATING                     (1UL << 15)
#       define TVE_CLK_SCLK1_SRC_SEL                    (1UL << 8)
#       define TVE_CLK_SCLK1_SRC_SEL_TVE_SCLK2          (0UL << 8)
#       define TVE_CLK_SCLK1_SRC_SEL_TVE_SCLK2_D2       (1UL << 8)
#       define __TVE_CLK_CLK_DIV_RATIO_M(__M)           (((__M) - 1) << 0)
//      M in [1 .. 16]
#       define TVE_CLK_CLK_DIV_RATIO_M(...)             __TVE_CLK_CLK_DIV_RATIO_M((16UL, ##__VA_ARGS__))
#   define CCU_TVD_CLK                                  0x124
#       define TVD_CLK_SCLK_GATING                      (1UL << 31)
#       define TVD_CLK_CLK_SRC_SEL                      (7UL << 24)
#       define TVD_CLK_CLK_SRC_SEL_PLL_VIDEO_1X         (0UL << 24)
#       define TVD_CLK_CLK_SRC_SEL_OSC24M               (1UL << 24)
#       define TVD_CLK_CLK_SRC_SEL_PLL_VIDEO_2X         (2UL << 24)
#       define __TVD_CLK_CLK_DIV_RATIO_M(__M)           (((__M) - 1) << 0)
//      M in [1 .. 16]
#       define TVD_CLK_CLK_DIV_RATIO_M(...)             __TVD_CLK_CLK_DIV_RATIO_M((16UL, ##__VA_ARGS__))
#   define CCU_CSI_CLK                                  0x134
#       define CSI_CLK_CSI_MCLK_GATING                  (1UL << 15)
#       define CSI_CLK_MCLK_SRC_SEL                     (7UL << 8)
#       define CSI_CLK_MCLK_SRC_SEL_PLL_VIDEO_1X        (0UL << 8)
#       define CSI_CLK_MCLK_SRC_SEL_OSC24M              (5UL << 8)
#       define __CSI_CLK_CLSI_MCLK_DIV_M(__M)           (((__M( - 1) << 0)
//      M in [1 .. 16]
#       define CSI_CLK_CLSI_MCLK_DIV_M(...)             __CSI_CLK_CLSI_MCLK_DIV_M((16UL, ##__VA_ARGS__))
#   define CCU_VE_CLK                                   0x13c
#       define VE_CLK_SCLK_GATING                       (1UL << 31)
#   define CCU_AUDIO_CODEC_CLK                          0x140
#       define AUDIO_CODEC_CLK_SCLK_GATING              (1UL << 31)
#   define CCU_AVS_CLK                                  0x144
#       define AVS_CLK_SCLK_GATING                      (1UL << 31)
#   define CCU_PLL_STABLE_TIME0                         0x200
#       define __PLL_STABLE_TIME0_PLL_LOCK_TIME(__T)    ((__T) << 0)
//      T in [0 .. 0xFFFF]
#       define PLL_STABLE_TIME0_PLL_LOCK_TIME(...)      __PLL_STABLE_TIME0_PLL_LOCK_TIME((0xFFFFUL, ##__VA_ARGS__))
#   define CCU_PLL_STABLE_TIME1                         0x204
#       define __PLL_STABLE_TIME1_PLL_LOCK_TIME(__T)    ((__T) << 0)
//      T in [0 .. 0xFFFF]
#       define PLL_STABLE_TIME1_PLL_LOCK_TIME(...)      __PLL_STABLE_TIME1_PLL_LOCK_TIME((0xFFFFUL, ##__VA_ARGS__))
#   define CCU_PLL_CPU_BIAS                             0x220
#   define CCU_PLL_AUDIO_BIAS                           0x224
#   define CCU_PLL_VIDEO_BIAS                           0x228
#   define CCU_PLL_VE_BIAS                              0x22c
#   define CCU_PLL_DDR0_BIAS                            0x230
#   define CCU_PLL_PERIPH_BIAS                          0x234
#   define CCU_PLL_CPU_TUN                              0x250
#   define CCU_PLL_DDR_TUN                              0x260
#   define CCU_PLL_AUDIO_PAT_CTRL                       0x284
#   define CCU_PLL_VIDEO_PAT_CTRL                       0x288
#   define CCU_PLL_DDR_PAT_CTRL                         0x290
#   define CCU_BUS_SOFT_RST0                            0x2c0
#       define BUS_SOFT_RST0_USBOTG_RST                 (1UL << 24)
#       define BUS_SOFT_RST0_SPI1_RST                   (1UL << 21)
#       define BUS_SOFT_RST0_SPI0_RST                   (1UL << 20)
#       define BUS_SOFT_RST0_SDRAM_RST                  (1UL << 14)
#       define BUS_SOFT_RST0_SD1_RST                    (1UL << 9)
#       define BUS_SOFT_RST0_SD0_RST                    (1UL << 8)
#       define BUS_SOFT_RST0_DMA_RST                    (1UL << 6)
#   define CCU_BUS_SOFT_RST1                            0x2c4
#       define BUS_SOFT_RST1_DEFE_RST                   (1UL << 14)
#       define BUS_SOFT_RST1_DEBE_RST                   (1UL << 12)
#       define BUS_SOFT_RST1_TVE_RST                    (1UL << 10)
#       define BUS_SOFT_RST1_RVD_RST                    (1UL << 9)
#       define BUS_SOFT_RST1_CSI_RST                    (1UL << 8)
#       define BUS_SOFT_RST1_DEINTERLACE_RST            (1UL << 5)
#       define BUS_SOFT_RST1_LCD_RST                    (1UL << 4)
#       define BUS_SOFT_RST1_VE_RST                     (1UL << 0)
#   define CCU_BUS_SOFT_RST2                            0x2d0
#       define BUS_SOFT_RST2_UART2_RST                  (1UL << 22)
#       define BUS_SOFT_RST2_UART1_RST                  (1UL << 21)
#       define BUS_SOFT_RST2_UART0_RST                  (1UL << 20)
#       define BUS_SOFT_RST2_TWI2_RST                   (1UL << 18)
#       define BUS_SOFT_RST2_TWI1_RST                   (1UL << 17)
#       define BUS_SOFT_RST2_TWI0_RST                   (1UL << 16)
#       define BUS_SOFT_RST2_DAUDIO_RST                 (1UL << 12)
#       define BUS_SOFT_RST2_RSB_RST                    (1UL << 3)
#       define BUS_SOFT_RST2_CIR_RST                    (1UL << 2)
#       define BUS_SOFT_RST2_OWA_RST                    (1UL << 1)
#       define BUS_SOFT_RST2_AUDIO_CODEC_RST            (1UL << 0)


// dram
#define DRAM_BASE                                       ((dram_reg_t *)0x01c01000)
#   define DRAM_SCONR                                   0x000
#   define DRAM_STMG0R                                  0x004
#   define DRAM_STMG1R                                  0x008
#   define DRAM_SCTLR                                   0x00c
#   define DRAM_SREFR                                   0x010
#   define DRAM_SEXTMR                                  0x014
#   define DRAM_DDLYR                                   0x024
#   define DRAM_DADRR                                   0x028
#   define DRAM_DVALR                                   0x02c
#   define DRAM_DRPTR0                                  0x030
#   define DRAM_DRPTR1                                  0x034
#   define DRAM_DRPTR2                                  0x038
#   define DRAM_DRPTR3                                  0x03c
#   define DRAM_SEFR                                    0x040
#   define DRAM_MAE                                     0x044
#   define DRAM_ASPR                                    0x048
#   define DRAM_SDLY0                                   0x04C
#   define DRAM_SDLY1                                   0x050
#   define DRAM_SDLY2                                   0x054
#   define DRAM_MCR0                                    0x100
#   define DRAM_MCR1                                    0x104
#   define DRAM_MCR2                                    0x108
#   define DRAM_MCR3                                    0x10c
#   define DRAM_MCR4                                    0x110
#   define DRAM_MCR5                                    0x114
#   define DRAM_MCR6                                    0x118
#   define DRAM_MCR7                                    0x11c
#   define DRAM_MCR8                                    0x120
#   define DRAM_MCR9                                    0x124
#   define DRAM_MCR10                                   0x128
#   define DRAM_MCR11                                   0x12c
#   define DRAM_BWCR                                    0x140


// PIO
#define PIO_BASE                                        ((pio_reg_t *)0x01c20800)


// UART
#define UART0_BASE                                      ((uart_reg_t *)0x01c25000)
#define UART1_BASE                                      ((uart_reg_t *)0x01c25400)
#define UART2_BASE                                      ((uart_reg_t *)0x01c25800)
#   define UART_RBR                                     0x000
#   define UART_THR                                     0x000
#   define UART_DLL                                     0x000
#   define UART_DLH                                     0x004
#   define UART_IER                                     0x004
#       define IER_PTIME                                (1UL << 7)
#       define IER_EDSSI                                (1UL << 3)
#       define IER_ELSI                                 (1UL << 2)
#       define IER_ETBEI                                (1UL << 1)
#       define IER_ERBFI                                (1UL << 0)
#   define UART_IIR                                     0x008
#   define UART_FCR                                     0x008
#       define FCR_RT                                   (3UL << 6)
#       define FCR_RT_1                                 (0UL << 6)
#       define FCR_RT_QUARTER                           (1UL << 6)
#       define FCR_RT_HALF                              (2UL << 6)
#       define FCR_RT_2_LESS                            (3UL << 6)
#       define FCR_TFT                                  (3UL << 4)
#       define FCR_TFT_EMPTY                            (0UL << 4)
#       define FCR_TFT_2                                (1UL << 4)
#       define FCR_TFT_QUARTER                          (2UL << 4)
#       define FCR_TFT_HALF                             (3UL << 4)
#       define FCR_DMAM                                 (1UL << 3)
#       define FCR_XFIFOR                               (1UL << 2)
#       define FCR_RFIFOR                               (1UL << 1)
#       define FCR_FIFOE                                (1UL << 0)
#   define UART_LCR                                     0x00c
#       define LCR_DLAB                                 (1UL << 7)
#       define LCR_BC                                   (1UL << 6)
#       define LCR_EPS                                  (3UL << 4)
#       define LCR_EPS_ODD                              (0UL << 4)
#       define LCR_EPS_EVEN                             (1UL << 4)
#       define LCR_PEN                                  (1UL << 3)
#       define LCR_STOP                                 (1UL << 2)
#       define LCR_STOP_1                               (0UL << 2)
#       define LCR_STOP_2                               (1UL << 2)
#       define LCR_DLS                                  (3UL << 0)
#       define LCR_DLS_5                                (0UL << 0)
#       define LCR_DLS_6                                (1UL << 0)
#       define LCR_DLS_7                                (2UL << 0)
#       define LCR_DLS_8                                (3UL << 0)
#   define UART_MCR                                     0x010
#       define MCR_SIRE                                 (1UL << 6)
#       define MCR_AFCE                                 (1UL << 5)
#       define MCR_LOOP                                 (1UL << 4)
#       define MCR_RTS                                  (1UL << 1)
#       define MCR_DTR                                  (1UL << 0)

#   define UART_LSR                                     0x014
#   define UART_MSR                                     0x018
#   define UART_SCH                                     0x01c
#   define UART_USR                                     0x07c
#       define USR_RFF                                  (1UL << 4)
#       define USR_RFNE                                 (1UL << 3)
#       define USR_TFE                                  (1UL << 2)
#       define USR_TFNF                                 (1UL << 1)
#       define USR_BUSY                                 (1UL << 0)
#   define UART_TFL                                     0x080
#   define UART_RFL                                     0x084
#   define UART_HALT                                    0x0a4


// SPI
#define SPI0_BASE                                       ((spi_reg_t *)0x01c05000)
#define SPI1_BASE                                       ((spi_reg_t *)0x01c06000)
#   define SPI_GCR                                      0x004
#       define GCR_EN                                   (1UL << 0)
#       define GCR_MODE                                 (1UL << 1)
#       define GCR_MODE_MASTE                           (1UL << 1)
#       define GCR_MODE_SLAVE                           (0UL << 1)
#       define GCR_TP_EN                                (1UL << 7)
#       define GCR_SRST                                 (1UL << 31)
#   define SPI_TCR                                      0x008
#       define __TCR_CPHA(__CPHA)                       ((__CPHA) << 0)
#       define __TCR_CPOL(__CPOL)                       (((__CPOL) ^ 1) << 1)
#       define __TCR_SPOL(__SPOL)                       (((__SPOL) ^ 1) << 2)
//      CPHA in [0, 1]
#       define TCR_CPHA(...)                            __TCR_CPHA((1, ##__VA_ARGS__))
//      CPOL in [0, 1]
#       define TCR_CPOL(...)                            __TCR_CPOL((0, ##__VA_ARGS__))
//      SPOL in [0, 1]
#       define TCR_SPOL(...)                            __TCR_SPOL((0, ##__VA_ARGS__))
#       define TCR_SSCTL                                (1UL << 3)
#       define __TCR_SS_SEL(__SEL)                      ((__SEL) << 4)
//      SEL in [0 .. 3]
#       define TCR_SS_SEL(...)                          __TCR_SS_SEL((3, ##__VA_ARGS__))
#       define TCR_SS_OWNER                             (1UL << 6)
#       define TCR_SS_OWNER_SPI                         (0UL << 6)
#       define TCR_SS_OWNER_SOFTWARE                    (1UL << 6)
#       define __TCR_SS_LEVEL(__LVL)                    ((__LVL) << 7)
//      LVL in [0 .. 1]
#       define TCR_SS_LEVEL(...)                        __TCR_SS_LEVEL((1, ##__VA_ARGS__))
#       define TCR_DHB                                  (1UL << 8)
#       define TCR_DDB                                  (1UL << 9)
#       define TCR_RPSM                                 (1UL << 10)
#       define TCR_RPSM_NORMAL                          (0UL << 10)
#       define TCR_RPSM_RAPID                           (1UL << 10)
#       define TCR_SDC                                  (1UL << 11)
#       define TCR_FBS                                  (1UL << 12)
#       define TCR_FBS_MSB_FIRST                        (0UL << 12)
#       define TCR_FBS_LSB_FIRST                        (1UL << 12)
#       define TCR_SDM                                  (1UL << 13)
#       define TCR_XCH                                  (1UL << 31)
#   define SPI_IER                                  0x010
#   define SPI_ISR                                  0x014
#   define SPI_FCR                                  0x018
#       define FCR_RX_TRIG_LEVEL(__LVL)             ((__LVL) << 0)
#       define FCR_RF_DRQ_EN                        (1UL << 8)
#       define FCR_RX_DMA_MODE                      (1UL << 9)
#       define FCR_RX_DMA_MODE_NORMAL               (0UL << 9)
#       define FCR_RX_DMA_MODE_DEDICATE             (1UL << 9)
#       define FCR_RX_FIFO_ACCESS_SIZE              (3UL << 10)
#       define FCR_RX_FIFO_ACCESS_SIZE_BYTE         (0UL << 10)
#       define FCR_RX_FIFO_ACCESS_SIZE_WORD         (1UL << 10)
#       define FCR_RX_FIFO_ACCESS_SIZE_BY_BUS       (3UL << 10)
#       define FCR_RF_TEST                          (1UL << 14)
#       define FCR_RF_RST                           (1UL << 15)
#       define FCR_TX_TRIG_LEVEL(__LVL)             ((__LVL) << 16)
#       define FCR_TX_FIFO_ACCESS_SIZE              (3UL << 26)
#       define FCR_TX_FIFO_ACCESS_SIZE_BYTE         (0UL << 26)
#       define FCR_TX_FIFO_ACCESS_SIZE_WORD         (1UL << 26)
#       define FCR_TX_FIFO_ACCESS_SIZE_BY_BUS       (3UL << 26)
#       define FCR_TF_TEST                          (1UL << 30)
#       define FCR_TF_RST                           (1UL << 31)
#   define SPI_FSR                                  0x01c
#   define SPI_WCR                                  0x020
#   define SPI_CCR                                  0x024
#       define CCR_CDR2(__N)                        ((__N) << 0)
#       define CCR_CDR1(_N)                         ((__N) << 8)
#       define CCR_DRS                              (1UL << 12)
#       define CCR_DRS_CDR1                         (0UL << 12)
#       define CCR_DRS_CDR2                         (1UL << 12)
#   define SPI_MBC                                  0x030
#   define SPI_MTC                                  0x034
#   define SPI_BCC                                  0x038
#   define SPI_TXD                                  0x200
#   define SPI_RXD                                  0x300


#define TCON_BASE                                   ((tcon_reg_t *)0x01C0C000)
#   define TCON_CTRL                                0x000
#       define TCON_CTRL_MODULE_EN                  (1UL << 31)
#       define TCON_CTRL_IO_MAP_SEL                 (1UL << 0)
#       define TCON_CTRL_IO_MAP_SEL_TCON0           (0UL << 0)
#       define TCON_CTRL_IO_MAP_SEL_TCON1           (1UL << 0)
#   define TCON_INT_REG0                            0x004
#   define TCON_INT_REG1                            0x008
#   define TCON_FRM_CTRL                            0x010
#       define TCON_FRM_CTRL_TCON0_FRM_EN           (1UL << 31)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_R       (1UL << 6)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_R6      (0UL << 6)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_R5      (1UL << 6)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_G       (1UL << 5)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_G6      (0UL << 5)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_G5      (1UL << 5)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_B       (1UL << 4)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_B6      (0UL << 4)
#       define TCON_FRM_CTRL_TCON0_FRM_MODE_B5      (1UL << 4)
#   define TCON_FRM_SEED0_R                         0x014
#   define TCON_FRM_SEED0_G                         0x018
#   define TCON_FRM_SEED0_B                         0x01c
#   define TCON_FRM_SEED1_R                         0x020
#   define TCON_FRM_SEED1_G                         0x024
#   define TCON_FRM_SEED1_B                         0x028
#   define TCON_FRM_TBL0                            0x02c
#   define TCON_FRM_TBL1                            0x030
#   define TCON_FRM_TBL2                            0x034
#   define TCON_FRM_TBL3                            0x038
#   define TCON0_CTRL                               0x040
#       define TCON0_CTRL_EN                        (1UL << 31)
#       define TCON0_CTRL_IF                        (3UL << 24)
#       define TCON0_CTRL_IF_HV                     (0UL << 24)
#       define TCON0_CTRL_IF_8080                   (1UL << 24)
#       define TCON0_CTRL_RBG_GBR                   (1UL << 23)
#       define __TCON0_CTRL_STA_DLY(__DLY)          ((__DLY) << 4)
#       define TCON0_CTRL_STA_DLY(...)              __TCON0_CTRL_STA_DLY((0x1FUL, ##__VA_ARGS__))
#   define TCON0_CLK_CTRL                           0x044
//      TODO: MAYBE ERROR
#       define TCON0_CLK_CTRL_LCKL_EN               (0xFUL << 28)
#       define __TCON0_CLK_CTRL_DCLKDIV(__DIV)      ((__DIV) << 0)
//      Tdclk = Tsclk * DCLKDIV
//      if DCLK1 and DCLK2 are used, DIV in (5, 96)
//      if DCLK only, DIV in [2, 4 .. 255]
#       define TCON0_CLK_CTRL_DCLKDIV(...)          __TCON0_CLK_CTRL_DCLKDIV((255UL, ##__VA_ARGS__))
#   define TCON0_BASIC_TIMING0                      0x048
#   define TCON0_BASIC_TIMING1                      0x04c
#   define TCON0_BASIC_TIMING2                      0x050
#   define TCON0_BASIC_TIMING3                      0x054
#   define TCON0_HV_TIMING                          0x058
#   define TCON0_CPU_IF                             0x060
#   define TCON0_CPU_WR                             0x064
#   define TCON0_CPU_RD                             0x068
#   define TCON0_CPU_RD_NX                          0x06c
#   define TCON0_IO_CTRL0                           0x088
#       define TCON0_IO_CTRL0_DCLK_SEL              (3UL << 28)
#       define TCON0_IO_CTRL0_DCLK_SEL_DCLK0        (0UL << 28)
#       define TCON0_IO_CTRL0_DCLK_SEL_DCLK1        (1UL << 28)
#       define TCON0_IO_CTRL0_DCLK_SEL_DCLK2        (2UL << 28)
#       define TCON0_IO_CTRL0_IO3_INV               (1UL << 27)     // DEN
#       define TCON0_IO_CTRL0_IO2_INV               (1UL << 26)     // CLK
#       define TCON0_IO_CTRL0_IO1_INV               (1UL << 25)     // H_SYNC
#       define TCON0_IO_CTRL0_IO0_INV               (1UL << 24)     // V_SYNC
#   define TCON0_IO_CTRL1                           0x08c
#   define TCON1_CTRL                               0x090
#       define TCON1_CTRL_EN                        (1UL << 31)
#   define TCON1_BASIC0                             0x094
#   define TCON1_BASIC1                             0x098
#   define TCON1_BASIC2                             0x09c
#   define TCON1_BASIC3                             0x0a0
#   define TCON1_BASIC4                             0x0a4
#   define TCON1_BASIC5                             0x0a8
#   define TCON1_IO_CTRL0                           0x0f0
#   define TCON1_IO_CTRL1                           0x0f4
#   define TCON_DEBUG_INFO                          0x0fc


#define DEBE_BASE                                   ((debe_reg_t *)0x01E60000)
#   define DEBE_MODE_CTRL                           0x800
//      LAYER in [0 .. 3]
#       define DEBE_MODE_CTRL_LAYER_EN(__LAYER)     ((1UL << __LAYER) << 8)
#       define DEBE_MODE_CTRL_CHANNEL_START         (1UL << 1)
#       define DEBE_MODE_CTRL_DEBE_EN               (1UL << 0)
#   define DEBE_BACKCOLOR                           0x804
#   define DEBE_DISP_SIZE                           0x808
//      W: 11-bit
#       define DEBE_DISP_SIZE_WIDTH(__W)            (((__W) - 1) << 0)
//      H: 11-bit
#       define DEBE_DISP_SIZE_HEIGHT(__H)           (((__H) - 1) << 16)
#   define DEBE_LAY0_SIZE                           0x810
#   define DEBE_LAY1_SIZE                           0x814
#   define DEBE_LAY2_SIZE                           0x818
#   define DEBE_LAY3_SIZE                           0x81c
//      W: 11-bit
#       define DEBE_LAY_SIZE_WIDTH(__W)             (((__W) - 1) << 0)
//      H: 11-bit
#       define DEBE_LAY_SIZE_HEIGHT(__H)            (((__H) - 1) << 16)
#   define DEBE_LAY0_CODNT                          0x820
#   define DEBE_LAY1_CODNT                          0x824
#   define DEBE_LAY2_CODNT                          0x828
#   define DEBE_LAY3_CODNT                          0x82c
#       define DEBE_LAY_CODNT_X(__X)                ((__X) << 0)
#       define DEBE_LAY_CODNT_Y(__Y)                ((__Y) << 16)
#   define DEBE_LAY0_LINEWIDTH                      0x840
#   define DEBE_LAY1_LINEWIDTH                      0x844
#   define DEBE_LAY2_LINEWIDTH                      0x848
#   define DEBE_LAY3_LINEWIDTH                      0x84c
#       define DEBE_LAY_LINEWIDTH_BIT(__W)          (__W)
#       define DEBE_LAY_LINEWIDTH_BYTE(__W)         ((__W) << 3)
#       define DEBE_LAY_LINEWIDTH_HWORD(__W)        ((__W) << 4)
#       define DEBE_LAY_LINEWIDTH_WORD(__W)         ((__W) << 5)
#   define DEBE_LAY0_FB_ADDR0                       0x850
#   define DEBE_LAY1_FB_ADDR0                       0x854
#   define DEBE_LAY2_FB_ADDR0                       0x858
#   define DEBE_LAY3_FB_ADDR0                       0x85c
#   define DEBE_LAY0_FB_ADDR1                       0x860
#   define DEBE_LAY1_FB_ADDR1                       0x864
#   define DEBE_LAY2_FB_ADDR1                       0x868
#   define DEBE_LAY3_FB_ADDR1                       0x86c
#   define DEBE_REGBUFF_CTRL                        0x870
#       define DEBE_REGBUFF_CTRL_DIABLE_AUTO_RELOAD (1UL << 1)
#       define DEBE_REGBUFF_CTRL_RELOAD             (1UL << 0)
#   define DEBE_CK_MAX                              0x880
#   define DEBE_CK_MIN                              0x884
#   define DEBE_CK_CFG                              0x888
#   define DEBE_LAY0_ATT_CTRL0                      0x890
#   define DEBE_LAY1_ATT_CTRL0                      0x894
#   define DEBE_LAY2_ATT_CTRL0                      0x898
#   define DEBE_LAY3_ATT_CTRL0                      0x89c
#   define DEBE_LAY0_ATT_CTRL1                      0x8a0
#   define DEBE_LAY1_ATT_CTRL1                      0x8a4
#   define DEBE_LAY2_ATT_CTRL1                      0x8a8
#   define DEBE_LAY3_ATT_CTRL1                      0x8ac
#   define DEBE_HWC_CTRL                            0x8d8
#   define DEBE_HWCFB_CTRL                          0x8e0
#   define DEBE_WB_CTRL                             0x8f0
#   define DEBE_WB_ADDR                             0x8f4
#   define DEBE_WB_LW                               0x8f8
#   define DEBE_IYUV_CH_CTRL                        0x920
#   define DEBE_CH0_YUV_FB_ADDR                     0x930
#   define DEBE_CH1_YUV_FB_ADDR                     0x934
#   define DEBE_CH2_YUV_FB_ADDR                     0x938
#   define DEBE_CH0_YUV_BLW                         0x940
#   define DEBE_CH1_YUV_BLW                         0x944
#   define DEBE_CH2_YUV_BLW                         0x948
#   define DEBE_COEF00                              0x950
#   define DEBE_COEF01                              0x954
#   define DEBE_COEF02                              0x958
#   define DEBE_COEF03                              0x95c
#   define DEBE_COEF10                              0x960
#   define DEBE_COEF11                              0x964
#   define DEBE_COEF12                              0x968
#   define DEBE_COEF13                              0x96c
#   define DEBE_COEF20                              0x970
#   define DEBE_COEF21                              0x974
#   define DEBE_COEF22                              0x978
#   define DEBE_COEF23                              0x97c


#define TVE_BASE                                    ((tve_reg_t *)0x01c0a000)
#   define TVE_ENABLE                               0x000
#       define __TVE_ENABLE_DAC_MAP(__DAC, __OUT)   ((__OUT) << (((__DAC) + 1) << 2))
#       define TVE_ENABLE_DAC_MAP(__DAC, ...)       __TVE_ENABLE_DAC_MAP(_DAC, (0xF, ##__VA_ARGS__))
#       define TVE_ENABLE_EN                        (1UL << 0)
#   define TVE_CFG0                                 0x004
#       define TVE_CFG0_YC_EN                       (1UL << 17)
#       define TVE_CFG0_CVBS_EN                     (1UL << 16)
#       define TVE_CFG0_TVMODE_SELECT(...)          ((0xF, ##__VA_ARGS__) << 0)
#   define TVE_DAC1                                 0x008
#       define TVE_DAC1_CLOCK_INVERT                (1UL << 24)
#       define TVE_DAC1_DAC_EN(__DAC)               (1UL << (__DAC))
#   define TVE_NOTCH                                0x00c
#   define TVE_CHROMA_FREQUENCY                     0x010
#   define TVE_PORCH                                0x014
#   define TVE_LINE                                 0x01c
#   define TVE_LEVEL                                0x020
#   define TVE_DAC2                                 0x024
#   define TVE_DETECT_STATUS                        0x038
#   define TVE_CBCR_LEVEL                           0x10c
#   define TVE_BURST_WIDTH                          0x114
#   define TVE_CBCR_GAIN                            0x118
#   define TVE_SYNC_VBI                             0x11c
#   define TVE_ACTIVE_LINE                          0x124
#   define TVE_CHROMA                               0x128
#   define TVE_ENCODER                              0x12c
#   define TVE_RESYNC                               0x130
#   define TVE_SLAVE                                0x134


#define TIMER_BASE                                  ((timer_reg_t *)0x01c20c00)
#   define TMR_IRQ_EN                               0x000
#   define TMR_IRQ_STA                              0x004
#   define TMR0_CTRL                                0x010
#   define TMR0_INTV_VALUE                          0x014
#   define TMR0_CUR_VALUE                           0x018
#   define TMR1_CTRL                                0x020
#   define TMR1_INTV_VALUE                          0x024
#   define TMR1_CUR_VALUE                           0x028
#   define TMR2_CTRL                                0x030
#   define TMR2_INTV_VALUE                          0x034
#   define TMR2_CUR_VALUE                           0x038
#       define TMR_CTRL_MODE                        (1UL << 7)
#       define TMR_CTRL_MODE_CONTINUOUS             (0UL << 7)
#       define TMR_CTRL_MODE_SINGLE                 (1UL << 7)
#       define TMR_CTRL_CLK_SRC                     (3UL << 2)
#       define TMR_CTRL_CLK_SRC_LOSC                (0UL << 2)
#       define TMR_CTRL_CLK_SRC_OSC24M              (1UL << 2)
#       define TMR_CTRL_RELOAD                      (1UL << 1)
#       define TMR_CTRL_EN                          (1UL << 0)
#   define AVS_CNT_CTL                              0x080
#   define AVS_CNT0                                 0x084
#   define AVS_CNT1                                 0x088
#   define AVS_CNT_DIV                              0x08c
#   define WDOG_IRQ_EN                              0x0a0
#   define WDOG_IRQ_STA                             0x0a4
#   define WDOG_CTRL                                0x0b0
#   define WDOG_CFG                                 0x0b4
#   define WDOG_MODE                                0x0b8


#define MUSB_BASE                                   ((musb_reg_t *)0x01c13000)
//  use naming spec form musbmhdrc document
#   define MUSB_FAddr                               0x0098
#   define MUSB_Power                               0x0040
#       define MUSBD_Power_ISOUpdate                (1UL << 7)
#       define MUSBD_Power_SoftConn                 (1UL << 6)
#       define MUSBD_Power_HSEnab                   (1UL << 5)
#       define MUSB_Power_HSMode                    (1UL << 4)
#       define MUSB_Power_Reset                     (1UL << 3)
#       define MUSB_Power_Resume                    (1UL << 2)
#       define MUSB_Power_SuspendMode               (1UL << 1)
#       define MUSB_Power_EnableSuspendM            (1UL << 0)
#   define MUSB_IntrTx                              0x0044
#   define MUSB_IntrRx                              0x0046
#   define MUSB_IntrTxE                             0x0048
#   define MUSB_IntrRxE                             0x004a
#   define MUSB_IntrUSB                             0x004c
#       define MUSBD_IntrUSB_VBusError              (1UL << 7)
#       define MUSB_IntrUSB_SessReq                 (1UL << 6)
#       define MUSB_IntrUSB_Discon                  (1UL << 5)
#       define MUSBH_IntrUSB_Conn                   (1UL << 4)
#       define MUSB_IntrUSB_SOF                     (1UL << 3)
#       define MUSBD_IntrUSB_Reset                  (1UL << 2)
#       define MUSBH_IntrUSB_Babble                 (1UL << 2)
#       define MUSB_IntrUSB_Resume                  (1UL << 1)
#       define MUSBD_IntrUSB_Suspend                (1UL << 0)
#   define MUSB_IntrUSBE                            0x0050
#       define MUSBD_IntrUSBE_VBusError             (1UL << 7)
#       define MUSB_IntrUSBE_SessReq                (1UL << 6)
#       define MUSB_IntrUSBE_Discon                 (1UL << 5)
#       define MUSBH_IntrUSBE_Conn                  (1UL << 4)
#       define MUSB_IntrUSBE_SOF                    (1UL << 3)
#       define MUSBD_IntrUSBE_Reset                 (1UL << 2)
#       define MUSBH_IntrUSBE_Babble                (1UL << 2)
#       define MUSB_IntrUSBE_Resume                 (1UL << 1)
#       define MUSBD_IntrUSBE_Suspend               (1UL << 0)
#   define MUSB_Frame                               0x0054
#   define MUSB_Index                               0x0042
#   define MUSB_Testmode                            0x007c

#   define MUSB_TxMaxP                              0x0080
#   define MUSB_CSR0                                0x0082
#       define MUSBD_CSR0_FlushFIFO                 (1UL << 8)
#       define MUSBD_CSR0_ServicedSetupEnd          (1UL << 7)
#       define MUSBD_CSR0_ServicedRxPktRdy          (1UL << 6)
#       define MUSBD_CSR0_SendStall                 (1UL << 5)
#       define MUSBD_CSR0_SetupEnd                  (1UL << 4)
#       define MUSBD_CSR0_DataEnd                   (1UL << 3)
#       define MUSBD_CSR0_SentStall                 (1UL << 2)
#       define MUSBD_CSR0_TxPktRdy                  (1UL << 1)
#       define MUSBD_CSR0_RxPktRdy                  (1UL << 0)

#       define MUSBH_CSR0_DisPing                   (1UL << 11)
#       define MUSBH_CSR0_DataToggleWrEnable        (1UL << 10)
#       define MUSBH_CSR0_DataToggle                (1UL << 9)
#       define MUSBH_CSR0_FlushFIFO                 (1UL << 8)
#       define MUSBH_CSR0_NAKTimeout                (1UL << 7)
#       define MUSBH_CSR0_StatusPkt                 (1UL << 6)
#       define MUSBH_CSR0_ReqPkt                    (1UL << 5)
#       define MUSBH_CSR0_Error                     (1UL << 4)
#       define MUSBH_CSR0_SetupPkt                  (1UL << 3)
#       define MUSBH_CSR0_RxStall                   (1UL << 2)
#       define MUSBH_CSR0_TxPktRdy                  (1UL << 1)
#       define MUSBH_CSR0_RxPktRdy                  (1UL << 0)
#   define MUSB_TxCSR                               0x0082
#       define MUSBD_TxCSRL_IncompTx                (1UL << 7)
#       define MUSBD_TxCSRL_ClrDataTog              (1UL << 6)
#       define MUSBD_TxCSRL_SentStall               (1UL << 5)
#       define MUSBD_TxCSRL_SendStall               (1UL << 4)
#       define MUSBD_TxCSRL_FlushFIFO               (1UL << 3)
#       define MUSBD_TxCSRL_UnderRun                (1UL << 2)
#       define MUSBD_TxCSRL_FIFONotEmpty            (1UL << 1)
#       define MUSBD_TxCSRL_TxPktRdy                (1UL << 0)
#       define MUSBD_TxCSRH_NAKTimeout              (1UL << 7)
#       define MUSBD_TxCSRH_AutoSet                 (1UL << 7)
#       define MUSBD_TxCSRH_ISO                     (1UL << 6)
#       define MUSBD_TxCSRH_Mode                    (1UL << 5)
#       define MUSBD_TxCSRH_Mode_Tx                 (1UL << 5)
#       define MUSBD_TxCSRH_Mode_Rx                 (0UL << 5)
#       define MUSBD_TxCSRH_DMAReqEnab              (1UL << 4)
#       define MUSBD_TxCSRH_FrcDataTog              (1UL << 3)
#       define MUSBD_TxCSRH_DMAReqMode              (1UL << 2)

#       define MUSBH_TxCSRL_NAKTimeout              (1UL << 7)
#       define MUSBH_TxCSRL_IncomTx                 (1UL << 7)
#       define MUSBH_TxCSRL_ClrDataTog              (1UL << 6)
#       define MUSBH_TxCSRL_RxStall                 (1UL << 5)
#       define MUSBH_TxCSRL_SetupPkt                (1UL << 4)
#       define MUSBH_TxCSRL_FlushFIFO               (1UL << 3)
#       define MUSBH_TxCSRL_Error                   (1UL << 2)
#       define MUSBH_TxCSRL_FIFONotEmpty            (1UL << 1)
#       define MUSBH_TxCSRL_TxPktRdy                (1UL << 0)
#       define MUSBH_TxCSRH_AutoSet                 (1UL << 7)
#       define MUSBH_TxCSRH_Mode                    (1UL << 6)
#       define MUSBH_TxCSRH_Mode_Tx                 (1UL << 5)
#       define MUSBH_TxCSRH_Mode_Rx                 (0UL << 5)
#       define MUSBH_TxCSRH_DMAReqEnab              (1UL << 4)
#       define MUSBH_TxCSRH_FrcDataTog              (1UL << 3)
#       define MUSBH_TxCSRH_DMAReqMode              (1UL << 2)
#       define MUSBH_TxCSRH_DataToggleWrEnable      (1UL << 1)
#       define MUSBH_TxCSRH_DataToggle              (1UL << 0)
#   define MUSB_RxMaxP                              0x0084
#   define MUSB_RxCSR                               0x0086
#       define MUSBD_RxCSRL_ClrDataTog              (1UL << 7)
#       define MUSBD_RxCSRL_SentStall               (1UL << 6)
#       define MUSBD_RxCSRL_SendStall               (1UL << 5)
#       define MUSBD_RxCSRL_FlushFIFO               (1UL << 4)
#       define MUSBD_RxCSRL_DataError               (1UL << 3)
#       define MUSBD_RxCSRL_OverRun                 (1UL << 2)
#       define MUSBD_RxCSRL_FIFOFull                (1UL << 1)
#       define MUSBD_RxCSRL_RxPktRdy                (1UL << 0)
#       define MUSBD_RxCSRH_AutoClear               (1UL << 7)
#       define MUSBD_RxCSRH_ISO                     (1UL << 6)
#       define MUSBD_RxCSRH_DMAReqEnab              (1UL << 5)
#       define MUSBD_RxCSRH_DisNyet                 (1UL << 4)
#       define MUSBD_RxCSRH_PIDError                (1UL << 4)
#       define MUSBD_RxCSRH_DMAReqMode              (1UL << 3)
#       define MUSBD_RxCSRH_IncompRx                (1UL << 0)

#       define MUSBH_RxCSRL_ClrDataTog              (1UL << 7)
#       define MUSBH_RxCSRL_RxStall                 (1UL << 6)
#       define MUSBH_RxCSRL_ReqPkt                  (1UL << 5)
#       define MUSBH_RxCSRL_FlushFIFO               (1UL << 4)
#       define MUSBH_RxCSRL_DataError               (1UL << 3)
#       define MUSBH_RxCSRL_NAKTimeout              (1UL << 3)
#       define MUSBH_RxCSRL_Error                   (1UL << 2)
#       define MUSBH_RxCSRL_FIFOFull                (1UL << 1)
#       define MUSBH_RxCSRL_RxPktRdy                (1UL << 0)
#       define MUSBH_RxCSRH_AutoClear               (1UL << 7)
#       define MUSBH_RxCSRH_AutoReq                 (1UL << 6)
#       define MUSBH_RxCSRH_DMAReqEnab              (1UL << 5)
#       define MUSBH_RxCSRH_PIDError                (1UL << 4)
#       define MUSBH_RxCSRH_DMAReqMode              (1UL << 3)
#       define MUSBH_RxCSRH_DataToggleWrEnable      (1UL << 2)
#       define MUSBH_RxCSRH_DataToggle              (1UL << 1)
#       define MUSBH_RxCSRH_IncompRx                (1UL << 0)
#   define MUSB_Count0                              0x0088
#   define MUSB_RxCount                             0x0088
#   define MUSB_Type0                               0x008c
#   define MUSB_TxType                              0x008c
#   define MUSB_NAKLimit0                           0x008d
#   define MUSB_TxInterval                          0x008d
#   define MUSB_RxType                              0x008e
#   define MUSB_RxInterval                          0x008f
#   define MUSB_ConfigData                          0x001f

#   define MUSB_FIFO0                               0x0000
#   define MUSB_FIFO1                               0x0004
#   define MUSB_FIFO2                               0x0008
#   define MUSB_FIFO3                               0x000c
#   define MUSB_FIFO4                               0x0010
#   define MUSB_FIFO5                               0x0014

#   define MUSB_DevCtl                              0x0041
#       define MUSB_DevCtl_FSDev                    (1UL << 6)
#       define MUSB_DevCtl_LSDev                    (1UL << 5)
#       define MUSB_DevCtl_HostMode                 (1UL << 2)
#       define MUSB_DevCtl_Session                  (1UL << 0)
#   define MUSB_MISC                                
#   define MUSB_TxFIFOsz                            0x0090
#   define MUSB_RxFIFOsz                            0x0094
#   define MUSB_TxFIFOadd                           0x0092
#   define MUSB_RxFIFOadd                           0x0096
#   define MUSB_TxFuncAddr                          0x0098
#   define MUSB_TxHubAddr                           0x009a
#   define MUSB_TxHubPort                           0x009b
#   define MUSB_RxFuncAddr                          0x009c
#   define MUSB_RxHubAddr                           0x009e
#   define MUSB_RxHubPort                           0x009f

#   define MUSB_Vendor0                             0x0043
#   define MUSB_Vendor1                             0x007d
#   define MUSB_Vendor2                             0x007e
#   define MUSB_HWVers                              
#   define MUSB_EPInfo                              0x0078
#   define MUSB_RAMInfo                             0x0079
#   define MUSB_LinkInfo                            0x007a
#   define MUSB_VPLen                               0x007b
#   define MUSB_HS_EOF1                             0x007c
#   define MUSB_FS_EOF1                             0x007d
#   define MUSB_LS_EOF1                             0x007e
#   define MUSB_SOFT_RST                            

#   define MUSB_ISCR                                0x0400
#       define MUSB_ISCR_VBUS_VALID_FROM_DATA       (1UL << 30)
#       define MUSB_ISCR_VBUS_VALID_FROM_VBUS       (1UL << 29)
#       define MUSB_ISCR_EXT_ID_STATUS              (1UL << 28)
#       define MUSB_ISCR_EXT_DM_STATUS              (1UL << 27)
#       define MUSB_ISCR_EXT_DP_STATUS              (1UL << 26)
#       define MUSB_ISCR_MERGED_VBUS_STATUS         (1UL << 25)
#       define MUSB_ISCR_MERGED_ID_STATUS           (1UL << 24)
#       define MUSB_ISCR_ID_PULLUP_EN               (1UL << 17)
#       define MUSB_ISCR_DPDM_PULLUP_EN             (1UL << 16)
#       define MUSB_ISCR_FORCE_ID                   (3UL << 14)
#       define MUSB_ISCR_FORCE_ID_DEVICE            (3UL << 14)
#       define MUSB_ISCR_FORCE_ID_HOST              (2UL << 14)
#       define MUSB_ISCR_FORCE_VBUS_VALID           (3UL << 12)
#       define MUSB_ISCR_FORCE_VBUS_VALID_HIGH      (3UL << 12)
#       define MUSB_ISCR_FORCE_VBUS_VALID_LOW       (2UL << 12)
#       define MUSB_ISCR_VBUS_VALID_SRC             (1UL << 10)
#       define MUSB_ISCR_HOSC_EN                    (1UL << 7)
#       define MUSB_ISCR_VBUS_CHANGE_DETECT         (1UL << 6)
#       define MUSB_ISCR_ID_CHANGE_DETECT           (1UL << 5)
#       define MUSB_ISCR_DPDM_CHANGE_DETECT         (1UL << 4)
#       define MUSB_ISCR_IRQ_ENABLE                 (1UL << 3)
#       define MUSB_ISCR_VBUS_CHANGE_DETECT_EN      (1UL << 2)
#       define MUSB_ISCR_ID_CHANGE_DETECT_EN        (1UL << 1)
#       define MUSB_ISCR_DPDM_CHANGE_DETECT_EN      (1UL << 0)


#define TP_BASE                                     ((tp_reg_t *)0x01c24800)
#   define TP_CTRL0                                 0x00
        #define ADC_FIRST_DLY(__DLY)                ((__DLY) << 24)         /* 8 bits */
        #define ADC_FIRST_DLY_MODE(__DLY_MODE)      ((__DLY_MODE) << 23)
        #define ADC_CLK_SEL(__CLK_SEL)              ((__CLK_SEL) << 22)
        #define ADC_CLK_DIV(__CLK_DIV)              ((__CLK_DIV) << 20)     /* 3 bits */
        #define FS_DIV(x)                           ((x) << 16)             /* 4 bits */
        #define T_ACQ(x)                            ((x) << 0)              /* 16 bits */
#   define TP_CTRL1                                 0x04
        #define STYLUS_UP_DEBOUN(x)                 ((x) << 12)             /* 8 bits */
        #define STYLUS_UP_DEBOUN_EN(x)              ((x) << 9)
        #define TOUCH_PAN_CALI_EN(x)                ((x) << 6)
        #define TP_DUAL_EN(x)                       ((x) << 5)
        #define TP_MODE_EN(x)                       ((x) << 4)
        #define TP_ADC_SELECT(x)                    ((x) << 3)
        #define ADC_CHAN_SELECT(x)                  ((x) << 0)              /* 3 bits */
#   define TP_CTRL2                                 0x08
        #define TP_SENSITIVE_ADJUST(x)              ((x) << 28)             /* 4 bits */
        #define TP_MODE_SELECT(x)                   ((x) << 26)             /* 2 bits */
        #define PRE_MEA_EN(x)                       ((x) << 24)
        #define PRE_MEA_THRE_CNT(x)                 ((x) << 0)              /* 24 bits */
#   define TP_CTRL3                                 0x0c
        #define FILTER_EN(x)                        ((x) << 2)
        #define FILTER_TYPE(x)                      ((x) << 0)              /* 2 bits */
#   define TP_INT_FIFOC                             0x10
        #define TEMP_IRQ_EN(x)                      ((x) << 18)
        #define OVERRUN_IRQ_EN(x)                   ((x) << 17)
        #define DATA_IRQ_EN(x)                      ((x) << 16)
        #define TP_DATA_XY_CHANGE(x)                ((x) << 13)
        #define FIFO_TRIG(x)                        ((x) << 8)              /* 5 bits */
        #define DATA_DRQ_EN(x)                      ((x) << 7)
        #define FIFO_FLUSH(x)                       ((x) << 4)
        #define TP_UP_IRQ_EN(x)                     ((x) << 1)
        #define TP_DOWN_IRQ_EN(x)                   ((x) << 0)
#   define TP_INT_FIFOS                             0x14
        #define TEMP_DATA_PENDING                   (1 << 18)
        #define FIFO_OVERRUN_PENDING                (1 << 17)
        #define FIFO_DATA_PENDING                   (1 << 16)
        #define TP_IDLE_FLG                         (1 << 2)
        #define TP_UP_PENDING                       (1 << 1)
        #define TP_DOWN_PENDING                     (1 << 0)
#   define TP_TPR                                   0x18
        #define TEMP_ENABLE(x)                      ((x) << 16)
        #define TEMP_PERIOD(x)                      ((x) << 0)              /* t = x * 256 * 16 / clkin */
#   define TP_CDAT                                  0x1c
#   define TP_TEMP_DATA                             0x20
#   define TP_DATA                                  0x24

/*============================ MACROFIED FUNCTIONS ===========================*/

#define read_reg8(__base, __reg)                    (*(volatile uint8_t *)((uint32_t)(__base) + (__reg)))
#define read_reg16(__base, __reg)                   (*(volatile uint16_t *)((uint32_t)(__base) + (__reg)))
#define read_reg32(__base, __reg)                   (*(volatile uint32_t *)((uint32_t)(__base) + (__reg)))
#define write_reg8(__base, __reg, __value)          (*(volatile uint8_t *)((uint32_t)(__base) + (__reg)) = (uint8_t)(__value))
#define write_reg16(__base, __reg, __value)         (*(volatile uint16_t *)((uint32_t)(__base) + (__reg)) = (uint16_t)(__value))
#define write_reg32(__base, __reg, __value)         (*(volatile uint32_t *)((uint32_t)(__base) + (__reg)) = (uint32_t)(__value))

/*============================ TYPES =========================================*/

// copied from utilities/compiler/__common/__type.h
#ifndef __REG_TYPE__
#define __REG_TYPE__

typedef volatile uint8_t            reg8_t;
typedef volatile uint16_t           reg16_t;
typedef volatile uint32_t           reg32_t;

#if defined(__IAR_SYSTEMS_ICC__)                                                \
    ||  (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)
#   define __REG_CONNECT(__A, __B)  __A##__B
#   define __REG_RSVD_NAME(__NAME)  __REG_CONNECT(__unused_, __NAME)
#else
#   define __REG_RSVD_NAME(__NAME)
#endif

#define ____REG_RSVD(__NAME, __BIT)                                             \
        reg##__BIT##_t              __NAME : __BIT;
#define ____REG_RSVD_N(__NAME, __BIT, __N)                                      \
        reg##__BIT##_t              __NAME[__N];
#define __REG_RSVD(__BIT)           ____REG_RSVD(REG_RSVD_NAME, __BIT)
#define __REG_RSVD_N(__BIT, __N)    ____REG_RSVD_N(REG_RSVD_NAME, __BIT, (__N))

#define REG_RSVD_NAME               __REG_RSVD_NAME(__LINE__)
#define REG_RSVD(__BIT)             __REG_RSVD(__BIT)
#define REG_RSVD_N(__BIT, __N)      __REG_RSVD_N(__BIT, (__N))

#define REG_RSVD_U8                 REG_RSVD(8)
#define REG_RSVD_U16                REG_RSVD(16)
#define REG_RSVD_U32                REG_RSVD(32)

#define REG_RSVD_U8N(__N)           REG_RSVD_N(8, (__N))
#define REG_RSVD_U16N(__N)          REG_RSVD_N(16, (__N))
#define REG_RSVD_U32N(__N)          REG_RSVD_N(32, (__N))

#define REG8_RSVD_N(__N)            REG_RSVD_U8N(__N)
#define REG8_RSVD_B(__BYTE_CNT)     REG8_RSVD_N(__BYTE_CNT)
#define REG8_RSVD_8B                REG8_RSVD_B(8)
#define REG8_RSVD_16B               REG8_RSVD_B(16)
#define REG8_RSVD_32B               REG8_RSVD_B(32)
#define REG8_RSVD_64B               REG8_RSVD_B(64)
#define REG8_RSVD_128B              REG8_RSVD_B(128)
#define REG8_RSVD_256B              REG8_RSVD_B(256)
#define REG8_RSVD_512B              REG8_RSVD_B(512)
#define REG8_RSVD_1K                REG8_RSVD_B(1024)
#define REG8_RSVD_2K                REG8_RSVD_B(2048)
#define REG8_RSVD_4K                REG8_RSVD_B(4096)
#define REG8_RSVD_8K                REG8_RSVD_B(8192)
#define REG8_RSVD_16K               REG8_RSVD_B(16 * 1024)
#define REG8_RSVD_32K               REG8_RSVD_B(32 * 1024)
#define REG8_RSVD_64K               REG8_RSVD_B(64 * 1024)
#define REG8_RSVD_128K              REG8_RSVD_B(128 * 1024)
#define REG8_RSVD_256K              REG8_RSVD_B(256 * 1024)
#define REG8_RSVD_512K              REG8_RSVD_B(512 * 1024)
#define REG8_RSVD_1M                REG8_RSVD_B(1024 * 1024)

#define REG16_RSVD_N(__N)           REG_RSVD_U16N(__N)
// __BYTE_CNT MUST be mutiple of 2
#define REG16_RSVD_B(__BYTE_CNT)    REG16_RSVD_N(__BYTE_CNT >> 1)
#define REG16_RSVD_8B               REG16_RSVD_B(8)
#define REG16_RSVD_16B              REG16_RSVD_B(16)
#define REG16_RSVD_32B              REG16_RSVD_B(32)
#define REG16_RSVD_64B              REG16_RSVD_B(64)
#define REG16_RSVD_128B             REG16_RSVD_B(128)
#define REG16_RSVD_256B             REG16_RSVD_B(256)
#define REG16_RSVD_512B             REG16_RSVD_B(512)
#define REG16_RSVD_1K               REG16_RSVD_B(1024)
#define REG16_RSVD_2K               REG16_RSVD_B(2048)
#define REG16_RSVD_4K               REG16_RSVD_B(4096)
#define REG16_RSVD_8K               REG16_RSVD_B(8192)
#define REG16_RSVD_16K              REG16_RSVD_B(16 * 1024)
#define REG16_RSVD_32K              REG16_RSVD_B(32 * 1024)
#define REG16_RSVD_64K              REG16_RSVD_B(64 * 1024)
#define REG16_RSVD_128K             REG16_RSVD_B(128 * 1024)
#define REG16_RSVD_256K             REG16_RSVD_B(256 * 1024)
#define REG16_RSVD_512K             REG16_RSVD_B(512 * 1024)
#define REG16_RSVD_1M               REG16_RSVD_B(1024 * 1024)

#define REG32_RSVD_N(__N)           REG_RSVD_U32N(__N)
// __BYTE_CNT MUST be mutiple of 4
#define REG32_RSVD_B(__BYTE_CNT)    REG_RSVD_U32N(__BYTE_CNT >> 2)
#define REG32_RSVD_8B               REG32_RSVD_B(8)
#define REG32_RSVD_16B              REG32_RSVD_B(16)
#define REG32_RSVD_32B              REG32_RSVD_B(32)
#define REG32_RSVD_64B              REG32_RSVD_B(64)
#define REG32_RSVD_128B             REG32_RSVD_B(128)
#define REG32_RSVD_256B             REG32_RSVD_B(256)
#define REG32_RSVD_512B             REG32_RSVD_B(512)
#define REG32_RSVD_1K               REG32_RSVD_B(1024)
#define REG32_RSVD_2K               REG32_RSVD_B(2048)
#define REG32_RSVD_4K               REG32_RSVD_B(4096)
#define REG32_RSVD_8K               REG32_RSVD_B(8192)
#define REG32_RSVD_16K              REG32_RSVD_B(16 * 1024)
#define REG32_RSVD_32K              REG32_RSVD_B(32 * 1024)
#define REG32_RSVD_64K              REG32_RSVD_B(64 * 1024)
#define REG32_RSVD_128K             REG32_RSVD_B(128 * 1024)
#define REG32_RSVD_256K             REG32_RSVD_B(256 * 1024)
#define REG32_RSVD_512K             REG32_RSVD_B(512 * 1024)
#define REG32_RSVD_1M               REG32_RSVD_B(1024 * 1024)

#endif      // __REG_TYPE__

typedef enum IRQn
{
    UART0_IRQn          = 1,
    UART1_IRQn,
    UART2_IRQn,
    OWA_IRQn            = 5,
    CIR_IRQn,
    TWI0_IRQn,
    TWI1_IRQn,                               /*!< IRQ8 */
    TWI2_IRQn,
    SPI0_IRQn,
    SPI1_IRQn,
                                            /*!< IRQ12 */ 
    Timer0_IRQn         = 13,
    Timer1_IRQn,
    Timer2_IRQn,
    WatchDog_IRQn,                           /*!< IRQ16 */
    RSB_IRQn,
    DMA_IRQn,

    TouchPanel_IRQn     = 20,               /*!< IRQ20 */
    AudioCodec_IRQn,
    KEYADC_IRQn,
    SDC0_IRQn,
    SDC1_IRQn,                               /*!< IRQ24 */
                         
    USBOTG_IRQn         = 26,
    TVD_IRQn,
    TVE_IRQn,                                /*!< IRQ28 */
    TCON_IRQn,
    DEFE_IRQn,
    DEBE_IRQn,
    CSI_IRQn,                                /*!< IRQ32 */
    DEInterlacer_IRQn,
    VE_IRQn,
    DAUDIO_IRQn,

    PIOD_IRQn           = 38,
    PIOE_IRQn,
    PIOF_IRQn,                               /*!< IRQ40 */

    SWI0_IRQn           = 60,               /*!< IRQ60 */
    SWI1_IRQn,
    SWI2_IRQn,
    SWI3_IRQn,
}IRQn_Type;



typedef struct syscon_reg_t {
    REG_RSVD_U32N(1)
    reg32_t USB_CTRL;
} syscon_reg_t;

typedef struct ccu_reg_t {
    reg32_t PLL_CPU_CTRL;                           // 0x000
    REG_RSVD_U32N(1)
    reg32_t PLL_AUDIO_CTRL;                         // 0x008
    REG_RSVD_U32N(1)
    reg32_t PLL_VIDEO_CTRL;                         // 0x010
    REG_RSVD_U32N(1)
    reg32_t PLL_VE_CTRL;                            // 0x018
    REG_RSVD_U32N(1)
    reg32_t PLL_DDR_CTRL;                           // 0x020
    REG_RSVD_U32N(1)
    reg32_t PLL_PERIPH_CTRL;                        // 0x028
    REG_RSVD_U32N(9)
    reg32_t CPU_CLK_SRC;                            // 0x050
    reg32_t AHB_APB_HCLKC_CFG;                      // 0x054
    REG_RSVD_U32N(2)
    reg32_t BUS_CLK_GATING0;                        // 0x060
    reg32_t BUS_CLK_GATING1;                        // 0x064
    reg32_t BUS_CLK_GATING2;                        // 0x068
    REG_RSVD_U32N(7)
    reg32_t SDMMC0_CLK;                             // 0x088
    reg32_t SDMMC1_CLK;                             // 0x08c
    REG_RSVD_U32N(8)
    reg32_t DAUDIO_CLK;                             // 0x0b0
    reg32_t OWA_CLK;                                // 0x0b4
    reg32_t CIR_CLK;                                // 0x0b8
    REG_RSVD_U32N(4)
    reg32_t USBPHY_CLK;                             // 0x0cc
    REG_RSVD_U32N(12)
    reg32_t DRAM_GATING;                            // 0x100
    reg32_t BE_CLK;                                 // 0x104
    REG_RSVD_U32N(1)
    reg32_t FE_CLK;                                 // 0x10c
    REG_RSVD_U32N(2)
    reg32_t TCON_CLK;                               // 0x118
    reg32_t DI_CLK;                                 // 0x11c
    reg32_t TVE_CLK;                                // 0x120
    reg32_t TVD_CLK;                                // 0x124
    REG_RSVD_U32N(3)
    reg32_t CSI_CLK;                                // 0x134
    REG_RSVD_U32N(1)
    reg32_t VE_CLK;                                 // 0x13c
    reg32_t AUDIO_CODEC_CLK;                        // 0x140
    reg32_t AVS_CLK;                                // 0x144
    REG_RSVD_U32N(46)
    reg32_t PLL_STABLE_TIME0;                       // 0x200
    reg32_t PLL_STABLE_TIME1;                       // 0x204
    REG_RSVD_U32N(6)
    reg32_t PLL_CPU_BIAS;                           // 0x220
    reg32_t PLL_AUDIO_BIAS;                         // 0x224
    reg32_t PLL_VIDEO_BIAS;                         // 0x228
    reg32_t PLL_VE_BIAS;                            // 0x22c
    reg32_t PLL_DDR0_BIAS;                          // 0x230
    reg32_t PLL_PERIPH_BIAS;                        // 0x234
    REG_RSVD_U32N(6)
    reg32_t PLL_CPU_TUN;                            // 0x250
    REG_RSVD_U32N(3)
    reg32_t PLL_DDR_TUN;                            // 0x260
    REG_RSVD_U32N(8)
    reg32_t PLL_AUDIO_PAT_CTRL;                     // 0x284
    reg32_t PLL_VIDEO_PAT_CTRL;                     // 0x288
    REG_RSVD_U32N(1)
    reg32_t PLL_DDR0_PAT_CTRL;                      // 0x290
    REG_RSVD_U32N(11)
    reg32_t BUS_SOFT_RST0;                          // 0x2c0
    reg32_t BUS_SOFT_RST1;                          // 0x2c4
    REG_RSVD_U32N(2)
    reg32_t BUS_SOFT_RST2;                          // 0x2d0
} ccu_reg_t;

typedef struct dram_reg_t {
    reg32_t SCONR;                                  // 0x000
    reg32_t STMG0R;                                 // 0x004
    reg32_t STMG1R;                                 // 0x008
    reg32_t SCTLR;                                  // 0x00c
    reg32_t SREFR;                                  // 0x010
    reg32_t SEXTMR;                                 // 0x014
    REG_RSVD_U32N(3)
    reg32_t DDLYR;                                  // 0x024
    reg32_t DADRR;                                  // 0x028
    reg32_t DVALR;                                  // 0x02c
    reg32_t DRPTR0;                                 // 0x030
    reg32_t DRPTR1;                                 // 0x034
    reg32_t DRPTR2;                                 // 0x038
    reg32_t DRPTR3;                                 // 0x03c
    reg32_t SEFR;                                   // 0x040
    reg32_t MAE;                                    // 0x044
    reg32_t ASPR;                                   // 0x048
    reg32_t SDLY0;                                  // 0x04C
    reg32_t SDLY1;                                  // 0x050
    reg32_t SDLY2;                                  // 0x054
    REG_RSVD_U32N(42)
    reg32_t MCR0;                                   // 0x100
    reg32_t MCR1;                                   // 0x104
    reg32_t MCR2;                                   // 0x108
    reg32_t MCR3;                                   // 0x10c
    reg32_t MCR4;                                   // 0x110
    reg32_t MCR5;                                   // 0x114
    reg32_t MCR6;                                   // 0x118
    reg32_t MCR7;                                   // 0x11c
    reg32_t MCR8;                                   // 0x120
    reg32_t MCR9;                                   // 0x124
    reg32_t MCR10;                                  // 0x128
    reg32_t MCR11;                                  // 0x12c
    REG_RSVD_U32N(4)
    reg32_t BWCR;                                   // 0x140
} dram_reg_t;

typedef struct pio_port_t {
    reg32_t CFG0;
    reg32_t CFG1;
    reg32_t CFG2;
    reg32_t CFG3;
    reg32_t DATA;
    reg32_t DRV0;
    reg32_t DRV1;
    reg32_t PUL0;
    reg32_t PUL1;
} pio_port_t;

typedef struct pio_port_int_t {
    reg32_t INT_CFG0;
    reg32_t INT_CFG1;
    reg32_t INT_CFG2;
    reg32_t INT_CFG3;
    reg32_t INT_CTRL;
    reg32_t INT_STA;
    reg32_t INT_DEB;
    REG_RSVD_U32N(1)
} pio_port_int_t;

typedef struct pio_reg_t {
    union {
        pio_port_t PORT[6];
        struct {
            pio_port_t PORTA;
            pio_port_t PORTB;
            pio_port_t PORTC;
            pio_port_t PORTD;
            pio_port_t PORTE;
            pio_port_t PORTF;
        };
    };
    REG_RSVD_U32N(74)
    pio_port_int_t PORT_INT[3];
    REG_RSVD_U32N(24)
    reg32_t SDR_PAD_DRV;
    reg32_t SDR_PAD_PUL;
} pio_reg_t;

typedef struct uart_reg_t {
    union {
        reg32_t RBR;                                // 0x000
        reg32_t THR;                                // 0x000
        reg32_t DLL;                                // 0x000
    };
    union {
        reg32_t DLH;                                // 0x004
        reg32_t IER;                                // 0x004
    };
    union {
        reg32_t IIR;                                // 0x008
        reg32_t FCR;                                // 0x008
    };
    reg32_t LCR;                                    // 0x00c
    reg32_t MCR;                                    // 0x010
    reg32_t LSR;                                    // 0x014
    reg32_t MSR;                                    // 0x018
    reg32_t SCH;                                    // 0x01c
    REG_RSVD_U32N(23)
    reg32_t USR;                                    // 0x07c
    reg32_t TFL;                                    // 0x080
    reg32_t RFL;                                    // 0x084
    REG_RSVD_U32N(7)
    reg32_t HALT;                                   // 0x0a4
} uart_reg_t;

typedef struct spi_reg_t {
    REG_RSVD_U32N(1)
    reg32_t GCR;                                    // 0x004
    reg32_t TCR;                                    // 0x008
    REG_RSVD_U32N(1)
    reg32_t IER;                                    // 0x010
    reg32_t ISR;                                    // 0x014
    reg32_t FCR;                                    // 0x018
    reg32_t FSR;                                    // 0x01c
    reg32_t WCR;                                    // 0x020
    reg32_t CCR;                                    // 0x024
    REG_RSVD_U32N(2)
    reg32_t MBC;                                    // 0x030
    reg32_t MTC;                                    // 0x034
    reg32_t BCC;                                    // 0x038
    REG_RSVD_U32N(113)
    union {
        reg32_t TXD_WORD;                           // 0x200
        reg8_t TXD_BYTE;                            // 0x200
    };
    REG_RSVD_U32N(63)
    union {
        reg32_t RXD_WORD;                           // 0x300
        reg8_t RXD_BYTE;                            // 0x300
    };
} spi_reg_t;

typedef struct tcon_reg_t {
    reg32_t CTRL;                                   // 0x000
    reg32_t INT_REG0;                               // 0x004
    reg32_t INT_REG1;                               // 0x008
    REG_RSVD_U32N(1)
    reg32_t FRM_CTRL;                               // 0x010
    union {
        struct {
            reg32_t FRM_SEED0_R;                    // 0x014
            reg32_t FRM_SEED0_G;                    // 0x018
            reg32_t FRM_SEED0_B;                    // 0x01c
            reg32_t FRM_SEED1_R;                    // 0x020
            reg32_t FRM_SEED1_G;                    // 0x024
            reg32_t FRM_SEED1_B;                    // 0x028
        };
        reg32_t FRM_SEED[6];
    };
    union {
        struct {
            reg32_t FRM_TBL0;                       // 0x02c
            reg32_t FRM_TBL1;                       // 0x030
            reg32_t FRM_TBL2;                       // 0x034
            reg32_t FRM_TBL3;                       // 0x038
        };
        reg32_t FRM_TBL[4];
    };
    REG_RSVD_U32N(1)
    struct {
        reg32_t CTRL;                               // 0x040
        reg32_t CLK_CTRL;                           // 0x044
        reg32_t BASIC_TIMING0;                      // 0x048: ACTIVE
        reg32_t BASIC_TIMING1;                      // 0x04c: HORIZONTAL
        reg32_t BASIC_TIMING2;                      // 0x050: VERTICAL
        reg32_t BASIC_TIMING3;                      // 0x054: SYNC
        reg32_t HV_TIMING;                          // 0x058
        REG_RSVD_U32N(1)
        reg32_t CPU_IF;                             // 0x060
        reg32_t CPU_WR;                             // 0x064
        reg32_t CPU_RD;                             // 0x068
        reg32_t CPU_RD_NX;                          // 0x06c
        REG_RSVD_U32N(6)
        reg32_t IO_CTRL0;                           // 0x088: PLORITY
        reg32_t IO_CTRL1;                           // 0x08c: TRISTATE
    } TCON0;
    struct {
        reg32_t CTRL;                               // 0x090
        reg32_t BASIC0;                             // 0x094
        reg32_t BASIC1;                             // 0x098
        reg32_t BASIC2;                             // 0x09c
        reg32_t BASIC3;                             // 0x0a0
        reg32_t BASIC4;                             // 0x0a4
        reg32_t BASIC5;                             // 0x0a8
        REG_RSVD_U32N(17)
        reg32_t IO_CTRL0;                           // 0x0f0: PLORITY
        reg32_t IO_CTRL1;                           // 0x0f4: TRISATE
    } TCON1;
    REG_RSVD_U32N(1)
    reg32_t DEBUG_INFO;                             // 0x0fc
} tcon_reg_t;

typedef struct debe_reg_t {
    REG_RSVD_U32N(512)
    reg32_t MODE_CTRL;                              // 0x800
    reg32_t BACKCOLOR;                              // 0x804
    reg32_t DISP_SIZE;                              // 0x808
    REG_RSVD_U32N(1)
    reg32_t LAY_SIZE[4];                            // 0x810
    reg32_t LAY_CODNT[4];                           // 0x820
    REG_RSVD_U32N(4)
    reg32_t LAY_LINEWIDTH[4];                       // 0x840
    reg32_t LAY_FB_ADDR0[4];                        // 0x850
    reg32_t LAY_FB_ADDR1[4];                        // 0x860
    reg32_t REGBUFF_CTRL;                           // 0x870
    REG_RSVD_U32N(3)
    reg32_t CK_MAX;                                 // 0x880
    reg32_t CK_MIN;                                 // 0x884
    reg32_t CK_CFG;                                 // 0x888
    REG_RSVD_U32N(1)
    reg32_t LAY_ATT_CTRL0[4];                       // 0x890
    reg32_t LAY_ATT_CTRL1[4];                       // 0x8a0
    REG_RSVD_U32N(10)
    reg32_t HWC_CTRL;                               // 0x8d8
    REG_RSVD_U32N(1)
    reg32_t HWCFB_CTRL;                             // 0x8e0
    REG_RSVD_U32N(3)
    reg32_t WB_CTRL;                                // 0x8f0
    reg32_t WB_ADDR;                                // 0x8f4
    reg32_t WB_LW;                                  // 0x8f8
    REG_RSVD_U32N(9)
    // YUV
    reg32_t IYUV_CH_CTRL;                           // 0x920
    REG_RSVD_U32N(3)
    reg32_t CH0_YUV_FB_ADDR;                        // 0x930
    reg32_t CH1_YUV_FB_ADDR;                        // 0x934
    reg32_t CH2_YUV_FB_ADDR;                        // 0x938
    REG_RSVD_U32N(1)
    reg32_t CH0_YUV_BLW;                            // 0x940
    reg32_t CH1_YUV_BLW;                            // 0x944
    reg32_t CH2_YUV_BLW;                            // 0x948
    REG_RSVD_U32N(1)
    reg32_t COEF00;                                 // 0x950
    reg32_t COEF01;                                 // 0x954
    reg32_t COEF02;                                 // 0x958
    reg32_t COEF03;                                 // 0x95c
    reg32_t COEF10;                                 // 0x960
    reg32_t COEF11;                                 // 0x964
    reg32_t COEF12;                                 // 0x968
    reg32_t COEF13;                                 // 0x96c
    reg32_t COEF20;                                 // 0x970
    reg32_t COEF21;                                 // 0x974
    reg32_t COEF22;                                 // 0x978
    reg32_t COEF23;                                 // 0x97c
    // TODO: add cursor buffer and platte
} debe_reg_t;

typedef struct tve_reg_t {
    reg32_t ENABLE;                                 // 0x000
    reg32_t CFG0;                                   // 0x004
    reg32_t DAC1;                                   // 0x008
    reg32_t NOTCH;                                  // 0x00c
    reg32_t CHROMA_FREQUENCY;                       // 0x010
    reg32_t PORCH;                                  // 0x014
    REG_RSVD_U32N(1)
    reg32_t LINE;                                   // 0x01c
    reg32_t LEVEL;                                  // 0x020
    reg32_t DAC2;                                   // 0x024
    REG_RSVD_U32N(4)
    reg32_t DETECT_STATUS;                          // 0x038
    REG_RSVD_U32N(52)
    reg32_t CBCR_LEVEL;                             // 0x10c
    REG_RSVD_U32N(1)
    reg32_t BURST_WIDTH;                            // 0x114
    reg32_t CBCR_GAIN;                              // 0x118
    reg32_t SYNC_VBI;                               // 0x11c
    REG_RSVD_U32N(1)
    reg32_t ACTIVE_LINE;                            // 0x124
    reg32_t CHROMA;                                 // 0x128
    reg32_t ENCODER;                                // 0x12c
    reg32_t RESYNC;                                 // 0x130
    reg32_t SLAVE;                                  // 0x134
} tve_reg_t;

typedef struct timer_reg_t {
    reg32_t IRQ_EN;                                 // 0x000
    reg32_t IRQ_STA;                                // 0x004
    REG_RSVD_U32N(2)

    struct {                                        // 0x010
        reg32_t CTRL;
        reg32_t INTV_VALUE;
        reg32_t CUR_VALUE;
        REG_RSVD_U32N(1)
    } TMR[3];

    REG_RSVD_U32N(16)

    struct {
        reg32_t CNT_CTL;                            // 0x080
        reg32_t CNT0;                               // 0x084
        reg32_t CNT1;                               // 0x088
        reg32_t CNT_DIV;                            // 0x08c
    } AVS;

    REG_RSVD_U32N(4)

    struct {
        reg32_t IRQ_EN;                             // 0x0a0
        reg32_t IRQ_STA;                            // 0x0a4
        REG_RSVD_U32N(2)
        reg32_t CTRL;                               // 0x0b0
        reg32_t CFG;                                // 0x0b4
        reg32_t MODE;                               // 0x0b8
    } WDOG;
} timer_reg_t;

typedef struct musb_reg_t {
    // use naming spec form musbmhdrc document
    union {
        struct {
            reg32_t FIFO0;                          // 0x0000
            reg32_t FIFO1;                          // 0x0004
            reg32_t FIFO2;                          // 0x0008
            reg32_t FIFO3;                          // 0x000c
            reg32_t FIFO4;                          // 0x0010
            reg32_t FIFO5;                          // 0x0014
        };
        reg32_t FIFO[6];
    };
    // unused FIFO registers
    REG_RSVD_U32N(10)

    struct {
        reg8_t Power;                               // 0x0040
        reg8_t DevCtl;                              // 0x0041
        reg8_t Index;                               // 0x0042
        reg8_t Vendor0;                             // 0x0043
        reg16_t IntrTx;                             // 0x0044
        reg16_t IntrRx;                             // 0x0046
        reg16_t IntrTxE;                            // 0x0048
        reg16_t IntrRxE;                            // 0x004a
        reg8_t IntrUSB;                             // 0x004c
        REG_RSVD_U8N(3)
        reg8_t IntrUSBE;                            // 0x0050
        REG_RSVD_U8N(3)
        reg16_t Frame;                              // 0x0054
        REG_RSVD_U8N(34)
        reg8_t EPInfo;                              // 0x0078
        reg8_t RAMInfo;                             // 0x0079
        reg8_t LinkInfo;                            // 0x007a
        reg8_t VPLen;                               // 0x007b
        reg8_t Testmode;                            // 0x007c
        reg8_t Vendor1;                             // 0x007d
        reg8_t Vendor2;                             // 0x007e
        REG_RSVD_U8N(1)
    } Common;

    struct {
        union {
            struct {
                union {
                    struct {
                        REG_RSVD_U16N(1)
                        reg16_t CSR0;               // 0x0082
                        REG_RSVD_U16N(2)
                        reg16_t Count0;             // 0x0088
                        REG_RSVD_U16N(3)
                    } EP0;
                    struct {
                        reg16_t TxMaxP;             // 0x0080
                        reg8_t TxCSRL;              // 0x0082
                        reg8_t TxCSRH;              // 0x0083
                        reg16_t RxMaxP;             // 0x0084
                        reg8_t RxCSRL;              // 0x0086
                        reg8_t RxCSRH;              // 0x0087
                        reg16_t RxCount;            // 0x0088
                        REG_RSVD_U16N(3)
                    } EPN;
                };
            } DC;
            struct {
                union {
                    struct {
                        REG_RSVD_U16N(1)
                        reg16_t CSR0;               // 0x0082
                        REG_RSVD_U16N(2)
                        reg16_t Count0;             // 0x0088
                        REG_RSVD_U16N(1)
                        reg8_t Type0;               // 0x008c
                        reg8_t NAKLimit0;           // 0x008d
                        REG_RSVD_U16N(1)
                    } EP0;
                    struct {
                        reg16_t TxMaxP;             // 0x0080
                        reg8_t TxCSRL;              // 0x0082
                        reg8_t TxCSRH;              // 0x0083
                        reg16_t RxMaxP;             // 0x0084
                        reg8_t RxCSRL;              // 0x0086
                        reg8_t RxCSRH;              // 0x0087
                        reg16_t RxCount;            // 0x0088
                        REG_RSVD_U16N(1)
                        reg8_t TxType;              // 0x008c
                        reg8_t TxInterval;          // 0x008d
                        reg8_t RxType;              // 0x008e
                        reg8_t RxInterval;          // 0x008f
                    } EPN;
                };
            } HC;
        };

        reg8_t TxFIFOsz;                            // 0x0090
        REG_RSVD_U8N(1)
        reg16_t TxFIFOadd;                          // 0x0092
        reg8_t RxFIFOsz;                            // 0x0094
        REG_RSVD_U8N(1)
        reg16_t RxFIFOadd;                          // 0x0096
        reg8_t TxFuncAddr;                          // 0x0098
        REG_RSVD_U8N(1)
        reg8_t TxHubAddr;                           // 0x009a
        reg8_t TxHubPort;                           // 0x009b
        reg8_t RxFuncAddr;                          // 0x009c
        REG_RSVD_U8N(1)
        reg8_t RxHubAddr;                           // 0x009e
        reg8_t RxHubPort;                           // 0x009f
    } Index;

    REG_RSVD_U32N(216)
    struct {
        reg32_t ISCR;                               // 0x0400
        reg32_t PHYCTL;                             // 0x0404
        reg32_t PHYBIST;                            // 0x0408
        reg32_t PHYTUNE;                            // 0x040c
    } Vendor;
} musb_reg_t;

typedef struct tp_reg_t {
    reg32_t CTRL0;                                  // 0x00
    reg32_t CTRL1;                                  // 0x04
    reg32_t CTRL2;                                  // 0x08
    reg32_t CTRL3;                                  // 0x0c
    reg32_t INT_FIFOC;                              // 0x10
    reg32_t INT_FIFOS;                              // 0x14
    reg32_t TPR;                                    // 0x18
    reg32_t CDAT;                                   // 0x1c
    reg32_t TEMP_DATA;                              // 0x20
    reg32_t DATA;                                   // 0x24
} tp_reg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
