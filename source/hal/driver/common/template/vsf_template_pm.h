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

#ifndef VSF_PM_CFG_MULTI_CLASS
#   define VSF_PM_CFG_MULTI_CLASS              DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_PM_CFG_MULTI_CLASS is enabled
#ifndef VSF_PM_CFG_IMPLEMENT_OP
#   if VSF_PM_CFG_MULTI_CLASS == ENABLED
#       define VSF_PM_CFG_IMPLEMENT_OP         ENABLED
#   else
#       define VSF_PM_CFG_IMPLEMENT_OP         DISABLED
#   endif
#endif

// VSF_PM_CFG_PREFIX: use for macro vsf_flash_{init, enable, ...}
#ifndef VSF_PM_CFG_PREFIX
#   if VSF_PM_CFG_MULTI_CLASS == ENABLED
#       define VSF_PM_CFG_PREFIX               vsf
#   elif defined(VSF_HW_PM_COUNT) && (VSF_HW_PM_COUNT != 0)
#       define VSF_PM_CFG_PREFIX               vsf_hw
#   else
#       warning "Enable VSF_HAL_USE_PM support but no known implementation found"
#   endif
#endif

#ifndef VSF_PM_CFG_FUNCTION_RENAME
#   define VSF_PM_CFG_FUNCTION_RENAME               ENABLED
#endif

#define DIV_(_N,_D)     DIV_##_N = (_N),

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
#   define VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER            DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER_MASK
#   define VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER_MASK       DISABLED
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

#ifndef VSF_PM_CFG_REIMPLEMENT_LPOSC_CFG
#   define VSF_PM_CFG_REIMPLEMENT_LPOSC_CFG         DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_LPOSC_SEL
#   define VSF_PM_CFG_REIMPLEMENT_LPOSC_SEL         DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_CLOCK_OUT_CFG
#   define VSF_PM_CFG_REIMPLEMENT_CLOCK_OUT_CFG     DISABLED
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_PM_CFG_REIMPLEMENT_CAPABILITY        DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PM_POWER_APIS(__prefix_name)                                                                                                                                        \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_power_status_t, pm, power_enable,        VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_power_cfg_no_t index)                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_power_status_t, pm, power_disable,       VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_power_cfg_no_t index)                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_power_status_t, pm, power_get_status,    VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_power_cfg_no_t index)                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pm, power_resume,        VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_power_cfg_no_t index, pm_power_status_t status)

#define VSF_PM_SLEEP_APIS(__prefix_name)                                                                                                                                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pm, sleep_enter,         VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_sleep_cfg_t *cfg_ptr)

#define VSF_PM_PCLK_APIS(__prefix_name)                                                                                                                                          \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_pclk_status_t,  pm, pclk_config,        VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pclk_no_t index, pm_pclk_cfg_t *cfg_ptr)         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,     pm, pclk_get_clock,     VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pclk_no_t index)                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_pclk_status_t,  pm, pclk_enable,        VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pclk_no_t index)                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_pclk_status_t,  pm, pclk_disable,       VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pclk_no_t index)                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_pclk_status_t,  pm, pclk_get_status,    VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pclk_no_t index)                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pm, pclk_resume,        VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pclk_no_t index, pm_pclk_status_t status)

#define VSF_PM_SCLK_APIS(__prefix_name)                                                                                                                                          \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_sclk_status_t,  pm, sclk_enable,        VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_sclk_no_t index)                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_sclk_status_t,  pm, sclk_disable,       VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_sclk_no_t index)                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_sclk_status_t,  pm, sclk_get_status,    VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_sclk_no_t index)                                 \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pm, sclk_resume,        VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_sclk_no_t index, pm_sclk_status_t status)

#define VSF_PM_MCLK_APIS(__prefix_name)                                                                                                                                          \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,          pm, mclk_init,          VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_mclk_cfg_t *cfg_ptr)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,     pm, mclk_get_clock,     VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_mclk_no_t sel)

#define VSF_PM_PLL_APIS(__prefix_name)                                                                                                                                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,          pm, pll_init,          VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pll_sel_t pll, pm_pll_cfg_t *cfg_ptr)            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, bool,              pm, pll_is_locked,     VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pll_sel_t pll)                                   \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,     pm, pll_get_clock_out, VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pll_sel_t pll)                                   \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,     pm, pll_get_clock_in,  VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_pll_sel_t pll)

