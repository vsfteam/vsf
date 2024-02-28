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

#ifndef __VSF_PL011_USART_REG_H__
#define __VSF_PL011_USART_REG_H__

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

// refer to: https://developer.arm.com/Processors/PL011
typedef struct vsf_pl011_usart_reg_t {
    // 0x000 UARTDR
    VSF_DEF_REG(UARTDR, 32,
        __IOM       reg32_t         DATA                        : 8;
        __IOM       reg32_t         FE                          : 1;
        __IOM       reg32_t         PE                          : 1;
        __IOM       reg32_t         BE                          : 1;
        __IOM       reg32_t         OE                          : 1;
    );
    union {
        // 0x004 UARTRSR/UARTECR
        VSF_DEF_REG(UARTRSR, 32,
            __IOM   reg32_t         FE                          : 1;
            __IOM   reg32_t         PE                          : 1;
            __IOM   reg32_t         BE                          : 1;
            __IOM   reg32_t         OE                          : 1;
        );
        VSF_DEF_REG(UARTECR, 32,
            __IOM   reg32_t         FE                          : 1;
            __IOM   reg32_t         PE                          : 1;
            __IOM   reg32_t         BE                          : 1;
            __IOM   reg32_t         OE                          : 1;
        );
    };
    // 0x008-0x014
    REG_RSVD_N(32, 4);
    // 0x018 UARTFR
    VSF_DEF_REG(UARTFR, 32,
        __IOM       reg32_t         CTS                         : 1;
        __IOM       reg32_t         DSR                         : 1;
        __IOM       reg32_t         DCD                         : 1;
        __IOM       reg32_t         BUSY                        : 1;
        __IOM       reg32_t         RXFE                        : 1;
        __IOM       reg32_t         TXFF                        : 1;
        __IOM       reg32_t         RXFF                        : 1;
        __IOM       reg32_t         TXFE                        : 1;
        __IOM       reg32_t         RI                          : 1;
    );
    // 0x01C
    REG_RSVD_N(32, 1);
    // 0x020 UARTILPR
    VSF_DEF_REG(UARTILPR, 32,
        __IOM       reg32_t         ILPDVSR                     : 8;
    );
    // 0x024 UARTIBRD
    VSF_DEF_REG(UARTIBRD, 32,
        __IOM       reg32_t         BAUD_DIVINT                 : 16;
    );
    // 0x028 UARTFBRD
    VSF_DEF_REG(UARTFBRD, 32,
        __IOM       reg32_t         BAUD_DIVFRAC                : 5;
    );
    // 0x02C UARTLCR_H
    VSF_DEF_REG(UARTLCR_H, 32,
        __IOM       reg32_t         BRK                         : 1;
        __IOM       reg32_t         PEN                         : 1;
        __IOM       reg32_t         EPS                         : 1;
        __IOM       reg32_t         STP2                        : 1;
        __IOM       reg32_t         FEN                         : 1;
        __IOM       reg32_t         WLEN                        : 2;
        __IOM       reg32_t         SPS                         : 1;
    );
    // 0x030 UARTCR
    VSF_DEF_REG(UARTCR, 32,
        __IOM       reg32_t         UARTEN                      : 1;
        __IOM       reg32_t         SIREN                       : 1;
        __IOM       reg32_t         SIRLP                       : 1;
        __IOM       reg32_t         RESERVED                    : 4;
        __IOM       reg32_t         LBE                         : 1;
        __IOM       reg32_t         TXE                         : 1;
        __IOM       reg32_t         RXE                         : 1;
        __IOM       reg32_t         DTR                         : 1;
        __IOM       reg32_t         RTS                         : 1;
        __IOM       reg32_t         OUT1                        : 1;
        __IOM       reg32_t         OUT2                        : 1;
        __IOM       reg32_t         RTSEN                       : 1;
        __IOM       reg32_t         CTSEN                       : 1;
    );
    // 0x034 UARTIFLS
    VSF_DEF_REG(UARTIFLS, 32,
        __IOM       reg32_t         TXIFLSEL                    : 3;
        __IOM       reg32_t         RXIFLSEL                    : 3;
    );
    // 0x038 UARTIMSC
    VSF_DEF_REG(UARTIMSC, 32,
        __IOM       reg32_t         RIMIM                       : 1;
        __IOM       reg32_t         CTSMIM                      : 1;
        __IOM       reg32_t         DCDMIM                      : 1;
        __IOM       reg32_t         DSRMIM                      : 1;
        __IOM       reg32_t         RXIM                        : 1;
        __IOM       reg32_t         TXIM                        : 1;
        __IOM       reg32_t         RTIM                        : 1;
        __IOM       reg32_t         FEIM                        : 1;
        __IOM       reg32_t         PEIM                        : 1;
        __IOM       reg32_t         BEIM                        : 1;
        __IOM       reg32_t         OEIM                        : 1;
    );
    // 0x03C UARTRIS
    VSF_DEF_REG(UARTRIS, 32,
        __IM        reg32_t         RIRMIS                      : 1;
        __IM        reg32_t         CTSRMIS                     : 1;
        __IM        reg32_t         DCDRMIS                     : 1;
        __IM        reg32_t         DSRRMIS                     : 1;
        __IM        reg32_t         RXRIS                       : 1;
        __IM        reg32_t         TXRIS                       : 1;
        __IM        reg32_t         RTRIS                       : 1;
        __IM        reg32_t         FERIS                       : 1;
        __IM        reg32_t         PERIS                       : 1;
        __IM        reg32_t         BERIS                       : 1;
        __IM        reg32_t         OERIS                       : 1;
    );
    // 0x040 UARTMIS
    VSF_DEF_REG(UARTMIS, 32,
        __IM        reg32_t         RIMMIS                      : 1;
        __IM        reg32_t         CTSMMIS                     : 1;
        __IM        reg32_t         DCDMMIS                     : 1;
        __IM        reg32_t         DSRMMIS                     : 1;
        __IM        reg32_t         RXMIS                       : 1;
        __IM        reg32_t         TXMIS                       : 1;
        __IM        reg32_t         RTMIS                       : 1;
        __IM        reg32_t         FEMIS                       : 1;
        __IM        reg32_t         PEMIS                       : 1;
        __IM        reg32_t         BEMIS                       : 1;
        __IM        reg32_t         OEMIS                       : 1;
    );
    // 0x044 UARTICR
    VSF_DEF_REG(UARTICR, 32,
        __OM        reg32_t         RIMIC                       : 1;
        __OM        reg32_t         CTSMIC                      : 1;
        __OM        reg32_t         DCDMMIC                     : 1;
        __OM        reg32_t         DSRMMIC                     : 1;
        __OM        reg32_t         RXIC                        : 1;
        __OM        reg32_t         TXIC                        : 1;
        __OM        reg32_t         RTIC                        : 1;
        __OM        reg32_t         FEIC                        : 1;
        __OM        reg32_t         PEIC                        : 1;
        __OM        reg32_t         BEIC                        : 1;
        __OM        reg32_t         OEIC                        : 1;
    );
    // 0x048 UARTDMACR
    VSF_DEF_REG(UARTDMACR, 32,
        __IOM       reg32_t         RXDMAE                      : 1;
        __IOM       reg32_t         TXDMAE                      : 1;
        __IOM       reg32_t         DMAONERR                    : 1;
    );
} vsf_pl011_usart_reg_t;


#ifdef __cplusplus
}
#endif

#endif

/* EOF */
