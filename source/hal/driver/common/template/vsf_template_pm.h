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

#ifndef __HAL_DRIVER_PM_INTERFACE_H__
#define __HAL_DRIVER_PM_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// application code can redefine it
#ifndef VSF_PM_CFG_PREFIX
#   if defined(VSF_HW_PM_COUNT) && (VSF_HW_PM_COUNT != 0)
#       define VSF_PM_CFG_PREFIX                    vsf_hw
#   else
#       define VSF_PM_CFG_PREFIX                    vsf
#   endif
#endif

#define __PM_DIV_(_N, _D)     _D ## DIV_ ## _N = (_N),

#ifndef VSF_HAL_DRV_PM_CFG_SUPPORT_PLL
#   define VSF_HAL_DRV_PM_CFG_SUPPORT_PLL           ENABLED
#endif

#ifndef VSF_HAL_DRV_PM_CFG_SUPPORT_LPOSC
#   define VSF_HAL_DRV_PM_CFG_SUPPORT_LPOSC         ENABLED
#endif

#ifndef VSF_HAL_DRV_PM_CFG_SUPPORT_CLK_OUT
#   define VSF_HAL_DRV_PM_CFG_SUPPORT_CLK_OUT       ENABLED
#endif

#ifndef VSF_HAL_DRV_PM_CFG_SUPPORT_PCLK
#   define VSF_HAL_DRV_PM_CFG_SUPPORT_PCLK          ENABLED
#endif

#ifndef VSF_HAL_DRV_PM_CFG_SUPPORT_SCLK
#   define VSF_HAL_DRV_PM_CFG_SUPPORT_SCLK          ENABLED
#endif

#ifndef VSF_HAL_DRV_PM_CFG_SUPPORT_PWR_CTRL
#   define VSF_HAL_DRV_PM_CFG_SUPPORT_PWR_CTRL      ENABLED
#endif

#ifndef VSF_HAL_DRV_PM_CFG_SUPPORT_SLEEP_CTRL
#   define VSF_HAL_DRV_PM_CFG_SUPPORT_SLEEP_CTRL    ENABLED
#endif

/********************* REIMPLEMENT ***************************/

#ifndef VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER
#   define VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER      DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER_MASK
#   define VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER_MASK DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_SLEEP_MODE
#   define VSF_PM_CFG_REIMPLEMENT_SLEEP_MODE        DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_PCLK_NUMBER
#   define VSF_PM_CFG_REIMPLEMENT_PCLK_NUMBER       DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_PCLK_CFG
#   define VSF_PM_CFG_REIMPLEMENT_PCLK_CFG          DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER
#   define VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER       DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER_MASK
#   define VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER_MASK  DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_SCLK_SEL
#   define VSF_PM_CFG_REIMPLEMENT_SCLK_SEL          DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_SCLK_DIV
#   define VSF_PM_CFG_REIMPLEMENT_SCLK_DIV          DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_MCLK_CFG
#   define VSF_PM_CFG_REIMPLEMENT_MCLK_CFG          DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_MCLK_NO
#   define VSF_PM_CFG_REIMPLEMENT_MCLK_NO           DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_PLL_SEL
#   define VSF_PM_CFG_REIMPLEMENT_PLL_SEL           DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_PLL_CFG
#   define VSF_PM_CFG_REIMPLEMENT_PLL_CFG           DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_POST_DIV
#   define VSF_PM_CFG_REIMPLEMENT_POST_DIV          DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_LPOSC_SEL
#   define VSF_PM_CFG_REIMPLEMENT_LPOSC_SEL         DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_CLOCK_OUT_CFG
#   define VSF_PM_CFG_REIMPLEMENT_CLOCK_OUT_CFG     DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PM_POWER_APIS(__prefix_name)                                                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_power_status_t, pm, power_enable,       vsf_pm_power_cfg_no_t index)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_power_status_t, pm, power_disable,      vsf_pm_power_cfg_no_t index)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_power_status_t, pm, power_get_status,   vsf_pm_power_cfg_no_t index)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pm, power_resume,           vsf_pm_power_cfg_no_t index, vsf_pm_power_status_t status)

#define VSF_PM_SLEEP_APIS(__prefix_name)                                                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pm, sleep_enter,            vsf_pm_sleep_cfg_t *cfg_ptr)

