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

#ifndef __VSF_HAL_H__
#define __VSF_HAL_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#include "./arch/vsf_arch.h"
//#include "./driver/driver.h"

/* \note: never include interface.h here, individual device drivers might 
 *        include it their own driver header files.  
 */
//#include "./interface.h"

/*============================ MACROS ========================================*/

#if __BYTE_ORDER == __BIG_ENDIAN
#   define cpu_to_le16 bswap_16
#   define cpu_to_le32 bswap_32
#   define cpu_to_le64 bswap_64
#   define le16_to_cpu bswap_16
#   define le32_to_cpu bswap_32
#   define le64_to_cpu bswap_64
#   define cpu_to_be16
#   define cpu_to_be32
#   define cpu_to_be64
#   define be16_to_cpu
#   define be32_to_cpu
#   define be64_to_cpu
#else
#   define cpu_to_le16
#   define cpu_to_le32
#   define cpu_to_le64
#   define le16_to_cpu
#   define le32_to_cpu
#   define le64_to_cpu
#   define cpu_to_be16 bswap_16
#   define cpu_to_be32 bswap_32
#   define cpu_to_be64 bswap_64
#   define be16_to_cpu bswap_16
#   define be32_to_cpu bswap_32
#   define be64_to_cpu bswap_64
#endif

#define declare_endian_func(__bitlen)                                           \
extern uint_fast##__bitlen##_t cpu_to_le##__bitlen##p(uint_fast##__bitlen##_t *);\
extern uint_fast##__bitlen##_t cpu_to_be##__bitlen##p(uint_fast##__bitlen##_t *);\
extern uint_fast##__bitlen##_t le##__bitlen##_to_cpup(uint_fast##__bitlen##_t *);\
extern uint_fast##__bitlen##_t be##__bitlen##_to_cpup(uint_fast##__bitlen##_t *);\
extern void cpu_to_le##__bitlen##s(uint_fast##__bitlen##_t *);                  \
extern void cpu_to_be##__bitlen##s(uint_fast##__bitlen##_t *);                  \
extern void le##__bitlen##_to_cpus(uint_fast##__bitlen##_t *);                  \
extern void be##__bitlen##_to_cpus(uint_fast##__bitlen##_t *);                  \
extern uint_fast##__bitlen##_t get_unaligned_##__bitlen(const void *);          \
extern uint_fast##__bitlen##_t get_unaligned_le##__bitlen(const void *);        \
extern uint_fast##__bitlen##_t get_unaligned_be##__bitlen(const void *);        \
extern void put_unaligned_##__bitlen(uint_fast##__bitlen##_t, void *);          \
extern void put_unaligned_le##__bitlen(uint_fast##__bitlen##_t, void *);        \
extern void put_unaligned_be##__bitlen(uint_fast##__bitlen##_t, void *);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast16_t bswap_8(uint_fast16_t);
extern uint_fast16_t bswap_16(uint_fast16_t);
extern uint_fast32_t bswap_32(uint_fast32_t);
extern uint_fast64_t bswap_64(uint_fast64_t);

declare_endian_func(16)
declare_endian_func(32)
declare_endian_func(64)

/*! \note initialize level 0/1 hardware abstract layer
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */  
extern bool vsf_hal_init( void );

/*! \note initialize level 2 hardware abstract layer
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */  
extern bool vsf_hal_advance_init(void);

#endif
/* EOF */
