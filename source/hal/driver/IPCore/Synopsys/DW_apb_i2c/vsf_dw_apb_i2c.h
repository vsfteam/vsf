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

#ifndef __HAL_DRIVER_DW_APB_I2C_H__
#define __HAL_DRIVER_DW_APB_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2C == ENABLED

#include "./vsf_dw_apb_i2c_reg.h"

#if     defined(__VSF_HAL_DW_APB_I2C_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_DW_APB_I2C_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_DW_APB_I2C_CFG_MULTI_CLASS
#   define VSF_DW_APB_I2C_CFG_MULTI_CLASS       VSF_I2C_CFG_MULTI_CLASS
#endif

#define VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE       ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD        ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK   ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL       ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_i2c_mode_t {
    VSF_I2C_MODE_MASTER                         = (0x1ul << 0) | (0x1ul << 6),
    VSF_I2C_MODE_SLAVE                          = (0x0ul << 0),

    // TODO: Ultra Fast-mode I2C-bus protocol
    VSF_I2C_SPEED_STANDARD_MODE                 = (0x1ul << 1), // up to 100 kbit/s
    VSF_I2C_SPEED_FAST_MODE                     = (0x2ul << 1), // up to 400 kbit/s
    VSF_I2C_SPEED_FAST_MODE_PLUS                = (0x2ul << 1), // up to 1 Mbit/s
    VSF_I2C_SPEED_HIGH_SPEED_MODE               = (0x3ul << 1), // up to 3.4 Mbit/s

    VSF_I2C_ADDR_7_BITS                         = (0x0ul << 3),
    VSF_I2C_ADDR_10_BITS                        = (0x3ul << 3),

    __VSF_DW_APB_I2C_MODE_MASK                  = VSF_I2C_MODE_MASTER
                                                | VSF_I2C_SPEED_HIGH_SPEED_MODE
                                                | VSF_I2C_ADDR_10_BITS,
} vsf_i2c_mode_t;

typedef enum vsf_i2c_cmd_t {
    VSF_I2C_CMD_WRITE                           = (0x00ul << 8),
    VSF_I2C_CMD_READ                            = (0x01ul << 8),

    VSF_I2C_CMD_START                           = (0x01ul << 16),   // virtual
    VSF_I2C_CMD_RESTART                         = (0x01ul << 10),
    VSF_I2C_CMD_STOP                            = (0x01ul << 9),

    VSF_I2C_CMD_7_BITS                          = (0x00ul << 17),
    VSF_I2C_CMD_10_BITS                         = (0x01ul << 17),

    __VSF_DW_APB_I2C_CMD_MASK                   = VSF_I2C_CMD_WRITE
                                                | VSF_I2C_CMD_READ
                                                | VSF_I2C_CMD_RESTART
                                                | VSF_I2C_CMD_STOP,

    VSF_I2C_CMD_NO_START                        = (0x01ul << 24),
    VSF_I2C_CMD_NO_STOP_RESTART                 = (0x01ul << 25),
} vsf_i2c_cmd_t;

