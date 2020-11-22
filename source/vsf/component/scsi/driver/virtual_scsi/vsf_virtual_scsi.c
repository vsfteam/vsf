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

#if VSF_USE_SCSI == ENABLED && VSF_SCSI_USE_VIRTUAL_SCSI == ENABLED

#define __VSF_SCSI_CLASS_INHERIT__
#define __VSF_VIRTUAL_SCSI_CLASS_IMPLEMENT

#include "../../vsf_scsi.h"
#include "./vsf_virtual_scsi.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static bool __vk_virtual_scsi_buffer(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem);
dcl_vsf_peda_methods(static, __vk_virtual_scsi_init)
dcl_vsf_peda_methods(static, __vk_virtual_scsi_fini)
dcl_vsf_peda_methods(static, __vk_virtual_scsi_execute)
#if VSF_USE_SIMPLE_STREAM == ENABLED
dcl_vsf_peda_methods(static, __vk_virtual_scsi_execute_stream)
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_scsi_drv_t vk_virtual_scsi_drv = {
    .init               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_virtual_scsi_init),
    .fini               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_virtual_scsi_fini),
    .buffer             = __vk_virtual_scsi_buffer,
    .execute            = (vsf_peda_evthandler_t)vsf_peda_func(__vk_virtual_scsi_execute),
#if VSF_USE_SIMPLE_STREAM == ENABLED
    .execute_stream     = (vsf_peda_evthandler_t)vsf_peda_func(__vk_virtual_scsi_execute_stream),
#endif
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ IMPLEMENTATION ================================*/

