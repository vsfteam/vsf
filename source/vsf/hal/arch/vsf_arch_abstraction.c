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
#include "vsf_arch_abstraction.h"
#include "./__vsf_arch_interface.h"
/*============================ MACROS ========================================*/

#define IMPLEMENT_ENDIAN_FUNC(__bitlen)                                         \
WEAK(cpu_to_le##__bitlen##p)                                                    \
uint_fast##__bitlen##_t cpu_to_le##__bitlen##p(uint_fast##__bitlen##_t *p)      \
{                                                                               \
    return cpu_to_le##__bitlen(*p);                                             \
}                                                                               \
WEAK(cpu_to_be##__bitlen##p)                                                    \
uint_fast##__bitlen##_t cpu_to_be##__bitlen##p(uint_fast##__bitlen##_t *p)      \
{                                                                               \
    return cpu_to_be##__bitlen(*p);                                             \
}                                                                               \
WEAK(le##__bitlen##_to_cpup)                                                    \
uint_fast##__bitlen##_t le##__bitlen##_to_cpup(uint_fast##__bitlen##_t *p)      \
{                                                                               \
    return le##__bitlen##_to_cpu(*p);                                           \
}                                                                               \
WEAK(be##__bitlen##_to_cpup)                                                    \
uint_fast##__bitlen##_t be##__bitlen##_to_cpup(uint_fast##__bitlen##_t *p)      \
{                                                                               \
    return be##__bitlen##_to_cpu(*p);                                           \
}                                                                               \
WEAK(cpu_to_le##__bitlen##s)                                                    \
void cpu_to_le##__bitlen##s(uint_fast##__bitlen##_t *p)                         \
{                                                                               \
    *p = cpu_to_le##__bitlen(*p);                                               \
}                                                                               \
WEAK(cpu_to_be##__bitlen##s)                                                    \
void cpu_to_be##__bitlen##s(uint_fast##__bitlen##_t *p)                         \
{                                                                               \
    *p = cpu_to_be##__bitlen(*p);                                               \
}                                                                               \
WEAK(le##__bitlen##_to_cpus)                                                    \
void le##__bitlen##_to_cpus(uint_fast##__bitlen##_t *p)                         \
{                                                                               \
    *p = le##__bitlen##_to_cpu(*p);                                             \
}                                                                               \
WEAK(be##__bitlen##_to_cpus)                                                    \
void be##__bitlen##_to_cpus(uint_fast##__bitlen##_t *p)                         \
{                                                                               \
    *p = be##__bitlen##_to_cpu(*p);                                             \
}                                                                               \
WEAK(get_unaligned_##__bitlen)                                                  \
uint_fast##__bitlen##_t get_unaligned_##__bitlen(const void *p)                 \
{                                                                               \
    struct PACKED __packed_##__bitlen_t {                                       \
        uint##__bitlen##_t __v;                                                 \
    } *__p = (struct __packed_##__bitlen_t *)p;                                 \
    return __p->__v;                                                            \
}                                                                               \
WEAK(get_unaligned_le##__bitlen)                                                \
uint_fast##__bitlen##_t get_unaligned_le##__bitlen(const void *p)               \
{                                                                               \
    return cpu_to_le##__bitlen(get_unaligned_##__bitlen(p));                    \
}                                                                               \
WEAK(get_unaligned_be##__bitlen)                                                \
uint_fast##__bitlen##_t get_unaligned_be##__bitlen(const void *p)               \
{                                                                               \
    return cpu_to_be##__bitlen(get_unaligned_##__bitlen(p));                    \
}                                                                               \
WEAK(put_unaligned_##__bitlen)                                                  \
void put_unaligned_##__bitlen(uint_fast##__bitlen##_t val, void *p)             \
{                                                                               \
    struct PACKED __packed_##__bitlen_t {                                       \
        uint##__bitlen##_t __v;                                                 \
    } *__p = (struct __packed_##__bitlen_t *)p;                                 \
    __p->__v = val;                                                             \
}                                                                               \
WEAK(put_unaligned_le##__bitlen)                                                \
void put_unaligned_le##__bitlen(uint_fast##__bitlen##_t val, void *p)           \
{                                                                               \
    put_unaligned_##__bitlen(cpu_to_le##__bitlen(val), p);                      \
}                                                                               \
WEAK(put_unaligned_be##__bitlen)                                                \
void put_unaligned_be##__bitlen(uint_fast##__bitlen##_t val, void *p)           \
{                                                                               \
    put_unaligned_##__bitlen(cpu_to_be##__bitlen(val), p);                      \
}


#ifndef __VSF_ARCH_SYSTIMER_BITS   
#error [DEPENDENCY ERROR] Architecture specific header files should provide\
 number of bits used for systimer via macro __VSF_ARCH_SYSTIMER_BITS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct __systimer_t {
    vsf_systimer_cnt_t tick;
    vsf_systimer_cnt_t unit;
    vsf_systimer_cnt_t max_tick_per_round;
    vsf_systimer_cnt_t reload;
    uint32_t           tick_freq;
} __systimer_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static volatile __systimer_t __systimer;

/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_SYSTIMER_EVTHANDLER_EXTERN)                            \
    &&  defined(WEAK_VSF_SYSTIMER_EVTHANDLER)
WEAK_VSF_SYSTIMER_EVTHANDLER_EXTERN
#endif

#if     defined(WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR_EXTERN)      \
    &&  defined(WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR)
WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR_EXTERN
#endif

#if     defined(WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR_EXTERN)            \
    &&  defined(WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR)
WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR_EXTERN
#endif

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
extern vsf_err_t vsf_arch_swi_init( uint_fast8_t idx, 
                                    vsf_arch_prio_t priority,
                                    vsf_swi_handler_t *handler, 
                                    void *param);

/*============================ IMPLEMENTATION ================================*/

WEAK(bswap_8)
uint_fast8_t bswap_8(uint_fast8_t value8)
{
    value8 = ((value8 >> 1) & 0x55) | ((value8 << 1) & 0xAA);
    value8 = ((value8 >> 2) & 0x33) | ((value8 << 2) & 0xCC);
    value8 = ((value8 >> 4) & 0x0F) | ((value8 << 4) & 0xF0);
    return value8;
}

WEAK(bswap_16)
uint_fast16_t bswap_16(uint_fast16_t value16)
{
    return ((uint_fast16_t)bswap_8(value16) << 8) | (uint_fast16_t)bswap_8(value16 >> 8);
}

WEAK(bswap_32)
uint_fast32_t bswap_32(uint_fast32_t value32)
{
    return ((uint_fast32_t)bswap_16(value32) << 16) | (uint_fast32_t)bswap_16(value32 >> 16);
}

#ifdef UINT64_MAX
WEAK(bswap_64)
uint_fast64_t bswap_64(uint_fast64_t value64)
{
    return (bswap_32(value64) << 16) | bswap_32(value64 >> 16);
}
#endif

IMPLEMENT_ENDIAN_FUNC(16)
IMPLEMENT_ENDIAN_FUNC(32)

#ifdef UINT64_MAX
IMPLEMENT_ENDIAN_FUNC(64)
#endif

WEAK(vsf_drv_usr_swi_trigger)
void vsf_drv_usr_swi_trigger(uint_fast8_t idx)
{
    VSF_HAL_ASSERT(false);
}


#if __IS_COMPILER_ARM_COMPILER_5__
#warning  Due to a known issue of armcc v5.06 (build 750), the code generation of\
 the following function is wrong. Please do not use armcc to compile vsf kernel\
 code.
/* The wrong code generation example: 
0x00002964 B510      PUSH     {r4,lr}
0x00002966 B110      CBZ      r0,0x0000296E
0x00002968 1E40      SUBS     r0,r0,#1
0x0000296A F7FFFC03  BL.W     vsf_drv_usr_swi_trigger (0x00002174)
0x0000296E E8BD4010  POP      {r4,lr}
0x00002972 F7FFBBA9  B.W      vsf_arch_swi_trigger (0x000020C8)
                 vsf_systick_clear_count:
0x00002976 F04F21E0  MOV      r1,#0xE000E000
0x0000297A 2000      MOVS     r0,#0x00
0x0000297C 6188      STR      r0,[r1,#0x18]
0x0000297E 4770      BX       lr
*/
 
#endif
WEAK(vsf_swi_trigger)
void vsf_swi_trigger(uint_fast8_t idx)
{
#if __VSF_HAL_SWI_NUM > 0 || !defined(__VSF_HAL_SWI_NUM)
#   if defined(VSF_ARCH_SWI_NUM) && VSF_ARCH_SWI_NUM > 0
    if (idx < VSF_ARCH_SWI_NUM) {
        vsf_arch_swi_trigger(idx);
        return;
    }
    idx -= VSF_ARCH_SWI_NUM;
#   endif

#   if (__VSF_HAL_SWI_NUM > VSF_ARCH_SWI_NUM) || !defined(__VSF_HAL_SWI_NUM)
    vsf_drv_usr_swi_trigger(idx);
#   else
    VSF_HAL_ASSERT(false);
#   endif
#else
    VSF_HAL_ASSERT(false);
#endif
}

WEAK(vsf_drv_usr_swi_init)
vsf_err_t vsf_drv_usr_swi_init(uint_fast8_t idx, 
                                    vsf_arch_prio_t priority,
                                    vsf_swi_handler_t *handler, 
                                    void *param)
{
    VSF_HAL_ASSERT(false);
    return VSF_ERR_FAIL;
}

WEAK(vsf_swi_init)
vsf_err_t vsf_swi_init(uint_fast8_t idx, 
                            vsf_arch_prio_t priority,
                            vsf_swi_handler_t *handler, 
                            void *param)
{
#if __VSF_HAL_SWI_NUM > 0 || !defined(__VSF_HAL_SWI_NUM)
#   if defined(VSF_ARCH_SWI_NUM) && VSF_ARCH_SWI_NUM > 0
    if (idx < VSF_ARCH_SWI_NUM) {
        return vsf_arch_swi_init(idx, priority, handler, param);
    }
    idx -= VSF_ARCH_SWI_NUM;
#   endif

#   if (__VSF_HAL_SWI_NUM > VSF_ARCH_SWI_NUM) || !defined(__VSF_HAL_SWI_NUM)
    return vsf_drv_usr_swi_init(idx, priority, handler, param);
#   else
    VSF_HAL_ASSERT(false);
    return VSF_ERR_FAIL;
#   endif
#else
    VSF_HAL_ASSERT(false);
    return VSF_ERR_FAIL;
#endif
}

/*----------------------------------------------------------------------------*
 * System Timer Common Logic                                                  *
 *----------------------------------------------------------------------------*/

#ifndef WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR
WEAK(vsf_arch_req___systimer_freq___from_usr)
uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void)
{
    return VSF_GET_MAIN_CLK();
}
#endif

#ifndef WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR
WEAK(vsf_arch_req___systimer_resolution___from_usr)
uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void)
{
    return 1000000ul;
}
#endif

#ifndef WEAK_VSF_SYSTIMER_EVTHANDLER
WEAK(vsf_systimer_evthandler)
void vsf_systimer_evthandler(vsf_systimer_cnt_t tick)
{
    VSF_HAL_ASSERT(false);
}
#endif

WEAK(on_arch_systimer_tick_evt)
bool on_arch_systimer_tick_evt(vsf_systimer_cnt_t tick)
{
    UNUSED_PARAM(tick);
    return true;
}

static vsf_systimer_cnt_t __vsf_systimer_update(void)
{
    vsf_systimer_cnt_t tick;
   
    tick = vsf_systimer_get();
    __systimer.tick = tick;
    return tick;
}

static bool __vsf_systimer_set_target(vsf_systimer_cnt_t tick_cnt)
{
    if (0 == tick_cnt) {
        return false;
    }
    tick_cnt *= __systimer.unit;
    
    vsf_systimer_low_level_disable();
    
    __systimer.reload = tick_cnt;
    vsf_systimer_set_reload_value(tick_cnt);
    
    vsf_systimer_reset_counter_value();
    vsf_systimer_clear_int_pending_bit();
    
    vsf_systimer_low_level_int_enable();
    vsf_systimer_low_level_enable();
    return true;
}

/*! \brief systimer overflow event handler which is called by target timer 
 *!        interrupt handler
 */
void vsf_systimer_ovf_evt_hanlder(void)
{
    vsf_systimer_cnt_t tick;
    vsf_systimer_low_level_int_disable();
    __vsf_systimer_update();
    tick = __systimer.tick;
    vsf_systimer_low_level_disable();
    
    if (on_arch_systimer_tick_evt(tick)) {
#ifndef WEAK_VSF_SYSTIMER_EVTHANDLER
        vsf_systimer_evthandler(tick);
#else
        WEAK_VSF_SYSTIMER_EVTHANDLER(tick);
#endif
    }
}



/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target tick frequency in Hz
 *! \return initialization result in vsf_err_t 
 */
WEAK(vsf_systimer_init)
vsf_err_t vsf_systimer_init(void)
{
    //! calculate the cycle count of 1 tick
#ifndef WEAK_VSF_ARCH_REQ__SYSTIMER_RESOLUTION__FROM_USR
    uint_fast32_t tick_res = vsf_arch_req___systimer_resolution___from_usr();
#else
    uint_fast32_t tick_res = WEAK_VSF_ARCH_REQ__SYSTIMER_RESOLUTION__FROM_USR();
#endif
#ifndef WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR
    __systimer.unit = vsf_arch_req___systimer_freq___from_usr() / tick_res;
#else
    __systimer.unit = WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR() / tick_res;
#endif
    __systimer.tick_freq = tick_res;
    __systimer.max_tick_per_round = 
        (((uintmax_t)1 << (__VSF_ARCH_SYSTIMER_BITS)) / __systimer.unit) - 1;

    return vsf_systimer_low_level_init(
        __systimer.max_tick_per_round * __systimer.unit);
}



WEAK(vsf_systimer_set_idle)
void vsf_systimer_set_idle(void)
{   
//    vsf_trace(VSF_TRACE_DEBUG, "systimer_idle\r\n");
    {
        vsf_gint_state_t gint_state = vsf_disable_interrupt(); 
        __vsf_systimer_update();
        __vsf_systimer_set_target(__systimer.max_tick_per_round);
        vsf_set_interrupt(gint_state);
    }
}

WEAK(vsf_systimer_get)
vsf_systimer_cnt_t vsf_systimer_get(void)
{
    vsf_systimer_cnt_t ticks = 0;
    bool auto_update = false;
    {
        vsf_gint_state_t gint_state = vsf_disable_interrupt(); 
        if (vsf_systimer_low_level_disable()) {       //!< the match bit will be cleared
            ticks += __systimer.reload;
            auto_update = true;
        }
        ticks += vsf_systimer_get_tick_elapsed();
        vsf_systimer_low_level_enable();
        ticks /= __systimer.unit;
        ticks += __systimer.tick;
        if (auto_update) {
            __systimer.tick = ticks;
        }
        vsf_set_interrupt(gint_state);
    }
    return ticks;
}

WEAK(vsf_systimer_start)
vsf_err_t vsf_systimer_start(void)
{
    {
        vsf_gint_state_t gint_state = vsf_disable_interrupt(); 
        __vsf_systimer_set_target(__systimer.max_tick_per_round);
        vsf_set_interrupt(gint_state);
    }
    return VSF_ERR_NONE;
}


WEAK(vsf_systimer_set)
bool vsf_systimer_set(vsf_systimer_cnt_t due)
{
    bool result = false;
    //vsf_systimer_cnt_t unit = __vsf_cm.systimer.unit;
    vsf_systimer_cnt_t max_tick_per_round = __systimer.max_tick_per_round;

    {
        vsf_gint_state_t gint_state = vsf_disable_interrupt(); 
        vsf_systimer_cnt_t current = __vsf_systimer_update();
        //vsf_systick_disable();
        vsf_systimer_cnt_t tick_cnt;
//        vsf_trace(VSF_TRACE_DEBUG, "systimer_set: %lld %lld %c\r\n",
//                    current, due, due > current ? '*' : ' ');
        /*
        if (due < current) {
            tick_cnt = 0xFFFFFFFF - current + due + 1;
        } else {
            tick_cnt = due - current;
        }
        */
        if (due > current) {
            tick_cnt = due - current;
            tick_cnt = min(max_tick_per_round, tick_cnt);
            result = __vsf_systimer_set_target(tick_cnt);
        }
        vsf_set_interrupt(gint_state);
    }
    
    return result;
}

WEAK(vsf_systimer_is_due)
bool vsf_systimer_is_due(vsf_systimer_cnt_t due)
{
    return (__systimer.tick >= due);
}



WEAK(vsf_systimer_us_to_tick)
vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    return ((uintmax_t)  ((uintmax_t)time_us 
                            * (uintmax_t)__systimer.tick_freq) 
                        / 1000000ul);
}

WEAK(vsf_systimer_ms_to_tick)
vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    return ((uintmax_t)  ((uintmax_t)time_ms 
                            * (uintmax_t)__systimer.tick_freq) 
                        / 1000ul);
}

WEAK(vsf_systimer_tick_to_us)
uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick)
{
    return tick * 1000000ul / __systimer.tick_freq;
}

WEAK(vsf_systimer_tick_to_ms)
uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick)
{
    return vsf_systimer_tick_to_us(tick) / 1000;
}

#ifndef WEAK_VSF_ARCH_INIT
/*! \note initialize architecture specific service 
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
WEAK(vsf_arch_init)
bool vsf_arch_init(void)
{
    vsf_arch_low_level_init();
    vsf_systimer_init();
    return true;
}
#endif

/* EOF */