typedef enum vsf_i2c_irq_mask_t {
    // 0..14 are hardware IRQs, 16..26 are virtually supported IRQs, 27~28 are unsupported IRQs
    VSF_I2C_IRQ_MASK_RX_UNDER                   = (0x01ul << 0),    // INTR.RX_UNDER
    VSF_I2C_IRQ_MASK_RX_OVER                    = (0x01ul << 1),    // INTR.RX_OVER
    VSF_I2C_IRQ_MASK_RX_FULL                    = (0x01ul << 2),    // INTR.RX_FULL
    VSF_I2C_IRQ_MASK_TX_OVER                    = (0x01ul << 3),    // INTR.RX_OVER
    VSF_I2C_IRQ_MASK_TX_EMPTY                   = (0x01ul << 4),    // INTR.TX_EMPTY
    VSF_I2C_IRQ_MASK_SLAVE_RD_REQ               = (0x01ul << 5),    // INTR.RD_REQ
    VSF_I2C_IRQ_MASK_TX_ABORT                   = (0x01ul << 6),    // INTR.TX_ABORT
    VSF_I2C_IRQ_MASK_RX_DONE                    = (0x01ul << 7),    // INTR.RX_DONE
    VSF_I2C_IRQ_MASK_ACTIVITY                   = (0x01ul << 8),    // INTR.ACTIVITY
    VSF_I2C_IRQ_MASK_STOP_DET                   = (0x01ul << 9),    // INTR.STOP_DET
    VSF_I2C_IRQ_MASK_START_DET                  = (0x01ul << 10),   // INTR.START_DET
    VSF_I2C_IRQ_MASK_GEN_CALL                   = (0x01ul << 11),   // INTR.GEN_CALL
    VSF_I2C_IRQ_MASK_RESTART_DET                = (0x01ul << 12),   // INTR.RESTART_DET
    VSF_I2C_IRQ_MASK_MST_ON_HOLD                = (0x01ul << 13),   // INTR.MST_ON_HOLD
    VSF_I2C_IRQ_MASK_SCL_STUCK_AT_LOW           = (0x01ul << 14),   // INTR.SCL_STUCK_AT_LOW

    // bit 4
    VSF_I2C_IRQ_MASK_MASTER_TX                  = VSF_I2C_IRQ_MASK_TX_EMPTY,
    // bit 2
    VSF_I2C_IRQ_MASK_MASTER_RX                  = VSF_I2C_IRQ_MASK_RX_FULL,
    VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT
                                                = VSF_I2C_IRQ_MASK_START_DET,
    VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT         = VSF_I2C_IRQ_MASK_STOP_DET,


    VSF_I2C_IRQ_MASK_SLAVE_TX                   = VSF_I2C_IRQ_MASK_SLAVE_RD_REQ,
    __VSF_DW_APB_I2C_SLAVE_OFFSET               = 16,
    // bit 18
    //VSF_I2C_IRQ_MASK_SLAVE_TX                   = VSF_I2C_IRQ_MASK_TX_EMPTY    << __VSF_DW_APB_I2C_SLAVE_OFFSET,
    // bit 20
    VSF_I2C_IRQ_MASK_SLAVE_RX                   = VSF_I2C_IRQ_MASK_RX_FULL     << __VSF_DW_APB_I2C_SLAVE_OFFSET,
    // bit 25
    VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT          = VSF_I2C_IRQ_MASK_STOP_DET    << __VSF_DW_APB_I2C_SLAVE_OFFSET,
    // bit 26
    VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT
                                                = VSF_I2C_IRQ_MASK_START_DET   << __VSF_DW_APB_I2C_SLAVE_OFFSET,


    VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE   = (0x01ul << 19),   // virtual
    VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE    = (0x01ul << 21),   // virtual

    // TX_ABRT.ABRT_7B_ADDR_NOACK (bit 0) | TX_ABRT.ABRT_10ADDR1_NOACK (bit 1) | TX_ABRT.ABRT_10ADDR2_NOACK (bit 2)
    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK        = (0x01ul << 22),
    // TX_ABRT.ABRT_TXDATA_NOACK (bit 3)
    VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT      = (0x01ul << 23),
    // TX_ABRT.ARB_LOST (bit 12)
    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST    = (0x01ul << 24),

    VSF_I2C_IRQ_MASK_MASTER_ERR                 = (0x01ul << 27),   // virtual, not supported
    VSF_I2C_IRQ_MASK_SLAVE_ERR                  = (0x01ul << 28),   // virtual, not supported

    VSF_DW_APB_I2C_IRQ_MASK_ALL                 = VSF_I2C_IRQ_MASK_MASTER_TX
                                                | VSF_I2C_IRQ_MASK_MASTER_RX
                                                | VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT
                                                | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT
                                                | VSF_I2C_IRQ_MASK_SLAVE_TX
                                                | VSF_I2C_IRQ_MASK_SLAVE_RX
                                                | VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT
                                                | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT
                                                | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                                                | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE
                                                | VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT
                                                | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                                | VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK,
    __VSF_DW_APB_I2C_SLAVE_IRQ_MASK             = VSF_I2C_IRQ_MASK_SLAVE_RX
                                                | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT
                                                | VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT,
    __VSF_DW_APB_I2C_IRQ_MASK                   = 0x7FF,
    __VSF_DW_APB_I2C_ERROR_MASK                 = VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT
                                                | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                                | VSF_I2C_IRQ_MASK_MASTER_ERR
                                                | VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK,
} vsf_i2c_irq_mask_t;

