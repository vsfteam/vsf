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

#ifndef __I_REG_UART_H__
#define __I_REG_UART_H__

/*============================ INCLUDES ======================================*/

/*! \note i_reg_xxxx.h header files must be standalong and assume following
 *!       conditions:
 *!       a. stdint.h exists
 *!       b. anonymouse structures and unions are supported
 */
#include "./utilities/compiler/__common/__type.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

#ifndef __AIC8800_UART_USE_BIT_FIELD
#   define __AIC8800_UART_USE_BIT_FIELD             ENABLED
#endif

/* Define structure member permissions : read only? */
#ifndef __IM
#   define __IM                                     const
#endif

/* Define structure member permissions : write only? */
#ifndef __OM
#   define __OM
#endif

/* Define structure member permissions : read or write? */
#ifndef __IOM
#   define __IOM
#endif

#define UART0_BASE_ADDRESS                          (0x40041000ul)
#define UART1_BASE_ADDRESS                          (0x40042000ul)
#define UART2_BASE_ADDRESS                          (0x40043000ul)

#define UART0                                       ((uart_reg_t *)UART0_BASE_ADDRESS)
#define UART1                                       ((uart_reg_t *)UART1_BASE_ADDRESS)
#define UART2                                       ((uart_reg_t *)UART2_BASE_ADDRESS)

#define TXRXD_REG                                   TXRXD.VALUE
#define DIV0_REG                                    DIV0.VALUE
#define IRQCTL_REG                                  IRQCTL.VALUE
#define DIV1_REG                                    DIV1.VALUE
#define IRQTYP_REG                                  IRQTYP.VALUE
#define DBUFCFG_REG                                 DBUFCFG.VALUE
#define DFMTCFG_REG                                 DFMTCFG.VALUE
#define MDMCFG_REG                                  MDMCFG.VALUE
#define IRQSTS_REG                                  IRQSTS.VALUE
#define MDMSTS_REG                                  MDMSTS.VALUE
#define DBUFSTS_REG                                 DBUFSTS.VALUE
#define DBUFTH_REG                                  DBUFTH.VALUE
#define DIV2_REG                                    DIV2.VALUE
/* -------------------- TXRXD register -------------------------------------- */

#define UART_RXDATA                                 0       /* 0x000000FF */
#define UART_RXDATA_MSK                             ((reg32_t)(0xff << UART_RXDATA))

#define UART_TXDATA                                 0       /* 0x000000FF */
#define UART_TXDATA_MSK                             ((reg32_t)(0xff << UART_TXDATA))
/* -------------------- DIV0 register --------------------------------------- */

#define UART_DIV0                                   0       /* 0x000000FF */
#define UART_DIV0_MSK                               ((reg32_t)(0xff << UART_DIV0))

/* -------------------- IRQCTL register ------------------------------------- */

#define UART_PTIRQEN                                7       /* 0x00000080 */
#define UART_PTIRQEN_MSK                            ((reg32_t)(0x1 << UART_PTIRQEN))

#define UART_MSIRQEN                                3       /* 0x00000008 */
#define UART_MSIRQEN_MSK                            ((reg32_t)(0x1 << UART_MSIRQEN))

#define UART_LSIRQEN                                2       /* 0x00000004 */
#define UART_LSIRQEN_MSK                            ((reg32_t)(0x1 << UART_LSIRQEN))

#define UART_TXIRQEN                                1       /* 0x00000002 */
#define UART_TXIRQEN_MSK                            ((reg32_t)(0x1 << UART_TXIRQEN))

#define UART_RXIRQEN                                0       /* 0x00000001 */
#define UART_RXIRQEN_MSK                            ((reg32_t)(0x1 << UART_RXIRQEN))

/* -------------------- DIV1 register --------------------------------------- */

#define UART_DIV1                                   0       /* 0x000000FF */
#define UART_DIV1_MSK                               ((reg32_t)(0xff << UART_DIV1))

/* -------------------- IRQTYP register ------------------------------------- */