#define VSF_PM_LPOSC_APIS(__prefix_name)                                                                                                                                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pm, lposc_init,          VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_lposc_sel_t lposc, pm_lposc_cfg_t *cfg_ptr)      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              pm, lposc_enable,        VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_lposc_sel_t lposc)                               \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              pm, lposc_disable,       VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_lposc_sel_t lposc)                               \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast32_t,     pm, lposc_get_clock,     VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr, pm_lposc_sel_t lposc)

#define VSF_PM_MISC_APIS(__prefix_name)                                                                                                                                          \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pm_capability_t,   pm, capability,    VSF_MCONNECT(__prefix_name, _pm_t) * pm_ptr)


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
typedef enum pm_power_cfg_no_t{
    POWER_IRC_IDX    = 1,
} pm_power_cfg_no_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_POWER_NUMBER_MASK == DISABLED
typedef enum pm_power_cfg_msk_t {
    POWER_IRC_MSK    = 1UL << 0,
} pm_power_cfg_msk_t;
#endif

typedef uint_fast32_t pm_power_status_t;

/*----------------------------------------------------------------------------*
 * Sleep Management                                                           *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_SLEEP_MODE == DISABLED
typedef enum pm_sleep_mode_t {
    PM_WAIT       = 0,
    PM_SLEEP      = 1,
    PM_DEEP_SLEEP = 2,
    PM_POWER_OFF  = 3,
} pm_sleep_mode_t;
#endif

typedef struct {
    pm_sleep_mode_t sleep_mode;         //!< sleep mode
    uint32_t        sleep_cfg;          //!< Sleep mode cfg
    uint32_t        wake_cfg;           //!< Awake mode cfg
    uint32_t        sleep_walking_cfg;  //!< Sleep Walking mode cfg
} pm_sleep_cfg_t;

/*----------------------------------------------------------------------------*
 * Peripheral Clock Management                                                *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_PCLK_NUMBER == DISABLED
typedef enum pm_pclk_no_t {
    PCLK_PM0_IDX    = 8,        //!< PM 0
    PCLK_SPI0_IDX   = 9,        //!< SPI 0
    PCLK_SPI1_IDX   = 10,       //!< SPI 1
    PCLK_USART0_IDX = 12,       //!< USART0
    PCLK_USART1_IDX = 13,       //!< USART1
} pm_pclk_no_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_PCLK_CFG == DISABLED
typedef struct pm_pclk_cfg_t {
    pm_clk_src_sel_t clk_src;
    uint16_t         div;
} pm_pclk_cfg_t;
#endif

typedef int_fast16_t pm_pclk_status_t;

/*----------------------------------------------------------------------------*
 * AHB Clock Management                                                       *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER == DISABLED
typedef enum pm_sclk_no_t {
    SCLK_CORE_IDX   = 0,
    SCLK_ROM0_IDX   = 1,
    SCLK_PM0_IDX = 2,
    SCLK_SRAM0_IDX  = 3,
} pm_sclk_no_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_SCLK_NUMBER_MASK == DISABLED
typedef enum pm_sclk_msk_t {
    SCLK_CORE_MSK    = BIT(SCLK_CORE_IDX),
    SCLK_ROM0_MSK    = BIT(SCLK_ROM0_IDX),
    SCLK_PM0_MSK  = BIT(SCLK_PM0_MSK),
    SCLK_SRAM0_MSK   = BIT(SCLK_SRAM0_IDX),
};
#endif

typedef uint_fast32_t pm_sclk_status_t;

/*----------------------------------------------------------------------------*
 * Main Clock Management                                                      *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_SCLK_SEL == DISABLED
typedef enum pm_clk_src_sel_t {
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
} pm_clk_src_sel_t;
#endif

//! \name main clock prescaler
//! @{
typedef enum {
    VSF_MREPEAT(255, DIV_, 0)
} pm_divider_t;
//! @}

#ifndef __cplusplus
typedef enum pm_mclk_core_div_t     pm_mclk_core_div_t;
typedef enum pm_mclk_axi_div_t      pm_mclk_axi_div_t;
typedef enum pm_mclk_ahb_div_t      pm_mclk_ahb_div_t;
typedef enum pm_mclk_apb_div_t      pm_mclk_apb_div_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_MCLK_CFG == DISABLED
typedef struct pm_mclk_cfg_t {
    pm_clk_src_sel_t                clk_src;                //!< main clock source
    uint32_t                        freq;                   //!< system oscilator frequency
    pm_mclk_core_div_t              core_div[CORE_NUM];     //!< system core clock divider
    pm_mclk_axi_div_t               axi_div[AXI_CLK_NUM];   //!< system AXI clock divider
    pm_mclk_ahb_div_t               ahb_div[AHB_CLK_NUM];   //!< system AHB clock divider
    pm_mclk_apb_div_t               apb_div[APB_CLK_NUM];   //!< system APB clock divider
} pm_mclk_cfg_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_MCLK_NO == DISABLED
typedef enum pm_mclk_no_t {
    MCLK_CORE_IDX = 0,
    MCLK_CORE0_IDX = 0,

    MCLK_AXI0_IDX,
    MCLK_AXI1_IDX,

    MCLK_AHB0_IDX,
    MCLK_AHB1_IDX,

    MCLK_APB0_IDX,
    MCLK_APB1_IDX,
} pm_mclk_no_t;
#endif

/*----------------------------------------------------------------------------*
 * PLL Control                                                                *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_PLL_SEL == DISABLED
typedef enum pm_pll_sel_t {
    PLL0_IDX,
    PLL1_IDX,
} pm_pll_sel_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_PLL_CFG == DISABLED
typedef struct pm_pll_cfg_t {
    pm_clk_src_sel_t                pll_clk_src;//!< pll clock source
    uint32_t                        freq;       //!< system oscilator frequency
    uint8_t                         Msel;       //!< PLL Feedback divider value
    uint8_t                         Psel;       //!< pll Feedback divider value
} pm_pll_cfg_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_POST_DIV == DISABLED
typedef enum pm_pll_post_div_t {
    PLL_POST_DIV_1                  = 0x00,     //!< pll post divider rate is 1
    PLL_POST_DIV_2                  = 0x01,     //!< pll post divider rate is 2
    PLL_POST_DIV_4                  = 0x02,     //!< pll post divider rate is 4
    PLL_POST_DIV_8                  = 0x03,     //!< pll post divider rate is 8
} pm_pll_post_div_t;
#endif

/*----------------------------------------------------------------------------*
 * Low Power Oscillator Management                                            *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_LPOSC_CFG == DISABLED
typedef struct lposc_cfg_t{
    union {
        struct {
            uint32_t DIV            : 5;
            uint32_t FREQ           : 4;
            reg32_t                 :23;
        };
        uint32_t wWord;
    };
} lposc_cfg_t;
#endif

#if VSF_PM_CFG_REIMPLEMENT_LPOSC_SEL == DISABLED
typedef enum pm_lposc_sel_t {
    LPOSC_ALWAYS_ON,
    LPOSC_32K_OSC
} pm_lposc_sel_t;
#endif

typedef struct pm_lposc_cfg_t pm_lposc_cfg_t;

/*----------------------------------------------------------------------------*
 * Clock Management                                                           *
 *----------------------------------------------------------------------------*/

