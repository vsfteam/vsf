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
#include "hal/driver/driver.h"
#include "hal/arch/__vsf_arch_interface.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_ARCH_SWI_NUM > 0
typedef volatile struct vsf_arm9_swi_t {

    //! SWI vector table
    struct {
        vsf_swi_handler_t *handler;
        void *param_ptr;
    } vectors[VSF_ARCH_SWI_NUM];

    uint32_t            enable;
    uint32_t            pending;
    vsf_arch_prio_t     base;
    bool                swi_mask_bit;

} vsf_arm9_swi_t;
#endif


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/


#if VSF_ARCH_SWI_NUM > 0
static NO_INIT vsf_arm9_swi_t __vsf_arm9_swi;
#endif

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
#if VSF_ARCH_SWI_NUM > 0
    memset((void *)&__vsf_arm9_swi, 0, sizeof(__vsf_arm9_swi));
#endif
    return true;
}

/*----------------------------------------------------------------------------*
 * SWI                                                                        *
 *----------------------------------------------------------------------------*/
#if VSF_ARCH_SWI_NUM > 0
ROOT ISR(SWI_Handler)
{
    /*!  loop until there is no enabled pending swi whose priority is higher 
     *!  than base
     */
    while (!__vsf_arm9_swi.swi_mask_bit) {
        //! find the highest enabled pending swi
        uint32_t mask = __vsf_arm9_swi.pending & __vsf_arm9_swi.enable;
        int_fast8_t idx = vsf_ffz(~(    mask 
                                    &   ~((1 << __vsf_arm9_swi.base) - 1)));
        if (idx >= 0) {
            if (__vsf_arm9_swi.vectors[idx].handler != NULL) {
                __vsf_arm9_swi.vectors[idx].handler(__vsf_arm9_swi.vectors[idx].param_ptr);
            }
            __vsf_arm9_swi.pending &= ~(1 << idx);
        } else {
            break;
        }
    }    
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#pragma diag_suppress=pe111
#endif


/*! \brief initialise a software interrupt
 *! \param param_ptr idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_arch_swi_init(uint_fast8_t idx, 
                            vsf_arch_prio_t priority,
                            vsf_swi_handler_t *handler, 
                            void *param_ptr)
{
#if VSF_ARCH_SWI_NUM > 0
    if (idx < VSF_ARCH_SWI_NUM) {
        __vsf_arm9_swi.vectors[idx].handler = handler;
        __vsf_arm9_swi.vectors[idx].param_ptr = param_ptr;
        __vsf_arm9_swi.enable |= 1 << idx;
        __vsf_arm9_swi.pending &= ~(1 << idx);
        return VSF_ERR_NONE;
    }
    VSF_HAL_ASSERT(false);
    return VSF_ERR_INVALID_PARAMETER;
#else
    return VSF_ERR_NONE;
#endif
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
    if (idx < VSF_ARCH_SWI_NUM) {
        __vsf_arm9_swi.pending |= 1 << idx;
        __asm__ __volatile__ ("swi 0");
        return;
    }
    VSF_HAL_ASSERT(false);
}

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
    vsf_arch_prio_t orig = __vsf_arm9_swi.base;
    __vsf_arm9_swi.base = priority;
    if (orig > priority) {
        __asm__ __volatile__ ("swi 0");
    }
    return orig;
}
#endif

/*----------------------------------------------------------------------------*
 * interrupt                                                                  *
 *----------------------------------------------------------------------------*/


vsf_gint_state_t vsf_get_interrupt(void)
{
    return GET_GLOBAL_INTERRUPT_STATE();
}

void vsf_set_interrupt(vsf_gint_state_t level)
{
    SET_GLOBAL_INTERRUPT_STATE(level);
}

vsf_gint_state_t vsf_disable_interrupt(void)
{
    return DISABLE_GLOBAL_INTERRUPT();
}

void vsf_enable_interrupt(void)
{
    ENABLE_GLOBAL_INTERRUPT();
}


/*----------------------------------------------------------------------------*
 * Others: sleep, reset, etc                                                  *
 *----------------------------------------------------------------------------*/

void vsf_arch_sleep(uint32_t mode)
{
    mode = 0;
    ENABLE_GLOBAL_INTERRUPT();
    // Wait for interrupt
    __asm__ __volatile__ (
        "mcr p15, 0, %0, c7, c0, 4"
        :
        : "r" (mode)
        : "memory"
    );
}

#if VSF_ARCH_MMU == ENABLED
static inline uint32_t __arm9_read_p15_c1(void)
{
    uint32_t value;

    __asm__ __volatile__(
        "mrc p15, 0, %0, c1, c0, 0"
        : "=r" (value)
        :
        : "memory"
    );

    return value;
}

static inline void __arm9_write_p15_c1(uint32_t value)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c1, c0, 0"
        :
        : "r" (value)
        : "memory"
    );
    __arm9_read_p15_c1();
}