#define UART_IRQTYP                                 0       /* 0x0000000F */
#define UART_IRQTYP_MSK                             ((reg32_t)(0xf << UART_IRQTYP))
#define UART_IRQTYP_MODEM_INT                       0x00
#define UART_IRQTYP_NO_INT                          0x01
#define UART_IRQTYP_TX_INT                          0x02
#define UART_IRQTYP_RX_INT                          0x04
#define UART_IRQTYP_RX_ERROR_INT                    0x06
#define UART_IRQTYP_TIMEOUT_INT                     0x0c

/* -------------------- DBUFCFG register ------------------------------------ */

#define UART_TXDRST                                 2       /* 0x00000004 */
#define UART_TXDRST_MSK                             ((reg32_t)(0x1 << UART_TXDRST))

#define UART_RXDRST                                 1       /* 0x00000002 */
#define UART_RXDRST_MSK                             ((reg32_t)(0x1 << UART_RXDRST))

#define UART_DBUFEN                                 0       /* 0x00000001 */
#define UART_DBUFEN_MSK                             ((reg32_t)(0x1 << UART_DBUFEN))

/* -------------------- DFMTCFG register ------------------------------------ */

#define UART_DIVMS                                  8       /* 0x00000100 */
#define UART1_DIVMS_MSK                             ((reg32_t)(0x1 << UART_DIVMS))

#define UART_DIVAE                                  7       /* 0x00000080 */
#define UART_DIVAE_MSK                              ((reg32_t)(0x1 << UART_DIVAE))

#define UART_BRK                                    6       /* 0x00000040 */
#define UART_BRK_MSK                                ((reg32_t)(0x1 << UART_BRK))

#define UART_EPS                                    4       /* 0x00000010 */
#define UART_EPS_MSK                                ((reg32_t)(0x1 << UART_EPS))

#define UART_PEN                                    3       /* 0x00000008 */
#define UART_PEN_MSK                                ((reg32_t)(0x1 << UART_PEN))

#define UART_STOP                                   2       /* 0x00000004 */
#define UART_STOP_MSK                               ((reg32_t)(0x1 << UART_STOP))

#define UART_DLS                                    0       /* 0x00000003 */
#define UART_DLS_MSK                                ((reg32_t)(0x3 << UART_DLS))

/* -------------------- MDMCFG register ------------------------------------- */

#define UART_CLK_P                                  8       /* 0x00000100 */
#define UART_CLK_P_MSK                              ((reg32_t)(0x1 << UART_CLK_P))

#define UART_AUTO_DET                               7       /* 0x00000080 */
#define UART_AUTO_DET_MSK                           ((reg32_t)(0x1 << UART_AUTO_DET))

#define UART_SIRE                                   6       /* 0x00000040 */
#define UART_SIRE_MSK                               ((reg32_t)(0x1 << UART_SIRE))

#define UART_AFCE                                   5       /* 0x00000020 */
#define UART_AFCE_MSK                               ((reg32_t)(0x1 << UART_AFCE))

#define UART_LOOPBACK                               4       /* 0x00000010 */
#define UART_LOOPBACK_MSK                           ((reg32_t)(0x1 << UART_LOOPBACK))

#define UART_OUT2                                   3       /* 0x00000008 */
#define UART_OUT2_MSK                               ((reg32_t)(0x1 << UART_OUT2))

#define UART_OUT1                                   2       /* 0x00000004 */
#define UART_OUT1_MSK                               ((reg32_t)(0x1 << UART_OUT1))

#define UART_RTS                                    1       /* 0x00000002 */
#define UART_RTS_MSK                                ((reg32_t)(0x1 << UART_RTS))

#define UART_DTR                                    0       /* 0x00000001 */
#define UART_DTR_MSK                                ((reg32_t)(0x1 << UART_DTR))

/* -------------------- IRQSTS register ------------------------------------- */

#define UART_RTDR                                   8       /* 0x00000100 */
#define UART_RTDR_MSK                               ((reg32_t)(0x1 << UART_RTDR))

