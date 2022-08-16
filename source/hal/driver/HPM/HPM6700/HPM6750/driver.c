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
#include "./device.h"

#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{
}

static void __isr_mchtmr(void)
{
    vsf_systimer_match_evthanlder();
}
SDK_DECLARE_MCHTMR_ISR(__isr_mchtmr)

/*! \brief initialise systimer (current value set to 0) without enable it
 */
vsf_err_t vsf_systimer_low_level_init(void)
{
    HPM_MCHTMR->MTIME = 0;
}

/*! \brief only enable systimer without clearing any flags
 */
void vsf_systimer_low_level_enable(void)
{
}

/*! \brief get current value of timer
 */
vsf_systimer_tick_t vsf_systimer_low_level_get_current(void)
{
    return HPM_MCHTMR->MTIME;
}

/*! \brief set match value, will be triggered when current >= match,
        vsf_systimer_match_evthanlder will be called if triggered.
 */
void vsf_systimer_low_level_set_match(vsf_systimer_tick_t match)
{
    HPM_MCHTMR->MTIMECMP = match;
}

void reset_handler(void)
{
    l1c_dc_disable();
    l1c_dc_invalidate_all();

#if !defined(__SEGGER_RTL_VERSION) || defined(__GNU_LINKER)
    /*
     * Initialize LMA/VMA sections.
     * Relocation for any sections that need to be copied from LMA to VMA.
     */
    extern void c_startup(void);
    c_startup();
#endif

    /* Call platform specific hardware initialization */
    extern void system_init(void);
    system_init();

#ifdef __cplusplus
    /* Do global constructors */
    __libc_init_array();
#endif

    /* Entry function */
    extern void __vsf_main_entry(void);
    __vsf_main_entry();
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    return true;
}


/* EOF */
