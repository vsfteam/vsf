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

/*============================ MACROS ========================================*/
#define DIV_(_N,_D)     DIV_##_N = (_N),

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
    POWER_IRCOUT_idx           =(0), 
    POWER_IRC_idx              =(1),
    ...
};

//! \name power set mask
//! @{
enum pm_power_cfg_msk_t {
    POWER_IRCOUT_msk           =(1UL<<0), 
    POWER_IRC_msk              =(1UL<<1), 
    ...
} ;
//! @}

//! @}
*/
typedef enum pm_power_cfg_no_t  pm_power_cfg_no_t;
typedef enum pm_power_cfg_msk_t pm_power_cfg_msk_t;

typedef uint_fast32_t pm_power_status_t;

//! \name power
//! @{
def_interface(i_pm_power_t)
    //! \brief Enable specific power domains with masks defined in 
    //!        em_power_cfg_msk_t
    pm_power_status_t           (*Enable)(pm_power_cfg_no_t index);      
    //! \brief Disable specific power domains with masks defined in 
    //!        em_power_cfg_msk_t
    pm_power_status_t           (*Disable)(pm_power_cfg_no_t index);
    
    pm_power_status_t           (*GetStatus)(pm_power_cfg_no_t index);
    vsf_err_t                   (*Resume)(  pm_power_cfg_no_t index, 
                                            pm_power_status_t status);
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
    PM_WAIT         = 0,
    PM_SLEEP        = 1,
    PM_DEEP_SLEEP   = 2,
    PM_POWER_OFF    = 3,
} ;
//! @}
*/
typedef enum pm_sleep_mode_t pm_sleep_mode_t;

 
//! \name sleep config struct
//! @{
typedef struct {
    pm_sleep_mode_t             sleep_mode;         //!< sleep mode
    uint32_t                    sleep_cfg;          //!< Sleep mode cfg
    uint32_t                    wake_cfg;           //!< Awake mode cfg
    uint32_t                    sleep_walking_cfg;  //!< Sleep Walking mode cfg
}pm_sleep_cfg_t;
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

def_interface(i_pm_sleep_t)
    vsf_err_t (*TryToSleep)(pm_sleep_cfg_t *ptCfg);
    i_pm_wakeup_t WakeUp;
end_def_interface(i_pm_sleep_t)

/*----------------------------------------------------------------------------*
 * Peripheral Clock Management                                                *
 *----------------------------------------------------------------------------*/
 
/*! \note pm_periph_asyn_clk_cfg_t and pm_pclk_no_t should be defined in device 
          specific header file device.h 
          E.g.

//! \name peripheral clock index
//! @{
enum pm_periph_async_clk_no_t{
    PCLK_ADC0_idx           = 8,    //!< ADC 0
    PCLK_SPI0_idx           = 9,    //!< SPI 0 
    PCLK_SPI1_idx           = 10,   //!< SPI 1
    PCLK_USART0_idx         = 12,   //!< USART0
    PCLK_USART1_idx         = 13,   //!< USART1
};

struct pm_periph_asyn_clk_cfg_t {
    pm_clk_src_sel_t    clk_src;
    uint16_t            div;
};
//! @}

 */ 

 
typedef enum pm_periph_async_clk_no_t pm_periph_async_clk_no_t;

typedef int_fast16_t pm_periph_async_clk_status_t;


typedef struct pm_periph_asyn_clk_cfg_t pm_periph_asyn_clk_cfg_t;

//! \name pclk
//! @{
def_interface(i_pm_periph_asyn_clk_t)
    pm_periph_async_clk_status_t    
                        (*Config)(   pm_periph_async_clk_no_t index , 
                                     const pm_periph_asyn_clk_cfg_t *pcfg);
    uint_fast32_t       (*GetClock)( pm_periph_async_clk_no_t index);
    pm_periph_async_clk_status_t    
                        (*GetStatus)(pm_periph_async_clk_no_t index);
    vsf_err_t           (*Resume)   (pm_periph_async_clk_no_t index , 
                                     pm_periph_async_clk_status_t status);
