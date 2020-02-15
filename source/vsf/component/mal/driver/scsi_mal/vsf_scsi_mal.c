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

#if VSF_USE_MAL == ENABLED && VSF_USE_SCSI == ENABLED && VSF_USE_SCSI_MAL == ENABLED

#define VSF_MAL_INHERIT
#define VSF_SCSI_MAL_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_SCSI_MAL_CFG_DEBUG == ENABLED
#   define __vk_scsi_mal_trace(...)                                             \
            vsf_trace(VSF_TRACE_DEBUG, "scsi_mal: " __VA_ARGS__)
#else
#   define __vk_scsi_mal_trace(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_scsi_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_scsi_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
static void __vk_scsi_mal_init(uintptr_t target, vsf_evt_t evt);
static void __vk_scsi_mal_fini(uintptr_t target, vsf_evt_t evt);
static void __vk_scsi_mal_read(uintptr_t target, vsf_evt_t evt);
static void __vk_scsi_mal_write(uintptr_t target, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

const i_mal_drv_t VK_SCSI_MAL_DRV = {
    .blksz          = __vk_scsi_mal_blksz,
    .buffer         = __vk_scsi_mal_buffer,
    .init           = __vk_scsi_mal_init,
    .fini           = __vk_scsi_mal_fini,
    .read           = __vk_scsi_mal_read,
    .write          = __vk_scsi_mal_write,
};

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
    put_unaligned_be32((uint32_t)(pthis->args.addr / pthis->block_size), &pthis->cbd[2]);
    put_unaligned_be16((uint16_t)(pthis->args.size / pthis->block_size), &pthis->cbd[7]);
    return vk_scsi_prepare_buffer(pthis->scsi, pthis->cbd, mem);
}

static void __vk_scsi_mal_init(uintptr_t target, vsf_evt_t evt)
{
    enum {
        STATE_INIT,
        STATE_INQUIRY,
        STATE_CAPACITY,
    };
    vk_scsi_mal_t *pthis = (vk_scsi_mal_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        VSF_MAL_ASSERT((pthis != NULL) && (pthis->scsi != NULL));
        vsf_eda_frame_user_value_set(STATE_INIT);
        vk_scsi_init(pthis->scsi);
        break;
    case VSF_EVT_RETURN: {
            uint8_t state;
            vsf_eda_frame_user_value_get(&state);
            memset(pthis->cbd, 0, sizeof(pthis->cbd));
            switch (state) {
            case STATE_INIT:
                pthis->cbd[0] = 0x12;
                pthis->cbd[4] = 0x24;
                pthis->mem.pchBuffer = (uint8_t *)&pthis->buffer.inquiry;
                pthis->mem.nSize = sizeof(pthis->buffer.inquiry);
                vsf_eda_frame_user_value_set(STATE_INQUIRY);
                vk_scsi_execute(pthis->scsi, pthis->cbd, &pthis->mem);
                break;
            case STATE_INQUIRY:
                vsf_trace(VSF_TRACE_INFO, "scsi_mal: vendor %8s, product %16s, revision %4s" VSF_TRACE_CFG_LINEEND,
                        pthis->buffer.inquiry.vendor, pthis->buffer.inquiry.product, pthis->buffer.inquiry.revision);

                pthis->cbd[0] = 0x25;
                pthis->mem.pchBuffer = (uint8_t *)&pthis->buffer.capacity;
                pthis->mem.nSize = sizeof(pthis->buffer.capacity);
                vsf_eda_frame_user_value_set(STATE_CAPACITY);
                vk_scsi_execute(pthis->scsi, pthis->cbd, &pthis->mem);
                break;
            case STATE_CAPACITY:
                pthis->block_size = be32_to_cpu(pthis->buffer.capacity.block_size);
                pthis->result.errcode = VSF_ERR_NONE;
                vsf_eda_return();
                break;
            }
        }
        break;
    }
}

static void __vk_scsi_mal_fini(uintptr_t target, vsf_evt_t evt)
{
    vsf_eda_return();
}

static void __vk_scsi_mal_read(uintptr_t target, vsf_evt_t evt)
{
    vk_scsi_mal_t *pthis = (vk_scsi_mal_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->mem.pchBuffer = pthis->args.buff;
        pthis->mem.nSize = pthis->args.size;
        pthis->cbd[0] = 0x28;
        put_unaligned_be32((uint32_t)(pthis->args.addr / pthis->block_size), &pthis->cbd[2]);
        put_unaligned_be16((uint16_t)(pthis->args.size / pthis->block_size), &pthis->cbd[7]);
        vk_scsi_execute(pthis->scsi, pthis->cbd, &pthis->mem);
        break;
    case VSF_EVT_RETURN:
        pthis->result.errcode = vk_scsi_get_errcode(pthis->scsi, &pthis->result.size);
        vsf_eda_return();
        break;
    }
}

static void __vk_scsi_mal_write(uintptr_t target, vsf_evt_t evt)
{
    vk_scsi_mal_t *pthis = (vk_scsi_mal_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->mem.pchBuffer = pthis->args.buff;
        pthis->mem.nSize = pthis->args.size;
        pthis->cbd[0] = 0x2A;
        put_unaligned_be32((uint32_t)(pthis->args.addr / pthis->block_size), &pthis->cbd[2]);
        put_unaligned_be16((uint16_t)(pthis->args.size / pthis->block_size), &pthis->cbd[7]);
        vk_scsi_execute(pthis->scsi, pthis->cbd, &pthis->mem);
        break;
    case VSF_EVT_RETURN:
        pthis->result.errcode = vk_scsi_get_errcode(pthis->scsi, &pthis->result.size);
        vsf_eda_return();
        break;
    }
}

#endif
