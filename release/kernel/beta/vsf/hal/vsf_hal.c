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

#include "./arch/vsf_arch.h"
#include "./driver/driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK uint_fast8_t bswap_8(uint_fast8_t value8)
{
    value8 = ((value8 >> 1) & 0x55) | ((value8 << 1) & 0xAA);
    value8 = ((value8 >> 2) & 0x33) | ((value8 << 2) & 0xCC);
    value8 = ((value8 >> 4) & 0x0F) | ((value8 << 4) & 0xF0);
    return value8;
}

WEAK uint_fast16_t bswap_16(uint_fast16_t value16)
{
    return (bswap_8(value16) << 8) | bswap_8(value16 >> 8);
}

WEAK uint_fast32_t bswap_32(uint_fast32_t value32)
{
    return (bswap_16(value32) << 16) | bswap_16(value32 >> 16);
}

WEAK uint_fast64_t bswap_64(uint_fast64_t value64)
{
    return (bswap_32(value64) << 16) | bswap_32(value64 >> 16);
}

WEAK bool driver_init(void) 
{
    return true;
}

/*! \note initialize hardware abstract layer
 *  \param none
 *  \retval true hal initialization succeeded.
 *  \retval false hal initialization failed
 */
bool vsf_hal_init( void )
{
    if (!vsf_arch_init() || !driver_init()) {
        return false;
    }

    return true;
}


/* EOF */