#if VSF_PM_CFG_REIMPLEMENT_CLOCK_OUT_CFG == DISABLED
typedef struct pm_clock_out_cfg_t {
    //! clk out source select
    pm_clk_src_sel_t                clk_src;    //!< clock source for output
    //! clk divider
    uint_fast8_t                    div;        //!< divider for output clock
} pm_clock_out_cfg_t;
#endif

#if VSF_PWM_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct pm_capability_t {
    inherit(peripheral_capability_t)
} pm_capability_t;
#endif

typedef struct vsf_pm_t vsf_pm_t;

typedef struct vsf_pm_op_t {
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_PM_APIS(vsf)
} vsf_pm_op_t;

#if VSF_PM_CFG_MULTI_CLASS == ENABLED
struct vsf_pm_t  {
    const vsf_pm_op_t * op;
};
#endif

/*----------------------------------------------------------------------------*
 * Interface                                                                  *
 *----------------------------------------------------------------------------*/

//! \name power
//! @{
dcl_interface(i_pm_power_t)
def_interface(i_pm_power_t)
    //! \brief Enable specific power domains with masks defined in
    //!        em_power_cfg_msk_t
    pm_power_status_t   (*Enable)   (pm_power_cfg_no_t index);
    //! \brief Disable specific power domains with masks defined in
    //!        em_power_cfg_msk_t
    pm_power_status_t   (*Disable)  (pm_power_cfg_no_t index);

    pm_power_status_t   (*GetStatus)(pm_power_cfg_no_t index);
    vsf_err_t           (*Resume)   (pm_power_cfg_no_t index, pm_power_status_t status);
