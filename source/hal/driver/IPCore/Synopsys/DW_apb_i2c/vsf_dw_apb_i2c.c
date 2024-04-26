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

#if VSF_HAL_USE_I2C == ENABLED

#define __VSF_HAL_DW_APB_I2C_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

// if __HAL_DRIVER_DW_APB_I2C_H__ is defined, means chip driver include "vsf_dw_apb_i2c.h".
#ifdef __HAL_DRIVER_DW_APB_I2C_H__

#include "./vsf_dw_apb_i2c_reg.h"

/*============================ MACROS ========================================*/

// IC_CON

#define I2C_IC_CON_IC_RESTART_EN_POS            5
#define I2C_IC_CON_IC_STOP_DET_IFADDRESSED_POS  7
#define I2C_IC_CON_TX_EMPTY_CTRL_POS            8
#define I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_POS    9

// IC_TAR

#define I2C_IC_TAR_POS                          12

// IC_INTR_XXXX

#define I2C_IC_INTR_RX_FULL_POS                 2
#define I2C_IC_INTR_RD_REQ_POS                  5
#define I2C_IC_INTR_TX_ABRT_POS                 6
#define I2C_IC_INTR_STOP_DET_POS                9

// IC_TX_ABRT

#define I2C_IC_TX_ABRT_ARBLOST                  12
#define I2C_IC_TX_ABRT_SLV_ARBLOST              14
#define I2C_IC_TX_ABRT_SLVRD_INTX               15
#define I2C_IC_TX_ABRT_USER_ABRT_POS            16

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_dw_apb_i2c_init(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_cfg_t *cfg_ptr, uint_fast32_t ic_clk_hz)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    vsf_dw_apb_i2c_reg_t *reg = dw_apb_i2c_ptr->reg;

    reg->IC_ENABLE.VALUE = 0;
    reg->IC_CON.VALUE = 0;
    reg->IC_INTR_MASK.VALUE = 0;

    reg->IC_CON.VALUE = (reg->IC_CON.VALUE & ~__VSF_DW_APB_I2C_MODE_MASK)
                    |   cfg_ptr->mode
                    |   (1 << I2C_IC_CON_IC_RESTART_EN_POS)
                    |   (1 << I2C_IC_CON_TX_EMPTY_CTRL_POS)
                    |   (1 << I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_POS);
    reg->IC_RX_TL.RX_TL = reg->IC_COMP_PARAM_1.RX_BUFFER_DEPTH;
    reg->IC_TX_TL.TX_TL = reg->IC_COMP_PARAM_1.TX_BUFFER_DEPTH;

    dw_apb_i2c_ptr->isr = cfg_ptr->isr;
    dw_apb_i2c_ptr->irq_mask = 0;
    dw_apb_i2c_ptr->is_master = !!(cfg_ptr->mode & VSF_I2C_MODE_MASTER);
    if (!dw_apb_i2c_ptr->is_master) {
        reg->IC_CON.VALUE |= (1 << I2C_IC_CON_IC_STOP_DET_IFADDRESSED_POS);
        reg->IC_SAR.IC_SAR = cfg_ptr->slave_addr & 0x3FF;

        // todo: set IC_SDA_HOLD and IC_SDA_SETUP
        reg->IC_SDA_HOLD.IC_SDA_TX_HOLD = 0;
        reg->IC_SDA_HOLD.IC_SDA_RX_HOLD = 0;
        reg->IC_SDA_SETUP.SDA_SETUP = 0;
    } else {
        uint32_t cycles_per_bit = (ic_clk_hz / cfg_ptr->clock_hz) >> 1;
        uint32_t *hcnt = NULL, *lcnt = NULL, *spklen;
        switch (cfg_ptr->mode & VSF_I2C_SPEED_HIGH_SPEED_MODE) {
        case VSF_I2C_SPEED_STANDARD_MODE:
            hcnt = (uint32_t *)&reg->IC_SS_SCL_HCNT;
            lcnt = (uint32_t *)&reg->IC_SS_SCL_LCNT;
            spklen = (uint32_t *)&reg->IC_FS_SPKLEN;
            break;
        case VSF_I2C_SPEED_FAST_MODE:
            hcnt = (uint32_t *)&reg->IC_FS_SCL_HCNT;
            lcnt = (uint32_t *)&reg->IC_FS_SCL_LCNT;
            spklen = (uint32_t *)&reg->IC_FS_SPKLEN;
            break;
        case VSF_I2C_SPEED_HIGH_SPEED_MODE:
            hcnt = (uint32_t *)&reg->IC_HS_SCL_HCNT;
            lcnt = (uint32_t *)&reg->IC_HS_SCL_LCNT;
            spklen = (uint32_t *)&reg->IC_HS_SPKLEN;
            break;
        }
        // high = hcnt + IC_XX_SPKLEN + 7
        // low = lcnt + 1
        *lcnt = cycles_per_bit - 1;
        *hcnt = cycles_per_bit - 7 - (*spklen & 0xFF);
    }
    return VSF_ERR_NONE;
}