#define UART_RFE                                    7       /* 0x00000080 */
#define UART_RFE_MSK                                ((reg32_t)(0x1 << UART_RFE))

#define UART_TEMT                                   6       /* 0x00000040 */
#define UART_TEMT_MSK                               ((reg32_t)(0x1 << UART_TEMT))

#define UART_THRE                                   5       /* 0x00000020 */
#define UART_THRE_MSK                               ((reg32_t)(0x1 << UART_THRE))

#define UART_OE                                     1       /* 0x00000002 */
#define UART_OE_MSK                                 ((reg32_t)(0x1 << UART_OE))

#define UART_DR                                     0       /* 0x00000001 */
#define UART_DR_MSK                                 ((reg32_t)(0x1 << UART_DR))

/* -------------------- MDMSTS register ------------------------------------- */

/* -------------------- DBUFSTS register ------------------------------------ */

#define UART_RX_DBUF_FULL                           21       /* 0x00200000 */
#define UART_RX_DBUF_FULL_MSK                       ((reg32_t)(0x1 << UART_RX_DBUF_FULL))

#define UART_RX_DBUF_EMPTY                          20       /* 0x00100000 */
#define UART_RX_DBUF_EMPTY_MSK                      ((reg32_t)(0x1 << UART_RX_DBUF_EMPTY))

#define UART_TX_DBUF_FULL                           19       /* 0x00080000 */
#define UART_TX_DBUF_FULL_MSK                       ((reg32_t)(0x1 << UART_TX_DBUF_FULL))

#define UART_TX_DBUF_EMPTY                          18       /* 0x00040000 */
#define UART_TX_DBUF_EMPTY_MSK                      ((reg32_t)(0x1 << UART_TX_DBUF_EMPTY))

#define UART_RX_COUNT                               8       /* 0x0001FE00 */
#define UART_RX_COUNT_MSK                           ((reg32_t)(0x1fe << UART_RX_COUNT))

#define UART_TX_COUNT                               0       /* 0x000000FF */
#define UART_TX_COUNT_MSK                           ((reg32_t)(0xff << UART_TX_COUNT))

/* -------------------- DBUFTH register ------------------------------------- */

#define UART_TXTRIGTH                               8       /* 0x0001FE00 */
#define UART_TXTRIGTH_MSK                           ((reg32_t)(0x1fe << UART_TXTRIGTH))

#define UART_RXTRIGTH                               0       /* 0x000000FF */
#define UART_RXTRIGTH_MSK                           ((reg32_t)(0xff << UART_RXTRIGTH))

/* -------------------- DIV2 register --------------------------------------- */

#define UART_DIV2                                   0       /* 0x000000FF */
#define UART_DIV2_MSK                               ((reg32_t)(0xff << UART_DIV2))
/*============================ MACROFIED FUNCTIONS ===========================*/

#if __AIC8800_UART_USE_BIT_FIELD == ENABLED
#   define DEF_UART_REG(__NAME, __TOTAL_SIZE, ...)                              \
    union {                                                                     \
        struct {                                                                \
            __VA_ARGS__                                                         \
        };                                                                      \
        reg##__TOTAL_SIZE##_t VALUE;                                            \
    } __NAME
#else
#   define DEF_UART_REG(__NAME, __TOTAL_SIZE, ...)                              \
    __VA_ARGS__ reg##__TOTAL_SIZE##_t __NAME
#endif
/*============================ TYPES =========================================*/

