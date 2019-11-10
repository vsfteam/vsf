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

#include "vsf.h"

/*============================ MACROS ========================================*/

#ifndef USRAPP_CFG_MSC_SIZE
#   define USRAPP_CFG_MSC_SIZE                  (18 * 1024)
#endif



enum scsi_pdt_t {
    SCSI_PDT_DIRECT_ACCESS_BLOCK                = 0x00,
    SCSI_PDT_CD_DVD                             = 0x05,
};
typedef enum scsi_pdt_t scsi_pdt_t;

enum scsi_sensekey_t {
    SCSI_SENSEKEY_NO_SENSE                      = 0,
    SCSI_SENSEKEY_RECOVERED_ERROR               = 1,
    SCSI_SENSEKEY_NOT_READY                     = 2,
    SCSI_SENSEKEY_MEDIUM_ERROR                  = 3,
    SCSI_SENSEKEY_HARDWARE_ERROR                = 4,
    SCSI_SENSEKEY_ILLEGAL_REQUEST               = 5,
    SCSI_SENSEKEY_UNIT_ATTENTION                = 6,
    SCSI_SENSEKEY_DATA_PROTECT                  = 7,
    SCSI_SENSEKEY_BLANK_CHECK                   = 8,
    SCSI_SENSEKEY_VENDOR_SPECIFIC               = 9,
    SCSI_SENSEKEY_COPY_ABORTED                  = 10,
    SCSI_SENSEKEY_ABORTED_COMMAND               = 11,
    SCSI_SENSEKEY_VOLUME_OVERFLOW               = 13,
    SCSI_SENSEKEY_MISCOMPARE                    = 14,
};
typedef enum scsi_sensekey_t scsi_sensekey_t;

enum scsi_asc_t {
    SCSI_ASC_NONE                               = 0x00,
    SCSI_ASC_PARAMETER_LIST_LENGTH_ERROR        = 0x1A,
    SCSI_ASC_INVALID_COMMAND                    = 0x20,
    SCSI_ASC_INVALID_FIELD_IN_COMMAND           = 0x24,
    SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST    = 0x26,
    SCSI_ASC_MEDIUM_HAVE_CHANGED                = 0x28,
    SCSI_ASC_ADDRESS_OUT_OF_RANGE               = 0x21,
    SCSI_ASC_MEDIUM_NOT_PRESENT                 = 0x3A,
};
typedef enum scsi_asc_t scsi_asc_t;

enum scsi_group_code_t {
    SCSI_GROUPCODE6                             = 0x00,
    SCSI_GROUPCODE10_1                          = 0x20,
    SCSI_GROUPCODE10_2                          = 0x40,
    SCSI_GROUPCODE16                            = 0x80,
    SCSI_GROUPCODE12                            = 0xA0,
};
typedef enum scsi_group_code_t scsi_group_code_t;

enum scsi_cmd_code_t {
    SCSI_CMDCODE_TEST_UNIT_READY                = 0x00,
    SCSI_CMDCODE_REQUEST_SENSE                  = 0x03, // SCSI_GROUPCODE6
    SCSI_CMDCODE_READ_FORMAT_CAPACITIES         = 0x03, // SCSI_GROUPCODE10_1
    SCSI_CMDCODE_READ_TOC                       = 0x03, // SCSI_GROUPCODE10_2
    SCSI_CMDCODE_FORMAT_UNIT                    = 0x04,
    SCSI_CMDCODE_READ_CAPACITY                  = 0x05,
    SCSI_CMDCODE_READ                           = 0x08,
    SCSI_CMDCODE_WRITE                          = 0x0A,
    SCSI_CMDCODE_GET_EVENT_STATUS_NOTIFY        = 0x0A, // SCSI_GROUPCODE10_2
    SCSI_CMDCODE_VERIFY                         = 0x0F,
    SCSI_CMDCODE_INQUIRY                        = 0x12,
    SCSI_CMDCODE_MODE_SELECT                    = 0x15,
    SCSI_CMDCODE_MODE_SENSE                     = 0x1A,
    SCSI_CMDCODE_START_STOP_UNIT                = 0x1B,
    SCSI_CMDCODE_SEND_DIAGNOSTIC                = 0x1D,
    SCSI_CMDCODE_ALLOW_MEDIUM_REMOVAL           = 0x1E,
};
typedef enum scsi_cmd_code_t scsi_cmd_code_t;

struct scsi_dev_param_t {
    uint32_t block_size;
    uint32_t block_num;
    char vendor[8];
    char product[16];
    char revision[4];
    bool removable;
    scsi_pdt_t type;
};
typedef struct scsi_dev_param_t scsi_dev_param_t;

