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

#ifndef __HAL_DRIVER_NUVOTON_M480_PM_H__
#define __HAL_DRIVER_NUVOTON_M480_PM_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name pll config struct
//! @{
struct pm_pll_cfg_t {
    pm_clk_src_sel_t            pll_clk_src;    //!< pll clock source
    uint32_t                    freq;           //!< system oscilator frequency
    uint8_t                     Msel;           //!< PLL Feedback divider value
    uint8_t                     Psel;           //!< pll Feedback divider value
};
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
#include "hal/interface/vsf_interface_pm.h"

/*============================ PROTOTYPES ====================================*/
extern pm_power_status_t vsf_pm_power_get_status(pm_power_cfg_no_t index);
extern vsf_err_t vsf_pm_power_resume(pm_power_cfg_no_t index, pm_power_status_t status);
extern pm_power_status_t vsf_pm_power_enable(pm_power_cfg_no_t index);
extern pm_power_status_t vsf_pm_power_disable(pm_power_cfg_no_t index);
extern vsf_err_t vsf_pm_sleep(pm_sleep_cfg_t *cfg);
extern  pm_periph_async_clk_status_t 
        vsf_pm_peripheral_get_status(pm_periph_async_clk_no_t index);
extern vsf_err_t vsf_pm_peripheral_resume(  pm_periph_async_clk_no_t index , 
                                            pm_periph_async_clk_status_t status);
extern pm_periph_async_clk_status_t 
        vsf_pm_peripheral_config(   pm_periph_async_clk_no_t index,
                                    const pm_periph_asyn_clk_cfg_t *cfg);

extern uint_fast32_t vsf_pm_peripheral_get_clock(pm_periph_async_clk_no_t index);
extern pm_ahbclk_status_t vsf_pm_ahbclk_get_status(pm_ahb_clk_no_t index);
extern vsf_err_t vsf_pm_ahbclk_resume(pm_ahb_clk_no_t index, pm_ahbclk_status_t status);
extern pm_ahbclk_status_t vsf_pm_ahbclk_enable(pm_ahb_clk_no_t index);
extern pm_ahbclk_status_t vsf_pm_ahbclk_disable(pm_ahb_clk_no_t index);
extern fsm_rt_t vsf_pm_mclk_init(pm_main_clk_cfg_t *cfg);
extern uint_fast32_t vsf_pm_mclk_get(pm_main_clk_no_t sel);
extern fsm_rt_t vsf_pm_pll_init(pm_pll_sel_t pll, pm_pll_cfg_t *cfg);
extern bool vsf_pm_pll_is_locked(pm_pll_sel_t pll);
extern uint_fast32_t vsf_pm_pll_get_clk_out(pm_pll_sel_t pll);
extern uint_fast32_t vsf_pm_pll_get_clk_in(pm_pll_sel_t pll);
#endif
/* EOF */
