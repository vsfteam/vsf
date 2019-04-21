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

/*============================ INCLUDES ======================================*/

#include "hal/arch/vsf_arch_abstraction.h"
#include "systick/systick.h"
#include "hal/driver/driver.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct __vsf_cm_t {
    struct {
        vsf_swi_hanler_t *handler;
        void *pparam;
#if __ARM_ARCH == 6
        bool enabled;
        bool sw_pending_bit;
#endif
    } pendsv;
};
typedef struct __vsf_cm_t __vsf_cm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __vsf_cm_t __vsf_cm;



/*============================ PROTOTYPES ====================================*/
extern void vsf_systimer_evthandler(void);

/*============================ IMPLEMENTATION ================================*/

ROOT void SysTick_Handler(void)
{
    vsf_systimer_evthandler();
}

ROOT void PendSV_Handler(void)
{
    if (__vsf_cm.pendsv.handler != NULL) {
        __vsf_cm.pendsv.handler(__vsf_cm.pendsv.pparam);
    }
}

/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target frequency in Hz
 *! \return initialization result in vsf_err_t 
 */
vsf_err_t vsf_systimer_init(uint32_t frequency)
{
    vsf_systick_cfg (
        ENABLE_SYSTICK              |
        SYSTICK_SOURCE_SYSCLK       |
        ENABLE_SYSTICK_INTERRUPT,
        (VSF_GET_MAIN_CLK() / frequency)
    );

    return VSF_ERR_NONE;
}

WEAK vsf_err_t vsf_drv_swi_init(uint_fast8_t idx, uint_fast8_t priority,
        vsf_swi_hanler_t *handler, void *pparam) { return VSF_ERR_FAIL; }
WEAK void vsf_drv_swi_trigger(uint_fast8_t idx) {}

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_swi_init(uint_fast8_t idx, uint_fast8_t priority,
        vsf_swi_hanler_t *handler, void *pparam)
{

    volatile uint32_t *shpr = (volatile uint32_t *)
                                                #if __CORTEX_M == 7
                                                    SCB->SHPR;
                                                #else
                                                    SCB->SHP;
                                                #endif
    if (0 == idx) {
        __vsf_cm.pendsv.handler = handler;
        __vsf_cm.pendsv.pparam = pparam;
#if __ARM_ARCH == 6
        __vsf_cm.pendsv.enabled = true;
        __vsf_cm.pendsv.sw_pending_bit = 0;
#endif
        //shpr[10] = priority;
        uint32_t temp = shpr[3];
        ((uint8_t *)&temp)[2] = priority; 
        shpr[3] = temp;
        return VSF_ERR_NONE;
    } else {
        return vsf_drv_swi_init(idx - 1, priority, handler, pparam);
    }
}



/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
void vsf_swi_trigger(uint_fast8_t idx)
{
    if (0 == idx) {
    #if __ARM_ARCH >= 7
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    #elif __ARM_ARCH == 6
        __SAFE_ATOM_CODE(
            if (__vsf_cm.pendsv.enabled) {
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            } else {
                __vsf_cm.pendsv.sw_pending_bit = 1;
            }
        )
    #endif
    } else {
        vsf_drv_swi_trigger(idx - 1);
    }
}

istate_t vsf_set_base_priority(istate_t priority)
{
#if __ARM_ARCH >= 7
    istate_t origlevel = __get_BASEPRI();
    __set_BASEPRI(priority);
    return origlevel;
#elif __ARM_ARCH == 6
    static istate_t __basepri = 0x100;
    
    istate_t origlevel = __basepri;
    __SAFE_ATOM_CODE(
        if (priority == VSF_ARCH_PRIO_0) {
            //! lock sched
            __vsf_cm.pendsv.enabled = false;
            __vsf_cm.pendsv.sw_pending_bit = 0;
        } else if (0x100 == priority) {
            //! unload sched
            __vsf_cm.pendsv.enabled = true;
            if (__vsf_cm.pendsv.sw_pending_bit) {
                __vsf_cm.pendsv.sw_pending_bit = 0;
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            }
        }
        __basepri = priority;
    )
    return origlevel;
#endif
}



istate_t vsf_get_interrupt(void)
{
    return GET_GLOBAL_INTERRUPT_STATE();
}

void vsf_set_interrupt(istate_t level)
{
    SET_GLOBAL_INTERRUPT_STATE(level);
}

istate_t vsf_disable_interrupt(void)
{
    return DISABLE_GLOBAL_INTERRUPT();
}

void vsf_enable_interrupt(void)
{
    ENABLE_GLOBAL_INTERRUPT();
}

void vsf_arch_sleep(uint32_t mode)
{
    ENABLE_GLOBAL_INTERRUPT();
    __WFI();
}



uint_fast16_t bswap_16(uint_fast16_t value16)
{
    return __REV16(value16);
}

uint_fast32_t bswap_32(uint_fast32_t value32)
{
    return __REV(value32);
}

/* EOF */
