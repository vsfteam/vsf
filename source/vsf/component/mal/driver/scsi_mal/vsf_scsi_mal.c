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

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_USE_SCSI == ENABLED && VSF_MAL_USE_SCSI_MAL == ENABLED

#define __VSF_MAL_CLASS_INHERIT__
#define __VSF_SCSI_MAL_CLASS_IMPLEMENT

#include "../../vsf_mal.h"
#include "./vsf_scsi_mal.h"

/*============================ MACROS ========================================*/

#if VSF_SCSI_MAL_CFG_DEBUG == ENABLED
#   define __vk_scsi_mal_trace(...)                                             \
            vsf_trace_debug("scsi_mal: " __VA_ARGS__)
#else
#   define __vk_scsi_mal_trace(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_scsi_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_scsi_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
dcl_vsf_peda_methods(static, __vk_scsi_mal_init)
dcl_vsf_peda_methods(static, __vk_scsi_mal_fini)
dcl_vsf_peda_methods(static, __vk_scsi_mal_read)
dcl_vsf_peda_methods(static, __vk_scsi_mal_write)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_mal_drv_t vk_scsi_mal_drv = {
    .blksz          = __vk_scsi_mal_blksz,
    .buffer         = __vk_scsi_mal_buffer,
    .init           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_scsi_mal_init),
    .fini           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_scsi_mal_fini),
    .read           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_scsi_mal_read),
    .write          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_scsi_mal_write),
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vk_scsi_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    return( (vk_scsi_mal_t *)mal)->block_size;
}

static bool __vk_scsi_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    vk_scsi_mal_t *pthis = (vk_scsi_mal_t *)mal;
    memset(pthis->cbd, 0, sizeof(pthis->cbd));
    pthis->cbd[0] = VSF_MAL_OP_READ == op ? 0x28 : 0x2A;
    put_unaligned_be32((uint32_t)(addr / pthis->block_size), &pthis->cbd[2]);
    put_unaligned_be16((uint16_t)(size / pthis->block_size), &pthis->cbd[7]);
    return vk_scsi_prepare_buffer(pthis->scsi, pthis->cbd, mem);
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_scsi_mal_init, vk_mal_init)
{
    vsf_peda_begin();
    enum {
        STATE_INIT,
        STATE_INQUIRY,
        STATE_CAPACITY,
    };
    vk_scsi_mal_t *pthis = (vk_scsi_mal_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        VSF_MAL_ASSERT((pthis != NULL) && (pthis->scsi != NULL));
        vsf_eda_frame_user_value_set(STATE_INIT);
        vk_scsi_init(pthis->scsi);
        break;
    case VSF_EVT_RETURN: {
            __vsf_frame_uint_t state;
            vsf_eda_frame_user_value_get(&state);
            memset(pthis->cbd, 0, sizeof(pthis->cbd));
            switch (state) {
            case STATE_INIT:
                pthis->cbd[0] = 0x12;
                pthis->cbd[4] = 0x24;
                pthis->mem.buffer = (uint8_t *)&pthis->buffer.inquiry;
                pthis->mem.size = sizeof(pthis->buffer.inquiry);
                vsf_eda_frame_user_value_set(STATE_INQUIRY);
                vk_scsi_execute(pthis->scsi, pthis->cbd, &pthis->mem);
                break;
            case STATE_INQUIRY:
                vsf_trace_info("scsi_mal: vendor %8s, product %16s, revision %4s" VSF_TRACE_CFG_LINEEND,
                        pthis->buffer.inquiry.vendor, pthis->buffer.inquiry.product, pthis->buffer.inquiry.revision);

                pthis->cbd[0] = 0x25;
                pthis->mem.buffer = (uint8_t *)&pthis->buffer.capacity;
                pthis->mem.size = sizeof(pthis->buffer.capacity);
                vsf_eda_frame_user_value_set(STATE_CAPACITY);
                vk_scsi_execute(pthis->scsi, pthis->cbd, &pthis->mem);
                break;
            case STATE_CAPACITY:
                pthis->block_size = be32_to_cpu(pthis->buffer.capacity.block_size);
                vsf_eda_return(VSF_ERR_NONE);
                break;
            }
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_scsi_mal_fini, vk_mal_fini)
{
    vsf_peda_begin();
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_scsi_mal_read, vk_mal_read)
{
    vsf_peda_begin();
    vk_scsi_mal_t *pthis = (vk_scsi_mal_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->mem.buffer = vsf_local.buff;
        pthis->mem.size = vsf_local.size;
        pthis->cbd[0] = 0x28;
        put_unaligned_be32((uint32_t)(vsf_local.addr / pthis->block_size), &pthis->cbd[2]);
        put_unaligned_be16((uint16_t)(vsf_local.size / pthis->block_size), &pthis->cbd[7]);
        vk_scsi_execute(pthis->scsi, pthis->cbd, &pthis->mem);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_scsi_mal_write, vk_mal_write)
{
    vsf_peda_begin();
    vk_scsi_mal_t *pthis = (vk_scsi_mal_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->mem.buffer = vsf_local.buff;
        pthis->mem.size = vsf_local.size;
        pthis->cbd[0] = 0x2A;
        put_unaligned_be32((uint32_t)(vsf_local.addr / pthis->block_size), &pthis->cbd[2]);
        put_unaligned_be16((uint16_t)(vsf_local.size / pthis->block_size), &pthis->cbd[7]);
        vk_scsi_execute(pthis->scsi, pthis->cbd, &pthis->mem);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
