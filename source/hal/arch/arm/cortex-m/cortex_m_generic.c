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

#include "hal/arch/vsf_arch_abstraction.h"
#include "SysTick/systick.h"
#include "hal/driver/vendor_driver.h"
#include "hal/arch/__vsf_arch_interface.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __vsf_cm_t {
    struct {
        vsf_swi_handler_t *handler;
        void *param;
#if __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
        bool enabled;
        bool sw_pending_bit;
#endif
    } pendsv;
    vsf_arch_prio_t  basepri;
#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED
    vsf_arch_text_region_t text_region;
#endif
} __vsf_cm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __vsf_cm_t __vsf_cm = {
    .basepri        = 0x100,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Infrastructure                                                             *
 *----------------------------------------------------------------------------*/
/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_arch_low_level_init(void)
{
    //memset(&__vsf_cm, 0, sizeof(__vsf_cm));
    //vsf_systimer_init();

#if __ARM_ARCH >= 7 && VSF_ARCH_CFG_ALIGNMENT_TEST == ENABLED
#   warning All unaligned access will be treated as faults for alignment test
    /*! disable processor's support for unaligned access for debug purpose */
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

    // SEVONPEND is maybe initialized to 0, which will make wfe not sensitive to interrupt
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED
#   if      __IS_COMPILER_IAR__
#pragma section = ".text"
    __vsf_cm.text_region.start = (uintptr_t)__section_begin(".text");
    __vsf_cm.text_region.size = (uintptr_t)__section_end(".text") - __vsf_cm.text_region.start;
#   elif    __IS_COMPILER_ARM_COMPILER_5__
    extern const int Image$$ER_ROM1$$Base;
    extern const int Image$$ER_ROM1$$Limit;
    __vsf_cm.text_region.start = (uintptr_t)&Image$$ER_ROM1$$Base;
    __vsf_cm.text_region.size = (uintptr_t)&Image$$ER_ROM1$$Limit - __vsf_cm.text_region.start;
#   elif    __IS_COMPILER_GCC__
    // if link error indicating _stext and _etext not exist,
    //  please add these in linker file for start text and end text to use callback trace
    extern const int _stext;
    extern const int _etext;
    __vsf_cm.text_region.start = (uintptr_t)&_stext;
    __vsf_cm.text_region.size = (uintptr_t)&_etext - __vsf_cm.text_region.start;
#   else
#       error contact author to add compiler support
#   endif
#endif
    return true;
}

/*----------------------------------------------------------------------------*
 * System Timer Implementation                                                *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER

vsf_systimer_tick_t vsf_systimer_get_tick_elapsed(void)
{
    return SYSTICK_RVR - vsf_systick_get_count();
}

void vsf_systimer_clear_int_pending_bit(void)
{
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;            //!< clear pending bit
}

void vsf_systimer_reset_counter_value(void)
{
    vsf_systick_clear_count();
}

/*! \brief disable systimer and return over-flow flag status
 *! \param none
 *! \retval true  overflow happened
 *! \retval false no overflow happened
 */
bool vsf_systimer_low_level_disable(void)
{
    bool result = false;
    vsf_gint_state_t orig = vsf_disable_interrupt();
        vsf_systick_disable();
        result = !!(SCB->ICSR & SCB_ICSR_PENDSTSET_Msk);
    vsf_set_interrupt(orig);

    return result;
}

/*! \brief only enable systimer without clearing any flags
 */
void vsf_systimer_low_level_enable(void)
{
    vsf_systick_enable();
}

void vsf_systimer_low_level_int_disable(void)
{
    vsf_systick_int_disable();
}

void vsf_systimer_low_level_int_enable(void)
{
    vsf_systick_int_enable();
}

void vsf_systimer_set_reload_value(vsf_systimer_tick_t tick_cnt)
{
    vsf_systick_set_reload((uint32_t)tick_cnt);
}

VSF_CAL_ROOT VSF_CAL_ISR(SysTick_Handler)
{
    vsf_systimer_ovf_evt_handler();
}


/*! \brief initialise systimer without enable it
 */
vsf_err_t vsf_systimer_low_level_init(uintmax_t ticks)
{
    vsf_gint_state_t orig = vsf_disable_interrupt();
        vsf_systick_cfg (
            DISABLE_SYSTICK             |
            SYSTICK_SOURCE_SYSCLK       |
            ENABLE_SYSTICK_INTERRUPT,
            (uint32_t)ticks
        );

        // NVIC_ClearPendingIRQ has no effect on IRQn < 0
//        NVIC_ClearPendingIRQ(SysTick_IRQn);
        SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
    vsf_set_interrupt(orig);

    return VSF_ERR_NONE;
}

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{
    NVIC_SetPriority(SysTick_IRQn, priority);
}

#endif

/*----------------------------------------------------------------------------*
 * SWI / PendSV                                                               *
 *----------------------------------------------------------------------------*/
#if VSF_USE_KERNEL == ENABLED
VSF_CAL_ROOT
#endif
VSF_CAL_ISR(PendSV_Handler)
{
    if (__vsf_cm.pendsv.handler != NULL) {
        __vsf_cm.pendsv.handler(__vsf_cm.pendsv.param);
    }
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#pragma diag_suppress=pe111
#endif

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_arch_swi_init(uint_fast8_t idx,
                            vsf_arch_prio_t priority,
                            vsf_swi_handler_t *handler,
                            void *param)
{
    if (0 == idx) {
        __vsf_cm.pendsv.handler = handler;
        __vsf_cm.pendsv.param = param;
#if __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
        __vsf_cm.pendsv.enabled = true;
        __vsf_cm.pendsv.sw_pending_bit = 0;
#endif
        NVIC_SetPriority(PendSV_IRQn, priority);
        return VSF_ERR_NONE;
    }
    VSF_ARCH_ASSERT(false);
    return VSF_ERR_INVALID_PARAMETER;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#pragma diag_warning=pe111
#endif


/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
void vsf_arch_swi_trigger(uint_fast8_t idx)
{
    if (0 == idx) {
    //! todo: the code is simplified, we need to verify its reilability
    #if __ARM_ARCH >= 7 || __TARGET_ARCH_THUMB == 4
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    #elif __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3
        vsf_interrupt_safe_simple(
            if (__vsf_cm.pendsv.enabled) {
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            } else {
                __vsf_cm.pendsv.sw_pending_bit = 1;
            }
        )
    #endif
        return;
    }
    VSF_ARCH_ASSERT(false);
}

vsf_arch_prio_t vsf_get_base_priority(void)
{
    return __vsf_cm.basepri;
}

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
#if __ARM_ARCH >= 7 || __TARGET_ARCH_THUMB == 4
    //static vsf_gint_state_t __basepri = 0x100;
    vsf_arch_prio_t origlevel = __vsf_cm.basepri;

    VSF_ARCH_ASSERT(priority >= 0);

    /*! \note When BASEPRI = 0, the CPU execution priority level is not raised
     *!       to 0 as one may think (this would have the effect of masking all
     *!       interrupts).
     *!       Instead, with BASEPRI = 0 all masking is disabled.  With masking
     *!        disabled, any interrupt even with the max priority level can
     *!        interrupt the core. On reset, BASEPRI = 0.
     *!
     *!        The only way to raise the CPU execution priority level to 0 is
     *!        through PRIMASK.
     */
    DISABLE_GLOBAL_INTERRUPT();                                             //! do this first for atomicity

    if (0 == priority) {
        //DISABLE_GLOBAL_INTERRUPT();
        __vsf_cm.basepri = 0;
        //__set_BASEPRI(0);
    } else if (priority >= 0x100) {
        __vsf_cm.basepri = 0x100;
        __set_BASEPRI(0);
        ENABLE_GLOBAL_INTERRUPT();
    } else {
        __set_BASEPRI(priority << (8 - VSF_ARCH_PRI_BIT));
        __vsf_cm.basepri = priority;
        ENABLE_GLOBAL_INTERRUPT();
    }

    return (vsf_arch_prio_t)origlevel;

#elif __ARM_ARCH == 6 || __TARGET_ARCH_THUMB == 3

    vsf_arch_prio_t origlevel = __vsf_cm.basepri;
    VSF_ARCH_ASSERT(priority >= 0);

    DISABLE_GLOBAL_INTERRUPT();


    if (priority) {
        if (priority >= 0x100) {
            __vsf_cm.basepri = 0x100;
        }

        __vsf_cm.basepri = priority;

        if (priority > NVIC_GetPriority(PendSV_IRQn)) {
            __vsf_cm.pendsv.enabled = true;
            if (__vsf_cm.pendsv.sw_pending_bit) {
                __vsf_cm.pendsv.sw_pending_bit = 0;
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            }
        } else {
            __vsf_cm.pendsv.enabled = false;
            __vsf_cm.pendsv.sw_pending_bit = 0;
        }

        ENABLE_GLOBAL_INTERRUPT();
    } else /* if (0 == priority) */{
        __vsf_cm.basepri = 0;
    }

    return (vsf_arch_prio_t)origlevel;

#else
    VSF_ARCH_ASSERT(false);
#endif
}

/*----------------------------------------------------------------------------*
 * interrupt                                                                  *
 *----------------------------------------------------------------------------*/

VSF_CAL_WEAK(vsf_get_interrupt)
vsf_gint_state_t vsf_get_interrupt(void)
{
    return (vsf_gint_state_t)vsf_get_base_priority();
}

VSF_CAL_WEAK(vsf_set_interrupt)
vsf_gint_state_t vsf_set_interrupt(vsf_gint_state_t prio)
{
    return (vsf_gint_state_t)vsf_set_base_priority(prio);
}

VSF_CAL_WEAK(vsf_disable_interrupt)
vsf_gint_state_t vsf_disable_interrupt(void)
{
    return (vsf_gint_state_t)vsf_set_base_priority(vsf_arch_prio_disable_all);
}

VSF_CAL_WEAK(vsf_enable_interrupt)
vsf_gint_state_t vsf_enable_interrupt(void)
{
    return (vsf_gint_state_t)vsf_set_base_priority(vsf_arch_prio_enable_all);
}

VSF_CAL_WEAK(vsf_get_interrupt_id)
int vsf_get_interrupt_id(void)
{
    return ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos);
}

/*----------------------------------------------------------------------------*
 * Others: sleep, reset, mpu, etc                                             *
 *----------------------------------------------------------------------------*/

VSF_CAL_WEAK(vsf_arch_sleep)
void vsf_arch_sleep(uint_fast32_t mode)
{
    switch (mode) {
    case 0:     SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;  break;
    case 1:     SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;  break;
    default:
        // arm core supports normalsleep(0) and deepsleep(1)
        // for other vendor specified sleep modes,
        //  chip vendor should rewrite vsf_arch_sleep in chip driver
        VSF_ARCH_ASSERT(false);
    }
    __DSB();
    __WFE();
}

VSF_CAL_WEAK(vsf_arch_reset)
void vsf_arch_reset(void)
{
    NVIC_SystemReset();
}

VSF_CAL_WEAK(vsf_arch_shutdown)
void vsf_arch_shutdown(void)
{
    VSF_ARCH_ASSERT(false);
}

#if __MPU_PRESENT
// the region added later will have higher priority
void vsf_arch_mpu_config_region(uint32_t idx, uint32_t baseaddr, uint32_t size, vsf_arch_mpu_feature_t feature)
{
    VSF_ARCH_ASSERT(idx <= 15);
    VSF_ARCH_ASSERT(!(size & (size - 1)));
    ARM_MPU_Disable();

    size = vsf_ffs32(size) - 1;
    VSF_ARCH_ASSERT((size >= 4) && (size <= 31));
    ARM_MPU_SetRegionEx(idx, baseaddr,
        feature                         |
        (size   << MPU_RASR_SIZE_Pos)   |
        MPU_RASR_ENABLE_Msk
    );

    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

void vsf_arch_mpu_remove_region(uint32_t idx)
{
    ARM_MPU_Disable();
    ARM_MPU_ClrRegion(idx);
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}
#endif

/*----------------------------------------------------------------------------*
 * arch enhancement                                                           *
 *----------------------------------------------------------------------------*/

uint_fast16_t bswap_16(uint_fast16_t value16)
{
    return __REV16(value16);
}

uint_fast32_t bswap_32(uint_fast32_t value32)
{
    return __REV(value32);
}

uint_fast8_t __vsf_arch_clz(uintalu_t a)
{
    return __CLZ(a);
}

int_fast8_t __vsf_arch_msb(uintalu_t a)
{
    return 31 - (int_fast8_t)__CLZ(a);
}

/*----------------------------------------------------------------------------*
 * callstack trace                                                            *
 *----------------------------------------------------------------------------*/

#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED

#if VSF_USE_KERNEL == ENABLED
#   define __VSF_EDA_CLASS_INHERIT__
#   include "kernel/vsf_kernel.h"
#endif

static bool __vsf_arch_test_text_region(uintptr_t pc)
{
    vsf_arch_text_region_t *text_region = &__vsf_cm.text_region;
    while (text_region != NULL) {
        if (    (pc >= (text_region->start + sizeof(uint32_t)))
            &&  (pc <= (text_region->start + text_region->size))) {
            return true;
        }
        text_region = text_region->next;
    }
    return false;
}

static bool __vsf_arch_test_bl_blx(uintptr_t pc)
{
    uint16_t ins1 = *(uint16_t *)(pc + 0);
    uint16_t ins2 = *(uint16_t *)(pc + 2);

    return  (((ins2 & 0xF800) == 0xF800) && ((ins1 & 0xF800) == 0xF000))
        ||  ((ins2 & 0xFF00) == 0x4700);
}

void vsf_arch_add_text_region(vsf_arch_text_region_t *region)
{
    vsf_arch_text_region_t *text_region = &__vsf_cm.text_region;
    while (text_region->next != NULL) {
        text_region = text_region->next;
    }
    region->next = NULL;
    text_region->next = region;
}

uint_fast16_t vsf_arch_get_callstack(uintptr_t sp, uintptr_t *callstack, uint_fast16_t callstack_num)
{
    uint32_t pc;
    uint_fast16_t realnum = 0;
    uintptr_t stack_bottom;

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    vsf_eda_t *eda = vsf_eda_get_cur();
    if ((NULL == eda) || !vsf_eda_is_stack_owner(eda)) {
        stack_bottom = *(uintptr_t *)SCB->VTOR;
    } else {
#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        __vsf_eda_frame_t *frame = eda->fn.frame;
        vsf_thread_cb_t *thread_cb = (vsf_thread_cb_t *)frame->ptr.target;
        stack_bottom = (uintptr_t)thread_cb->stack + thread_cb->stack_size;
#   else
        vsf_thread_t *thread = (vsf_thread_t *)eda;
        stack_bottom = (uintptr_t)thread->stack + thread->stack_size;
#   endif
    }
#else
    stack_bottom = *(uintptr_t *)SCB->VTOR;
#endif

    // if callstack is allocated on steak, avoid garbage in callstack itself
    memset(callstack, 0, sizeof(uintptr_t) * callstack_num);
    for (; (sp < stack_bottom) && (realnum < callstack_num); sp += sizeof(uint32_t)) {
        pc = *(uint32_t *)sp;
        if (!(pc & 1)) {
            continue;
        }
        pc -= 1;    /* fix thumb */
        if (__vsf_arch_test_text_region(pc) && __vsf_arch_test_bl_blx(pc - 4)) {
            *callstack++ = pc;
            realnum++;
        }
    }
    return realnum;
}

#endif

/*----------------------------------------------------------------------------*
 * arch abi for applet                                                        *
 *----------------------------------------------------------------------------*/

#if VSF_APPLET_USE_ARCH_ABI == ENABLED && !defined(__VSF_APPLET__)
//typedef struct {
//    long long quot;
//    long long rem;
//} lldiv_t;
//typedef struct {
//    int quot;
//    int rem;
//} div_t;
typedef struct {
    unsigned long long quot;
    unsigned long long rem;
} ulldiv_t;
typedef struct {
    unsigned quot;
    unsigned rem;
} uidiv_return;

extern double __aeabi_dadd(double, double);
extern double __aeabi_ddiv(double, double);
extern double __aeabi_dmul(double, double);
extern double __aeabi_drsub(double, double);
extern double __aeabi_dsub(double, double);
extern void __aeabi_cdcmpeq(double, double);
extern void __aeabi_cdcmple(double, double);
extern void __aeabi_cdrcmple(double, double);
extern int __aeabi_dcmpeq(double, double);
extern int __aeabi_dcmplt(double, double);
extern int __aeabi_dcmple(double, double);
extern int __aeabi_dcmpge(double, double);
extern int __aeabi_dcmpgt(double, double);
extern int __aeabi_dcmpun(double, double);
extern float __aeabi_fadd(float, float);
extern float __aeabi_fdiv(float, float);
extern float __aeabi_fmul(float, float);
extern float __aeabi_frsub(float, float);
extern float __aeabi_fsub(float, float);
extern void __aeabi_cfcmpeq(float, float);
extern void __aeabi_cfcmple(float, float);
extern void __aeabi_cfrcmple(float, float);
extern int __aeabi_fcmpeq(float, float);
extern int __aeabi_fcmplt(float, float);
extern int __aeabi_fcmple(float, float);
extern int __aeabi_fcmpge(float, float);
extern int __aeabi_fcmpgt(float, float);
extern int __aeabi_fcmpun(float, float);
extern int __aeabi_d2iz(double);
extern unsigned __aeabi_d2uiz(double);
extern long long __aeabi_d2lz(double);
extern unsigned long long __aeabi_d2ulz(double);
extern int __aeabi_f2iz(float);
extern unsigned __aeabi_f2uiz(float);
extern long long __aeabi_f2lz(float);
extern unsigned long long __aeabi_f2ulz(float);
extern float __aeabi_d2f(double);
extern double __aeabi_f2d(float);
extern double __aeabi_i2d(int);
extern double __aeabi_ui2d(unsigned);
extern double __aeabi_l2d(long long);
extern double __aeabi_ul2d(unsigned long long);
extern float __aeabi_i2f(int);
extern float __aeabi_ui2f(unsigned);
extern float __aeabi_l2f(long long);
extern float __aeabi_ul2f(unsigned long long);
extern long long __aeabi_lmul(long long, long long);
extern lldiv_t __aeabi_ldivmod(long long, long long);
extern ulldiv_t __aeabi_uldivmod(unsigned long long, unsigned long long);
extern long long __aeabi_llsl(long long, int);
extern long long __aeabi_llsr(long long, int);
extern long long __aeabi_lasr(long long, int);
extern int __aeabi_lcmp(long long, long long);
extern int __aeabi_ulcmp(unsigned long long, unsigned long long);
extern int __aeabi_idiv(int, int);
extern unsigned __aeabi_uidiv(unsigned, unsigned);
extern div_t __aeabi_idivmod(int, int);
extern uidiv_return __aeabi_uidivmod(unsigned, unsigned);
extern int __aeabi_idiv0(int);
extern long long __aeabi_ldiv0(long long);
extern int __aeabi_uread4(void *);
extern int __aeabi_uwrite4(int, void *);
extern long long __aeabi_uread8(void *);
extern long long __aeabi_uwrite8(long long, void *);
extern void __aeabi_memcpy8(void *, const void *, size_t);
extern void __aeabi_memcpy4(void *, const void *, size_t);
extern void __aeabi_memcpy(void *, const void *, size_t);
extern void __aeabi_memmove8(void *, const void *, size_t);
extern void __aeabi_memmove4(void *, const void *, size_t);
extern void __aeabi_memmove(void *, const void *, size_t);
extern void __aeabi_memset8(void *, size_t, int);
extern void __aeabi_memset4(void *, size_t, int);
extern void __aeabi_memset(void *, size_t, int);
extern void __aeabi_memclr8(void *, size_t);
extern void __aeabi_memclr4(void *, size_t);
extern void __aeabi_memclr(void *, size_t);
extern void *__aeabi_read_tp(void);

__VSF_VPLT_DECORATOR__ vsf_arch_abi_vplt_t vsf_arch_abi_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_arch_abi_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_cdcmpeq),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_cdcmple),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_cdrcmple),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_cfcmpeq),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_cfcmple),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_cfrcmple),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_ldivmod),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_uldivmod),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_idivmod),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_uidivmod),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_d2f),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_f2d),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_lmul),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_llsl),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_llsr),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_lasr),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_lcmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_ulcmp),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_idiv),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_uidiv),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_uread4),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_uwrite4),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_uread8),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_uwrite8),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dadd),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_ddiv),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dmul),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_drsub),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dsub),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dcmpeq),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dcmplt),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dcmple),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dcmpge),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dcmpgt),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_dcmpun),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fadd),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fdiv),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fmul),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_frsub),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fsub),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fcmpeq),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fcmplt),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fcmple),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fcmpge),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fcmpgt),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_fcmpun),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_d2iz),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_d2uiz),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_d2lz),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_d2ulz),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_f2iz),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_f2uiz),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_f2lz),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_f2ulz),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_i2d),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_ui2d),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_l2d),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_ul2d),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_i2f),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_ui2f),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_l2f),
    VSF_APPLET_VPLT_ENTRY_FUNC(__aeabi_ul2f),
};
#endif

/* EOF */