static inline void __arm9_mmu_enable(void)
{
    uint32_t value = __arm9_read_p15_c1();
    __arm9_write_p15_c1(value | (1 << 0));
}

static inline void __arm9_dcache_enable(void)
{
    uint32_t value = __arm9_read_p15_c1();
    __arm9_write_p15_c1(value | (1 << 2));
}

static inline void __arm9_icache_enable(void)
{
    uint32_t value = __arm9_read_p15_c1();
    __arm9_write_p15_c1(value | (1 << 12));
}

#if 0
static inline void __arm9_mmu_disable(void)
{
    uint32_t value = __arm9_read_p15_c1();
    __arm9_write_p15_c1(value & ~(1 << 0));
}

static inline void __arm9_dcache_disable(void)
{
    uint32_t value = __arm9_read_p15_c1();
    __arm9_write_p15_c1(value & ~(1 << 2));
}

static inline void __arm9_icache_disable(void)
{
    uint32_t value = __arm9_read_p15_c1();
    __arm9_write_p15_c1(value & ~(1 << 12));
}
#endif

static inline void __arm9_mmu_ttb_set(uint32_t base)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c2, c0, 0"
        :
        : "r" (base)
        : "memory"
    );
}

static inline void __arm9_mmu_domain_set(uint32_t domain)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c3, c0, 0"
        :
        : "r" (domain)
        : "memory"
    );
}

static inline void __arm9_mmu_tlb_invalidate(void)
{
    __asm__ __volatile__(
        "mov r0, #0\n"
        "mcr p15, 0, r0, c8, c7, 0\n"
        "mcr p15, 0, r0, c8, c6, 0\n"
        "mcr p15, 0, r0, c8, c5, 0\n"
        :
        :
        : "r0"
    );
}

void vsf_arch_mmu_map(uint32_t *ttb, virtual_addr_t virt, physical_addr_t phys, physical_size_t size, int type)
{
    physical_size_t i;

    virt >>= 20;
    phys >>= 20;
    size >>= 20;
    type &= 0x3;

    for (i = size; i > 0; i--, virt++, phys++) {
        ttb[virt] = (phys << 20) | (1 << 16) | (0x3 << 10) | (0x0 << 5) | (type << 2) | (0x2 << 0);
    }
}

void vsf_arch_mmu_refresh(void)
{
    __arm9_mmu_tlb_invalidate();
}

void vsf_arch_mmu_enable(uint32_t *ttb)
{
    __arm9_mmu_ttb_set((uint32_t)ttb);
    __arm9_mmu_tlb_invalidate();
    __arm9_mmu_domain_set(0x3);
    __arm9_mmu_enable();
    __arm9_icache_enable();
    __arm9_dcache_enable();
}
#endif

static inline uint_fast32_t vsf_cache_get(void)
{
    uint_fast32_t cache;
    __asm__ __volatile__(
        "mrc p15, 0, %0, c0, c0, 1"
        : "=r" (cache)
    );
    return cache;
}

static inline void __vsf_cache_invalidate_range(uint_fast32_t start, uint_fast32_t stop)
{
    uint_fast32_t cache, line, mva;

    cache = vsf_cache_get();
    line = 1 << ((cache & 0x3) + 3);

    start &= ~(line - 1);
    if (stop & (line - 1)) {
        stop = (stop + line) & ~(line - 1);
    }
    for(mva = start; mva < stop; mva = mva + line) {
        __asm__ __volatile__(
            "mcr p15, 0, %0, c7, c6, 1"
            :
            : "r" (mva)
        );
    }
    dsb();
}

static inline void __vsf_cache_flush_range(uint_fast32_t start, uint_fast32_t stop)
{
    uint_fast32_t cache, line, mva;

    cache = vsf_cache_get();
    line = 1 << ((cache & 0x3) + 3);

    start &= ~(line - 1);
    if (stop & (line - 1)) {
        stop = (stop + line) & ~(line - 1);
    }
    for(mva = start; mva < stop; mva = mva + line) {
        __asm__ __volatile__("mcr p15, 0, %0, c7, c14, 1"
            :
            : "r" (mva)
        );
    }
    dsb();
}

void vsf_cache_sync(void * addr, uint_fast32_t size, bool is_from_device)
{
    uint_fast32_t start = (uint_fast32_t)addr;
    uint_fast32_t stop = start + size;

    if (is_from_device) {
        __vsf_cache_invalidate_range(start, stop);
    } else {
        __vsf_cache_flush_range(start, stop);
    }
}

/*----------------------------------------------------------------------------*
 * arch enhancement                                                           *
 *----------------------------------------------------------------------------*/



/* EOF */