void vsf_dw_apb_i2c_fini(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_dw_apb_i2c_reg_t *reg = dw_apb_i2c_ptr->reg;

    reg->IC_ENABLE.VALUE = 0;
    reg->IC_CON.VALUE = 0;
    reg->IC_INTR_MASK.VALUE = 0;
}

fsm_rt_t vsf_dw_apb_i2c_enable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_dw_apb_i2c_disable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);

    return fsm_rt_cpl;
}

vsf_i2c_irq_mask_t vsf_dw_apb_i2c_irq_enable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_dw_apb_i2c_reg_t *reg = dw_apb_i2c_ptr->reg;

    dw_apb_i2c_ptr->irq_mask |= irq_mask;
    reg->IC_INTR_MASK.VALUE |= irq_mask & __VSF_DW_APB_I2C_IRQ_MASK;
    if (dw_apb_i2c_ptr->is_master) {
        reg->IC_INTR_MASK.VALUE |= (1 << I2C_IC_INTR_TX_ABRT_POS) | (1 << I2C_IC_INTR_RX_FULL_POS);
    } else {
        reg->IC_INTR_MASK.VALUE |= (1 << I2C_IC_INTR_RD_REQ_POS) | (1 << I2C_IC_INTR_TX_ABRT_POS);
    }
    return (vsf_i2c_irq_mask_t)reg->IC_INTR_MASK.VALUE & __VSF_DW_APB_I2C_IRQ_MASK;
}

vsf_i2c_irq_mask_t vsf_dw_apb_i2c_irq_disable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_dw_apb_i2c_reg_t *reg = dw_apb_i2c_ptr->reg;

    dw_apb_i2c_ptr->irq_mask &= ~irq_mask;
    reg->IC_INTR_MASK.VALUE &= ~(irq_mask & __VSF_DW_APB_I2C_IRQ_MASK);
    return (vsf_i2c_irq_mask_t)reg->IC_INTR_MASK.VALUE & __VSF_DW_APB_I2C_IRQ_MASK;
}

vsf_i2c_status_t vsf_dw_apb_i2c_status(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_dw_apb_i2c_reg_t *reg = dw_apb_i2c_ptr->reg;
    return (vsf_i2c_status_t){
        .use_as__vsf_peripheral_status_t.is_busy = reg->IC_STATUS.IC_STATUS_ACTIVITY,
    };
}

static void __vsf_dw_apb_i2c_continue(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_dw_apb_i2c_reg_t *reg = dw_apb_i2c_ptr->reg;
    uint32_t stop;

    if (dw_apb_i2c_ptr->is_read) {
        while (reg->IC_STATUS.RFNE) {
            *dw_apb_i2c_ptr->ptr++ = reg->IC_DATA_CMD.DAT;
            dw_apb_i2c_ptr->count--;

            if (dw_apb_i2c_ptr->rx_req_count > 0) {
                stop = dw_apb_i2c_ptr->need_stop && (1 == dw_apb_i2c_ptr->rx_req_count) ? VSF_I2C_CMD_STOP : 0;
                dw_apb_i2c_ptr->rx_req_count--;
                reg->IC_DATA_CMD.VALUE = VSF_I2C_CMD_READ | stop;
            }
        }
    } else {
        while (dw_apb_i2c_ptr->count && reg->IC_STATUS.TFNF) {
            stop = dw_apb_i2c_ptr->need_stop && (1 == dw_apb_i2c_ptr->count) ? VSF_I2C_CMD_STOP : 0;
            reg->IC_DATA_CMD.VALUE = *dw_apb_i2c_ptr->ptr++ | stop;
            dw_apb_i2c_ptr->count--;
        }
    }
}

static void __vsf_dw_apb_i2c_notify(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_irq_mask_t mask)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);

    mask &= dw_apb_i2c_ptr->irq_mask;
    if (mask && (dw_apb_i2c_ptr->isr.handler_fn != NULL)) {
        dw_apb_i2c_ptr->isr.handler_fn(dw_apb_i2c_ptr->isr.target_ptr,
                (vsf_i2c_t *)dw_apb_i2c_ptr, mask);
    }
}