end_def_interface(i_pm_periph_asyn_clk_t)
//! @}

/*----------------------------------------------------------------------------*
 * AHB Clock Management                                                       *
 *----------------------------------------------------------------------------*/

 
/*! \note pm_ahb_clk_no_t and pm_ahb_clk_msk_t should be defined in device 
          specific header file device.h 
          E.g.

//! \name Peripheral AHB Clock Macros
//! @{
enum pm_ahb_clk_no_t { 
    AHBCLK_CORE_idx         = 0,
    AHBCLK_ROM0_idx         = 1,
    AHBCLK_FLASH0_idx       = 2,
    AHBCLK_SRAM0_idx        = 3,

    ...

    AHBCLK_ARA0_idx         = 32,
    AHBCLK_APPREG0_idx      = 33,
};

enum pm_ahb_clk_msk_t { 
    AHBCLK_CORE_msk         = _BV(AHBCLK_CORE_idx),
    AHBCLK_ROM0_msk         = _BV(AHBCLK_ROM0_idx),
    AHBCLK_FLASH0_msk       = _BV(AHBCLK_FLASH0_msk),
    AHBCLK_SRAM0_msk        = _BV(AHBCLK_SRAM0_idx),

    ...

    AHBCLK_ARA0_msk         = _BV(AHBCLK_ARA0_idx - 32),
    AHBCLK_APPREG0_msk      = _BV(AHBCLK_APPREG0_msk - 32),
};
//! @}
*/

typedef enum pm_ahb_clk_no_t pm_ahb_clk_no_t;
typedef enum pm_ahb_clk_msk_t pm_ahb_clk_msk_t;

typedef uint_fast32_t pm_ahbclk_status_t;

//! \name AHB Clock Management
//! @{
def_interface(i_pm_ahb_clk_t)
    pm_ahbclk_status_t  (*Enable)(pm_ahb_clk_no_t index);
    pm_ahbclk_status_t  (*Disable)(pm_ahb_clk_no_t index);
    pm_ahbclk_status_t  (*GetStatus)(pm_ahb_clk_no_t index);
    vsf_err_t           (*Resume)(pm_ahb_clk_no_t index, pm_ahbclk_status_t status);
end_def_interface(i_pm_ahb_clk_t)
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
 
typedef enum pm_clk_src_sel_t pm_clk_src_sel_t;

//! \name main clock prescaler
//! @{
typedef enum {
    MREPEAT(255,DIV_,0)
} pm_divider_t;
//! @}


/*! \note pm_main_clk_cfg_t, CORE_NUM and XXX_CLK_NUM should be defined in 
          device specific header filer device.h
          Example:
 
//! \name main clock config sturct
//! @{
struct pm_main_clk_cfg_t {
    pm_clk_src_sel_t    clk_src;                //!< main clock source
    uint32_t            freq;                   //!< system oscilator frequency
    uint16_t            core_div[CORE_NUM];     //!< system core clock divider
    uint16_t            axi_div[AXI_CLK_NUM];   //!< system AXI clock divider
    uint16_t            ahb_div[AHB_CLK_NUM];   //!< system AHB clock divider
    uint16_t            apb_div[APB_CLK_NUM];   //!< system APB clock divider
};
//! @}
 */
typedef struct pm_main_clk_cfg_t pm_main_clk_cfg_t;

//! \name main clock struct type
//! @{
def_interface(i_pm_main_clk_t)
    fsm_rt_t            (*Init)         (pm_main_clk_cfg_t *tCfg);
    uint_fast32_t       (*GetClock)     (void);
end_def_interface(i_pm_main_clk_t)
//! @}



/*----------------------------------------------------------------------------*
 * PLL Control                                                                *
 *----------------------------------------------------------------------------*/

/*! \note pm_pll_sel_t should be defined in device specific header file device.h 
          E.g.

enum pm_pll_sel_t {
    PLL0_idx,
    PLL1_idx,
};

 */
typedef enum pm_pll_sel_t pm_pll_sel_t;

