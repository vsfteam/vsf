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
#include "./spi.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "kernel/vsf_kernel.h"
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

#ifndef VSF_MULTI_SPI_PRIO
#   define VSF_MULTI_SPI_PRIO           vsf_prio_0
#endif

#ifndef VSF_MULTI_SPI_PROTECT_LEVEL
#   define VSF_MULTI_SPI_PROTECT_LEVEL  interrupt
#endif

#ifndef VSF_MULTI_SPI_MAX_CS
#   define VSF_MULTI_SPI_MAX_CS         8
#endif

#define vsf_multi_spi_protect           vsf_protect(VSF_MULTI_SPI_PROTECT_LEVEL)
#define vsf_multi_spi_unprotect         vsf_unprotect(VSF_MULTI_SPI_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_multi_spi_low_init(vsf_spi_cs_t* spi_cs_ptr,
                                          uint8_t cs,
                                          i_spi_cfg_t *cfg_ptr);

void vsf_multi_spi_cs_active(vsf_spi_cs_t* spi_cs_ptr,
                             uint8_t cs,
                             uint_fast8_t index);

void vsf_multi_spi_cs_inactive(vsf_spi_cs_t* spi_cs_ptr,
                               uint8_t cs,
                               uint_fast8_t index);
/*============================ IMPLEMENTATION ================================*/

// TODO: support dync change prio

static void __vsf_spi_cfg_clean_auto_cs(spi_cfg_t * cfg_ptr)
{
    cfg_ptr->mode =   (cfg_ptr->mode & ~SPI_AUTO_SLAVE_SELECTION_MSK)
                    | SPI_AUTO_SLAVE_SELECTION_DISABLE;
}

static void __vsf_spi_request_transfer_push(vsf_spi_cs_t* spi_cs_ptr,
                                            vsf_spi_cs_request_t * request)
{
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(request != NULL);
    vsf_dlist_add_to_head(vsf_spi_cs_request_t, node, &spi_cs_ptr->list, request);
}

static void __vsf_multi_spi_irq_handler(void *target_ptr,
                                        vsf_spi_t *spi_ptr,
                                        em_spi_irq_mask_t irq_mask)
{
    vsf_spi_cs_t *spi_cs_ptr = (vsf_spi_cs_t *)target_ptr;
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->cur_cs != -1);

    vsf_spi_cs_one_t *spi_cs_one = &spi_cs_ptr->cfgs[spi_cs_ptr->cur_cs];
    vsf_i_spi_isr_t *isr_ptr = &spi_cs_one->spi_cfg.isr;

    em_spi_irq_mask_t one_irq_mask = irq_mask & spi_cs_one->irq_mask;

    if ((isr_ptr->handler_fn != NULL) && (one_irq_mask != 0)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, spi_cs_one->i_spi, one_irq_mask);
    }
}

static vsf_err_t __vsf_multi_spi_low_init(vsf_spi_cs_t* spi_cs_ptr,
                                          uint8_t cs,
                                          i_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);
    VSF_HAL_ASSERT(cfg_ptr != 0);

    spi_cfg_t local_cfg;
    local_cfg.mode = cfg_ptr->mode;
    local_cfg.clock_hz = cfg_ptr->clock_hz;
    local_cfg.isr.handler_fn = __vsf_multi_spi_irq_handler;
    local_cfg.isr.target_ptr = spi_cs_ptr;

    if (cs > 0) {       // TODO: support multi hardware cs pin
        __vsf_spi_cfg_clean_auto_cs(&local_cfg);
    }

    return vsf_spi_init(spi_cs_ptr->spi, &local_cfg);
}

static vsf_err_t __vsf_multi_spi_reconfig(vsf_spi_cs_t* spi_cs_ptr,
                                          uint8_t cs,
                                          i_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);
    VSF_HAL_ASSERT(cfg_ptr != 0);

    vsf_err_t result;
    while (fsm_rt_cpl != vsf_spi_disable(spi_cs_ptr->spi));
    result = __vsf_multi_spi_low_init(spi_cs_ptr, cs, cfg_ptr);
    while (fsm_rt_cpl != vsf_spi_enable(spi_cs_ptr->spi));

    return result;
}