end_def_interface(i_pm_power_t)
//! @}

dcl_interface(i_pm_wakeup_t)

/*! \note i_pm_wakeup_t should be defined in device specific header file
          pm.h.

 *! \note The same type of UseXXX should have a consistent interface.

    typedef struct rtc_cfg_t rtc_cfg_t;
    typedef struct io_wakeup_cfg_t io_wakeup_cfg_t;
    typedef struct usart_wakeup_cfg_t usart_wakeup_cfg_t;
    typedef struct wdt_wakeup_cfg_t wdt_wakeup_cfg_t;
    typedef struct bod_wakeup_cfg_t bod_wakeup_cfg_t;

    def_interface( i_pm_wakeup_t )
        struct {
            vsf_err_t (*Enable)(io_wakeup_cfg_t *pcfg, uint_fast8_t size);
            vsf_err_t (*Disable)(io_port_no_t port, uint_fast32_t msk);
        }UseIO;
        struct {
            vsf_err_t (*Enable)(uint_fast8_t index, usart_wakeup_cfg_t *pcfg);
            vsf_err_t (*Disable)(uint_fast8_t index);
        }UseUSART;
        struct {
            vsf_err_t (*Enable)(uint_fast8_t index);
            vsf_err_t (*Disable)(uint_fast8_t index);
        }UseRTC;
        struct {
            vsf_err_t (*Enable)(wdt_wakeup_cfg_t *pcfg);
            vsf_err_t (*Disable)(void);
        }UseWDT;
        struct {
            vsf_err_t (*Enable)(bod_wakeup_cfg_t *pcfg);
            vsf_err_t (*Disable)(void);
        }UseBOD;
        ...
    end_def_interface( i_pm_wakeup_t )
*/
dcl_interface(i_pm_sleep_t)
def_interface(i_pm_sleep_t)
    vsf_err_t   (*TryToSleep)       (pm_sleep_cfg_t *cfg_ptr);
    i_pm_wakeup_t WakeUp;
end_def_interface(i_pm_sleep_t)


//! \name pclk
//! @{
dcl_interface(i_pm_pclk_t)
def_interface(i_pm_pclk_t)
    pm_pclk_status_t(*Config)       (pm_pclk_no_t index, pm_pclk_cfg_t *cfg_ptr);
    uint_fast32_t   (*GetClock)     (pm_pclk_no_t index);
    pm_pclk_status_t(*Enable)       (pm_pclk_no_t index);
    pm_pclk_status_t(*Disable)      (pm_pclk_no_t index);
    pm_pclk_status_t(*GetStatus)    (pm_pclk_no_t index);
    vsf_err_t       (*Resume)       (pm_pclk_no_t index, pm_pclk_status_t status);
end_def_interface(i_pm_pclk_t)
//! @}


//! \name AHB Clock Management
//! @{
dcl_interface(i_pm_sclk_t)
def_interface(i_pm_sclk_t)
    pm_sclk_status_t(*Enable)       (pm_sclk_no_t index);
    pm_sclk_status_t(*Disable)      (pm_sclk_no_t index);
    pm_sclk_status_t(*GetStatus)    (pm_sclk_no_t index);
    vsf_err_t       (*Resume)       (pm_sclk_no_t index, pm_sclk_status_t status);
end_def_interface(i_pm_sclk_t)
//! @}

//! \name main clock struct type
//! @{
dcl_interface(i_pm_mclk_t)
def_interface(i_pm_mclk_t)
    fsm_rt_t        (*Init)         (pm_mclk_cfg_t *cfg_ptr);
    uint_fast32_t   (*GetClock)     (pm_mclk_no_t sel);
end_def_interface(i_pm_mclk_t)
//! @}


//! \name pll struct type
//! @{
dcl_interface(i_pm_pll_t)
def_interface(i_pm_pll_t)
    //! Pll config
    fsm_rt_t        (*Init)         (pm_pll_sel_t pll, pm_pll_cfg_t *cfg_ptr);
    //! PLL Check Is Pll Locked
    bool            (*IsLocked)     (pm_pll_sel_t pll);
    //! get pll output clock
    uint_fast32_t   (*GetClockOut)  (pm_pll_sel_t pll);
    //! get pll input clock
    uint_fast32_t   (*GetClockIn)   (pm_pll_sel_t pll);
end_def_interface(i_pm_pll_t)
//! @}


