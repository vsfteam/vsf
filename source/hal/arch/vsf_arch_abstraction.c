/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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
uint_fast##__bitlen##_t cpu_to_le##__bitlen##p(uint##__bitlen##_t *p)           \
{                                                                               \
    return cpu_to_le##__bitlen(*p);                                             \
}                                                                               \
WEAK(cpu_to_be##__bitlen##p)                                                    \
uint_fast##__bitlen##_t cpu_to_be##__bitlen##p(uint##__bitlen##_t *p)           \
{                                                                               \
    return cpu_to_be##__bitlen(*p);                                             \
}                                                                               \
WEAK(le##__bitlen##_to_cpup)                                                    \
uint_fast##__bitlen##_t le##__bitlen##_to_cpup(uint##__bitlen##_t *p)           \
{                                                                               \
    return le##__bitlen##_to_cpu(*p);                                           \
}                                                                               \
WEAK(be##__bitlen##_to_cpup)                                                    \
uint_fast##__bitlen##_t be##__bitlen##_to_cpup(uint##__bitlen##_t *p)           \
{                                                                               \
    return be##__bitlen##_to_cpu(*p);                                           \
}                                                                               \
WEAK(cpu_to_le##__bitlen##s)                                                    \
void cpu_to_le##__bitlen##s(uint##__bitlen##_t *p)                              \
{                                                                               \
    *p = cpu_to_le##__bitlen(*p);                                               \
}                                                                               \
WEAK(cpu_to_be##__bitlen##s)                                                    \
void cpu_to_be##__bitlen##s(uint##__bitlen##_t *p)                              \
{                                                                               \
    *p = cpu_to_be##__bitlen(*p);                                               \
}                                                                               \
WEAK(le##__bitlen##_to_cpus)                                                    \
void le##__bitlen##_to_cpus(uint##__bitlen##_t *p)                              \
{                                                                               \
    *p = le##__bitlen##_to_cpu(*p);                                             \
}                                                                               \
WEAK(be##__bitlen##_to_cpus)                                                    \
void be##__bitlen##_to_cpus(uint##__bitlen##_t *p)                              \
{                                                                               \
    *p = be##__bitlen##_to_cpu(*p);                                             \
}                                                                               \
WEAK(get_unaligned_cpu##__bitlen)                                               \
uint_fast##__bitlen##_t get_unaligned_cpu##__bitlen(const void *p)              \
{                                                                               \
    struct PACKED __packed_##__bitlen_t {                                       \
        uint##__bitlen##_t __v;                                                 \
    } *__p = (struct __packed_##__bitlen_t *)p;                                 \
    return __p->__v;                                                            \
}                                                                               \
WEAK(get_unaligned_le##__bitlen)                                                \
uint_fast##__bitlen##_t get_unaligned_le##__bitlen(const void *p)               \
{                                                                               \
    return cpu_to_le##__bitlen(get_unaligned_cpu##__bitlen(p));                 \
}                                                                               \
WEAK(get_unaligned_be##__bitlen)                                                \
uint_fast##__bitlen##_t get_unaligned_be##__bitlen(const void *p)               \
{                                                                               \
    return cpu_to_be##__bitlen(get_unaligned_cpu##__bitlen(p));                 \
}                                                                               \
WEAK(put_unaligned_cpu##__bitlen)                                               \
void put_unaligned_cpu##__bitlen(uint_fast##__bitlen##_t val, void *p)          \
{                                                                               \
    struct PACKED __packed_##__bitlen_t {                                       \
        uint##__bitlen##_t __v;                                                 \
    } *__p = (struct __packed_##__bitlen_t *)p;                                 \
    __p->__v = val;                                                             \
}                                                                               \
WEAK(put_unaligned_le##__bitlen)                                                \
void put_unaligned_le##__bitlen(uint_fast##__bitlen##_t val, void *p)           \
{                                                                               \
    put_unaligned_cpu##__bitlen(cpu_to_le##__bitlen(val), p);                   \
}                                                                               \
WEAK(put_unaligned_be##__bitlen)                                                \
void put_unaligned_be##__bitlen(uint_fast##__bitlen##_t val, void *p)           \
{                                                                               \
    put_unaligned_cpu##__bitlen(cpu_to_be##__bitlen(val), p);                   \
}


#if     VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER       \
    ||  VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_COMP_TIMER

#   ifndef __VSF_ARCH_SYSTIMER_BITS
#       error [DEPENDENCY ERROR] Architecture specific header files should provide\
 number of bits used for systimer via macro __VSF_ARCH_SYSTIMER_BITS
#   endif
#endif


#ifndef VSF_GET_MAIN_CLK
#   define VSF_GET_MAIN_CLK()         (0)
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
#if     VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER       \
    ||  VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_COMP_TIMER
typedef struct __systimer_t {
    vsf_systimer_tick_t tick;
    vsf_systimer_tick_t unit;
    vsf_systimer_tick_t max_tick_per_round;
    vsf_systimer_tick_t reload;
    //uint32_t           tick_freq;
} __systimer_t;
#elif  VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_TICK_MODE
typedef struct __systimer_t {
    vsf_systimer_tick_t tick;
    vsf_systimer_tick_t cycle_per_tick;
    vsf_systimer_tick_t cycle_per_us;
} __systimer_t;
#endif




/*============================ PROTOTYPES ====================================*/

#ifdef VSF_SYSTIMER_CFG_IMPL_MODE
extern void vsf_systimer_evthandler(vsf_systimer_tick_t tick);
extern bool on_arch_systimer_tick_evt(vsf_systimer_tick_t tick);

#   if  VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER       \
    ||  VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_COMP_TIMER
extern uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void);
#   endif
#   if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_TICK_MODE
extern uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void);
extern uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void);
#   endif
#endif

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
extern vsf_err_t vsf_arch_swi_init( uint_fast8_t idx,
                                    vsf_arch_prio_t priority,
                                    vsf_swi_handler_t *handler,
                                    void *param);

static vsf_protect_t __vsf_protect_region_int_enter(void);
static void __vsf_protect_region_int_leave(vsf_protect_t orig);
static vsf_protect_t __vsf_protect_region_none_enter(void);
static void __vsf_protect_region_none_leave(vsf_protect_t orig);

/*============================ LOCAL VARIABLES ===============================*/

#if     VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER       \
    ||  VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_COMP_TIMER         \
    ||  VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_TICK_MODE
static volatile __systimer_t __systimer;
#endif


/*============================ GLOBAL VARIABLES ==============================*/

const vsf_protect_region_t vsf_protect_region_int = {
    .enter  = __vsf_protect_region_int_enter,
    .leave  = __vsf_protect_region_int_leave,
};

const vsf_protect_region_t vsf_protect_region_none = {
    .enter  = __vsf_protect_region_none_enter,
    .leave  = __vsf_protect_region_none_leave,
};

/*============================ IMPLEMENTATION ================================*/

static vsf_protect_t __vsf_protect_region_int_enter(void)
{
    return vsf_protect_int();
}

static void __vsf_protect_region_int_leave(vsf_protect_t orig)
{
    vsf_unprotect_int(orig);
}

static vsf_protect_t __vsf_protect_region_none_enter(void)
{
    return vsf_protect_none();
}

static void __vsf_protect_region_none_leave(vsf_protect_t orig)
{
    vsf_unprotect_none(orig);
}

/*----------------------------------------------------------------------------*
 * Architecture Infrastructure                                                *
 *----------------------------------------------------------------------------*/
WEAK(bswap_16)
uint_fast16_t bswap_16(uint_fast16_t value16)
{
    return ((uint_fast16_t)(value16 & 0x00FF) << 8) | ((uint_fast16_t)(value16 & 0xFF00) >> 8);
}

WEAK(bswap_32)
uint_fast32_t bswap_32(uint_fast32_t value32)
{
    return ((uint_fast32_t)bswap_16((uint_fast16_t)value32) << 16) | (uint_fast32_t)bswap_16((uint_fast16_t)(value32 >> 16));
}

#ifdef UINT64_MAX
WEAK(bswap_64)
uint_fast64_t bswap_64(uint_fast64_t value64)
{
    return ((uint_fast64_t)bswap_32((uint_fast32_t)value64) << 32) | (uint_fast64_t)bswap_32((uint_fast32_t)(value64 >> 32));
}
#endif

IMPLEMENT_ENDIAN_FUNC(16)
IMPLEMENT_ENDIAN_FUNC(32)

#ifdef UINT64_MAX
IMPLEMENT_ENDIAN_FUNC(64)
#endif


#ifndef __VSF_ARCH_CLZ
WEAK(__vsf_arch_clz)
uint_fast8_t __vsf_arch_clz(uintalu_t a)
{
    uint_fast8_t num = __optimal_bit_sz;
    uintalu_t bitmask = ((uintalu_t)1 << (__optimal_bit_sz - 1));
    do {
        if (a & bitmask) {
            break;
        }
        a <<= 1;
    } while(--num);

    return __optimal_bit_sz - num;
}
#endif


#ifndef __VSF_ARCH_MSB
WEAK(__vsf_arch_msb)
int_fast8_t __vsf_arch_msb(uintalu_t a)
{
    int_fast8_t c = -1;
    while (a > 0) {
        c++;
        a >>= 1;
    }
    return c;
}
#endif

#ifndef __VSF_ARCH_FFS
WEAK(__vsf_arch_ffs)
int_fast8_t __vsf_arch_ffs(uintalu_t a)
{
#   ifndef __VSF_ARCH_MSB
    return __vsf_arch_msb(a & -(uintalu_t)a);
#else
    return __VSF_ARCH_MSB(a & -(uintalu_t)a);
#endif
}
#endif

#ifndef __VSF_ARCH_FFZ
WEAK(__vsf_arch_ffz)
int_fast8_t __vsf_arch_ffz(uintalu_t a)
{
#   ifndef __VSF_ARCH_FFS
    return __vsf_arch_ffs(~a);
#else
    return __VSF_ARCH_FFS(~a);
#endif
}
#endif

#ifndef VSF_CLZ32
WEAK(vsf_clz32)
uint_fast8_t vsf_clz32(uint_fast32_t a)
{
    uint_fast8_t num = 0;
    if (__optimal_bit_sz >= 32) {
#ifndef __VSF_ARCH_CLZ
        num = __vsf_arch_clz((uintalu_t)a);
#else
        num = __VSF_ARCH_CLZ((uintalu_t)a);
#endif
        VSF_ARCH_ASSERT(num >= (__optimal_bit_sz - 32));
        return num - (__optimal_bit_sz - 32);
    } else {
        int_fast8_t word_size = (32 + __optimal_bit_sz - 1) / __optimal_bit_sz;
        uint_fast8_t temp;
        uintalu_t* src = (uintalu_t*)&a + (word_size - 1);

        do {
#ifndef __VSF_ARCH_CLZ
            temp = __vsf_arch_clz(*src--);
#else
            temp = __VSF_ARCH_CLZ(*src--);
#endif
            if (temp < __optimal_bit_sz) {
                num += temp;
                break;
            }
            num += __optimal_bit_sz;
        } while (--word_size);
    }
    return num;
}
#endif

#ifndef VSF_MSB32
WEAK(vsf_msb32)
int_fast8_t vsf_msb32(uint_fast32_t a)
{
    if (__optimal_bit_sz >= 32) {
#ifndef __VSF_ARCH_MSB
        int_fast8_t index = __vsf_arch_msb(a);
#else
        int_fast8_t index = __VSF_ARCH_MSB(a);
#endif
        VSF_ARCH_ASSERT(index < 32);
        return index;
    } else {
        int_fast8_t word_size = (32 + __optimal_bit_sz - 1) / __optimal_bit_sz;
        int_fast8_t index = 31, temp;
        uintalu_t* src = (uintalu_t*)&a + (word_size - 1);

        do {
#ifndef __VSF_ARCH_MSB
            temp = __vsf_arch_msb(*src--);
#else
            temp = __VSF_ARCH_MSB(*src--);
#endif
            index -= __optimal_bit_sz;
            if (temp >= 0) {
                index += temp+1;
                break;
            }
        } while(--word_size);
        return index;
    }
}
#endif

#ifndef VSF_FFS32
WEAK(vsf_ffs32)
int_fast8_t vsf_ffs32(uint_fast32_t a)
{
    if (__optimal_bit_sz >= 32) {
#ifndef __VSF_ARCH_FFS
        int_fast8_t temp = __vsf_arch_ffs(a);
#else
        int_fast8_t temp = __VSF_ARCH_FFs(a);
#endif
        VSF_ARCH_ASSERT(temp < 32);
        return temp;
    } else {
        int_fast16_t word_size = (32 + __optimal_bit_sz - 1) / __optimal_bit_sz;
        int_fast16_t index = 0, temp;
        uintalu_t* src = (uintalu_t*)&a;

        do {
#ifndef __VSF_ARCH_FFS
            temp = __vsf_arch_ffs(*src++);
#else
            temp = __VSF_ARCH_FFS(*src++);
#endif
            if (temp >= 0) {
                index += temp;
                return index;
            }
            index += __optimal_bit_sz;
        } while (--word_size);

        return -1;
    }
}
#endif

#ifndef VSF_FFZ32
WEAK(vsf_ffz32)
int_fast8_t vsf_ffz32(uint_fast32_t a)
{
#   ifndef VSF_FFS32
    return vsf_ffs32(~a);
#   else
    return VSF_FFS32(~a);
#   endif
}
#endif

/*----------------------------------------------------------------------------*
 * SWI                                                                        *
 *----------------------------------------------------------------------------*/

WEAK(vsf_drv_usr_swi_trigger)
void vsf_drv_usr_swi_trigger(uint_fast8_t idx)
{
    VSF_UNUSED_PARAM(idx);
    VSF_ARCH_ASSERT(false);
}

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
    VSF_ARCH_ASSERT(false);
#   endif
#else
    VSF_ARCH_ASSERT(false);
#endif
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

WEAK(vsf_drv_usr_swi_init)
vsf_err_t vsf_drv_usr_swi_init(     uint_fast8_t idx,
                                    vsf_arch_prio_t priority,
                                    vsf_swi_handler_t *handler,
                                    void *param)
{
    VSF_UNUSED_PARAM(idx);
    VSF_UNUSED_PARAM(priority);
    VSF_UNUSED_PARAM(handler);
    VSF_UNUSED_PARAM(param);

    VSF_ARCH_ASSERT(false);
    return VSF_ERR_FAIL;
}

WEAK(vsf_swi_init)
vsf_err_t vsf_swi_init(     uint_fast8_t idx,
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
    VSF_ARCH_ASSERT(false);
    return VSF_ERR_FAIL;
#   endif
#else
    VSF_ARCH_ASSERT(false);
    return VSF_ERR_FAIL;
#endif
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

/*----------------------------------------------------------------------------*
 * System Timer                                                               *
 *----------------------------------------------------------------------------*/

#ifdef VSF_SYSTIMER_CFG_IMPL_MODE
#   ifndef WEAK_VSF_SYSTIMER_EVTHANDLER
WEAK(vsf_systimer_evthandler)
void vsf_systimer_evthandler(vsf_systimer_tick_t tick)
{
    VSF_UNUSED_PARAM(tick);
    VSF_ARCH_ASSERT(false);
}
#   endif

#   ifndef WEAK_ON_ARCH_SYSTIMER_TICK_EVT
WEAK(on_arch_systimer_tick_evt)
bool on_arch_systimer_tick_evt(vsf_systimer_tick_t tick)
{
    VSF_UNUSED_PARAM(tick);
    return true;
}
#endif



#if     VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER       \
    ||  VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_COMP_TIMER         

#   ifndef WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR
WEAK(vsf_arch_req___systimer_freq___from_usr)
uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void)
{
    return VSF_GET_MAIN_CLK();
}
#   endif


WEAK(vsf_systimer_us_to_tick)
vsf_systimer_tick_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    return ((vsf_systimer_tick_t)
                (vsf_systimer_tick_t)time_us *
                (vsf_systimer_tick_t)__systimer.unit);
}

WEAK(vsf_systimer_ms_to_tick)
vsf_systimer_tick_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    return ((vsf_systimer_tick_t)(
                (vsf_systimer_tick_t)time_ms *
                (vsf_systimer_tick_t)__systimer.unit * 1000));
}

WEAK(vsf_systimer_tick_to_us)
vsf_systimer_tick_t vsf_systimer_tick_to_us(vsf_systimer_tick_t tick)
{
    return tick / __systimer.unit;
}

WEAK(vsf_systimer_tick_to_ms)
vsf_systimer_tick_t vsf_systimer_tick_to_ms(vsf_systimer_tick_t tick)
{
    return vsf_systimer_tick_to_us(tick) / 1000;
}

#endif

/*----------------------------------------------------------------------------*
 * System Timer : Implement tickless mode with Normal Timer (Count down or    *
 *                Count up)                                                   *
 *----------------------------------------------------------------------------*/
#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
static vsf_systimer_tick_t __vsf_systimer_update(void)
{
    vsf_systimer_tick_t tick;

    tick = vsf_systimer_get();
    __systimer.tick = tick;
    return tick;
}

static bool __vsf_systimer_set_target(vsf_systimer_tick_t tick_cnt)
{
    if (0 == tick_cnt) {
        return false;
    }

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
    vsf_systimer_tick_t tick;
    vsf_systimer_low_level_int_disable();
    __vsf_systimer_update();
    tick = __systimer.tick;
    vsf_systimer_low_level_disable();

    if (on_arch_systimer_tick_evt(tick)) {
        vsf_systimer_evthandler(tick);
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
    __systimer.unit = vsf_arch_req___systimer_freq___from_usr() / 1000000ul;
    if (0 == __systimer.unit) {
        /*! assume that systimer frequency is at least 1MHz */
        __systimer.unit = 1;
    }

    //__systimer.tick_freq = tick_res;
    __systimer.max_tick_per_round = ((uintmax_t)1 << __VSF_ARCH_SYSTIMER_BITS) - 1;

    return vsf_systimer_low_level_init(__systimer.max_tick_per_round);
}

WEAK(vsf_systimer_get_freq)
uint32_t vsf_systimer_get_freq(void)
{
    return vsf_arch_req___systimer_freq___from_usr();
}

WEAK(vsf_systimer_set_idle)
void vsf_systimer_set_idle(void)
{
//    vsf_trace_debug("systimer_idle\r\n");
    {
        vsf_arch_prio_t gint_state = vsf_disable_interrupt();
        __vsf_systimer_update();
        __vsf_systimer_set_target(__systimer.max_tick_per_round);
        vsf_set_interrupt(gint_state);
    }
}

WEAK(vsf_systimer_get)
vsf_systimer_tick_t vsf_systimer_get(void)
{
    vsf_systimer_tick_t ticks = 0;
    bool auto_update = false;
    {
        vsf_arch_prio_t gint_state = vsf_disable_interrupt();
        if (vsf_systimer_low_level_disable()) {       //!< the match bit will be cleared
            ticks += __systimer.reload;
            auto_update = true;
        }
        ticks += vsf_systimer_get_tick_elapsed();
        vsf_systimer_low_level_enable();
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
        vsf_arch_prio_t gint_state = vsf_disable_interrupt();
        __vsf_systimer_set_target(__systimer.max_tick_per_round);
        vsf_set_interrupt(gint_state);
    }
    return VSF_ERR_NONE;
}


WEAK(vsf_systimer_set)
bool vsf_systimer_set(vsf_systimer_tick_t due)
{
    bool result = false;
    //vsf_systimer_tick_t unit = __vsf_cm.systimer.unit;
    vsf_systimer_tick_t max_tick_per_round = __systimer.max_tick_per_round;

    {
        vsf_arch_prio_t gint_state = vsf_disable_interrupt();
        vsf_systimer_tick_t current = __vsf_systimer_update();
        //vsf_systick_disable();
        vsf_systimer_tick_t tick_cnt;
//        vsf_trace_debug("systimer_set: %lld %lld %c\r\n",
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
            tick_cnt = vsf_min(max_tick_per_round, tick_cnt);
            result = __vsf_systimer_set_target(tick_cnt);
        }
        vsf_set_interrupt(gint_state);
    }

    return result;
}

WEAK(vsf_systimer_is_due)
bool vsf_systimer_is_due(vsf_systimer_tick_t due)
{
    return (__systimer.tick >= due);
}

#endif

/*----------------------------------------------------------------------------*
 * System Timer : Implement tickless mode with a timer supporting compare     *
 *                match                                                       *
 *----------------------------------------------------------------------------*/
#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_COMP_TIMER
//! todo
#endif


/*----------------------------------------------------------------------------*
 * System Timer : implement tick mode                                         *
 *----------------------------------------------------------------------------*/
#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_TICK_MODE


#   ifndef WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR
WEAK(vsf_arch_req___systimer_freq___from_usr)
uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void)
{
    return VSF_GET_MAIN_CLK();
}
#   endif



#   ifndef WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR
WEAK(vsf_arch_req___systimer_resolution___from_usr)
uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void)
{
    return 1000ul;          //!< 1ms (1KHz)
}
#   endif


WEAK(vsf_systimer_us_to_tick)
vsf_systimer_tick_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    vsf_systimer_tick_t tick = (vsf_systimer_tick_t)time_us
                             *  __systimer.cycle_per_us;
    tick /= __systimer.cycle_per_tick;
    tick = vsf_max(1, tick);
    
    return tick;
}

WEAK(vsf_systimer_ms_to_tick)
vsf_systimer_tick_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    vsf_systimer_tick_t tick = (vsf_systimer_tick_t)time_ms
                             *  __systimer.cycle_per_us * 1000ul;
    tick /= __systimer.cycle_per_tick;
    tick = vsf_max(1, tick);
    
    return tick;
}

WEAK(vsf_systimer_tick_to_us)
vsf_systimer_tick_t vsf_systimer_tick_to_us(vsf_systimer_tick_t tick)
{
    tick *= __systimer.cycle_per_tick;
    tick /= __systimer.cycle_per_us;
    return tick;
}

WEAK(vsf_systimer_tick_to_ms)
vsf_systimer_tick_t vsf_systimer_tick_to_ms(vsf_systimer_tick_t tick)
{
    return vsf_systimer_tick_to_us(tick) / 1000;
}

/*! \brief systimer overflow event handler which is called by target timer
 *!        interrupt handler
 */
void vsf_systimer_ovf_evt_hanlder(void)
{
    __systimer.tick ++;
    if (on_arch_systimer_tick_evt(__systimer.tick)) {
        vsf_systimer_evthandler(__systimer.tick);
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
    vsf_systimer_tick_t clock_src_freq 
        = vsf_arch_req___systimer_freq___from_usr();
    vsf_systimer_tick_t tick_freq 
        = vsf_arch_req___systimer_resolution___from_usr();
    
    VSF_ARCH_ASSERT( clock_src_freq > 0 );
    VSF_ARCH_ASSERT( tick_freq > 0 );
    
    __systimer.cycle_per_tick = clock_src_freq / tick_freq;

    
    if (0 == __systimer.cycle_per_tick) {
        __systimer.cycle_per_tick = 1;
    }
    
    __systimer.cycle_per_us = clock_src_freq / 1000000ul;
    if (0 == __systimer.cycle_per_us) {
        __systimer.cycle_per_us = 1;
    }

    return vsf_systimer_low_level_init(__systimer.cycle_per_tick);
}

WEAK(vsf_systimer_get_freq)
uint32_t vsf_systimer_get_freq(void)
{
    return vsf_arch_req___systimer_resolution___from_usr();
}

WEAK(vsf_systimer_set_idle)
void vsf_systimer_set_idle(void)
{
//    vsf_trace_debug("systimer_idle\r\n");
}

WEAK(vsf_systimer_get)
vsf_systimer_tick_t vsf_systimer_get(void)
{
    return __systimer.tick;
}

WEAK(vsf_systimer_start)
vsf_err_t vsf_systimer_start(void)
{
    {
        vsf_arch_prio_t gint_state = vsf_disable_interrupt();
            vsf_systimer_low_level_disable();

            vsf_systimer_reset_counter_value();
            vsf_systimer_clear_int_pending_bit();

            vsf_systimer_low_level_int_enable();
            vsf_systimer_low_level_enable();
        vsf_set_interrupt(gint_state);
    }
    return VSF_ERR_NONE;
}


WEAK(vsf_systimer_set)
bool vsf_systimer_set(vsf_systimer_tick_t due)
{
    return true;
}

WEAK(vsf_systimer_is_due)
bool vsf_systimer_is_due(vsf_systimer_tick_t due)
{
    return (__systimer.tick >= due);
}

#endif

/*----------------------------------------------------------------------------*
 * System Timer : Implement with request / response model                     *
 *----------------------------------------------------------------------------*/
#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_REQUEST_RESPONSE

/*! \brief systimer timeout event handler which is called by request response
 *!        service.
 */
void vsf_systimer_timeout_evt_hanlder(vsf_systimer_tick_t tick)
{
    if (on_arch_systimer_tick_evt(tick)) {
        vsf_systimer_evthandler(tick);
    }
}

WEAK(vsf_systimer_get_freq)
uint32_t vsf_systimer_get_freq(void)
{
#ifdef VSF_ARCH_SYSTIMER_FREQ
    return VSF_ARCH_SYSTIMER_FREQ;
#else
    return vsf_arch_req___systimer_freq___from_usr();
#endif
}

#endif
#endif      // VSF_SYSTIMER_CFG_IMPL_MODE

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
#   ifdef VSF_SYSTIMER_CFG_IMPL_MODE
    vsf_systimer_init();
#   endif
    return true;
}
#endif

/* EOF */