typedef struct uart_reg_t {
    union {
        __IM            reg32_t         BASE_ADDR;
        DEF_UART_REG(TXRXD, 32,
            __IOM       reg32_t         TXRXDATA        : 8;
                        reg32_t                         : 24;
        );
        DEF_UART_REG(DIV0, 32,
            __IOM       reg32_t         DIV0            : 8;
                        reg32_t                         : 24;
        );
    };
    union {
        DEF_UART_REG(IRQCTL, 32,
            __IOM       reg32_t         RXIRQEN         : 1;
            __IOM       reg32_t         TXIRQEN         : 1;
            __IOM       reg32_t         LSIRQEN         : 1;
            __IOM       reg32_t         MSIRQEN         : 1;
                        reg32_t                         : 1;
                        reg32_t                         : 1;
                        reg32_t                         : 1;
            __IOM       reg32_t         PTIRQEN         : 1;
                        reg32_t                         : 24;
        );
        DEF_UART_REG(DIV1, 32,
            __IOM       reg32_t         DIV1            : 8;
                        reg32_t                         : 24;
        );
    };
    union {
        DEF_UART_REG(IRQTYP, 32,
            __IM        reg32_t         IRQTYP          : 4;
                        reg32_t                         : 28;
        );
        DEF_UART_REG(DBUFCFG, 32,
            __OM        reg32_t         DBUFEN          : 1;
            __OM        reg32_t         RXDRST          : 1;
            __OM        reg32_t         TXDRST          : 1;
                        reg32_t                         : 29;
        );
    };
    DEF_UART_REG(DFMTCFG, 32,
        __IOM           reg32_t         DLS             : 2;
        __IOM           reg32_t         STOP            : 1;
        __IOM           reg32_t         PEN             : 1;
        __IOM           reg32_t         EPS             : 1;
                        reg32_t                         : 1;
        __IOM           reg32_t         BRK             : 1;
        __IOM           reg32_t         DIVAE           : 1;
        __IOM           reg32_t         DIVMS           : 1;
                        reg32_t                         : 23;
    );
    DEF_UART_REG(MDMCFG, 32,
        __IOM           reg32_t         DTR             : 1;
        __IOM           reg32_t         RTS             : 1;
        __IOM           reg32_t         OUT1            : 1;
        __IOM           reg32_t         OUT2            : 1;
        __IOM           reg32_t         LOOPBACK        : 1;
        __IOM           reg32_t         AFCE            : 1;
        __IOM           reg32_t         SIRE            : 1;
        __IOM           reg32_t         AUTO_DET        : 1;
        __IOM           reg32_t         CLK_P           : 1;
                        reg32_t                         : 23;
    );
    DEF_UART_REG(IRQSTS, 32,
        __IM            reg32_t         DR              : 1;
        __IM            reg32_t         OE              : 1;
                        reg32_t                         : 1;
                        reg32_t                         : 1;
                        reg32_t                         : 1;
        __IM            reg32_t         THRE            : 1;
        __IM            reg32_t         TEMT            : 1;
        __IM            reg32_t         RFE             : 1;
        __IM            reg32_t         RTDR            : 1;
                        reg32_t                         : 23;
    );
    DEF_UART_REG(MDMSTS, 32,
        __IM            reg32_t         MDMSTS          : 32;
    );
    REG_RSVD_U32
    DEF_UART_REG(DBUFSTS, 32,
        __IM            reg32_t         TX_COUNT        : 8;
                        reg32_t                         : 1;
        __IM            reg32_t         RX_COUNT        : 8;
                        reg32_t                         : 1;
        __IM            reg32_t         TX_DBUF_EMPTY   : 1;
        __IM            reg32_t         TX_DBUF_FULL    : 1;
        __IM            reg32_t         RX_DBUF_EMPTY   : 1;
        __IM            reg32_t         RX_DBUF_FULL    : 1;
                        reg32_t                         : 10;
    );
    DEF_UART_REG(DBUFTH, 32,
        __IOM           reg32_t         RXTRIGTH        : 8;
                        reg32_t                         : 1;
        __IOM           reg32_t         TXTRIGTH        : 8;
                        reg32_t                         : 15;
    );
    DEF_UART_REG(DIV2, 32,
        __IOM           reg32_t         DIV2            : 8;
                        reg32_t                         : 24;
    );
} uart_reg_t;


#ifdef __cplusplus
}
#endif

#endif

/* EOF */