static vsf_err_t __vsf_multi_spi_init(vsf_spi_cs_t* spi_cs_ptr,
                                      uint8_t cs,
                                      i_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);
    VSF_HAL_ASSERT(cfg_ptr != 0);

    spi_cs_ptr->cfgs[cs].spi_cfg = *cfg_ptr;
    vsf_err_t result;

    // Call __vsf_multi_spi_init() with different cs, only the first real call.
    // To avoid:
    // - reinitialize while spi iw working
    // - call other spi api first before call vsf_spi_init()
    // - priority safety

    vsf_protect_t state = vsf_multi_spi_protect();
    if (spi_cs_ptr->init_mask == 0) {
        spi_cs_ptr->init_mask = (1 << cs);
        result = __vsf_multi_spi_low_init(spi_cs_ptr, cs, cfg_ptr);
    } else {
        result = VSF_ERR_NONE;
    }
    vsf_multi_spi_unprotect(state);

    return result;
}

static fsm_rt_t __vsf_multi_spi_enable(vsf_spi_cs_t* spi_cs_ptr,
                                       uint8_t cs)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);

    vsf_protect_t state = vsf_multi_spi_protect();
    if (spi_cs_ptr->en_mask == 0) {
        // TODO: support fsm
        while (fsm_rt_cpl != vsf_spi_enable(spi_cs_ptr->spi));
    }
    spi_cs_ptr->en_mask |= (1 << cs);
    vsf_multi_spi_unprotect(state);

    return fsm_rt_cpl;
}

static fsm_rt_t __vsf_multi_spi_disable(vsf_spi_cs_t* spi_cs_ptr,
                                        uint8_t cs)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);

    uint8_t cs_msk = (1 << cs);

    vsf_protect_t state = vsf_multi_spi_protect();
    spi_cs_ptr->en_mask &= ~(1 << cs);
    if (spi_cs_ptr->en_mask == 0) {
        spi_cs_ptr->init_mask = 0;
        // TODO: support fsm
        while (fsm_rt_cpl != vsf_spi_disable(spi_cs_ptr->spi));
    }
    vsf_multi_spi_unprotect(state);

    return fsm_rt_cpl;
}

static void __vsf_multi_spi_irq_enable(vsf_spi_cs_t* spi_cs_ptr,
                                       uint8_t cs,
                                       em_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);

    vsf_spi_cs_one_t *spi_cs_one_ptr = &spi_cs_ptr->cfgs[cs];

    vsf_protect_t state = vsf_multi_spi_protect();
    spi_cs_one_ptr->irq_mask |= irq_mask;
    spi_cs_ptr->irq_mask     |= irq_mask;
    vsf_multi_spi_unprotect(state);

    vsf_spi_irq_enable(spi_cs_ptr->spi, irq_mask);
}

static void __vsf_multi_spi_irq_disable(vsf_spi_cs_t* spi_cs_ptr,
                                        uint8_t cs,
                                        em_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);

    vsf_protect_t state = vsf_multi_spi_protect();
    spi_cs_ptr->cfgs[cs].irq_mask &= ~irq_mask;
    vsf_multi_spi_unprotect(state);
}

static spi_status_t __vsf_multi_spi_status(vsf_spi_cs_t* spi_cs_ptr,
                                           uint8_t cs)
{
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);

    // TODO: make sure priority safety
    return vsf_spi_status(spi_cs_ptr->spi);
}

static void __vsf_multi_spi_fifo_transfer(vsf_spi_cs_t* spi_cs_ptr,
                                          uint8_t cs,
                                          void *out_buffer_ptr,
                                          uint_fast32_t* out_count_ptr,
                                          void *in_buffer_ptr,
                                          uint_fast32_t* in_count_ptr)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);

    // TODO
    VSF_HAL_ASSERT(0);
}

static bool __vsf_multi_spi_fifo_flush(vsf_spi_cs_t* spi_cs_ptr,
                                       uint8_t cs)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);

    return vsf_spi_fifo_flush(spi_cs_ptr->spi);
}