//! \name low power oscillator
//! @{
dcl_interface(i_pm_lposc_t)
def_interface(i_pm_lposc_t)
    vsf_err_t       (*Init)         (pm_lposc_sel_t lposc, pm_lposc_cfg_t *cfg_ptr);
    void            (*Enable)       (pm_lposc_sel_t lposc);
    void            (*Disable)      (pm_lposc_sel_t lposc);
    uint_fast32_t   (*GetClock)     (pm_lposc_sel_t lposc);
end_def_interface(i_pm_lposc_t)
//! @}


dcl_interface(i_pm_clk_t)
def_interface(i_pm_clk_t)

    /*! \note Main here is used to set dividers for CPU, sync-clocks (ahb, axi,
     *!       apb and etc) hence generate clock for CPU, sync-clocks from system
     *!       clock.
     */
    i_pm_mclk_t     Main;

    /*! \note the system clock is the root of the clock tree output
     *!       cpu clock and Sync-Clock of bus and peripherals are derived
     *!       from system clock with **their own dividers**.
     *!       One should not suppose that the cpu clock is the same as the
     *!       system clock. The Sync-Clock can also be different from cpu
     *!       clock.
     */
    uint_fast32_t   (*GetSysClk)(void);

#if VSF_HAL_DRV_PM_CFG_SUPPORT_CLK_OUT == ENABLED
    vsf_err_t       (*ClkOutCfg)(pm_clock_out_cfg_t *cfg_ptr);
#endif

#if VSF_HAL_DRV_PM_CFG_SUPPORT_PCLK == ENABLED
    i_pm_pclk_t     Peripheral;
#endif

#if VSF_HAL_DRV_PM_CFG_SUPPORT_SCLK == ENABLED
    i_pm_sclk_t     SyncClock;
#endif

#if VSF_HAL_DRV_PM_CFG_SUPPORT_LPOSC == ENABLED
    i_pm_lposc_t    LPOSC;                      //!< low power oscillators
#endif

#if VSF_HAL_DRV_PM_CFG_SUPPORT_PLL == ENABLED
    i_pm_pll_t      PLL;                        //!< pll control
#endif

end_def_interface(i_pm_clk_t)


/*----------------------------------------------------------------------------*
 * PM Interface                                                               *
 *----------------------------------------------------------------------------*/
//! \name pmu struct
//! @{
dcl_interface(i_pm_t)
def_interface(i_pm_t)
    //! \brief set the clock auto . the main clock frequency and the div is needed
    fsm_rt_t        (*AutoClock)(   pm_clk_src_sel_t clk_src,
                                    uint_fast32_t src_frq,
                                    uint_fast32_t target_frq);
    i_pm_clk_t      Clock;

#if VSF_HAL_DRV_PM_CFG_SUPPORT_PWR_CTRL == ENABLED
    i_pm_power_t    Power;
#endif

#if VSF_HAL_DRV_PM_CFG_SUPPORT_SLEEP_CTRL == ENABLED
    i_pm_sleep_t    Sleep;
#endif

