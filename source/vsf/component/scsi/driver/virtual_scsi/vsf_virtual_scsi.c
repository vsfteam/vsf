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

#include "../../vsf_scsi_cfg.h"

#if VSF_USE_SCSI == ENABLED && VSF_USE_MAL_SCSI == ENABLED

#define VSF_SCSI_INHERIT
#define VSF_VIRTUAL_SCSI_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static void __vk_virtual_scsi_dummy(uintptr_t target, vsf_evt_t evt);
static bool __vk_virtual_scsi_buffer(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem);
static void __vk_virtual_scsi_init(uintptr_t target, vsf_evt_t evt);
static void __vk_virtual_scsi_execute(uintptr_t target, vsf_evt_t evt);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void __vk_virtual_scsi_execute_stream(uintptr_t target, vsf_evt_t evt);
#endif

/*============================ LOCAL VARIABLES ===============================*/

const i_scsi_drv_t VK_VIRTUAL_SCSI_DRV = {
    .init               = __vk_virtual_scsi_init,
    .fini               = __vk_virtual_scsi_dummy,
    .buffer             = __vk_virtual_scsi_buffer,
    .execute            = __vk_virtual_scsi_execute,
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
    .execute_stream     = __vk_virtual_scsi_execute_stream,
#endif
};

/*============================ IMPLEMENTATION ================================*/

static void __vk_virtual_scsi_dummy(uintptr_t target, vsf_evt_t evt)
{
    vk_scsi_t *pthis = (vk_scsi_t *)target;
    VSF_SCSI_ASSERT(pthis != NULL);
    pthis->result.errcode = VSF_ERR_NONE;
    vsf_eda_return();
}

static bool __vk_virtual_scsi_buffer(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem)
{
    vk_virtual_scsi_t *virtual_scsi = (vk_virtual_scsi_t *)pthis;
    scsi_cmd_code_t cmd_code;
    bool is_read;
    VSF_SCSI_ASSERT((virtual_scsi != NULL) && (mem != NULL) && (virtual_scsi->virtual_scsi_drv != NULL));

    cmd_code = (scsi_cmd_code_t)(cbd[0] & 0x1F);
    is_read = cmd_code == SCSI_CMDCODE_READ;
    if ((cmd_code != SCSI_CMDCODE_READ) && (cmd_code != SCSI_CMDCODE_WRITE)) {
        mem->nSize = sizeof(virtual_scsi->reply);
        mem->pchBuffer = virtual_scsi->reply;
        return true;
    } else if (virtual_scsi->virtual_scsi_drv->buffer != NULL) {
        uint64_t addr;
        uint32_t size;

        vk_scsi_get_rw_param(cbd, &addr, &size);
        return virtual_scsi->virtual_scsi_drv->buffer(pthis, is_read, addr, size, mem);
    }
    return false;
}

static void __vk_virtual_scsi_init(uintptr_t target, vsf_evt_t evt)
{
    vk_virtual_scsi_t *pthis = (vk_virtual_scsi_t *)target;

    switch (evt) {
    case VSF_EVT_INIT: {
            const i_virtual_scsi_drv_t *drv = pthis->virtual_scsi_drv;

            switch (drv->drv_type) {
            case VSF_VIRTUAL_SCSI_DRV_NORMAL:
                pthis->result.errcode = drv->normal.init(&pthis->use_as__vk_scsi_t);
                vsf_eda_return();
                break;
            case VSF_VIRTUAL_SCSI_DRV_PARAM_SUBCALL:
                pthis->result.errcode = vsf_eda_call_param_eda(drv->param_subcall.init, pthis);
                break;
            default:
                VSF_SCSI_ASSERT(false);
                pthis->result.errcode = VSF_ERR_NOT_SUPPORT;
                vsf_eda_return();
                break;
            }
        }
        return;
    case VSF_EVT_RETURN:
        vsf_eda_return();
        break;
    }
}

