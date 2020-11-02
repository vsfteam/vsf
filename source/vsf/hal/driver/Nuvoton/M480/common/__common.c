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
#include "../__device.h"

    #if __IS_COMPILER_IAR__
    //! statement is unreachable
    #pragma diag_suppress=pe111
    #endif

    /*============================ MACROS ========================================*/
    /*============================ MACROFIED FUNCTIONS ===========================*/
    /*============================ TYPES =========================================*/
    /*============================ GLOBAL VARIABLES ==============================*/
    /*============================ LOCAL VARIABLES ===============================*/
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
    uint_fast8_t bit_offset = bf & BITMASK(5);
    uint_fast8_t ptr_offset = (bf >> 5) & BITMASK(3);
    uint_fast8_t bit_len = (bf >> 8) & BITMASK(5);

    uint_fast32_t mask = (((1 << bit_len) - 1) << bit_offset);
    return ptr[ptr_offset] & mask;
    }

    void m480_bit_field_set_atom(uint_fast16_t bf, uint32_t *ptr, uint_fast32_t value)
    {
    uint_fast8_t bit_offset = bf & BITMASK(5);
    uint_fast8_t ptr_offset = (bf >> 5) & BITMASK(3);
    uint_fast8_t bit_len = (bf >> 8) & BITMASK(5);

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



    // USB PHY configuration
    void m480_enable_usbphy(m480_usbphy_t phy, m480_usbphy_role_t role)
    {
    bool state = m480_reg_unlock();
        SYS->USBPHY = (SYS->USBPHY & (0xFFFF << (16 - phy)))
                | ((SYS_USBPHY_USBEN_Msk | role) << phy)
                | SYS_USBPHY_SBO_Msk;
    m480_reg_lock(state);

    if (phy == M480_USBPHY_HS) {
        for (volatile int i=0; i < 0x1000; i++); 
        SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;
    }
    }

    void m480_disable_usbphy(m480_usbphy_t phy)
    {
    SYS->USBPHY &= ~(SYS_USBPHY_USBEN_Msk << phy);
    }

    #if __IS_COMPILER_IAR__
    //! statement is unreachable
    #   pragma diag_warning=pe111
    #endif