static bool __vk_virtual_scsi_buffer(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem)
{
    vk_virtual_scsi_t *virtual_scsi = (vk_virtual_scsi_t *)pthis;
    scsi_cmd_code_t cmd_code;
    bool is_read;
    VSF_SCSI_ASSERT((virtual_scsi != NULL) && (mem != NULL) && (virtual_scsi->virtual_scsi_drv != NULL));

    cmd_code = (scsi_cmd_code_t)(cbd[0] & 0x1F);
    is_read = cmd_code == SCSI_CMDCODE_READ;
    if ((cmd_code != SCSI_CMDCODE_READ) && (cmd_code != SCSI_CMDCODE_WRITE)) {
        mem->size = sizeof(virtual_scsi->reply);
        mem->buffer = virtual_scsi->reply;
        return true;
    } else if (virtual_scsi->virtual_scsi_drv->buffer != NULL) {
        uint64_t addr;
        uint32_t size;

        vk_scsi_get_rw_param(cbd, &addr, &size);
        return virtual_scsi->virtual_scsi_drv->buffer(pthis, is_read, addr, size, mem);
    }
    return false;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_virtual_scsi_init, vk_scsi_init)
{
    vsf_peda_begin();
    vk_virtual_scsi_t *pthis = (vk_virtual_scsi_t *)&vsf_this;
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT: {
            const vk_virtual_scsi_drv_t *drv = pthis->virtual_scsi_drv;

            switch (drv->drv_type) {
            case VSF_VIRTUAL_SCSI_DRV_NORMAL:
                vsf_eda_return(drv->normal.init(&pthis->use_as__vk_scsi_t));
                break;
            case VSF_VIRTUAL_SCSI_DRV_PARAM_SUBCALL:
                __vsf_component_call_peda_ifs(vk_virtual_scsi_init, err, drv->param_subcall.init, 0, pthis)
                if (err != VSF_ERR_NONE) {
                    goto do_return;
                }
                break;
            default:
                VSF_SCSI_ASSERT(false);
                vsf_eda_return(VSF_ERR_NOT_SUPPORT);
                break;
            }
        }
        return;
    case VSF_EVT_RETURN:
        err = (vsf_err_t)vsf_eda_get_return_value();
    do_return:
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_virtual_scsi_fini, vk_scsi_fini)
{
    vsf_peda_begin();
    vk_scsi_t *pthis = (vk_scsi_t *)&vsf_this;
    VSF_SCSI_ASSERT(pthis != NULL);
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

static vsf_err_t __vk_virtual_scsi_rw(vk_virtual_scsi_t *pthis, uint8_t *cbd, void *mem_stream)
{
    const vk_virtual_scsi_drv_t *drv = pthis->virtual_scsi_drv;
    uint64_t addr;
    uint32_t size;
    int_fast32_t result;

    vk_scsi_get_rw_param(cbd, &addr, &size);

    scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(cbd[0] & 0x1F);
    bool is_read = cmd_code == SCSI_CMDCODE_READ;

    switch (drv->drv_type) {
    case VSF_VIRTUAL_SCSI_DRV_NORMAL:
        result = is_read ?
                drv->normal.read(&pthis->use_as__vk_scsi_t, addr, size)
            :   drv->normal.write(&pthis->use_as__vk_scsi_t, addr, size);
        vsf_eda_return(result);
        return VSF_ERR_NONE;
    case VSF_VIRTUAL_SCSI_DRV_PARAM_SUBCALL:
        if (is_read) {
            vsf_err_t err;
            __vsf_component_call_peda_ifs(vk_virtual_scsi_read, err, drv->param_subcall.read, 0, pthis,
                .addr       = addr,
                .size       = size,
                .mem_stream = mem_stream,
            )
            UNUSED_PARAM(err);
        } else {
            vsf_err_t err;
            __vsf_component_call_peda_ifs(vk_virtual_scsi_write, err, drv->param_subcall.write, 0, pthis,
                .addr       = addr,
                .size       = size,
                .mem_stream = mem_stream,
            )
            UNUSED_PARAM(err);
        }
        return VSF_ERR_NOT_READY;
    default:
        VSF_SCSI_ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
    }
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

__vsf_component_peda_ifs_entry(__vk_virtual_scsi_execute, vk_scsi_execute)
{
    vsf_peda_begin();
    vk_virtual_scsi_t *pthis = (vk_virtual_scsi_t *)&vsf_this;
    vk_virtual_scsi_param_t *param = pthis->param;

    uint8_t *reply = vsf_local.mem.buffer;
    int_fast32_t reply_len = vsf_local.mem.size;

    switch (evt) {
    case VSF_EVT_INIT: {
            uint8_t *scsi_cmd = vsf_local.cbd;
            scsi_group_code_t group_code = (scsi_group_code_t)(scsi_cmd[0] & 0xE0);
            scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(scsi_cmd[0] & 0x1F);

#if VSF_USE_SIMPLE_STREAM == ENABLED
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
                    __vk_virtual_scsi_rw(pthis, scsi_cmd, &vsf_local.mem);
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
                    __vk_virtual_scsi_rw(pthis, scsi_cmd, &vsf_local.mem);
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
                    __vk_virtual_scsi_rw(pthis, scsi_cmd, &vsf_local.mem);
                    return;
                default:
                    goto exit_invalid_cmd;
                }
                break;
            case SCSI_GROUPCODE12:
                switch (cmd_code) {
                case SCSI_CMDCODE_READ:
                case SCSI_CMDCODE_WRITE:
                    __vk_virtual_scsi_rw(pthis, scsi_cmd, &vsf_local.mem);
                    return;
                default:
                    goto exit_invalid_cmd;
                }
                break;
            default:
                goto exit_invalid_cmd;
            }

            vsf_eda_return(reply_len);
            break;
        }
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    return;

exit_invalid_cmd:
    pthis->sense_key = SCSI_SENSEKEY_ILLEGAL_REQUEST;
    pthis->asc = SCSI_ASC_INVALID_COMMAND;
    vsf_eda_return(VSF_ERR_FAIL);
    return;
exit_invalid_field_in_cmd:
    pthis->sense_key = SCSI_SENSEKEY_ILLEGAL_REQUEST;
    pthis->asc = SCSI_ASC_INVALID_FIELD_IN_COMMAND;
    vsf_eda_return(VSF_ERR_FAIL);
    return;
/*
exit_not_ready:
    pthis->sense_key = SCSI_SENSEKEY_NOT_READY;
    pthis->asc = SCSI_ASC_NONE;
    vsf_eda_return(VSF_ERR_FAIL);
    return;
*/
    vsf_peda_end();
}

#if VSF_USE_SIMPLE_STREAM == ENABLED
__vsf_component_peda_ifs_entry(__vk_virtual_scsi_execute_stream, vk_scsi_execute_stream)
{
    vsf_peda_begin();
    switch (evt) {
    case VSF_EVT_INIT: {
            vk_virtual_scsi_t *pthis = (vk_virtual_scsi_t *)&vsf_this;
            scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(vsf_local.cbd[0] & 0x1F);
            VSF_SCSI_ASSERT((SCSI_CMDCODE_READ == cmd_code) || (SCSI_CMDCODE_WRITE == cmd_code));
            pthis->is_stream = true;
            __vk_virtual_scsi_rw(pthis, vsf_local.cbd, vsf_local.stream);
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}
#endif

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
