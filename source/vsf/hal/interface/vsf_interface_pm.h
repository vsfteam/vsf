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

#ifndef __HAL_DRIVER_PM_INTERFACE_H__
#define __HAL_DRIVER_PM_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*----------------------------------------------------------------------------*
 * Power Domain Management                                                    *
 *----------------------------------------------------------------------------*/

/*! \note pm_power_cfg_no_t and pm_power_cfg_msk_t should be defined in device
           specific header file: device.h

//! \name power set index
//! @{
enum pm_power_cfg_no_t{
    POWER_IRCOUT_IDX           =(0),
    POWER_IRC_IDX              =(1),
    ...
};

//! \name power set mask
//! @{
enum pm_power_cfg_msk_t {
    POWER_IRCOUT_MSK           =(1UL<<0),
    POWER_IRC_MSK              =(1UL<<1),
    ...
} ;
//! @}

//! @}
*/
#ifndef __cplusplus
typedef enum pm_power_cfg_no_t  pm_power_cfg_no_t;
typedef enum pm_power_cfg_msk_t pm_power_cfg_msk_t;
#endif

typedef uint_fast32_t pm_power_status_t;

//! \name power
//! @{
declare_interface(i_pm_power_t)
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


/*----------------------------------------------------------------------------*
 * Sleep Management                                                           *
 *----------------------------------------------------------------------------*/

/*! \note pm_sleep_mode_t should be defined in device specific header file
          device.h

//! \name the lowpower mode
//! @{
enum pm_sleep_mode_t{
    PM_WAIT                         = 0,
    PM_SLEEP                        = 1,
    PM_DEEP_SLEEP                   = 2,
    PM_POWER_OFF                    = 3,
} ;
//! @}
*/
#ifndef __cplusplus
typedef enum pm_sleep_mode_t pm_sleep_mode_t;
#endif

//! \name sleep config struct
//! @{
typedef struct {
    pm_sleep_mode_t                 sleep_mode;         //!< sleep mode
    uint32_t                        sleep_cfg;          //!< Sleep mode cfg
    uint32_t                        wake_cfg;           //!< Awake mode cfg
    uint32_t                        sleep_walking_cfg;  //!< Sleep Walking mode cfg
} pm_sleep_cfg_t;
//! @}

declare_interface(i_pm_wakeup_t)

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
declare_interface(i_pm_sleep_t)
def_interface(i_pm_sleep_t)
    vsf_err_t   (*TryToSleep)       (pm_sleep_cfg_t *cfg_ptr);
    i_pm_wakeup_t WakeUp;
end_def_interface(i_pm_sleep_t)

/*----------------------------------------------------------------------------*
 * Peripheral Clock Management                                                *
 *----------------------------------------------------------------------------*/

/*! \note pm_pclk_cfg_t and pm_pclk_no_t should be defined in device
          specific header file device.h
          E.g.

//! \name peripheral clock index
//! @{
enum pm_pclk_no_t {
    PCLK_ADC0_IDX                   = 8,        //!< ADC 0
    PCLK_SPI0_IDX                   = 9,        //!< SPI 0
    PCLK_SPI1_IDX                   = 10,       //!< SPI 1
    PCLK_USART0_IDX                 = 12,       //!< USART0
    PCLK_USART1_IDX                 = 13,       //!< USART1
};

struct pm_pclk_cfg_t {
    pm_clk_src_sel_t                clk_src;
    uint16_t                        div;
};
//! @}

 */

#ifndef __cplusplus
typedef enum pm_pclk_no_t pm_pclk_no_t;
#endif

typedef int_fast16_t pm_pclk_status_t;


typedef struct pm_pclk_cfg_t pm_pclk_cfg_t;

//! \name pclk
//! @{
declare_interface(i_pm_pclk_t)
def_interface(i_pm_pclk_t)
    pm_pclk_status_t(*Config)       (pm_pclk_no_t index, pm_pclk_cfg_t *cfg_ptr);
    uint_fast32_t   (*GetClock)     (pm_pclk_no_t index);
    pm_pclk_status_t(*Enable)       (pm_pclk_no_t index);
    pm_pclk_status_t(*Disable)      (pm_pclk_no_t index);
    pm_pclk_status_t(*GetStatus)    (pm_pclk_no_t index);
    vsf_err_t       (*Resume)       (pm_pclk_no_t index, pm_pclk_status_t status);