#define VSF_PM_PCLK_APIS(__prefix_name)                                                                                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_pclk_status_t,  pm, pclk_config,        vsf_pm_pclk_no_t index, vsf_pm_pclk_cfg_t *cfg_ptr)        \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,         pm, pclk_get_clock,         vsf_pm_pclk_no_t index)                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_pclk_status_t,  pm, pclk_enable,        vsf_pm_pclk_no_t index)                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_pclk_status_t,  pm, pclk_disable,       vsf_pm_pclk_no_t index)                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_pclk_status_t,  pm, pclk_get_status,    vsf_pm_pclk_no_t index)                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pm, pclk_resume,            vsf_pm_pclk_no_t index, vsf_pm_pclk_status_t status)

#define VSF_PM_SCLK_APIS(__prefix_name)                                                                                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_sclk_status_t,  pm, sclk_enable,        vsf_pm_sclk_no_t index)                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_sclk_status_t,  pm, sclk_disable,       vsf_pm_sclk_no_t index)                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_sclk_status_t,  pm, sclk_get_status,    vsf_pm_sclk_no_t index)                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pm, sclk_resume,            vsf_pm_sclk_no_t index, vsf_pm_sclk_status_t status)

#define VSF_PM_MCLK_APIS(__prefix_name)                                                                                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,              pm, mclk_init,              vsf_pm_mclk_cfg_t *cfg_ptr)                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,         pm, mclk_get_clock,         vsf_pm_mclk_no_t sel)

#define VSF_PM_PLL_APIS(__prefix_name)                                                                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,              pm, pll_init,               vsf_pm_pll_sel_t pll, vsf_pm_pll_cfg_t *cfg_ptr)           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, bool,                  pm, pll_is_locked,          vsf_pm_pll_sel_t pll)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,         pm, pll_get_clock_out,      vsf_pm_pll_sel_t pll)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,         pm, pll_get_clock_in,       vsf_pm_pll_sel_t pll)

#define VSF_PM_LPOSC_APIS(__prefix_name)                                                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pm, lposc_init,             vsf_pm_lposc_sel_t lposc, vsf_pm_lposc_cfg_t *cfg_ptr)     \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  pm, lposc_enable,           vsf_pm_lposc_sel_t lposc)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  pm, lposc_disable,          vsf_pm_lposc_sel_t lposc)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,         pm, lposc_get_clock,        vsf_pm_lposc_sel_t lposc)

#define VSF_PM_MISC_APIS(__prefix_name)                                                                                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pm_capability_t,   pm, capability,             VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr)


#define VSF_PM_APIS(__prefix_name)      \
    VSF_PM_POWER_APIS(__prefix_name)    \
    VSF_PM_SLEEP_APIS(__prefix_name)    \
    VSF_PM_PCLK_APIS(__prefix_name)     \
    VSF_PM_SCLK_APIS(__prefix_name)     \
    VSF_PM_MCLK_APIS(__prefix_name)     \
    VSF_PM_PLL_APIS(__prefix_name)      \
    VSF_PM_LPOSC_APIS(__prefix_name)    \
    VSF_PM_MISC_APIS(__prefix_name)

/*============================ TYPES =========================================*/


