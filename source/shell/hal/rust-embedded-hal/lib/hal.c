#include <hal/vsf_hal.h>

#if defined(VSF_HW_INTERRUPTS_NUM) && defined(__CPU_ARM__)

#   define DEFINE_VSF_HW_INTERRUPT(__N)     (irqhandler_t)VSF_MCONNECT(VSF_HW_INTERRUPT, __N),
#   define VSF_HW_INTERRUPTS                VSF_MREPEAT(VSF_HW_INTERRUPTS_NUM, DEFINE_VSF_HW_INTERRUPT)

typedef void(*irqhandler_t)( void );

VSF_CAL_SECTION(".vector_table.interrupts")
const irqhandler_t __eexceptions[] = {
    VSF_HW_INTERRUPTS
};
#endif
