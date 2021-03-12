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
#define __VSF_MULTIPLEX_IIC_CLASS_IMPLEMENT
#define __VSF_MULTIPLEX_HAL_CLASS_INHERIT
#define __VSF_IO_IIC_CLASS_INHERIT
#include "./vsf_multiplex_iic.h"

#if VSF_USE_MULTIPLEX_HAL == ENABLED && VSF_USE_MULTIPLEX_IIC == ENABLED

#include "component/vsf_component.h"

/*============================ MACROS ========================================*/

#ifndef IIC_READ
#   define IIC_READ                     1
#endif

#ifndef IIC_WRITE
#   define IIC_WRITE                    0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_multiplex_iic_transact_handler(vsf_io_iic_t *io_iic, int_fast32_t remain_size)
{
    vsf_multiplex_iic_t *iic = (vsf_multiplex_iic_t *)io_iic;
    vsf_eda_post_evt(iic->notifier_eda, VSF_EVT_RETURN);
}

vsf_err_t vsf_multiplex_iic_init(vsf_multiplex_iic_t *iic)
{
    vsf_eda_mutex_init(&iic->mutex);
    iic->callback.handler = __vsf_multiplex_iic_transact_handler;
    return vsf_io_iic_init(&iic->use_as__vsf_io_iic_t);
}

__vsf_component_peda_private_entry(__vsf_multiplex_iic_rw,
    bool is_read;
    uint8_t chip_addr;
    uint8_t flags;
    uint8_t *buffer;
    uint32_t size;
) {
    vsf_peda_begin();
    vsf_multiplex_iic_t *iic = (vsf_multiplex_iic_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_mutex_enter(&iic->mutex)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        iic->state = 0;
        iic->notifier_eda = vsf_eda_get_cur();
        // fall through
    case VSF_EVT_RETURN:
        if (!iic->state) {
            if (vsf_local.is_read) {
                vsf_io_iic_read(&iic->use_as__vsf_io_iic_t, vsf_local.chip_addr, vsf_local.buffer, vsf_local.size, vsf_local.flags);
            } else {
                vsf_io_iic_write(&iic->use_as__vsf_io_iic_t, vsf_local.chip_addr, vsf_local.buffer, vsf_local.size, vsf_local.flags);
            }
        } else {
            vsf_eda_mutex_leave(&iic->mutex);
            vsf_eda_return();
        }
        iic->state++;
        break;
    }
    vsf_peda_end();
}

vsf_err_t vsf_multiplex_iic_read(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint8_t *buffer, uint_fast32_t size, uint_fast8_t flags)
{
    vsf_err_t err;
    VSF_OSA_HAL_ASSERT(iic != NULL);
    __vsf_component_call_peda(__vsf_multiplex_iic_rw, err, iic,
        .is_read        = true,
        .chip_addr      = chip_addr,
        .buffer         = buffer,
        .size           = size,
        .flags          = flags,
    )
    return err;
}

vsf_err_t vsf_multiplex_iic_write(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint8_t *buffer, uint_fast32_t size, uint_fast8_t flags)
{
    vsf_err_t err;
    VSF_OSA_HAL_ASSERT(iic != NULL);
    __vsf_component_call_peda(__vsf_multiplex_iic_rw, err, iic,
        .is_read        = false,
        .chip_addr      = chip_addr,
        .buffer         = buffer,
        .size           = size,
        .flags          = flags,
    )
    return err;
}

