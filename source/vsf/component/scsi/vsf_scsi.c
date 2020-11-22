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

#include "./vsf_scsi_cfg.h"

#if VSF_USE_SCSI == ENABLED

#define __VSF_SCSI_CLASS_IMPLEMENT
#include "kernel/vsf_kernel.h"
#include "./vsf_scsi.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use scsi
#endif

#if VSF_KERNEL_USE_SIMPLE_SHELL != ENABLED
#   error VSF_KERNEL_USE_SIMPLE_SHELL must be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vk_scsi_init(vk_scsi_t *pthis)
{
    vsf_err_t err;
    VSF_SCSI_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->init != NULL));
    __vsf_component_call_peda_ifs(vk_scsi_init, err, pthis->drv->init, 0, pthis);
    return err;
}

vsf_err_t vk_scsi_fini(vk_scsi_t *pthis)
{
    vsf_err_t err;
    VSF_SCSI_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->fini != NULL));
    __vsf_component_call_peda_ifs(vk_scsi_fini, err, pthis->drv->fini, 0, pthis);
    return err;
}

bool vk_scsi_prepare_buffer(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem)
{
    VSF_SCSI_ASSERT((pthis != NULL) && (pthis->drv != NULL));
    if (pthis->drv->buffer != NULL) {
        return pthis->drv->buffer(pthis, cbd, mem);
    }
    return false;
}

vsf_err_t vk_scsi_execute(vk_scsi_t *pthis, uint8_t *cbd, vsf_mem_t *mem)
{
    vsf_err_t err;
    vsf_mem_t execute_mem;
    VSF_SCSI_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->execute != NULL));

    if (mem != NULL) {
        execute_mem = *mem;
    } else {
        execute_mem.buffer = NULL;
        execute_mem.size = 0;
    }
    __vsf_component_call_peda_ifs(vk_scsi_execute, err, pthis->drv->execute, 0, pthis,
        .cbd    = cbd,
        .mem    = execute_mem,
#if VSF_USE_SIMPLE_STREAM == ENABLED
        .stream = NULL,
#endif
    );
    return err;
}

#if VSF_USE_SIMPLE_STREAM == ENABLED
vsf_err_t vk_scsi_execute_stream(vk_scsi_t *pthis, uint8_t *cbd, vsf_stream_t *stream)
{
    vsf_err_t err;
    VSF_SCSI_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->execute != NULL));
    __vsf_component_call_peda_ifs(vk_scsi_execute_stream, err, pthis->drv->execute_stream, 0, pthis,
        .cbd    = cbd,
        .stream = stream,
    );
    return err;
}
#endif

uint_fast8_t vk_scsi_get_command_len(uint8_t *cbd)
{
    scsi_group_code_t group_code = (scsi_group_code_t)(cbd[0] & 0xE0);
    switch (group_code) {
    case SCSI_GROUPCODE6:       return 6;
    case SCSI_GROUPCODE10_1:
    case SCSI_GROUPCODE10_2:    return 10;
    case SCSI_GROUPCODE16:      return 16;
    case SCSI_GROUPCODE12:      return 12;
    }
    VSF_SCSI_ASSERT(false);
    return 0;
}

bool vk_scsi_get_rw_param(uint8_t *cbd, uint64_t *addr, uint32_t *size)
{
    scsi_group_code_t group_code = (scsi_group_code_t)(cbd[0] & 0xE0);
    scsi_cmd_code_t cmd_code = (scsi_cmd_code_t)(cbd[0] & 0x1F);
    if ((SCSI_CMDCODE_READ == cmd_code) || (SCSI_CMDCODE_WRITE == cmd_code)) {
        switch (group_code) {
        case SCSI_GROUPCODE6:
            *addr = get_unaligned_be16(&cbd[2]);
            *size = cbd[4];
            break;
        case SCSI_GROUPCODE10_1:
            *addr = get_unaligned_be32(&cbd[2]);
            *size = get_unaligned_be16(&cbd[7]);
            break;
        case SCSI_GROUPCODE16:
            *addr = get_unaligned_be64(&cbd[2]);
            *size = get_unaligned_be32(&cbd[7]);
            break;
        case SCSI_GROUPCODE12:
            *addr = get_unaligned_be32(&cbd[2]);
            *size = get_unaligned_be32(&cbd[6]);
            break;
        default:
            return false;
        }
        return true;
    }
    return false;
}

#endif
