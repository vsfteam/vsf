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

#include "./rtp.h"
#include "./tsfilter.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

vsf_rtp_t VSF_RTP0  = {
    .bitlen         = 12,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK(vsf_hal_on_rtp)
void vsf_hal_on_rtp(vsf_rtp_t *rtp, uint_fast8_t id, bool is_down, uint_fast16_t x, uint_fast16_t y)
{
}

__arm void TouchPanel_Handler(void) 
{
    int x, y;
    uint_fast32_t val;

    val = TP_BASE->INT_FIFOS;
    if (val & TP_DOWN_PENDING) {
        VSF_RTP0.is_down = true;
    }
    if (val & TP_UP_PENDING) {
        VSF_RTP0.is_down = false;
        vsf_hal_on_rtp(&VSF_RTP0, 0, false, VSF_RTP0.x, VSF_RTP0.y);
        VSF_RTP0.x = -1;
        VSF_RTP0.y = -1;
        tsfilter_clear(VSF_RTP0.filter);
    }
    if (val & FIFO_DATA_PENDING) {
        y = TP_BASE->DATA;
        x = TP_BASE->DATA;

        if (VSF_RTP0.is_down) {
            tsfilter_update(VSF_RTP0.filter, &x, &y);
            if ((VSF_RTP0.x != x) || (VSF_RTP0.y != y)) {
                vsf_hal_on_rtp(&VSF_RTP0, 0, true, x, y);
                VSF_RTP0.x = x;
                VSF_RTP0.y = y;
            }
        }
    }

    TP_BASE->INT_FIFOS = val;
}

// todo: remove cal, calibration is not done in driver layer
void vsf_rtp_init(vsf_rtp_t *rtp, void *cal)
{
    // TODO: use ioctrl
    /* PA0-PA3: TPX1 TPY1 TPX2 TPY2 */
    PIO_BASE->PORTA.CFG0 = (PIO_BASE->PORTA.CFG0 & ~0xFFFF) | 0x2222;

    /*FRAME Freq = 24M / 6 = 4Mhz(CLK_IN)/ 2^13(8192) = 488 hz*/
    /*Conversion Time = 1 / (4MHz/13Cycles) = 3.25us*/
    /*T_ACQ  = CLK_IN /(16*(1+63)) = 3906.25hz*/
    TP_BASE->CTRL0 = (0x1f << 23) | ADC_CLK_SEL(0) | ADC_CLK_DIV(2) | FS_DIV(6) | T_ACQ(63);

    /*00: FIFO store X1,Y1 data for single touch no pressure mode*/
    TP_BASE->CTRL2 = (0x08 << 28) | TP_MODE_SELECT(0) | (0 << 24) | 0xFFF;
    //Enable HW filte, bit3-31 MUST be kept
    TP_BASE->CTRL3 |= FILTER_EN(1) | FILTER_TYPE(0);

    TP_BASE->CDAT = 0xc00;
    TP_BASE->CTRL1 = (20 << 12) | (1 << 9) | (0 << 8) | (1 << 7) | (1 << 5) | TP_MODE_EN(0) | 0x00;

    *((volatile uint32_t *)(TP_BASE + 0x38)) = 0x22;

    TP_BASE->INT_FIFOC = TEMP_IRQ_EN(0) | OVERRUN_IRQ_EN(0) | DATA_IRQ_EN(1) | FIFO_TRIG(1) | TP_DATA_XY_CHANGE(0)| FIFO_FLUSH(1) | TP_UP_IRQ_EN(1) | TP_DOWN_IRQ_EN(1);

    memset(rtp, 0, sizeof(*rtp));
    rtp->filter = tsfilter_alloc(5, 5);
    tsfilter_setcal(rtp->filter, cal);
    intc_enable_irq(TouchPanel_IRQn);
}