#ifndef WEAK_VSF_MULTI_SPI_CS_ACTIVE
WEAK(vsf_multi_spi_cs_active)
void vsf_multi_spi_cs_active(vsf_spi_cs_t* spi_cs_ptr,
                             uint8_t cs,
                             uint_fast8_t index)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);
    VSF_HAL_ASSERT(index == 0);

    vsf_spi_cs_active(spi_cs_ptr->spi, index);
}
#endif

static void __vsf_multi_spi_cs_active(vsf_spi_cs_t* spi_cs_ptr,
                                      uint8_t cs,
                                      uint_fast8_t index)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);
    VSF_HAL_ASSERT(index == 0);
    uint8_t cs_mask = (1 << cs);
    bool is_cs_need;

    vsf_protect_t state = vsf_multi_spi_protect();
    spi_cs_ptr->cs_mask |= cs_mask;
    if (spi_cs_ptr->cur_cs == -1) {
        is_cs_need = true;
        spi_cs_ptr->cur_cs = cs;
    } else {
        is_cs_need = false;
    }
    vsf_multi_spi_unprotect(state);

    if (!is_cs_need) {
        return ;
    }

    if (spi_cs_ptr->init_mask != (1 << cs)) {
        spi_cs_ptr->init_mask = (1 << cs);

        __vsf_multi_spi_reconfig(spi_cs_ptr,
                                 cs,
                                 &spi_cs_ptr->cfgs[cs].spi_cfg);
    }

    vsf_multi_spi_cs_active(spi_cs_ptr, cs, index);
}

#ifndef WEAK_VSF_MULTI_SPI_CS_INACTIVE
WEAK(vsf_multi_spi_cs_inactive)
void vsf_multi_spi_cs_inactive(vsf_spi_cs_t* spi_cs_ptr,
                               uint8_t cs,
                               uint_fast8_t index)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);
    VSF_HAL_ASSERT(index == 0);

    vsf_spi_cs_inactive(spi_cs_ptr->spi , index);
}
#endif

static void __vsf_multi_spi_cs_inactive(vsf_spi_cs_t* spi_cs_ptr,
                                        uint8_t cs,
                                        uint_fast8_t index)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);
    VSF_HAL_ASSERT(index == 0);
    uint8_t cs_mask = (1 << cs);
    bool is_cs_need;

    vsf_spi_cs_request_t *request_ptr = NULL;

    vsf_protect_t state = vsf_multi_spi_protect();
    spi_cs_ptr->cs_mask &= ~cs_mask;
    is_cs_need = (spi_cs_ptr->cur_cs == cs);
    if (!vsf_dlist_is_empty(&spi_cs_ptr->list)) {
        vsf_dlist_remove_tail(vsf_spi_cs_request_t, node,
                              &spi_cs_ptr->list, request_ptr);
        VSF_ASSERT(spi_cs_ptr->cs_mask & (1 << request_ptr->cs));
        spi_cs_ptr->init_mask = 1 << request_ptr->cs;
        spi_cs_ptr->cur_cs = request_ptr->cs;
    } else {
        spi_cs_ptr->cur_cs = -1;
    }
    vsf_multi_spi_unprotect(state);

    if (is_cs_need) {
        vsf_multi_spi_cs_inactive(spi_cs_ptr, cs, index);
    }

    if (request_ptr == NULL) {
        return ;
    }

    VSF_ASSERT(request_ptr->cs != cs);

    __vsf_multi_spi_reconfig(spi_cs_ptr,
                             request_ptr->cs,
                             &spi_cs_ptr->cfgs[request_ptr->cs].spi_cfg);

    vsf_multi_spi_cs_active(spi_cs_ptr, request_ptr->cs, index);

    vsf_err_t result = vsf_spi_request_transfer(spi_cs_ptr->spi,
                                                request_ptr->out_buffer_ptr,
                                                request_ptr->in_buffer_ptr,
                                                request_ptr->count);
    (void)result;
    VSF_HAL_ASSERT(result == VSF_ERR_NONE);
}