end_def_interface(i_pm_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_pm_t VSF_PM;

/*============================ PROTOTYPES ====================================*/

extern pm_power_status_t  vsf_pm_power_enable         (vsf_pm_t* pm_ptr, pm_power_cfg_no_t index);
extern pm_power_status_t  vsf_pm_power_disable        (vsf_pm_t* pm_ptr, pm_power_cfg_no_t index);
extern pm_power_status_t  vsf_pm_power_get_status     (vsf_pm_t* pm_ptr, pm_power_cfg_no_t index);
extern vsf_err_t          vsf_pm_power_resume         (vsf_pm_t* pm_ptr, pm_power_cfg_no_t index, pm_power_status_t status);

extern vsf_err_t          vsf_pm_sleep_enter          (vsf_pm_t* pm_ptr, pm_sleep_cfg_t *cfg_ptr);

extern pm_pclk_status_t   vsf_pm_pclk_config          (vsf_pm_t* pm_ptr, pm_pclk_no_t index, pm_pclk_cfg_t *cfg_ptr);
extern uint_fast32_t      vsf_pm_pclk_get_clock       (vsf_pm_t* pm_ptr, pm_pclk_no_t index);
extern pm_pclk_status_t   vsf_pm_pclk_enable          (vsf_pm_t* pm_ptr, pm_pclk_no_t index);
extern pm_pclk_status_t   vsf_pm_pclk_disable         (vsf_pm_t* pm_ptr, pm_pclk_no_t index);
extern pm_pclk_status_t   vsf_pm_pclk_get_status      (vsf_pm_t* pm_ptr, pm_pclk_no_t index);
extern vsf_err_t          vsf_pm_pclk_resume          (vsf_pm_t* pm_ptr, pm_pclk_no_t index, pm_pclk_status_t status);

extern pm_sclk_status_t   vsf_pm_sclk_enable          (vsf_pm_t* pm_ptr, pm_sclk_no_t index);
extern pm_sclk_status_t   vsf_pm_sclk_disable         (vsf_pm_t* pm_ptr, pm_sclk_no_t index);
extern pm_sclk_status_t   vsf_pm_sclk_get_status      (vsf_pm_t* pm_ptr, pm_sclk_no_t index);
extern vsf_err_t          vsf_pm_sclk_resume          (vsf_pm_t* pm_ptr, pm_sclk_no_t index, pm_sclk_status_t status);

extern fsm_rt_t           vsf_pm_mclk_init            (vsf_pm_t* pm_ptr, pm_mclk_cfg_t *cfg_ptr);
extern uint_fast32_t      vsf_pm_mclk_get_clock       (vsf_pm_t* pm_ptr, pm_mclk_no_t sel);

extern fsm_rt_t           vsf_pm_pll_init             (vsf_pm_t* pm_ptr, pm_pll_sel_t pll, pm_pll_cfg_t *cfg_ptr);
extern bool               vsf_pm_pll_is_locked        (vsf_pm_t* pm_ptr, pm_pll_sel_t pll);
extern uint_fast32_t      vsf_pm_pll_get_clock_out    (vsf_pm_t* pm_ptr, pm_pll_sel_t pll);
extern uint_fast32_t      vsf_pm_pll_get_clock_in     (vsf_pm_t* pm_ptr, pm_pll_sel_t pll);

extern vsf_err_t          vsf_pm_lposc_init           (vsf_pm_t* pm_ptr, pm_lposc_sel_t lposc, pm_lposc_cfg_t *cfg_ptr);
extern void               vsf_pm_lposc_enable         (vsf_pm_t* pm_ptr, pm_lposc_sel_t lposc);
extern void               vsf_pm_lposc_disable        (vsf_pm_t* pm_ptr, pm_lposc_sel_t lposc);
extern uint_fast32_t      vsf_pm_lposc_get_clock      (vsf_pm_t* pm_ptr, pm_lposc_sel_t lposc);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_PM_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_pm_power_enable(__PM, ...)                                       \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_power_enable)       ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_power_disable(__PM, ...)                                      \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_power_disable)      ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_power_get_status(__PM, ...)                                   \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_power_get_status)   ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_power_resume(__PM, ...)                                       \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_power_resume)       ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_sleep_enter(__PM, ...)                                 \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_sleep_try_to_sleep) ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pclk_config(__PM, ...)                                        \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pclk_config)        ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pclk_get_clock(__PM, ...)                                     \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pclk_get_clock)     ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pclk_enable(__PM, ...)                                        \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pclk_enable)        ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pclk_disable(__PM, ...)                                       \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pclk_disable)       ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pclk_get_status(__PM, ...)                                    \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pclk_get_status)    ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pclk_resume(__PM, ...)                                        \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pclk_resume)        ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_sclk_enable(__PM, ...)                                        \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_sclk_enable)        ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_sclk_disable(__PM, ...)                                       \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_sclk_disable)       ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_sclk_get_status(__PM, ...)                                    \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_sclk_get_status)    ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_sclk_resume(__PM, ...)                                        \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_sclk_resume)        ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_mclk_init(__PM, ...)                                          \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_mclk_init)          ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_mclk_get_clock(__PM, ...)                                     \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_mclk_get_clock)     ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pll_init(__PM, ...)                                           \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pll_init)           ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pll_is_locked(__PM, ...)                                      \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pll_is_locked)      ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pll_get_clock_out(__PM, ...)                                  \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pll_get_clock_out)  ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_pll_get_clock_in(__PM, ...)                                   \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_pll_get_clock_in)   ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_lposc_init(__PM, ...)                                         \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_lposc_init)         ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_lposc_enable(__PM, ...)                                       \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_lposc_enable)       ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_lposc_disable(__PM, ...)                                      \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_lposc_disable)      ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pm_lposc_get_clock(__PM, ...)                                    \
        VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_lposc_get_clock)    ((VSF_MCONNECT(VSF_PM_CFG_PREFIX, _pm_t) *)__PM, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HAL_DRIVER_PM_INTERFACE_H__ */
/* EOF */