end_def_interface(i_pm_pclk_t)
//! @}

/*----------------------------------------------------------------------------*
 * AHB Clock Management                                                       *
 *----------------------------------------------------------------------------*/


/*! \note pm_sclk_no_t and pm_sclk_msk_t should be defined in device
          specific header file device.h
          E.g.

//! \name Peripheral AHB Clock Macros
//! @{
enum pm_sclk_no_t {
    SCLK_CORE_IDX                   = 0,
    SCLK_ROM0_IDX                   = 1,
    SCLK_FLASH0_IDX                 = 2,
    SCLK_SRAM0_IDX                  = 3,

    ...

    SCLK_ARA0_IDX                   = 32,
    SCLK_APPREG0_IDX                = 33,
};

enum pm_sclk_msk_t {
    SCLK_CORE_MSK                   = BIT(SCLK_CORE_idx),
    SCLK_ROM0_MSK                   = BIT(SCLK_ROM0_idx),
    SCLK_FLASH0_MSK                 = BIT(SCLK_FLASH0_msk),
    SCLK_SRAM0_MSK                  = BIT(SCLK_SRAM0_idx),

    ...

    SCLK_ARA0_MSK                   = BIT(SCLK_ARA0_idx - 32),
    SCLK_APPREG0_MSK                = BIT(SCLK_APPREG0_msk - 32),
};
//! @}
*/

#ifndef __cplusplus
typedef enum pm_sclk_no_t pm_sclk_no_t;
typedef enum pm_sclk_msk_t pm_sclk_msk_t;
#endif

typedef uint_fast32_t pm_sclk_status_t;

//! \name AHB Clock Management
//! @{
declare_interface(i_pm_sclk_t)
def_interface(i_pm_sclk_t)
    pm_sclk_status_t(*Enable)       (pm_sclk_no_t index);
    pm_sclk_status_t(*Disable)      (pm_sclk_no_t index);
    pm_sclk_status_t(*GetStatus)    (pm_sclk_no_t index);
    vsf_err_t       (*Resume)       (pm_sclk_no_t index, pm_sclk_status_t status);
end_def_interface(i_pm_sclk_t)
//! @}

/*----------------------------------------------------------------------------*
 * Main Clock Management                                                      *
 *----------------------------------------------------------------------------*/

/*! \note pm_clk_src_sel_t should be defined in device specific header file device.h
          E.g.

//! @{
enum pm_clk_src_sel_t {
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
}em_clk_src_sel_t;
//! @}
*/

#ifndef __cplusplus
typedef enum pm_clk_src_sel_t pm_clk_src_sel_t;
#endif

//! \name main clock prescaler
//! @{
typedef enum {
    REPEAT_MACRO(255,DIV_,0)
} pm_divider_t;
//! @}

#ifndef __cplusplus
typedef enum pm_mclk_core_div_t     pm_mclk_core_div_t;
typedef enum pm_mclk_axi_div_t      pm_mclk_axi_div_t;
typedef enum pm_mclk_ahb_div_t      pm_mclk_ahb_div_t;
typedef enum pm_mclk_apb_div_t      pm_mclk_apb_div_t;
#endif

/*! \note pm_mclk_cfg_t, CORE_NUM and XXX_CLK_NUM should be defined in
          device specific header filer device.h
          Example:

//! \name main clock config sturct
//! @{
struct pm_mclk_cfg_t {
    pm_clk_src_sel_t                clk_src;                //!< main clock source
    uint32_t                        freq;                   //!< system oscilator frequency
    pm_mclk_core_div_t              core_div[CORE_NUM];     //!< system core clock divider
    pm_mclk_axi_div_t               axi_div[AXI_CLK_NUM];   //!< system AXI clock divider
    pm_mclk_ahb_div_t               ahb_div[AHB_CLK_NUM];   //!< system AHB clock divider
    pm_mclk_apb_div_t               apb_div[APB_CLK_NUM];   //!< system APB clock divider
};
//! @}
 */
typedef struct pm_mclk_cfg_t pm_mclk_cfg_t;