static vsf_err_t __vsf_multi_spi_request_transfer(vsf_spi_cs_t* spi_cs_ptr,
                                                  uint8_t cs,
                                                  void *out_buffer_ptr,
                                                  void *in_buffer_ptr,
                                                  uint_fast32_t count)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);

    // Before request, cs must be active
    VSF_HAL_ASSERT(spi_cs_ptr->cur_cs != -1);

    if (spi_cs_ptr->cur_cs == cs) {
        spi_cs_ptr->cfgs[cs].transfered_count = 0;
        return vsf_spi_request_transfer(spi_cs_ptr->spi,
                                        out_buffer_ptr,
                                        in_buffer_ptr,
                                        count);
    } else {
        vsf_spi_cs_request_t * request = &spi_cs_ptr->cfgs[cs].request;
        request->out_buffer_ptr = out_buffer_ptr;
        request->in_buffer_ptr  = in_buffer_ptr;
        request->count          = count;
        request->cs             = cs;

        vsf_protect_t state = vsf_multi_spi_protect();
        vsf_dlist_add_to_head(vsf_spi_cs_request_t,
                              node, &spi_cs_ptr->list, request);
        vsf_multi_spi_unprotect(state);
        return VSF_ERR_NONE;
    }
}

static vsf_err_t __vsf_multi_spi_cancel_transfer(vsf_spi_cs_t* spi_cs_ptr,
                                                 uint8_t cs)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);
    VSF_HAL_ASSERT(spi_cs_ptr->spi != NULL);

    vsf_err_t result;
    vsf_spi_cs_one_t *spi_cs_one_ptr = &spi_cs_ptr->cfgs[cs];

    vsf_protect_t state = vsf_multi_spi_protect();
    if (spi_cs_ptr->cur_cs == cs) { // current cs request
        spi_cs_ptr->cur_cs = -1;
        result = vsf_spi_cancel_transfer(spi_cs_ptr->spi);
        spi_cs_ptr->cfgs[cs].transfered_count = vsf_spi_get_transfered_count(spi_cs_ptr->spi);
    } else {                        // other cs request
        result = VSF_ERR_FAIL;
    }
    vsf_multi_spi_unprotect(state);

    return result;
}

static int_fast32_t __vsf_multi_spi_get_transfered_count(vsf_spi_cs_t* spi_cs_ptr,
                                                         uint8_t cs)
{
    VSF_HAL_ASSERT(cs < VSF_MULTI_SPI_MAX_CS);
    VSF_HAL_ASSERT(spi_cs_ptr != NULL);

    return spi_cs_ptr->cfgs[cs].transfered_count;
}

#define __MULTI_SPI_VAR_DEC(__N, __MSPI)                                       \
const i_spi_t __MSPI##__N;