__vsf_component_peda_private_entry(__vsf_multiplex_iic_reg_access,
    bool is_read;
    uint8_t chip_addr;
    uint8_t reg_addr_size;
    uint32_t reg_addr;
    uint8_t *buffer;
    uint32_t size;
) {
    vsf_peda_begin();
    vsf_multiplex_iic_t *iic = (vsf_multiplex_iic_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_mutex_enter(&iic->mutex)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        iic->state = 0;
        iic->notifier_eda = vsf_eda_get_cur();
        // fall through
    case VSF_EVT_RETURN:
        if (vsf_local.is_read) {
            switch (iic->state) {
            case 0:
                vsf_io_iic_transact(&iic->use_as__vsf_io_iic_t, vsf_local.chip_addr | IIC_WRITE, (uint8_t *)&vsf_local.reg_addr, vsf_local.reg_addr_size, VSF_IO_IIC_NO_STOP);
                break;
            case 1:
                vsf_io_iic_transact(&iic->use_as__vsf_io_iic_t, vsf_local.chip_addr | IIC_READ, vsf_local.buffer, vsf_local.size, 0);
                break;
            case 2:
                vsf_eda_mutex_leave(&iic->mutex);
                vsf_eda_return();
                break;
            }
        } else {
            switch (iic->state) {
            case 0:
                vsf_io_iic_transact(&iic->use_as__vsf_io_iic_t, vsf_local.chip_addr | IIC_WRITE, (uint8_t *)&vsf_local.reg_addr, vsf_local.reg_addr_size, VSF_IO_IIC_NO_STOP);
                break;
            case 1:
                vsf_io_iic_transact(&iic->use_as__vsf_io_iic_t, vsf_local.chip_addr | IIC_WRITE, vsf_local.buffer, vsf_local.size, VSF_IO_IIC_NO_START | VSF_IO_IIC_NO_ADDR);
                break;
            case 2:
                vsf_eda_mutex_leave(&iic->mutex);
                vsf_eda_return();
                break;
            }
        }
        iic->state++;
        break;
    }
    vsf_peda_end();
}

vsf_err_t vsf_multiplex_iic_read_reg(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint_fast32_t reg_addr, uint_fast8_t reg_addr_size, uint8_t *buffer, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_OSA_HAL_ASSERT(iic != NULL);
    __vsf_component_call_peda(__vsf_multiplex_iic_reg_access, err, iic,
        .is_read        = true,
        .chip_addr      = chip_addr << 1,
        .reg_addr       = cpu_to_le32(reg_addr),
        .reg_addr_size  = reg_addr_size,
        .buffer         = buffer,
        .size           = size,
    )
    return err;
}

vsf_err_t vsf_multiplex_iic_write_reg(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint_fast32_t reg_addr, uint_fast8_t reg_addr_size, uint8_t *buffer, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_OSA_HAL_ASSERT(iic != NULL);
    __vsf_component_call_peda(__vsf_multiplex_iic_reg_access, err, iic,
        .is_read        = false,
        .chip_addr      = chip_addr << 1,
        .reg_addr       = cpu_to_le32(reg_addr),
        .reg_addr_size  = reg_addr_size,
        .buffer         = buffer,
        .size           = size,
    )
    return err;
}

__vsf_component_peda_private_entry(__vsf_multiplex_iic_execute,
    uint8_t chip_addr;
    uint8_t *cmd;
    uint32_t size;
) {
    vsf_peda_begin();
    vsf_multiplex_iic_t *iic = (vsf_multiplex_iic_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_RETURN:
    case VSF_EVT_INIT:
        if (vsf_local.size > 0) {
            // format: [msb|lsb]
            //  [4b-isread|4b-reg_size] [nB-reg] [2B-size] [nB-data]
            uint8_t *cmd = vsf_local.cmd;
            uint_fast8_t reg_size = cmd[0] & 0x0F;
            bool is_read = cmd[0] & 0xF0;
            uint_fast32_t reg, data_size;

            switch (reg_size) {
            case 1:     reg = cmd[1];                       cmd += 2;   break;
            case 2:     reg = get_unaligned_le16(&cmd[1]);  cmd += 3;   break;
            case 4:     reg = get_unaligned_le32(&cmd[1]);  cmd += 5;   break;
            default:    VSF_OSA_HAL_ASSERT(false);
            }

            data_size = get_unaligned_le16(cmd);
            if (is_read) {
                vsf_multiplex_iic_read_reg(iic, vsf_local.chip_addr, reg, reg_size, cmd + 2, data_size);
            } else {
                vsf_multiplex_iic_write_reg(iic, vsf_local.chip_addr, reg, reg_size, cmd + 2, data_size);
            }
            vsf_local.size -= reg_size + data_size + 3;
            vsf_local.cmd += reg_size + data_size + 3;
        } else {
            vsf_eda_return();
        }
        break;
    }
    vsf_peda_end();
}

vsf_err_t vsf_multiplex_iic_execute(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint8_t *cmd, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_OSA_HAL_ASSERT(iic != NULL);
    __vsf_component_call_peda(__vsf_multiplex_iic_execute, err, iic,
        .chip_addr      = chip_addr,
        .cmd            = cmd,
        .size           = size,
    )
    return err;
}

#endif
/* EOF */