#ifndef __cplusplus
typedef enum pm_mclk_no_t pm_mclk_no_t;
#endif
/*
enum pm_mclk_no_t {
    MCLK_CORE_IDX = 0,
    MCLK_CORE0_IDX = 0,

    MCLK_AXI0_IDX,
    MCLK_AXI1_IDX,

    MCLK_AHB0_IDX,
    MCLK_AHB1_IDX,

    MCLK_APB0_IDX,
    MCLK_APB1_IDX,
};
 */

//! \name main clock struct type
//! @{
declare_interface(i_pm_mclk_t)
def_interface(i_pm_mclk_t)
    fsm_rt_t        (*Init)         (pm_mclk_cfg_t *cfg_ptr);
    uint_fast32_t   (*GetClock)     (pm_mclk_no_t sel);
end_def_interface(i_pm_mclk_t)
//! @}



/*----------------------------------------------------------------------------*
 * PLL Control                                                                *
 *----------------------------------------------------------------------------*/

/*! \note pm_pll_sel_t should be defined in device specific header file device.h
          E.g.

enum pm_pll_sel_t {
    PLL0_IDX,
    PLL1_IDX,
};

 */
#ifndef __cplusplus
typedef enum pm_pll_sel_t pm_pll_sel_t;
#endif

/*! \note  pm_pll_cfg_t should be defined in device specific PM header file.
//! \name pll config struct
//! @{
struct pm_pll_cfg_t {
    pm_clk_src_sel_t                pll_clk_src;//!< pll clock source
    uint32_t                        freq;       //!< system oscilator frequency
    uint8_t                         Msel;       //!< PLL Feedback divider value
    uint8_t                         Psel;       //!< pll Feedback divider value
};
//! @}
*/

typedef struct pm_pll_cfg_t pm_pll_cfg_t;

#ifndef __cplusplus
typedef enum pm_pll_post_div_t pm_pll_post_div_t;
#endif

/*
//! \name pll post divider
//! @{
enum pm_pll_post_div_t {
    PLL_POST_DIV_1                  = 0x00,     //!< pll post divider rate is 1
    PLL_POST_DIV_2                  = 0x01,     //!< pll post divider rate is 2
    PLL_POST_DIV_4                  = 0x02,     //!< pll post divider rate is 4
    PLL_POST_DIV_8                  = 0x03,     //!< pll post divider rate is 8
} ;
//! @}
*/


//! \name pll struct type
//! @{
declare_interface(i_pm_pll_t)
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

/*----------------------------------------------------------------------------*
 * Low Power Oscillator Management                                            *
 *----------------------------------------------------------------------------*/

/*! \note pm_pll_sel_t should be defined in device specific header file device.h
          E.g.
//! \name always-on oscillator configuration structure
//! @{
struct lposc_cfg_t{
    union {
        struct {
            uint32_t DIV            : 5;
            uint32_t FREQ           : 4;
            reg32_t                 :23;
        };
        uint32_t wWord;
    };
};
//! @}

enum pm_lposc_sel_t {
    LPOSC_ALWAYS_ON,
    LPOSC_32K_OSC
};
*/

#ifndef __cplusplus
typedef enum pm_lposc_sel_t pm_lposc_sel_t;
#else
typedef uint16_t pm_lposc_sel_t;
#endif

typedef struct pm_lposc_cfg_t pm_lposc_cfg_t;

//! \name low power oscillator
//! @{
declare_interface(i_pm_lposc_t)
def_interface(i_pm_lposc_t)
    vsf_err_t       (*Init)         (pm_lposc_sel_t lposc, pm_lposc_cfg_t *cfg_ptr);
    void            (*Enable)       (pm_lposc_sel_t lposc);
    void            (*Disable)      (pm_lposc_sel_t lposc);
    uint_fast32_t   (*GetClock)     (pm_lposc_sel_t lposc);
end_def_interface(i_pm_lposc_t)
//! @}

/*----------------------------------------------------------------------------*
 * Clock Management                                                           *
 *----------------------------------------------------------------------------*/

//! \name pll struct type
//! @{
typedef struct {
    //! clk out source select
    pm_clk_src_sel_t                clk_src;    //!< clock source for output
    //! clk divider
    uint_fast8_t                    div;        //!< divider for output clock
} pm_clock_out_cfg_t;
//! @}

declare_interface(i_pm_clk_t)
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
declare_interface(i_pm_t)
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

#ifdef __cplusplus
}
#endif
#endif
/* EOF */