typedef enum vsf_i2c_ctrl_t {
    VSF_I2C_CTRL_GET_MODE,
    #define VSF_I2C_CTRL_GET_MODE           VSF_I2C_CTRL_GET_MODE

    VSF_I2C_CTRL_SET_MODE,
    #define VSF_I2C_CTRL_SET_MODE           VSF_I2C_CTRL_SET_MODE
    
    VSF_I2C_CTRL_SLAVE_GET_ADDRESS,
    #define VSF_I2C_CTRL_SLAVE_GET_ADDRESS  VSF_I2C_CTRL_SLAVE_GET_ADDRESS
    
    VSF_I2C_CTRL_SLAVE_SET_ADDRESS,
    #define VSF_I2C_CTRL_SLAVE_SET_ADDRESS  VSF_I2C_CTRL_SLAVE_SET_ADDRESS
} vsf_i2c_ctrl_t;

/*============================ INCLUDES ======================================*/

// IP cores which will export class(es), need to include template before definition
//  of the class. Because the class member(s) maybe depend on the template.
//#include "hal/driver/common/template/vsf_template_i2c.h"
#include "hal/driver/common/template/vsf_template_i2c.h"

/*============================ TYPES =========================================*/

vsf_class(vsf_dw_apb_i2c_t) {
#if VSF_DW_APB_I2C_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_i2c_t               vsf_i2c;
    )
#endif
    public_member(
        vsf_dw_apb_i2c_reg_t    *reg;
    )
    private_member(
        vsf_i2c_isr_t           isr;
        vsf_i2c_irq_mask_t      irq_mask;
        uint8_t                 *ptr;
        uint_fast16_t           master_request_count;
        uint_fast16_t           slave_request_count;
        uint_fast16_t           rx_req_count;
        uint_fast16_t           rx_fifo_count;

        uint16_t                is_master           : 1;
        uint16_t                is_master_request   : 1;
        uint16_t                is_slave_request    : 1;
        uint16_t                master_fifo_started : 1;
        uint16_t                is_read             : 1;
        uint16_t                need_stop           : 1;
        uint16_t                stop_detect         : 1;
    )
};

/*============================ PROTOTYPES ======================================*/

extern vsf_err_t vsf_dw_apb_i2c_init(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
        vsf_i2c_cfg_t *cfg_ptr, uint_fast32_t ic_clk_hz);
extern void vsf_dw_apb_i2c_fini(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern fsm_rt_t vsf_dw_apb_i2c_enable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern fsm_rt_t vsf_dw_apb_i2c_disable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern vsf_i2c_irq_mask_t vsf_dw_apb_i2c_irq_enable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
        vsf_i2c_irq_mask_t irq_mask);
extern vsf_i2c_irq_mask_t vsf_dw_apb_i2c_irq_disable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
        vsf_i2c_irq_mask_t irq_mask);
extern vsf_i2c_status_t vsf_dw_apb_i2c_status(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern void vsf_dw_apb_i2c_isrhandler(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern void vsf_dw_apb_i2c_master_fifo_transfer(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
    uint16_t address, vsf_i2c_cmd_t cmd, uint_fast16_t count, uint8_t *buffer_ptr,
    vsf_i2c_cmd_t *cur_cmd_ptr, uint_fast16_t *offset_ptr);
extern uint_fast16_t vsf_dw_apb_i2c_slave_fifo_transfer(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
    bool transmit_or_receive, uint_fast16_t count, uint8_t *buffer_ptr);
extern vsf_err_t vsf_dw_apb_i2c_master_request(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
    uint16_t address, vsf_i2c_cmd_t cmd, uint_fast16_t count, uint8_t *buffer);
extern vsf_err_t vsf_dw_apb_i2c_slave_request(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
    bool transmit_or_receive, uint_fast16_t count, uint8_t *buffer_ptr);
extern uint_fast32_t vsf_dw_apb_i2c_master_get_transferred_count(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern uint_fast32_t vsf_dw_apb_i2c_slave_get_transferred_count(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern vsf_err_t vsf_dw_apb_i2c_ctrl(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_ctrl_t ctrl, void * param);
extern vsf_i2c_capability_t vsf_dw_apb_i2c_capability(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern void vsf_dw_apb_i2c_irqhandler(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);


#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_I2C == ENABLED
#endif
/* EOF */
