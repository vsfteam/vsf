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
#include "hal/vsf_hal_cfg.h"
#include "./common.h"
#include "hal/arch/vsf_arch.h"


/*============================ MACROS ========================================*/
#define SWI_COUNT           25

#define __M480_SWI(__N, __VALUE)                                                \
    void SWI##__N##_IRQHandler(void)                                            \
    {                                                                           \
        if (__m480_common.swi[__N].handler != NULL) {                           \
            __m480_common.swi[__N].handler(__m480_common.swi[__N].pparam);      \
        }                                                                       \
    }

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

static const IRQn_Type m480_soft_irq[SWI_COUNT] = {
    5, 45, 50, 69, 81, 83, 91, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111
};

struct __m480_common_t {
    struct {
        vsf_swi_hanler_t *handler;
        void *pparam;
    } swi[dimof(m480_soft_irq)];
};
typedef struct __m480_common_t __m480_common_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __m480_common_t __m480_common;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

bool m480_reg_unlock(void)
{
    bool is_unlocked = SYS->REGLCTL;
    if (!is_unlocked) {
        SYS->REGLCTL = 0x59;
        SYS->REGLCTL = 0x16;
        SYS->REGLCTL = 0x88;
    }
    return is_unlocked;
}

void m480_reg_lock(bool is_unlocked)
{
    if (!is_unlocked) {
        SYS->REGLCTL = 0;
    }
}

uint_fast32_t m480_bit_field_get(uint_fast16_t bf, uint32_t *ptr)
{
    uint_fast8_t bit_offset = bf & __MASK(5);
    uint_fast8_t ptr_offset = (bf >> 5) & __MASK(3);
    uint_fast8_t bit_len = (bf >> 8) & __MASK(5);

    uint_fast32_t mask = (((1 << bit_len) - 1) << bit_offset);
    return ptr[ptr_offset] & mask;
}

void m480_bit_field_set_atom(uint_fast16_t bf, uint32_t *ptr, uint_fast32_t value)
{
    uint_fast8_t bit_offset = bf & __MASK(5);
    uint_fast8_t ptr_offset = (bf >> 5) & __MASK(3);
    uint_fast8_t bit_len = (bf >> 8) & __MASK(5);

    uint_fast32_t mask = (((1 << bit_len) - 1) << bit_offset);
    vsf_protect_t state = vsf_protect_interrupt();
        ptr[ptr_offset] &= ~mask;
        ptr[ptr_offset] |= value << bit_offset;
    vsf_unprotect_interrupt(state);
}

void m480_bit_field_set_wprotect(uint_fast16_t bf, uint32_t *ptr, uint_fast32_t value)
{
    bool state = m480_reg_unlock();
        m480_bit_field_set_atom(bf, ptr, value);
    m480_reg_lock(state);
}

void m480_bit_field_set(uint_fast16_t bf, uint32_t *ptr, uint_fast32_t value)
{
    if (bf & (1 << 13)) {
        m480_bit_field_set_wprotect(bf, ptr, value);
    } else {
        m480_bit_field_set_atom(bf, ptr, value);
    }
}

// SWI
MREPEAT(SWI_COUNT, __M480_SWI, NULL)

vsf_err_t vsf_drv_swi_init(uint_fast8_t idx, uint_fast8_t priority,
                            vsf_swi_hanler_t *handler, void *pparam)
{
    ASSERT(idx < dimof(m480_soft_irq));

    if (handler != NULL) {
        __m480_common.swi[idx].handler = handler;
        __m480_common.swi[idx].pparam = pparam;

        NVIC_SetPriority(m480_soft_irq[idx], priority);
        NVIC_EnableIRQ(m480_soft_irq[idx]);
    } else {
        NVIC_DisableIRQ(m480_soft_irq[idx]);
    }
    return VSF_ERR_NONE;
}

void vsf_drv_swi_trigger(uint_fast8_t idx)
{
    ASSERT(idx < dimof(m480_soft_irq));

    NVIC_SetPendingIRQ(m480_soft_irq[idx]);
}

// USB PHY configuration
void m480_enable_usbphy(m480_usbphy_t phy, m480_usbphy_role_t role)
{
    SYS->USBPHY = (SYS->USBPHY & (0xFFFF << (16 - phy)))
                | ((SYS_USBPHY_USBEN_Msk | role) << phy)
                | SYS_USBPHY_SBO_Msk;
    for (int i=0; i < 0x2000; i++); 
	SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;
}

void m480_disable_usbphy(m480_usbphy_t phy)
{
    SYS->USBPHY &= ~(SYS_USBPHY_USBEN_Msk << phy);
}