#define __MULTI_SPI_VAR_INIT(__N, __MSPI)                                      \
{.i_spi = &__MSPI##__N},

#define __MULTI_SPI_BODY_EX(__N, __MSPI)                                        \
static vsf_err_t __MSPI##__N##_init(i_spi_cfg_t *cfg_ptr)                       \
{                                                                               \
    return __vsf_multi_spi_init(&__MSPI.spi_cs, __N, cfg_ptr);                  \
}                                                                               \
static fsm_rt_t __MSPI##__N##_enable(void)                                      \
{                                                                               \
    return __vsf_multi_spi_enable(&__MSPI.spi_cs, __N);                         \
}                                                                               \
static fsm_rt_t __MSPI##__N##_disable(void)                                     \
{                                                                               \
    return __vsf_multi_spi_enable(&__MSPI.spi_cs, __N);                         \
}                                                                               \
static void __MSPI##__N##_irq_enable(                                           \
                em_spi_irq_mask_t irq_mask)                                     \
{                                                                               \
    __vsf_multi_spi_irq_enable(&__MSPI.spi_cs, __N, irq_mask);                  \
}                                                                               \
static void __MSPI##__N##_irq_disable(                                          \
                em_spi_irq_mask_t irq_mask)                                     \
{                                                                               \
    __vsf_multi_spi_irq_disable(&__MSPI.spi_cs, __N, irq_mask);                 \
}                                                                               \
static spi_status_t __MSPI##__N##_status(void)                                  \
{                                                                               \
    return __vsf_multi_spi_status(&__MSPI.spi_cs, __N);                         \
}                                                                               \
static void __MSPI##__N##_fifo_transfer(void *out_buffer_ptr,                   \
        uint_fast32_t* out_count_ptr, void *in_buffer_ptr,                      \
        uint_fast32_t* in_count_ptr)                                            \
{                                                                               \
    __vsf_multi_spi_fifo_transfer(&__MSPI.spi_cs,                               \
                                  __N,                                          \
                                  out_buffer_ptr,                               \
                                  out_count_ptr,                                \
                                  in_buffer_ptr,                                \
                                  in_count_ptr);                                \
}                                                                               \
static bool __MSPI##__N##_fifo_flush(void)                                      \
{                                                                               \
    return __vsf_multi_spi_fifo_flush(&__MSPI.spi_cs, __N);                     \
}                                                                               \
static void __MSPI##__N##_cs_active(uint_fast8_t index)                         \
{                                                                               \
    __vsf_multi_spi_cs_active(&__MSPI.spi_cs, __N, index);                      \
}                                                                               \
static void __MSPI##__N##_cs_inactive(uint_fast8_t index)                       \
{                                                                               \
    __vsf_multi_spi_cs_inactive(&__MSPI.spi_cs, __N, index);                    \
}                                                                               \
static vsf_err_t __MSPI##__N##_request_transfer(                                \
           void *out_buffer_ptr, void *in_buffer_ptr,                           \
           uint_fast32_t count)                                                 \
{                                                                               \
    return __vsf_multi_spi_request_transfer(&__MSPI.spi_cs,                     \
                                            __N,                                \
                                            out_buffer_ptr,                     \
                                            in_buffer_ptr,                      \
                                            count);                             \
}                                                                               \
static vsf_err_t __MSPI##__N##_cancel_transfer(void)                            \
{                                                                               \
    return __vsf_multi_spi_cancel_transfer(&__MSPI.spi_cs,                      \
                                           __N);                                \
}                                                                               \
static int_fast32_t __MSPI##__N##_get_transfered_count(void)                    \
{                                                                               \
    return __vsf_multi_spi_get_transfered_count(&__MSPI.spi_cs,                 \
                                                __N);                           \
}                                                                               \
const i_spi_t __MSPI##__N = {                                                   \
    .SPI = {                                                                    \
        .Status             = __MSPI##__N##_status,                             \
        .Capability         = NULL,                                             \
    },                                                                          \
                                                                                \
    .Enable                 = __MSPI##__N##_enable,                             \
    .Disable                = __MSPI##__N##_disable,                            \
                                                                                \
    .Init                   = __MSPI##__N##_init,                               \
                                                                                \
    .CS = {                                                                     \
        .Set                = __MSPI##__N##_cs_active,                          \
        .Clear              = __MSPI##__N##_cs_inactive,                        \
    },                                                                          \
                                                                                \
    .FIFO = {                                                                   \
        .Transfer           = __MSPI##__N##_fifo_transfer,                      \
        .Flush              = __MSPI##__N##_fifo_flush,                         \
    },                                                                          \
                                                                                \
    .Block = {                                                                  \
        .RequestTransfer    =                                                   \
            __MSPI##__N##_request_transfer,                                     \
        .Cancel             =                                                   \
            __MSPI##__N##_cancel_transfer,                                      \
        .GetTransferedCount =                                                   \
            __MSPI##__N##_get_transfered_count,                                 \
    },                                                                          \
                                                                                \
    .IRQ = {                                                                    \
        .Enable             = __MSPI##__N##_irq_enable,                         \
        .Disable            = __MSPI##__N##_irq_disable,                        \
    }                                                                           \
};

#define __MULTI_SPI_FUNC_DEF(__MSPI, __SPI, __N)                                \
VSF_MREPEAT(__N, __MULTI_SPI_VAR_DEC, __MSPI)                                   \
typedef struct __MSPI##_t {                                                     \
    vsf_spi_cs_t spi_cs;                                                        \
    vsf_spi_cs_one_t ones[__N];                                                 \
} __MSPI##_t;                                                                   \
static __MSPI##_t __MSPI = {                                                    \
    .spi_cs = {                                                                 \
        .spi = &__SPI,                                                          \
        .cur_cs = -1,                                                           \
        .list.head = NULL,                                                      \
        .list.tail = NULL,                                                      \
    },                                                                          \
    .ones = {                                                                   \
        VSF_MREPEAT(__N, __MULTI_SPI_VAR_INIT, __MSPI)                          \
    }                                                                           \
};                                                                              \
VSF_MREPEAT(__N, __MULTI_SPI_BODY_EX, __MSPI)

