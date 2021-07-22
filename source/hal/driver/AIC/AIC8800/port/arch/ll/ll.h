/**
 ****************************************************************************************
 *
 * @file ll.h
 *
 * @brief Declaration of low level functions.
 *
 ****************************************************************************************
 */

#ifndef LL_H_
#define LL_H_

#include "hal/arch/vsf_arch.h"

/** @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 */
#define GLOBAL_INT_START()          vsf_enable_interrupt()

/** @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 */
#define GLOBAL_INT_STOP()           vsf_disable_interrupt()


/** @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 */

#define GLOBAL_INT_DISABLE()                                                \
do {                                                                        \
    vsf_arch_prio_t __vsf_orig_int_state = vsf_disable_interrupt();         \


/** @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 */

#define GLOBAL_INT_RESTORE()                                                \
    vsf_set_interrupt(__vsf_orig_int_state);                                \
} while(0)

/** @brief Invoke the wait for interrupt procedure of the processor.
 *
 * @warning It is suggested that this macro is called while the interrupts are disabled
 * to have performed the checks necessary to decide to move to sleep mode.
 *
 */
#define WFI()                                                                            \
do {                                                                                     \
    GLOBAL_INT_DISABLE();                                                                \
    __WFI();                                                                             \
    GLOBAL_INT_RESTORE();                                                                \
} while (0)

__STATIC_FORCEINLINE uint32_t global_irq_lock(void)
{
    uint32_t pmask = __get_PRIMASK();
    if ((pmask & 0x1) == 0) {
        __disable_irq();
    }
    return pmask;
}

__STATIC_FORCEINLINE void global_irq_unlock(uint32_t pmask)
{
    if ((pmask & 0x1) == 0) {
        __enable_irq();
    }
}

/**
 * Mark the start of a critical section
 *
 * On the first call to enter a critical section this function MUST store the
 * state of any interrupts or other application settings it will modify to
 * facilitate the critical section.
 */
void critical_section_start(void);

/** Mark the end of a critical section.
 *
 * The purpose of this function is to restore any state that was modified upon
 * entering the critical section, allowing other threads or interrupts to change
 * the processor control.
 */
void critical_section_end(void);


#endif // LL_H_
