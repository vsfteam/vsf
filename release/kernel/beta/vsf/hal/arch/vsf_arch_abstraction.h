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

#ifndef __VSF_ARCH_ABSTRACTION_H__
#define __VSF_ARCH_ABSTRACTION_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*\note generic prototypes MUST be defined before including the arch header. */
typedef void vsf_irq_handler_t(void *p);
typedef vsf_irq_handler_t vsf_swi_handler_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

/*============================ INCLUDES ======================================*/
#if !defined(VSF_ARCH_HEADER)
# if      (defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M') || __TARGET_PROFILE_M
#   define VSF_ARCH_HEADER      "./arm/cortex-m/cortex_m_generic.h"
# elif    defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'A' || __TARGET_PROFILE_A
#   define VSF_ARCH_HEADER      "./arm/cortex-a/cortex_a_generic.h"
# elif    defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'R' || __TARGET_PROFILE_R
#   define VSF_ARCH_HEADER      "./arm/cortex-r/cortex_r_generic.h"
# elif   (defined(__CPU_X86__) || defined(__CPU_X64__)) && __LINUX__
#   define  VSF_ARCH_HEADER     "./x86/linux/linux_generic.h"
# elif   (defined(__CPU_X86__) || defined(__CPU_X64__)) && __WIN__
#   define  VSF_ARCH_HEADER     "./x86/win/win_generic.h"
# elif   defined(__CPU_MCS51__)
#   define  VSF_ARCH_HEADER     "./mcs51/mcs51_generic.h"
# elif   defined(__CPU_RV__)
#   define  VSF_ARCH_HEADER     "./rv/rv_generic.h"
# else
#   warning no supported architecture found, use default arch template!
#   define  VSF_ARCH_HEADER     "./template/template_generic.h"
# endif
#endif
#include VSF_ARCH_HEADER

/*============================ MACROS ========================================*/

#ifndef VSF_ARCH_SWI_NUM
#   define VSF_ARCH_SWI_NUM         0
#endif
#ifndef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM          0
#endif

#ifndef __BYTE_ORDER
#   error "__BYTE_ORDER MUST be defined in arch"
#endif

#define VSF_SWI_NUM                 (VSF_ARCH_SWI_NUM + VSF_DEV_SWI_NUM)

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

#define DECLARE_ENDIAN_FUNC(__bitlen)                                           \
extern uint_fast##__bitlen##_t cpu_to_le##__bitlen##p(uint##__bitlen##_t *);    \
extern uint_fast##__bitlen##_t cpu_to_be##__bitlen##p(uint##__bitlen##_t *);    \
extern uint_fast##__bitlen##_t le##__bitlen##_to_cpup(uint##__bitlen##_t *);    \
extern uint_fast##__bitlen##_t be##__bitlen##_to_cpup(uint##__bitlen##_t *);    \
extern void cpu_to_le##__bitlen##s(uint##__bitlen##_t *);                       \
extern void cpu_to_be##__bitlen##s(uint##__bitlen##_t *);                       \
extern void le##__bitlen##_to_cpus(uint##__bitlen##_t *);                       \
extern void be##__bitlen##_to_cpus(uint##__bitlen##_t *);                       \
extern uint_fast##__bitlen##_t get_unaligned_##__bitlen(const void *);          \
extern uint_fast##__bitlen##_t get_unaligned_le##__bitlen(const void *);        \
extern uint_fast##__bitlen##_t get_unaligned_be##__bitlen(const void *);        \
extern void put_unaligned_##__bitlen(uint_fast##__bitlen##_t, void *);          \
extern void put_unaligned_le##__bitlen(uint_fast##__bitlen##_t, void *);        \
extern void put_unaligned_be##__bitlen(uint_fast##__bitlen##_t, void *);


#ifndef ffs
#   define ffs(__N)        vsf_ffs(__N)
#endif
#ifndef ffz
#   define ffz(__N)        vsf_ffz(__N)
#endif
#ifndef msb
#   define msb(__N)        vsf_msb(__N)
#endif
#ifndef clz
#   define clz(__N)        vsf_clz(__N)
#endif

/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Architecture Infrastructure                                                *
 *----------------------------------------------------------------------------*/
extern uint_fast16_t bswap_16(uint_fast16_t);
extern uint_fast32_t bswap_32(uint_fast32_t);
#ifdef UINT64_MAX
extern uint_fast64_t bswap_64(uint_fast64_t);
#endif

#ifndef VSF_FFS
extern int_fast8_t vsf_ffs(uint_fast32_t);
#endif

#ifndef VSF_FFZ
extern int_fast8_t vsf_ffz(uint_fast32_t);
#endif

#ifndef VSF_MSB
extern int_fast8_t vsf_msb(uint_fast32_t);
#endif

#ifndef VSF_CLZ
extern uint_fast8_t vsf_clz(uint_fast32_t);
#endif

DECLARE_ENDIAN_FUNC(16)
DECLARE_ENDIAN_FUNC(32)
#ifdef UINT64_MAX
DECLARE_ENDIAN_FUNC(64)
#endif


/*----------------------------------------------------------------------------*
 * SWI                                                                        *
 *----------------------------------------------------------------------------*/
extern vsf_err_t vsf_swi_init(  uint_fast8_t idx, 
                                vsf_arch_prio_t priority,
                                vsf_swi_handler_t *handler, 
                                void *param);
extern void vsf_swi_trigger(uint_fast8_t idx);

/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
extern void vsf_arch_swi_trigger(uint_fast8_t idx);

/*----------------------------------------------------------------------------*
 * System Timer                                                               *
 *----------------------------------------------------------------------------*/
#ifdef VSF_SYSTIMER_CFG_IMPL_MODE
extern vsf_err_t vsf_systimer_start(void);
extern vsf_systimer_cnt_t vsf_systimer_get(void);
extern bool vsf_systimer_set(vsf_systimer_cnt_t due);
extern void vsf_systimer_set_idle(void);
extern bool vsf_systimer_is_due(vsf_systimer_cnt_t due);
extern void vsf_systimer_prio_set(vsf_arch_prio_t priority);

extern vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us);
extern vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms);
extern uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick);
extern uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick);
#endif


/*----------------------------------------------------------------------------*
 * Interrupt                                                                  *
 *----------------------------------------------------------------------------*/
extern vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority);

extern vsf_gint_state_t vsf_get_interrupt(void);
extern void vsf_set_interrupt(vsf_gint_state_t level);
extern vsf_gint_state_t vsf_disable_interrupt(void);
extern void vsf_enable_interrupt(void);

extern void vsf_arch_sleep(uint32_t mode);

#endif
/* EOF */
