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

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#include "hal/driver/common/swi/arm/vsf_swi_template.inc"

// TODO: if these initialization is implemented in startup file, remove here
int __low_level_init(void)
{
    return 1;
}

// TODO: implement vsf_hw_clk_*

static void __vsf_driver_clock_init(void)
{
    /* reset crm */
    crm_reset();

    /* config flash psr register */
    flash_psr_set(FLASH_WAIT_CYCLE_6);

    /* enable pwc periph clock */
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

    /* set power ldo output voltage to 1.3v */
    pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V3);

    crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

    /* wait till hext is ready */
    while (crm_hext_stable_wait() == ERROR) {
    }

    /* if pll parameter has changed, please use the AT32_New_Clock_Configuration tool for new configuration. */
    crm_pll_config(CRM_PLL_SOURCE_HEXT, 72, 1, CRM_PLL_FP_4);

    /* config pllu div */
    crm_pllu_div_set(CRM_PLL_FU_18);

    /* enable pll */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

    /* wait till pll is ready */
    while (crm_flag_get(CRM_PLL_STABLE_FLAG) != SET) {
    }

    /* config ahbclk */
    crm_ahb_div_set(CRM_AHB_DIV_1);

    /* config apb2clk, the maximum frequency of APB2 clock is 216 MHz */
    crm_apb2_div_set(CRM_APB2_DIV_1);

    /* config apb1clk, the maximum frequency of APB1 clock is 120 MHz */
    crm_apb1_div_set(CRM_APB1_DIV_2);

    /* enable auto step mode */
    crm_auto_step_mode_enable(TRUE);

    /* select pll as system clock source */
    crm_sysclk_switch(CRM_SCLK_PLL);

    /* wait till pll is used as system clock source */
    while (crm_sysclk_switch_status_get() != CRM_SCLK_PLL) {
    }

    /* disable auto step mode */
    crm_auto_step_mode_enable(FALSE);

    // usb clock
    /* select usb 48m clcok source : USB_CLK_HEXT */
    crm_pllu_output_set(TRUE);
    /* wait till pll is ready */
    while (crm_flag_get(CRM_PLLU_STABLE_FLAG) != SET);
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_PLLU);


    /* update system_core_clock global variable */
    system_core_clock_update();
}


/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    __vsf_driver_clock_init();

    return true;
}


/* EOF */
