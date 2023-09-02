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

#ifndef __VSF_DW_APB_I2C_REG_H__
#define __VSF_DW_APB_I2C_REG_H__

/*============================ INCLUDES ======================================*/

/*! \note i_reg_xxxx.h header files must be standalong and assume following
 *!       conditions:
 *!       a. stdint.h exists
 *!       b. anonymous structures and unions are supported
 */
#include "utilities/compiler/__common/__type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/* Define structure member permissions : read only/write only/read write */
#ifndef __IM
#   define __IM                                     const
#endif
#ifndef __OM
#   define __OM
#endif
#ifndef __IOM
#   define __IOM
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_DEF_REG
#   define VSF_DEF_REG(__NAME, __TOTAL_SIZE, ...)                               \
    union {                                                                     \
        struct {                                                                \
            __VA_ARGS__                                                         \
        };                                                                      \
        reg##__TOTAL_SIZE##_t VALUE;                                            \
    } __NAME
#endif

/*============================ TYPES =========================================*/

// refer to: https://www.intel.com/content/www/us/en/programmable/hps/stratix-10/index.html#uvh1505405814792.html
typedef struct vsf_dw_apb_i2c_reg_t {
    VSF_DEF_REG(IC_CON, 32,
        __IOM       reg32_t         MASTER_MODE                 : 1;
        __IOM       reg32_t         SPEED                       : 2;
        __IOM       reg32_t         IC_10BITADDR_SLAVE          : 1;
        __IOM       reg32_t         IC_10BITADDR_MASTER         : 1;
        __IOM       reg32_t         IC_RESTART_EN               : 1;
        __IOM       reg32_t         IC_SLAVE_DISABLE            : 1;
        __IOM       reg32_t         STOP_DET_IFADDRESSED        : 1;
        __IOM       reg32_t         TX_EMPTY_CTRL               : 1;
        __IOM       reg32_t         RX_FIFO_FULL_HLD_CTRL       : 1;
        __IM        reg32_t         STOP_DET_IF_MASTER_ACTIVE   : 1;
    );
    VSF_DEF_REG(IC_TAR, 32,
        __IOM       reg32_t         IC_TAR                      : 10;
        __IOM       reg32_t         GC_OR_START                 : 1;
        __IOM       reg32_t         SPECIAL                     : 1;
        __IOM       reg32_t         IC_10BITADDR_MASTER         : 1;
    );
    VSF_DEF_REG(IC_SAR, 32,
        __IOM       reg32_t         IC_SAR                      : 10;
    );
    VSF_DEF_REG(IC_HS_MADDR, 32,
        __IOM       reg32_t         HS_MADDR                    : 3;
    );
    VSF_DEF_REG(IC_DATA_CMD, 32,
        __IOM       reg32_t         DAT                         : 8;
        __OM        reg32_t         CMD                         : 1;
        __OM        reg32_t         STOP                        : 1;
        __OM        reg32_t         RESTART                     : 1;
        __IM        reg32_t         FIRST_DATA_BYTE             : 1;
    );
    VSF_DEF_REG(IC_SS_SCL_HCNT, 32,
        __IOM       reg32_t         IC_SS_SCL_HCNT              : 16;
    );
    VSF_DEF_REG(IC_SS_SCL_LCNT, 32,
        __IOM       reg32_t         IC_SS_SCL_LCNT              : 16;
    );
    VSF_DEF_REG(IC_FS_SCL_HCNT, 32,
        __IOM       reg32_t         IC_FS_SCL_HCNT              : 16;
    );
    VSF_DEF_REG(IC_FS_SCL_LCNT, 32,
        __IOM       reg32_t         IC_FS_SCL_LCNT              : 16;
    );
    VSF_DEF_REG(IC_HS_SCL_HCNT, 32,
        __IOM       reg32_t         IC_HS_SCL_HCNT              : 16;
    );
    VSF_DEF_REG(IC_HS_SCL_LCNT, 32,
        __IOM       reg32_t         IC_HS_SCL_LCNT              : 16;
    );
    VSF_DEF_REG(IC_INTR_STAT, 32,
        __IM        reg32_t         R_RX_UNDER                  : 1;
        __IM        reg32_t         R_RX_OVER                   : 1;
        __IM        reg32_t         R_RX_FULL                   : 1;
        __IM        reg32_t         R_TX_OVER                   : 1;
        __IM        reg32_t         R_TX_EMPTY                  : 1;
        __IM        reg32_t         R_RD_REQ                    : 1;
        __IM        reg32_t         R_TX_ABRT                   : 1;
        __IM        reg32_t         R_RX_DONE                   : 1;
        __IM        reg32_t         R_ACTIVITY                  : 1;
        __IM        reg32_t         R_STOP_DET                  : 1;
        __IM        reg32_t         R_START_DET                 : 1;
        __IM        reg32_t         R_GEN_CALL                  : 1;
        __IM        reg32_t         R_RESTART_DET               : 1;
        __IM        reg32_t         R_MASTER_ON_HOLD            : 1;
    );
    VSF_DEF_REG(IC_INTR_MASK, 32,
        __IOM       reg32_t         M_RX_UNDER                  : 1;
        __IOM       reg32_t         M_RX_OVER                   : 1;
        __IOM       reg32_t         M_RX_FULL                   : 1;
        __IOM       reg32_t         M_TX_OVER                   : 1;
        __IOM       reg32_t         M_TX_EMPTY                  : 1;
        __IOM       reg32_t         M_RD_REQ                    : 1;
        __IOM       reg32_t         M_TX_ABRT                   : 1;
        __IOM       reg32_t         M_RX_DONE                   : 1;
        __IOM       reg32_t         M_ACTIVITY                  : 1;
        __IOM       reg32_t         M_STOP_DET                  : 1;
        __IOM       reg32_t         M_START_DET                 : 1;
        __IOM       reg32_t         M_GEN_CALL                  : 1;
        __IOM       reg32_t         M_RESTART_DET               : 1;
        __IOM       reg32_t         M_MASTER_ON_HOLD            : 1;
    );
    VSF_DEF_REG(IC_RAW_INTR_STAT, 32,
        __IM        reg32_t         RX_UNDER                    : 1;
        __IM        reg32_t         RX_OVER                     : 1;
        __IM        reg32_t         RX_FULL                     : 1;
        __IM        reg32_t         TX_OVER                     : 1;
        __IM        reg32_t         TX_EMPTY                    : 1;
        __IM        reg32_t         RD_REQ                      : 1;
        __IM        reg32_t         TX_ABRT                     : 1;
        __IM        reg32_t         RX_DONE                     : 1;
        __IM        reg32_t         ACTIVITY                    : 1;
        __IM        reg32_t         STOP_DET                    : 1;
        __IM        reg32_t         START_DET                   : 1;
        __IM        reg32_t         GEN_CALL                    : 1;
        __IM        reg32_t         RESTART_DET                 : 1;
        __IOM       reg32_t         MASTER_ON_HOLD              : 1;
    );
    VSF_DEF_REG(IC_RX_TL, 32,
        __IOM       reg32_t         RX_TL                       : 8;
    );
    VSF_DEF_REG(IC_TX_TL, 32,
        __IOM       reg32_t         TX_TL                       : 8;
    );
    VSF_DEF_REG(IC_CLR_INTR, 32,
        __IM        reg32_t         CLR_INTR                    : 1;
    );
    VSF_DEF_REG(IC_CLR_RX_UNDER, 32,
        __IM        reg32_t         CLR_RX_UNDER                : 1;
    );
    VSF_DEF_REG(IC_CLR_RX_OVER, 32,
        __IM        reg32_t         CLR_RX_OVER                 : 1;
    );
    VSF_DEF_REG(IC_CLR_TX_OVER, 32,
        __IM        reg32_t         CLR_TX_OVER                 : 1;
    );
    VSF_DEF_REG(IC_CLR_RD_REQ, 32,
        __IM        reg32_t         CLR_RD_REQ                  : 1;
    );
    VSF_DEF_REG(IC_CLR_TX_ABRT, 32,
        __IM        reg32_t         CLR_TX_ABRT                 : 1;
    );
    VSF_DEF_REG(IC_CLR_RX_DONE, 32,
        __IM        reg32_t         CLR_RX_DONE                 : 1;
    );
    VSF_DEF_REG(IC_CLR_ACTIVITY, 32,
        __IM        reg32_t         CLR_ACTIVITY                : 1;
    );
    VSF_DEF_REG(IC_CLR_STOP_DET, 32,
        __IM        reg32_t         CLR_STOP_DET                : 1;
    );
    VSF_DEF_REG(IC_CLR_START_DET, 32,
        __IM        reg32_t         CLR_START_DET               : 1;
    );
    VSF_DEF_REG(IC_CLR_GEN_CALL, 32,
        __IM        reg32_t         CLR_GEN_CALL                : 1;
    );
    VSF_DEF_REG(IC_ENABLE, 32,
        __IOM       reg32_t         ENABLE                      : 1;
        __IOM       reg32_t         ABORT                       : 1;
        __IOM       reg32_t         TX_CMD_BLOCK                : 1;
    );
    VSF_DEF_REG(IC_STATUS, 32,
        __IM        reg32_t         IC_STATUS_ACTIVITY          : 1;
        __IM        reg32_t         TFNF                        : 1;
        __IM        reg32_t         TFE                         : 1;
        __IM        reg32_t         RFNE                        : 1;
        __IM        reg32_t         RFF                         : 1;
        __IM        reg32_t         MST_AVTIVITY                : 1;
        __IM        reg32_t         SLV_ACTIVITY                : 1;
    );
    VSF_DEF_REG(IC_TXFLR, 32,
        __IM        reg32_t         TXFLR                       : 7;
    );
    VSF_DEF_REG(IC_RXFLR, 32,
        __IM        reg32_t         RXFLR                       : 7;
    );
    VSF_DEF_REG(IC_SDA_HOLD, 32,
        __IOM       reg32_t         IC_SDA_TX_HOLD              : 16;
        __IOM       reg32_t         IC_SDA_RX_HOLD              : 8;
    );
    VSF_DEF_REG(IC_TX_ABRT_SOURCE, 32,
        __IM        reg32_t         ABRT_7B_ADDR_NOACK          : 1;
        __IM        reg32_t         ABRT_10ADDR1_NOACK          : 1;
        __IM        reg32_t         ABRT_10ADDR2_NOACK          : 1;
        __IM        reg32_t         ABRT_TXDATA_NOACK           : 1;
        __IM        reg32_t         ABRT_GCALL_NOACK            : 1;
        __IM        reg32_t         ABRT_GCALL_READ             : 1;
        __IM        reg32_t         ABRT_HS_ACKDET              : 1;
        __IM        reg32_t         ABRT_SBYTE_ACKDET           : 1;
        __IM        reg32_t         ABRT_HS_NORSTRT             : 1;
        __IM        reg32_t         ABRT_SBYTE_NORSTRT          : 1;
        __IM        reg32_t         ABRT_10B_RD_NORSTRT         : 1;
        __IM        reg32_t         ABRT_MASTER_DIS             : 1;
        __IM        reg32_t         ARB_LOST                    : 1;
        __IM        reg32_t         ABRT_SLVFLUSH_TXFIFO        : 1;
        __IM        reg32_t         ABRT_SLV_ARBLOST            : 1;
        __IM        reg32_t         ABRT_SLVRD_INTX             : 1;
        __IM        reg32_t         ABRT_USER_ABRT              : 1;
        __IM        reg32_t                                     : 6;
        __IM        reg32_t         ABRT_TX_FLUSH_CNT           : 9;
    );
    VSF_DEF_REG(IC_SLV_DATA_NACK_ONLY, 32,
        __IOM       reg32_t         NACK                        : 1;
    );
    VSF_DEF_REG(IC_DMA_CR, 32,
        __IOM       reg32_t         RDMAE                       : 1;
        __IOM       reg32_t         TDMAE                       : 1;
    );
    VSF_DEF_REG(IC_DMA_TDLR, 32,
        __IOM       reg32_t         DMATDL                      : 6;
    );
    VSF_DEF_REG(IC_DMA_RDLR, 32,
        __IOM       reg32_t         DMARDL                      : 6;
    );
    VSF_DEF_REG(IC_SDA_SETUP, 32,
        __IOM       reg32_t         SDA_SETUP                   : 8;
    );
    VSF_DEF_REG(IC_ACK_GENERAL_CALL, 32,
        __IOM       reg32_t         ACK_GEN_CALL                : 1;
    );
    VSF_DEF_REG(IC_ENABLE_STATUS, 32,
        __IM        reg32_t         IC_EN                       : 1;
        __IM        reg32_t         SLV_DISABLED_WHILE_BUSY     : 1;
        __IM        reg32_t         SLV_RX_DATA_LOST            : 1;
    );
    VSF_DEF_REG(IC_FS_SPKLEN, 32,
        __IOM       reg32_t         IC_FS_SPKLEN                : 8;
    );
    VSF_DEF_REG(IC_HS_SPKLEN, 32,
        __IOM       reg32_t         IC_HS_SPKLEN                : 8;
    );
    VSF_DEF_REG(IC_CLR_RESTART_DET, 32,
        __IM        reg32_t         CLR_RESTART_DET             : 1;
    );
    REG_RSVD_U32N(18)
    VSF_DEF_REG(IC_COMP_PARAM_1, 32,
        __IM        reg32_t         APB_DATA_WIDTH              : 2;
        __IM        reg32_t         MAX_SPEED_MODE              : 2;
        __IM        reg32_t         HC_COUNT_VALUES             : 1;
        __IM        reg32_t         INTR_IO                     : 1;
        __IM        reg32_t         HAS_DMA                     : 1;
        __IM        reg32_t         ADD_ENCODED_PARAMS          : 1;
        __IM        reg32_t         RX_BUFFER_DEPTH             : 8;
        __IM        reg32_t         TX_BUFFER_DEPTH             : 8;
    );
    VSF_DEF_REG(IC_COMP_VERSION, 32,
        __IM        reg32_t         IC_COMP_VERSION             : 32;
    );
    VSF_DEF_REG(IC_COMP_TYPE, 32,
        __IM        reg32_t         IC_COMP_TYPE                : 32;
    );
} vsf_dw_apb_i2c_reg_t;


#ifdef __cplusplus
}
#endif

#endif

/* EOF */