static vsf_err_t __vk_virtual_scsi_rw(vk_virtual_scsi_t *pthis)
{
    const i_virtual_scsi_drv_t *drv = pthis->virtual_scsi_drv;
    uint8_t *cbd = pthis->args.cbd;
    uint64_t addr;
    uint32_t size;

    vk_scsi_get_rw_param(cbd, &addr, &size);
    pthis->addr = addr;
    pthis->size = size;

    scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(cbd[0] & 0x1F);
    bool is_read = cmd_code == SCSI_CMDCODE_READ;

    switch (drv->drv_type) {
    case VSF_VIRTUAL_SCSI_DRV_NORMAL:
        pthis->result.errcode = is_read ?
                drv->normal.read(&pthis->use_as__vk_scsi_t, pthis->addr, pthis->size)
            :   drv->normal.write(&pthis->use_as__vk_scsi_t, pthis->addr, pthis->size);
        vsf_eda_return();
        return VSF_ERR_NONE;
    case VSF_VIRTUAL_SCSI_DRV_PARAM_SUBCALL:
        pthis->result.errcode = is_read ?
                vsf_eda_call_param_eda(drv->param_subcall.read, pthis)
            :   vsf_eda_call_param_eda(drv->param_subcall.write, pthis);
        return VSF_ERR_NOT_READY;
    default:
        VSF_SCSI_ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
    }
}