#if SPI_MAX_PORT >= 0 && VSF_HAL_USE_SPI0 == ENABLED && (SPI_PORT_MASK & (1 << 0))
#   ifndef VSF_HAL_SPI0_CS_CNT
#       define VSF_HAL_SPI0_CS_CNT 2
#   endif
__MULTI_SPI_FUNC_DEF(VSF_SPI0_CS, vsf_spi0, VSF_HAL_SPI0_CS_CNT)
#endif

#if SPI_MAX_PORT >= 1 && VSF_HAL_USE_SPI1 == ENABLED && (SPI_PORT_MASK & (1 << 1))
#   ifndef VSF_HAL_SPI1_CS_CNT
#       define VSF_HAL_SPI1_CS_CNT 1
#   endif
__MULTI_SPI_FUNC_DEF(VSF_SPI1_CS, vsf_spi0, VSF_HAL_SPI0_CS_CNT)
#endif

#if SPI_MAX_PORT >= 2 && VSF_HAL_USE_SPI2 == ENABLED && (SPI_PORT_MASK & (1 << 2))
#   ifndef VSF_HAL_SPI2_CS_CNT
#       define VSF_HAL_SPI2_CS_CNT 1
#   endif
__MULTI_SPI_FUNC_DEF(VSF_SPI2_CS, vsf_spi0, VSF_HAL_SPI0_CS_CNT)
#endif

#if SPI_MAX_PORT >= 3 && VSF_HAL_USE_SPI3 == ENABLED && (SPI_PORT_MASK & (1 << 3))
#   ifndef VSF_HAL_SPI3_CS_CNT
#       define VSF_HAL_SPI3_CS_CNT 1
#   endif
__MULTI_SPI_FUNC_DEF(VSF_SPI3_CS, vsf_spi0, VSF_HAL_SPI0_CS_CNT)
#endif

#if SPI_MAX_PORT >= 4 && VSF_HAL_USE_SPI4 == ENABLED && (SPI_PORT_MASK & (1 << 4))
#   ifndef VSF_HAL_SPI4_CS_CNT
#       define VSF_HAL_SPI4_CS_CNT 1
#   endif
__MULTI_SPI_FUNC_DEF(VSF_SPI4_CS, vsf_spi0, VSF_HAL_SPI0_CS_CNT)
#endif

#if SPI_MAX_PORT >= 5 && VSF_HAL_USE_SPI5 == ENABLED && (SPI_PORT_MASK & (1 << 5))
#   ifndef VSF_HAL_SPI5_CS_CNT
#       define VSF_HAL_SPI5_CS_CNT 1
#   endif
__MULTI_SPI_FUNC_DEF(VSF_SPI5_CS, vsf_spi0, VSF_HAL_SPI0_CS_CNT)
#endif

#if SPI_MAX_PORT >= 6 && VSF_HAL_USE_SPI6 == ENABLED && (SPI_PORT_MASK & (1 << 6))
#   ifndef VSF_HAL_SPI6_CS_CNT
#       define VSF_HAL_SPI6_CS_CNT 1
#   endif
__MULTI_SPI_FUNC_DEF(VSF_SPI6_CS, vsf_spi0, VSF_HAL_SPI0_CS_CNT)
#endif

#if SPI_MAX_PORT >= 7 && VSF_HAL_USE_SPI7 == ENABLED && (SPI_PORT_MASK & (1 << 7))
#   ifndef VSF_HAL_SPI7_CS_CNT
#       define VSF_HAL_SPI7_CS_CNT 1
#   endif
__MULTI_SPI_FUNC_DEF(VSF_SPI7_CS, vsf_spi0, VSF_HAL_SPI0_CS_CNT)
#endif

#endif
