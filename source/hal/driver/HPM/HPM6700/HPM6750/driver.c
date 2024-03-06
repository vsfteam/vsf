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

#include "hal/driver/common/swi/vsf_swi_template.h"

#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_mchtmr_drv.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * System Timer                                                               *
 *----------------------------------------------------------------------------*/

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{
}

static void __isr_mchtmr(void)
{
    HPM_MCHTMR->MTIMECMP = 0xFFFFFFFFFFFFFFFF;

    extern void vsf_systimer_match_evthandler(void);
    vsf_systimer_match_evthandler();
}
SDK_DECLARE_MCHTMR_ISR(__isr_mchtmr)

/*! \brief initialise systimer (current value set to 0) without enable it
 */
vsf_err_t vsf_systimer_low_level_init(void)
{
    sysctl_set_cpu_lp_mode(HPM_SYSCTL, HPM_CORE0, cpu_lp_mode_ungate_cpu_clock);
    enable_mchtmr_irq();
    return VSF_ERR_NONE;
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
        vsf_systimer_match_evthandler will be called if triggered.
 */
void vsf_systimer_low_level_set_match(vsf_systimer_tick_t match)
{
    HPM_MCHTMR->MTIMECMP = match;
}

unsigned int __hpm_systimer_get_frequency(void)
{
    return clock_get_frequency(clock_mchtmr0);
}

/*----------------------------------------------------------------------------*
 * SWI                                                                        *
 *----------------------------------------------------------------------------*/

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
    // TODO: is PLICSW affected by PLIC threshold?
    volatile uint32_t *threshold_ptr = (volatile uint32_t *)(HPM_PLIC_BASE +
            HPM_PLIC_THRESHOLD_OFFSET +
            (HPM_PLIC_TARGET_M_MODE << HPM_PLIC_THRESHOLD_SHIFT_PER_TARGET));
    vsf_arch_prio_t orig = (vsf_arch_prio_t)*threshold_ptr;
    *threshold_ptr = priority;
    return orig;
}

#if     (VSF_ARCH_SWI_NUM > 0)                                                \
    ||  (defined(VSF_DEV_SWI_NUM) && (VSF_DEV_SWI_NUM > 0))
typedef struct __hpm_swi_t {
    vsf_swi_handler_t *handler;
    void *param;
} __hpm_swi_t;
static __hpm_swi_t __hpm_swi[VSF_ARCH_SWI_NUM + VSF_DEV_SWI_NUM];
#endif

#if VSF_ARCH_SWI_NUM > 0
#   if VSF_ARCH_SWI_NUM != 1
#       error VSF_ARCH_SWI_NUM MUST be 1
#   endif

void ASWI_IRQHandler(void)
{
    __hpm_swi_t *swi = &__hpm_swi[0];
    if (swi->handler != NULL) {
        swi->handler(swi->param);
    }
}
SDK_DECLARE_SWI_ISR(ASWI_IRQHandler)

vsf_err_t vsf_arch_swi_init(    uint_fast8_t idx,
                                vsf_arch_prio_t priority,
                                vsf_swi_handler_t *handler,
                                void *param)
{
    if (idx < VSF_ARCH_SWI_NUM) {
        __hpm_swi_t *swi = &__hpm_swi[0];
        if (handler != NULL) {
            VSF_HAL_ASSERT(priority == vsf_arch_prio_0);
            swi->handler = handler;
            swi->param = param;
            set_csr(CSR_MIE, CSR_MIE_MSIE_MASK);
            __plic_set_irq_priority(HPM_PLICSW_BASE, PLICSWI, 1);
            __plic_enable_irq(HPM_PLICSW_BASE, HPM_PLIC_TARGET_M_MODE, PLICSWI);
        } else {
            clear_csr(CSR_MIE, CSR_MIE_MSIE_MASK);
        }
        return VSF_ERR_NONE;
    }
    VSF_HAL_ASSERT(false);
    return VSF_ERR_INVALID_PARAMETER;
}

void vsf_arch_swi_trigger(uint_fast8_t idx)
{
    if (idx < VSF_ARCH_SWI_NUM) {
        __plic_set_irq_pending(HPM_PLICSW_BASE, PLICSWI);
        return;
    }
    VSF_HAL_ASSERT(false);
}
#endif

#ifdef VSF_DEV_SWI_NUM
static const uint8_t __hpm_swi_irq[VSF_DEV_SWI_NUM] = {
#define __HPM_SWI_IRQ(__N, __NUM)       VSF_MCONNECT(VSF_DEV_SWI, __N ,_IRQ),
    VSF_MREPEAT(VSF_DEV_SWI_NUM, __HPM_SWI_IRQ, null)
};

#define __HPM_SWI(__N, __VALUE)                                                 \
    VSF_CAL_ROOT void VSF_MCONNECT(SWI, __N, _IRQHandler)(void)                 \
    {                                                                           \
        __hpm_swi_t *swi = &__hpm_swi[VSF_ARCH_SWI_NUM + __N];                  \
        if (swi->handler != NULL) {                                             \
            swi->handler(swi->param);                                           \
        }                                                                       \
    }                                                                           \
    SDK_DECLARE_EXT_ISR_M(VSF_MCONNECT(VSF_DEV_SWI, __N, _IRQ), VSF_MCONNECT(SWI, __N, _IRQHandler))

VSF_MREPEAT(VSF_DEV_SWI_NUM, __HPM_SWI, NULL)

vsf_err_t vsf_drv_usr_swi_init( uint_fast8_t idx,
                                vsf_arch_prio_t priority,
                                vsf_swi_handler_t *handler,
                                void *param)
{
    if (idx < VSF_DEV_SWI_NUM) {
        __hpm_swi_t *swi = &__hpm_swi[VSF_ARCH_SWI_NUM + idx];
        uint32_t irq = __hpm_swi_irq[idx];
        if (handler != NULL) {
            swi->handler = handler;
            swi->param = param;
            __plic_set_irq_priority(HPM_PLIC_BASE, irq, priority);
            __plic_enable_irq(HPM_PLIC_BASE, HPM_PLIC_TARGET_M_MODE, irq);
        } else {
            __plic_disable_irq(HPM_PLIC_BASE, HPM_PLIC_TARGET_M_MODE, irq);
        }
        return VSF_ERR_NONE;
    }
    VSF_HAL_ASSERT(false);
    return VSF_ERR_FAIL;
}

void vsf_drv_usr_swi_trigger(   uint_fast8_t idx)
{
    if (idx < VSF_DEV_SWI_NUM) {
        __plic_set_irq_pending(HPM_PLIC_BASE, __hpm_swi_irq[idx]);
        return;
    }
    VSF_HAL_ASSERT(false);
}

#endif

/*----------------------------------------------------------------------------*
 * reset                                                                      *
 *----------------------------------------------------------------------------*/

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