void vsf_dw_apb_i2c_irqhandler(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_dw_apb_i2c_reg_t *reg = dw_apb_i2c_ptr->reg;

    vsf_i2c_irq_mask_t mask = reg->IC_INTR_STAT.VALUE;
    vsf_i2c_irq_mask_t mask_notify = mask & __VSF_DW_APB_I2C_IRQ_MASK;

    if (!mask) {
        return;
    }
    if (mask & (1 << I2C_IC_INTR_TX_ABRT_POS)) {
        mask_notify |= reg->IC_TX_ABRT_SOURCE.VALUE & __VSF_DW_APB_I2C_ABRT_MASK;
        (void)reg->IC_CLR_TX_ABRT.VALUE;
    }
    if (mask & (1 << I2C_IC_INTR_RD_REQ_POS)) {
        (void)reg->IC_CLR_RD_REQ.VALUE;
    }
    if (mask & (1 << I2C_IC_INTR_STOP_DET_POS)) {
        (void)reg->IC_CLR_STOP_DET.VALUE;
        mask_notify |=  VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                    |   VSF_I2C_IRQ_MASK_MASTER_STOPPED;
    }

    if (dw_apb_i2c_ptr->is_master) {
    } else {
        if (mask & (1 << I2C_IC_INTR_RD_REQ_POS)) {
        }
    }

    if (dw_apb_i2c_ptr->count > 0) {
        mask_notify &= ~VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY;
        if (!(mask_notify & __VSF_DW_APB_I2C_ERROR_MASK)) {
            __vsf_dw_apb_i2c_continue(dw_apb_i2c_ptr);
            if (dw_apb_i2c_ptr->is_read && !dw_apb_i2c_ptr->count) {
                if (!dw_apb_i2c_ptr->need_stop) {
                    mask_notify |= VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;
                }
            }
        }
    } else {
        if (!(mask_notify & __VSF_DW_APB_I2C_ERROR_MASK)) {
            if (!dw_apb_i2c_ptr->is_read) {
                reg->IC_INTR_MASK.VALUE &= ~VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY;
                mask_notify |= VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY;
            }
            if (dw_apb_i2c_ptr->need_stop) {
                mask_notify |= VSF_I2C_IRQ_MASK_MASTER_STOPPED;
            }
        }
        if (!dw_apb_i2c_ptr->need_stop) {
            mask_notify |= VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;
        }
    }
    if (mask_notify != 0) {
        __vsf_dw_apb_i2c_notify(dw_apb_i2c_ptr, mask_notify);
    }
}

vsf_err_t vsf_dw_apb_i2c_master_request(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
            uint16_t address, vsf_i2c_cmd_t cmd, uint16_t count, uint8_t *buffer)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_dw_apb_i2c_reg_t *reg = dw_apb_i2c_ptr->reg;

    if ((cmd & (VSF_I2C_CMD_STOP | VSF_I2C_CMD_RESTART)) && (0 == count)) {
        return VSF_ERR_NOT_SUPPORT;
    }

    reg->IC_ENABLE.ENABLE = 0;
    dw_apb_i2c_ptr->need_stop = !!(cmd & VSF_I2C_CMD_STOP);
    dw_apb_i2c_ptr->is_read = !!(cmd & VSF_I2C_CMD_READ);
    cmd &= ~VSF_I2C_CMD_STOP;
    if (cmd & VSF_I2C_CMD_10_BITS) {
        reg->IC_CON.IC_10BITADDR_MASTER = 1;
        reg->IC_TAR.IC_TAR = (address & 0x3FF) | (1 << I2C_IC_TAR_POS);
    } else {
        reg->IC_CON.IC_10BITADDR_MASTER = 0;
        reg->IC_TAR.IC_TAR = address & 0x7F;
    }
    reg->IC_ENABLE.ENABLE = 1;

    if (dw_apb_i2c_ptr->need_stop) {
        reg->IC_INTR_MASK.VALUE |= 1 << I2C_IC_INTR_STOP_DET_POS;
    }
    if (count > 0) {
        dw_apb_i2c_ptr->ptr = buffer;
        dw_apb_i2c_ptr->count = count;

        if (dw_apb_i2c_ptr->is_read) {
            uint16_t rx_fifo_max = reg->IC_COMP_PARAM_1.RX_BUFFER_DEPTH + 1;
            uint32_t stop;

            dw_apb_i2c_ptr->rx_req_count = count;
            count = vsf_min(count, rx_fifo_max);
            for (uint16_t i = 0; i < count; i++) {
                stop = dw_apb_i2c_ptr->need_stop && (1 == dw_apb_i2c_ptr->rx_req_count) ? VSF_I2C_CMD_STOP : 0;
                dw_apb_i2c_ptr->rx_req_count--;
                reg->IC_DATA_CMD.VALUE = VSF_I2C_CMD_READ | stop;
            }
        }
        __vsf_dw_apb_i2c_continue(dw_apb_i2c_ptr);

        if (!(cmd & VSF_I2C_CMD_READ)) {
            // enable VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY for VSF_I2C_CMD_WRITE
            reg->IC_INTR_MASK.VALUE |= VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY;
        }
    }

    if (cmd & VSF_I2C_CMD_START) {
        __vsf_dw_apb_i2c_notify(dw_apb_i2c_ptr, VSF_I2C_IRQ_MASK_MASTER_STARTED);
    }
    return VSF_ERR_NONE;
}

uint_fast32_t vsf_dw_apb_i2c_get_transferred_count(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    return 0;
}

vsf_i2c_capability_t vsf_dw_apb_i2c_capability(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_i2c_capability_t capability = {
        .irq_mask = VSF_DW_APB_I2C_IRQ_MASK_ALL,
        .support_restart = true,
    };
    return capability;
}

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#endif      // __HAL_DRIVER_DW_APB_I2C_H__
#endif      // VSF_HAL_USE_I2C