struct scsi_dev_t {
    scsi_dev_param_t *param;
    scsi_sensekey_t sense_key;
    scsi_asc_t asc;
};
typedef struct scsi_dev_t scsi_dev_t;

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
    struct {
        scsi_dev_param_t param;
    } scsi_dev;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    struct {
        uint8_t mem[USRAPP_CFG_MSC_SIZE];
        scsi_dev_t device;
    } scsi_dev;

    vsf_usbd_msc_t *msc;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/

static int_fast32_t vsf_usbd_msc_scsi_execute(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx,
                            vsf_mem_t *mem);
static vsf_err_t vsf_usbd_msc_scsi_read(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx,
                            vsf_mem_t *mem);
static vsf_err_t vsf_usbd_msc_scsi_write(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx,
                            vsf_mem_t *mem);

/*============================ LOCAL VARIABLES ===============================*/

vsf_usbd_msc_op_t usrapp_msc_op = {
    .execute                = vsf_usbd_msc_scsi_execute,
    .read                   = vsf_usbd_msc_scsi_read,
    .write                  = vsf_usbd_msc_scsi_write,
};

static const usrapp_const_t usrapp_const = {
    .scsi_dev.param         = {
        .block_size         = 512,
        .block_num          = USRAPP_CFG_MSC_SIZE / 512,
        .vendor             = "Simon   ",
        .product            = "VSFDriver       ",
        .revision           = "1.00",
        .type               = SCSI_PDT_DIRECT_ACCESS_BLOCK,
    },
};

static usrapp_t usrapp = {
    .scsi_dev.device        = {
        .param              = (scsi_dev_param_t *)&usrapp_const.scsi_dev.param,
        .sense_key          = SCSI_SENSEKEY_NO_SENSE,
        .asc                = SCSI_ASC_NONE,
    },
};

/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t usrapp_block_async_access(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx,
                    uint_fast64_t address, uint_fast32_t num)
{
    scsi_dev_t *scsi_dev = &usrapp.scsi_dev.device;
    scsi_dev_param_t *scsi_param = scsi_dev->param;

    ctx->addr = address * scsi_param->block_size;
    ctx->size = num * scsi_param->block_size;
    return ctx->size;
}