/*----------------------------------------------------------------------------*
 * Power Domain Management                                                    *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER == DISABLED
typedef enum vsf_pm_power_cfg_no_t{
    POWER_IRC_IDX    = 1,
} vsf_pm_power_cfg_no_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER_MASK == DISABLED
typedef enum vsf_pm_power_cfg_msk_t {
    POWER_IRC_MSK    = 1UL << 0,
} vsf_pm_power_cfg_msk_t;
#endif

typedef uint_fast32_t vsf_pm_power_status_t;

/*----------------------------------------------------------------------------*
 * Sleep Management                                                           *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_SLEEP_MODE == DISABLED
typedef enum vsf_pm_sleep_mode_t {
    PM_WAIT       = 0,
    PM_SLEEP      = 1,
    PM_DEEP_SLEEP = 2,
    PM_POWER_OFF  = 3,
} vsf_pm_sleep_mode_t;
#endif

typedef struct vsf_pm_sleep_cfg_t {
    vsf_pm_sleep_mode_t sleep_mode;         //!< sleep mode
    uint32_t        sleep_cfg;          //!< Sleep mode cfg
    uint32_t        wake_cfg;           //!< Awake mode cfg
    uint32_t        sleep_walking_cfg;  //!< Sleep Walking mode cfg
} vsf_pm_sleep_cfg_t;

/*----------------------------------------------------------------------------*
 * AHB Clock Management                                                       *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER == DISABLED
typedef enum vsf_pm_sclk_no_t {
    SCLK_CORE_IDX   = 0,
    SCLK_ROM0_IDX   = 1,
    SCLK_PM0_IDX    = 2,
    SCLK_SRAM0_IDX  = 3,
} vsf_pm_sclk_no_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER_MASK == DISABLED
typedef enum vsf_pm_sclk_msk_t {
    SCLK_CORE_MSK    = 0x1ul << SCLK_CORE_IDX,
    SCLK_ROM0_MSK    = 0x1ul << SCLK_ROM0_IDX,
    SCLK_PM0_MSK     = 0x1ul << SCLK_ROM0_IDX,
    SCLK_SRAM0_MSK   = 0x1ul << SCLK_SRAM0_IDX,
} vsf_pm_sclk_msk_t;
#endif

typedef uint_fast32_t vsf_pm_sclk_status_t;

/*----------------------------------------------------------------------------*
 * Main Clock Management                                                      *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_SCLK_SEL == DISABLED
typedef enum vsf_pm_clk_src_sel_t {
    AUTO_CLKSRC_IRC                 = 0x0,      //!< source clk is IRC
    AUTO_CLKSRC_SYSOSC0             = 0x1,      //!< source clk is System oscilator
    AUTO_CLKSRC_SYSOSC1             = 0x2,      //!< source clock is WDTOSC
    AUTO_CLKSRC_EXTCLK0             = 0x3,      //!< Source clk is extern clock0
    AUTO_CLKSRC_EXTCLK1             = 0x4,      //!< source clk is extern clock1


    PLL_CLKSRC_IRC                  = 0x0,      //!< pll source clk is IRC
    PLL_CLKSRC_SYSOSC0              = 0x1,      //!< pll source clk is System oscilator
    PLL_CLKSRC_SYSOSC1              = 0x2,      //!< pll source clk is CLKIN
    PLL_CLKSRC_EXTCLK0              = 0x3,      //!< pll source clk is extern clock0
    PLL_CLKSRC_EXTCLK1              = 0x4,      //!< pll source clk is extern clock1

    MAIN_CLKSRC_IRC                 = 0x0,      //!< Maniclk source is IRC
    MAIN_CLKSRC_PLLIN               = 0x1,      //!< Maniclk source is System OSC0
    MAIN_CLKSRC_LPOSC               = 0x2,      //!< Maniclk source is LPOSC
    MAIN_CLKSRC_PLLOUT              = 0x3,      //!< Maniclk source is PLLOUT

    CLKOUT_CLKSRC_IRC               = 0x0,      //!< Clockout source is IRC
    CLKOUT_CLKSRC_SYSOSC0           = 0x1,      //!< Clockout source is System Oscillator 0
    CLKOUT_CLKSRC_LPOSC             = 0x2,      //!< Clockout source is LPOSC
    CLKOUT_CLKSRC_MCLK              = 0x3,      //!< Clockout source is PLLOUT
} vsf_pm_clk_src_sel_t;
#endif

//! main clock prescaler
#if VSF_PM_CFG_REIMPLEMENT_SCLK_DIV == DISABLED
typedef enum vsf_pm_divider_t {
    VSF_MREPEAT(255, __PM_DIV_, MAIN)
} vsf_pm_divider_t;
#endif

/*----------------------------------------------------------------------------*
 * Peripheral Clock Management                                                *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_PCLK_NUMBER == DISABLED
typedef enum vsf_pm_pclk_no_t {
    PCLK_PM0_IDX    = 8,        //!< PM 0
    PCLK_SPI0_IDX   = 9,        //!< SPI 0
    PCLK_SPI1_IDX   = 10,       //!< SPI 1
    PCLK_USART0_IDX = 12,       //!< USART0
    PCLK_USART1_IDX = 13,       //!< USART1
} vsf_pm_pclk_no_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_PCLK_CFG == DISABLED
typedef struct vsf_pm_pclk_cfg_t {
    vsf_pm_clk_src_sel_t clk_src;
    uint16_t         div;
} vsf_pm_pclk_cfg_t;
#endif

typedef int_fast16_t vsf_pm_pclk_status_t;

#if VSF_PM_CFG_REIMPLEMENT_MCLK_CORE_DIV == DISABLED
typedef enum vsf_pm_mclk_core_div_t {
    VSF_MREPEAT(255, __PM_DIV_, MCLK_CORE)
} vsf_pm_mclk_core_div_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_MCLK_AXI_DIV == DISABLED
typedef enum vsf_pm_mclk_axi_div_t {
    VSF_MREPEAT(255, __PM_DIV_, MCLK_AXI)
} vsf_pm_mclk_axi_div_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_MCLK_AHB_DIV == DISABLED
typedef enum vsf_pm_mclk_ahb_div_t {
    VSF_MREPEAT(255, __PM_DIV_, MCLK_AHB)
} vsf_pm_mclk_ahb_div_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_MCLK_APB_DIV == DISABLED
typedef enum vsf_pm_mclk_apb_div_t {
    VSF_MREPEAT(255, __PM_DIV_, MCLK_APB)
} vsf_pm_mclk_apb_div_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_MCLK_CFG == DISABLED
typedef struct vsf_pm_mclk_cfg_t {
    vsf_pm_clk_src_sel_t                clk_src;                //!< main clock source
    uint32_t                        freq;                   //!< system oscilator frequency
} vsf_pm_mclk_cfg_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_MCLK_NO == DISABLED
typedef enum vsf_pm_mclk_no_t {
    MCLK_CORE_IDX = 0,
    MCLK_CORE0_IDX = 0,

    MCLK_AXI0_IDX,
    MCLK_AXI1_IDX,

    MCLK_AHB0_IDX,
    MCLK_AHB1_IDX,

    MCLK_APB0_IDX,
    MCLK_APB1_IDX,
} vsf_pm_mclk_no_t;
#endif

/*----------------------------------------------------------------------------*
 * PLL Control                                                                *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_PLL_SEL == DISABLED
typedef enum vsf_pm_pll_sel_t {
    PLL0_IDX,
    PLL1_IDX,
} vsf_pm_pll_sel_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_PLL_CFG == DISABLED
typedef struct vsf_pm_pll_cfg_t {
    vsf_pm_clk_src_sel_t pll_clk_src;//!< pll clock source
    uint32_t         freq;       //!< system oscilator frequency
    uint8_t          msel;       //!< PLL Feedback divider value
    uint8_t          ssel;       //!< pll Feedback divider value
} vsf_pm_pll_cfg_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_POST_DIV == DISABLED
typedef enum vsf_pm_pll_post_div_t {
    PLL_POST_DIV_1                  = 0x00,     //!< pll post divider rate is 1
    PLL_POST_DIV_2                  = 0x01,     //!< pll post divider rate is 2
    PLL_POST_DIV_4                  = 0x02,     //!< pll post divider rate is 4
    PLL_POST_DIV_8                  = 0x03,     //!< pll post divider rate is 8
} vsf_pm_pll_post_div_t;
#endif

/*----------------------------------------------------------------------------*
 * Low Power Oscillator Management                                            *
 *----------------------------------------------------------------------------*/

typedef struct vsf_lposc_cfg_t vsf_lposc_cfg_t;

#if VSF_PM_CFG_REIMPLEMENT_LPOSC_SEL == DISABLED
typedef enum vsf_pm_lposc_sel_t {
    LPOSC_ALWAYS_ON,
    LPOSC_32K_OSC
} vsf_pm_lposc_sel_t;
#endif

typedef struct vsf_pm_lposc_cfg_t vsf_pm_lposc_cfg_t;

/*----------------------------------------------------------------------------*
 * Clock Management                                                           *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_CLOCK_OUT_CFG == DISABLED
typedef struct vsf_pm_clock_out_cfg_t {
    //! clk out source select
    vsf_pm_clk_src_sel_t                clk_src;    //!< clock source for output
    //! clk divider
    uint_fast8_t                    div;        //!< divider for output clock
} vsf_pm_clock_out_cfg_t;
#endif

typedef struct vsf_pm_capability_t {
    inherit(vsf_peripheral_capability_t)
} vsf_pm_capability_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifdef __cplusplus
}
#endif

#endif /* __HAL_DRIVER_PM_INTERFACE_H__ */
/* EOF */
