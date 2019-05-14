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
#include "hal/vsf_hal.h"

#include "./arch/vsf_arch.h"
#include "./driver/driver.h"

/*============================ MACROS ========================================*/

#define implement_endian_func(__bitlen)                                         \
WEAK uint_fast##__bitlen##_t cpu_to_le##__bitlen##p(uint_fast##__bitlen##_t *p) \
{                                                                               \
    return cpu_to_le##__bitlen(*p);                                             \
}                                                                               \
WEAK uint_fast##__bitlen##_t cpu_to_be##__bitlen##p(uint_fast##__bitlen##_t *p) \
{                                                                               \
    return cpu_to_be##__bitlen(*p);                                             \
}                                                                               \
WEAK uint_fast##__bitlen##_t le##__bitlen##_to_cpup(uint_fast##__bitlen##_t *p) \
{                                                                               \
    return le##__bitlen##_to_cpu(*p);                                           \
}                                                                               \
WEAK uint_fast##__bitlen##_t be##__bitlen##_to_cpup(uint_fast##__bitlen##_t *p) \
{                                                                               \
    return be##__bitlen##_to_cpu(*p);                                           \
}                                                                               \
WEAK void cpu_to_le##__bitlen##s(uint_fast##__bitlen##_t *p)                    \
{                                                                               \
    *p = cpu_to_le##__bitlen(*p);                                               \
}                                                                               \
WEAK void cpu_to_be##__bitlen##s(uint_fast##__bitlen##_t *p)                    \
{                                                                               \
    *p = cpu_to_be##__bitlen(*p);                                               \
}                                                                               \
WEAK void le##__bitlen##_to_cpus(uint_fast##__bitlen##_t *p)                    \
{                                                                               \
    *p = le##__bitlen##_to_cpu(*p);                                             \
}                                                                               \
WEAK void be##__bitlen##_to_cpus(uint_fast##__bitlen##_t *p)                    \
{                                                                               \
    *p = be##__bitlen##_to_cpu(*p);                                             \
}                                                                               \
WEAK uint_fast##__bitlen##_t get_unaligned_##__bitlen(const void *p)            \
{                                                                               \
    struct PACKED __packed_##__bitlen_t {                                       \
        uint##__bitlen##_t __v;                                                 \
    } *__p = (struct __packed_##__bitlen_t *)p;                                 \
    return __p->__v;                                                            \
}                                                                               \
WEAK uint_fast##__bitlen##_t get_unaligned_le##__bitlen(const void *p)          \
{                                                                               \
    return cpu_to_le##__bitlen(get_unaligned_##__bitlen(p));                    \
}                                                                               \
WEAK uint_fast##__bitlen##_t get_unaligned_be##__bitlen(const void *p)          \
{                                                                               \
    return cpu_to_be##__bitlen(get_unaligned_##__bitlen(p));                    \
}                                                                               \
WEAK void put_unaligned_##__bitlen(uint_fast##__bitlen##_t val, void *p)        \
{                                                                               \
    struct PACKED __packed_##__bitlen_t {                                       \
        uint##__bitlen##_t __v;                                                 \
    } *__p = (struct __packed_##__bitlen_t *)p;                                 \
    __p->__v = val;                                                             \
}                                                                               \
WEAK void put_unaligned_le##__bitlen(uint_fast##__bitlen##_t val, void *p)      \
{                                                                               \
    put_unaligned_##__bitlen(cpu_to_le##__bitlen(val), p);                      \
}                                                                               \
WEAK void put_unaligned_be##__bitlen(uint_fast##__bitlen##_t val, void *p)      \
{                                                                               \
    put_unaligned_##__bitlen(cpu_to_be##__bitlen(val), p);                      \
}

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

implement_endian_func(16)
implement_endian_func(32)
implement_endian_func(64)

WEAK bool vsf_driver_init(void) 
{
    return true;
}

/*! \note initialize level 0/1 hardware abstract layer
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */  
bool vsf_hal_init( void )
{
    if (!vsf_arch_init() || !vsf_driver_init()) {
        return false;
    }

    return true;
}

/*! \note initialize level 2 hardware abstract layer
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */  
WEAK bool vsf_hal_advance_init(void)
{
    //! level 2 hal init
    return true;
}


/* EOF */