static int_fast32_t vsf_usbd_msc_scsi_execute(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx,
                            vsf_mem_t *mem)
{
    scsi_dev_t *scsi_dev = &usrapp.scsi_dev.device;
    scsi_dev_param_t *scsi_param = scsi_dev->param;

    uint8_t scsi_cmd[sizeof(ctx->cbw.CBWCB)];
    scsi_group_code_t group_code;
    scsi_cmd_code_t cmd_code;
    uint8_t *reply = ctx->reply;
    int_fast32_t data_len = ctx->cbw.dCBWDataTransferLength;
    bool is_read;

    memcpy(scsi_cmd, ctx->cbw.CBWCB, sizeof(ctx->cbw.CBWCB));
    group_code = scsi_cmd[0] & 0xE0;
    cmd_code = scsi_cmd[0] & 0x1F;
    is_read = cmd_code == SCSI_CMDCODE_READ;

    memset(reply, 0, sizeof(ctx->reply));
    switch (group_code) {
    case SCSI_GROUPCODE6:
        switch (cmd_code) {
        case SCSI_CMDCODE_MODE_SELECT:
        case SCSI_CMDCODE_TEST_UNIT_READY:
        case SCSI_CMDCODE_VERIFY:
        case SCSI_CMDCODE_FORMAT_UNIT:
        case SCSI_CMDCODE_START_STOP_UNIT:
        case SCSI_CMDCODE_ALLOW_MEDIUM_REMOVAL:
            break;
        case SCSI_CMDCODE_REQUEST_SENSE: {
                uint_fast8_t len = scsi_cmd[4];

                ASSERT(sizeof(ctx->reply) >= data_len);
                reply[0] = 0x70;
                reply[2] = scsi_dev->sense_key;
                reply[7] = 0x0A;
                reply[12] = scsi_dev->asc;
                data_len = min(len, 18);
                break;
            }
        case SCSI_CMDCODE_READ:
        case SCSI_CMDCODE_WRITE:
            data_len = usrapp_block_async_access(msc, ctx, get_unaligned_be16(&scsi_cmd[2]), scsi_cmd[4]);
            reply = is_read ? NULL : &usrapp.scsi_dev.mem[ctx->addr];
            break;
        case SCSI_CMDCODE_INQUIRY:
            if (scsi_cmd[1] & 1) {
                // When the EVPD bit is set to one,
                // the PAGE CODE field specifies which page of
                // vital product data information the device server shall return
                if (scsi_cmd[2] != 0) {
                    goto exit_invalid_field_in_cmd;
                }

                data_len = 5;
                ASSERT(sizeof(ctx->reply) >= data_len);
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

                data_len = 36;
                ASSERT(sizeof(ctx->reply) >= data_len);

                reply[0] = scsi_param->type;
                if (scsi_param->removable) {
                    reply[1] = 0x80;
                }
                reply[3] = 2;
                reply[4] = 31;
                memcpy(&reply[8], scsi_param->vendor, 8);
                memcpy(&reply[16], scsi_param->product, 16);
                memcpy(&reply[32], scsi_param->revision, 4);
            }
            break;
        case SCSI_CMDCODE_MODE_SENSE:
            data_len = 4;
            ASSERT(sizeof(ctx->reply) >= data_len);
            reply[0] = 3;
            break;
        default:
            goto exit_invalid_cmd;
        }
        break;
    case SCSI_GROUPCODE10_1:
        switch (cmd_code) {
        case SCSI_CMDCODE_READ_FORMAT_CAPACITIES:
            data_len = 12;
            ASSERT(sizeof(ctx->reply) >= data_len);
            reply[3] = 8;
            put_unaligned_be32(scsi_param->block_num, &reply[4]);
            put_unaligned_be32(scsi_param->block_size, &reply[8]);
            reply[8] = 2;
            break;
        case SCSI_CMDCODE_READ_CAPACITY:
            ASSERT(sizeof(ctx->reply) >= data_len);
            put_unaligned_be32(scsi_param->block_num - 1, &reply[0]);
            put_unaligned_be32(scsi_param->block_size, &reply[4]);
            break;
        case SCSI_CMDCODE_READ:
        case SCSI_CMDCODE_WRITE:
            data_len = usrapp_block_async_access(msc, ctx, get_unaligned_be32(&scsi_cmd[2]), get_unaligned_be16(&scsi_cmd[7]));
            reply = is_read ? NULL : &usrapp.scsi_dev.mem[ctx->addr];
            break;
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
            data_len = usrapp_block_async_access(msc, ctx, get_unaligned_be64(&scsi_cmd[2]), get_unaligned_be32(&scsi_cmd[7]));
            reply = is_read ? NULL : &usrapp.scsi_dev.mem[ctx->addr];
            break;
        default:
            goto exit_invalid_cmd;
        }
        break;
    case SCSI_GROUPCODE12:
        switch (cmd_code) {
        case SCSI_CMDCODE_READ:
        case SCSI_CMDCODE_WRITE:
            data_len = usrapp_block_async_access(msc, ctx, get_unaligned_be32(&scsi_cmd[2]), get_unaligned_be32(&scsi_cmd[6]));
            reply = is_read ? NULL : &usrapp.scsi_dev.mem[ctx->addr];
            break;
        default:
            goto exit_invalid_cmd;
        }
        break;
    default:
        goto exit_invalid_cmd;
    }
    mem->nSize = data_len;
    mem->pchBuffer = reply;
    return data_len;

exit_invalid_cmd:
    scsi_dev->sense_key = SCSI_SENSEKEY_ILLEGAL_REQUEST;
    scsi_dev->asc = SCSI_ASC_INVALID_COMMAND;
    return -1;
exit_invalid_field_in_cmd:
    scsi_dev->sense_key = SCSI_SENSEKEY_ILLEGAL_REQUEST;
    scsi_dev->asc = SCSI_ASC_INVALID_FIELD_IN_COMMAND;
    return -1;
exit_not_ready:
    scsi_dev->sense_key = SCSI_SENSEKEY_NOT_READY;
    scsi_dev->asc = SCSI_ASC_NONE;
    return -1;
}

static vsf_err_t vsf_usbd_msc_scsi_read(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx,
                            vsf_mem_t *mem)
{
    mem->pchBuffer = &usrapp.scsi_dev.mem[ctx->addr];
    mem->nSize = ctx->size;
    vsf_usbd_msc_send_data(msc);
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_msc_scsi_write(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx,
                            vsf_mem_t *mem)
{
    ASSERT(ctx->size >= mem->nSize);
    ASSERT(mem->pchBuffer == &usrapp.scsi_dev.mem[ctx->addr]);

    // no need to copy memory, because buffer provided is the target memory
//    memcpy(&usrapp.scsi_dev.mem[ctx->addr], mem->pchBuffer, mem->nSize);
    ctx->addr += mem->nSize;
    ctx->size -= mem->nSize;
    return VSF_ERR_NONE;
}

void usrapp_msc_scsi_init(vsf_usbd_msc_t *msc)
{
    usrapp.msc = msc;
}

/* EOF */