static void __vk_virtual_scsi_execute(uintptr_t target, vsf_evt_t evt)
{
    vk_virtual_scsi_t *pthis = (vk_virtual_scsi_t *)target;
    vk_virtual_scsi_param_t *param = pthis->param;

    uint8_t *reply = pthis->args.mem.pchBuffer;
    int_fast32_t reply_len = pthis->args.mem.nSize;

    switch (evt) {
    case VSF_EVT_INIT: {
            uint8_t *scsi_cmd = pthis->args.cbd;
            scsi_group_code_t group_code = (scsi_group_code_t)(scsi_cmd[0] & 0xE0);
            scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(scsi_cmd[0] & 0x1F);

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
            pthis->is_stream = false;
#endif
            if (reply == pthis->reply) {
                VSF_SCSI_ASSERT(reply_len <= sizeof(pthis->reply));
                memset(reply, 0, reply_len);
            }
            switch (group_code) {
            case SCSI_GROUPCODE6:
                switch (cmd_code) {
                case SCSI_CMDCODE_MODE_SELECT:
                case SCSI_CMDCODE_TEST_UNIT_READY:
                case SCSI_CMDCODE_VERIFY:
                case SCSI_CMDCODE_FORMAT_UNIT:
                case SCSI_CMDCODE_START_STOP_UNIT:
                case SCSI_CMDCODE_ALLOW_MEDIUM_REMOVAL:
                    reply_len = 0;
                    break;
                case SCSI_CMDCODE_REQUEST_SENSE: {
                        uint_fast8_t len = scsi_cmd[4];

                        ASSERT(reply_len >= len);
                        reply[0] = 0x70;
                        reply[2] = pthis->sense_key;
                        reply[7] = 0x0A;
                        reply[12] = pthis->asc;
                        reply_len = min(len, 18);
                        break;
                    }
                case SCSI_CMDCODE_READ:
                case SCSI_CMDCODE_WRITE:
                    __vk_virtual_scsi_rw(pthis);
                    return;
                case SCSI_CMDCODE_INQUIRY:
                    if (scsi_cmd[1] & 1) {
                        // When the EVPD bit is set to one,
                        // the PAGE CODE field specifies which page of
                        // vital product data information the device server shall return
                        if (scsi_cmd[2] != 0) {
                            goto exit_invalid_field_in_cmd;
                        }

                        ASSERT(reply_len >= 5);
                        reply_len = 5;
                        break;
                    } else {
                        if (scsi_cmd[2] != 0) {
                            // If the PAGE CODE field is not set to zero
                            // when the EVPD bit is set to zero,
                            // the command shall be terminated with CHECK CONDITION status,
                            // with the sense key set to ILLEGAL REQUEST,
                            // and the additional sense code set to INVALID FIELD IN CDB.
                            goto exit_invalid_field_in_cmd;
                        }

                        ASSERT(reply_len >= 36);
                        reply_len = 36;

                        reply[0] = param->type;
                        if (param->removable) {
                            reply[1] = 0x80;
                        }
                        reply[3] = 2;
                        reply[4] = 31;
                        memcpy(&reply[8], param->vendor, 8);
                        memcpy(&reply[16], param->product, 16);
                        memcpy(&reply[32], param->revision, 4);
                    }
                    break;
                case SCSI_CMDCODE_MODE_SENSE:
                    ASSERT(reply_len >= 4);
                    reply_len = 4;
                    reply[0] = 3;
                    break;
                default:
                    goto exit_invalid_cmd;
                }
                break;
            case SCSI_GROUPCODE10_1:
                switch (cmd_code) {
                case SCSI_CMDCODE_READ_FORMAT_CAPACITIES:
                    ASSERT(reply_len >= 12);
                    reply_len = 12;
                    reply[3] = 8;
                    put_unaligned_be32(param->block_num, &reply[4]);
                    put_unaligned_be32(param->block_size, &reply[8]);
                    reply[8] = 2;
                    break;
                case SCSI_CMDCODE_READ_CAPACITY:
                    ASSERT(reply_len >= 8);
                    reply_len = 8;
                    put_unaligned_be32(param->block_num - 1, &reply[0]);
                    put_unaligned_be32(param->block_size, &reply[4]);
                    break;
                case SCSI_CMDCODE_READ:
                case SCSI_CMDCODE_WRITE:
                    __vk_virtual_scsi_rw(pthis);
                    return;
                default:
                    goto exit_invalid_cmd;
                }
                break;
            case SCSI_GROUPCODE10_2:
                switch (cmd_code) {
                case SCSI_CMDCODE_MODE_SELECT:
                    break;
                case SCSI_CMDCODE_MODE_SENSE:
                    break;
                default:
                    goto exit_invalid_cmd;
                }
                break;
            case SCSI_GROUPCODE16:
                switch (cmd_code) {
                case SCSI_CMDCODE_READ:
                case SCSI_CMDCODE_WRITE:
                    __vk_virtual_scsi_rw(pthis);
                    return;
                default:
                    goto exit_invalid_cmd;
                }
                break;
            case SCSI_GROUPCODE12:
                switch (cmd_code) {
                case SCSI_CMDCODE_READ:
                case SCSI_CMDCODE_WRITE:
                    __vk_virtual_scsi_rw(pthis);
                    return;
                default:
                    goto exit_invalid_cmd;
                }
                break;
            default:
                goto exit_invalid_cmd;
            }

            pthis->result.reply_len = reply_len;
            pthis->result.errcode = VSF_ERR_NONE;
            vsf_eda_return();
            break;
        }
    case VSF_EVT_RETURN:
        vsf_eda_return();
        break;
    }
    return;

exit_invalid_cmd:
    pthis->sense_key = SCSI_SENSEKEY_ILLEGAL_REQUEST;
    pthis->asc = SCSI_ASC_INVALID_COMMAND;
    pthis->result.errcode = VSF_ERR_FAIL;
    vsf_eda_return();
    return;
exit_invalid_field_in_cmd:
    pthis->sense_key = SCSI_SENSEKEY_ILLEGAL_REQUEST;
    pthis->asc = SCSI_ASC_INVALID_FIELD_IN_COMMAND;
    pthis->result.errcode = VSF_ERR_FAIL;
    vsf_eda_return();
    return;
exit_not_ready:
    pthis->sense_key = SCSI_SENSEKEY_NOT_READY;
    pthis->asc = SCSI_ASC_NONE;
    pthis->result.errcode = VSF_ERR_FAIL;
    vsf_eda_return();
    return;
}

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void __vk_virtual_scsi_execute_stream(uintptr_t target, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT: {
            vk_virtual_scsi_t *pthis = (vk_virtual_scsi_t *)target;
            scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(pthis->args.cbd[0] & 0x1F);
            VSF_SCSI_ASSERT((SCSI_CMDCODE_READ == cmd_code) || (SCSI_CMDCODE_WRITE == cmd_code));
            pthis->is_stream = true;
            __vk_virtual_scsi_rw(pthis);
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return();
        break;
    }
}
#endif

#endif