/*! \note  pm_pll_cfg_t should be defined in device specific PM header file.
//! \name pll config struct
//! @{
struct pm_pll_cfg_t {
    pm_clk_src_sel_t            pll_clk_src;    //!< pll clock source
    uint32_t                    freq;           //!< system oscilator frequency
    uint8_t                     Msel;           //!< PLL Feedback divider value
    uint8_t                     Psel;           //!< pll Feedback divider value
};
//! @}
*/

typedef struct pm_pll_cfg_t pm_pll_cfg_t;

//! \name pll post divider
//! @{
typedef enum {
    PLL_POST_DIV_1              = 0x00,     //!< pll post divider rate is 1
    PLL_POST_DIV_2              = 0x01,     //!< pll post divider rate is 2
    PLL_POST_DIV_4              = 0x02,     //!< pll post divider rate is 4
    PLL_POST_DIV_8              = 0x03,     //!< pll post divider rate is 8
} pm_pll_post_div_t;
//! @}

//! \name pll struct type
//! @{
def_interface(i_pm_pll_t)
    //! Pll config
    fsm_rt_t                    (*Init)(pm_pll_sel_t pll, pm_pll_cfg_t *pcfg);
    //! PLL Check Is Pll Locked
    bool                        (*IsLocked)(pm_pll_sel_t pll);
    //! get pll output clock
    uint_fast32_t               (*GetClockOut)(pm_pll_sel_t pll); 
    //! get pll input clock
    uint_fast32_t               (*GetClockIn)(pm_pll_sel_t pll); 
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
            uint32_t DIV                     : 5;
            uint32_t FREQ                    : 4;
            reg32_t                          :23;
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
typedef enum pm_lposc_sel_t pm_lposc_sel_t;
typedef struct pm_lposc_cfg_t pm_lposc_cfg_t;

//! \name low power oscillator 
//! @{
def_interface(i_pm_lposc_t)
    vsf_err_t       (*Init)     (pm_lposc_sel_t lposc, pm_lposc_cfg_t *ptCFG);
    void            (*Enable)   (pm_lposc_sel_t lposc);
    void            (*Disable)  (pm_lposc_sel_t lposc);
    uint_fast32_t   (*GetClock) (pm_lposc_sel_t lposc);
end_def_interface(i_pm_lposc_t)
//! @}

/*----------------------------------------------------------------------------*
 * Clock Management                                                           *
 *----------------------------------------------------------------------------*/

//! \name pll struct type
//! @{
typedef struct {
    //! clk out source select
    pm_clk_src_sel_t            clk_src;        //!< clock source for output
    //! clk divider
    uint_fast8_t                div;            //!< divider for output clock
} pm_clock_out_cfg_t;
//! @}


def_interface(i_pm_clk_t)

    i_pm_main_clk_t         Main;
    uint_fast32_t           (*GetSysClk)(void);
    vsf_err_t               (*ClkOutCfg)( pm_clock_out_cfg_t *ptCfg);
    i_pm_periph_asyn_clk_t  Peripheral;
    i_pm_ahb_clk_t          AHB;
    i_pm_lposc_t            LPOSC;                  //!< low power oscillators
    i_pm_pll_t              PLL;                    //!< pll control

end_def_interface(i_pm_clk_t)


/*----------------------------------------------------------------------------*
 * PM Interface                                                               *
 *----------------------------------------------------------------------------*/

//! \name pmu struct
//! @{
def_interface(i_pm_t)   
    //! \brief set the clock auto . the main clock frequency and the div is needed
    fsm_rt_t            (*AutoClock)(   pm_clk_src_sel_t clk_src,
                                        uint_fast32_t src_frq,
                                        uint_fast32_t target_frq);
    i_pm_clk_t          Clock;
    i_pm_power_t        Power;
    i_pm_sleep_t        Sleep;
    
end_def_interface(i_pm_t)
//! @}


/*============================ GLOBAL VARIABLES ==============================*/

extern const i_pm_t VSF_PM;

/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
