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

#include "./dram.h"

/*============================ MACROS ========================================*/

#define SDR_T_CAS               0x2
#define SDR_T_RAS               0x8
#define SDR_T_RCD               0x3
#define SDR_T_RP                0x3
#define SDR_T_WR                0x3
#define SDR_T_RFC               0xd
#define SDR_T_XSR               0xf9
#define SDR_T_RC                0xb
#define SDR_T_INIT              0x8
#define SDR_T_INIT_REF          0x7
#define SDR_T_WTR               0x2
#define SDR_T_RRD               0x2
#define SDR_T_XP                0x0

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __dram_delay(int ms)
{
    __delay_loops(ms * 2 * 1000);
}

static int __dram_initial(void)
{
    uint_fast32_t time = 0xffffff;

    DRAM_BASE->SCTLR |= 1;
    while ((DRAM_BASE->SCTLR) & 1) {
        if (0 == time--) {
            return 0;
        }
    }
    return 1;
}

static int __dram_delay_scan(void)
{
    uint_fast32_t time = 0xffffff;

    DRAM_BASE->DDLYR |= 1;
    while ((DRAM_BASE->DDLYR) & 1) {
        if (0 == time--) {
            return 0;
        }
    }
    return 1;
}

static void __dram_set_autofresh_cycle(uint_fast32_t clk)
{
    uint_fast32_t val = 0;
    uint_fast32_t row = 0;
    uint_fast32_t temp = 0;

    row = DRAM_BASE->SCONR;
    row &= 0x1e0;
    row >>= 0x5;

    if (row == 0xc) {
        if (clk >= 1000000) {
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while (temp >= (10000000 >> 6)) {
                temp -= (10000000 >> 6);
                val++;
            }
        } else {
            val = (clk * 499) >> 6;
        }
    } else if (row == 0xb) {
        if (clk >= 1000000) {
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while (temp >= (10000000 >> 7)) {
                temp -= (10000000 >> 7);
                val++;
            }
        } else {
            val = (clk * 499) >> 5;
        }
    }
    DRAM_BASE->SREFR = val;
}

static int __dram_param_setup(f1cx00s_dram_param_t * param)
{
    uint_fast32_t val = 0;

    val =   (param->ddr8_remap)
        |   (0x1 << 1)
        |   ((param->bank_size >> 2) << 3)
        |   ((param->cs_num >> 1) << 4)
        |   ((param->row_width - 1) << 5)
        |   ((param->col_width - 1) << 9)
        |   ((param->sdr_ddr ? (param->bwidth >> 4) : (param->bwidth >> 5)) << 13)
        |   (param->access_mode << 15)
        |   (param->sdr_ddr << 16);

    DRAM_BASE->SCONR = val;
    DRAM_BASE->SCTLR |= 0x1 << 19;
    return __dram_initial();
}

static uint_fast32_t __dram_check_delay(uint_fast32_t bwidth)
{
    uint_fast32_t dsize;
    uint_fast32_t i,j;
    uint_fast32_t num = 0;
    uint_fast32_t dflag = 0;

    dsize = ((bwidth == 16) ? 4 : 2);
    for (i = 0; i < dsize; i++) {
        if (i == 0) {
            dflag = DRAM_BASE->DRPTR0;
        } else if (i == 1) {
            dflag = DRAM_BASE->DRPTR1;
        } else if (i == 2) {
            dflag = DRAM_BASE->DRPTR2;
        } else if (i == 3) {
            dflag = DRAM_BASE->DRPTR3;
        }

        for (j = 0; j < 32; j++) {
            if (dflag & 0x1) {
                num++;
            }
            dflag >>= 1;
        }
    }
    return num;
}

static int __sdr_readpipe_scan(void)
{
    uint_fast32_t k = 0;
    uint32_t *ptr = (uint32_t *)F1CX00S_DRAM_ADDR;

    for (k = 0; k < 32; k++) {
        ptr[k] = k;
    }
    for (k = 0; k < 32; k++) {
        if (ptr[k] != k) {
            return 0;
        }
    }
    return 1;
}

static uint_fast32_t sdr_readpipe_select(void)
{
    uint_fast32_t value = 0;
    uint_fast32_t i = 0;
    for (i = 0; i < 8; i++) {
        DRAM_BASE->SCTLR = (DRAM_BASE->SCTLR & (~(0x7 << 6))) | (i << 6);
        if (__sdr_readpipe_scan()) {
            value = i;
            return value;
        }
    }
    return value;
}

static uint_fast32_t __dram_check_type(f1cx00s_dram_param_t * param)
{
    uint_fast32_t times = 0;
    uint_fast32_t i;

    for (i = 0; i < 8; i++) {
        DRAM_BASE->SCTLR = (DRAM_BASE->SCTLR & ~(0x7 << 6)) | (i << 6);

        __dram_delay_scan();
        if (DRAM_BASE->DDLYR & 0x30) {
            times++;
        }
    }

    if (times == 8) {
        param->sdr_ddr = DRAM_TYPE_SDR;
        return 0;
    } else {
        param->sdr_ddr = DRAM_TYPE_DDR;
        return 1;
    }
}

static uint_fast32_t __dram_scan_readpipe(f1cx00s_dram_param_t * param)
{
    uint_fast32_t i, rp_best = 0, rp_val = 0;
    uint_fast32_t readpipe[8];

    if (param->sdr_ddr == DRAM_TYPE_DDR) {
        for (i = 0; i < 8; i++) {
            DRAM_BASE->SCTLR = (DRAM_BASE->SCTLR & ~(0x7 << 6)) | (i << 6);
            __dram_delay_scan();
            readpipe[i] = 0;
            if (    (((DRAM_BASE->DDLYR >> 4) & 0x3) == 0x0)
                &&  (((DRAM_BASE->DDLYR >> 4) & 0x1) == 0x0)) {
                readpipe[i] = __dram_check_delay(param->bwidth);
            }
            if (rp_val < readpipe[i]) {
                rp_val = readpipe[i];
                rp_best = i;
            }
        }
        DRAM_BASE->SCTLR = (DRAM_BASE->SCTLR & ~(0x7 << 6)) | (rp_best << 6);
        __dram_delay_scan();
    } else {
        DRAM_BASE->SCONR &= ~((1 << 16) | (3 << 13));
        rp_best = sdr_readpipe_select();
        DRAM_BASE->SCTLR = (DRAM_BASE->SCTLR & ~(0x7 << 6)) | (rp_best << 6);
    }
    return 0;
}

static uint_fast32_t __dram_get_dram_size(f1cx00s_dram_param_t * param)
{
    uint_fast32_t colflag = 10, rowflag = 13;
    uint_fast32_t i = 0;
    uint_fast32_t val1 = 0;
    uint_fast32_t count = 0;
    uint_fast32_t addr1, addr2;

    param->col_width = colflag;
    param->row_width = rowflag;
    __dram_param_setup(param);
    __dram_scan_readpipe(param);
    for (i = 0; i < 32; i++) {
        *((uint32_t *)(0x80000200 + i)) = 0x11111111;
        *((uint32_t *)(0x80000600 + i)) = 0x22222222;
    }
    for (i = 0; i < 32; i++) {
        val1 = *((uint32_t *)(0x80000200 + i));
        if (val1 == 0x22222222) {
            count++;
        }
    }
    if (count == 32) {
        colflag = 9;
    } else {
        colflag = 10;
    }
    count = 0;
    param->col_width = colflag;
    param->row_width = rowflag;
    __dram_param_setup(param);
    if (colflag == 10) {
        addr1 = 0x80400000;
        addr2 = 0x80c00000;
    } else {
        addr1 = 0x80200000;
        addr2 = 0x80600000;
    }
    for (i = 0; i < 32; i++) {
        *((uint32_t *)(addr1 + i)) = 0x33333333;
        *((uint32_t *)(addr2 + i)) = 0x44444444;
    }
    for (i = 0; i < 32; i++) {
        val1 = *((uint32_t *)(addr1 + i));
        if (val1 == 0x44444444) {
            count++;
        }
    }
    if (count == 32) {
        rowflag = 12;
    } else {
        rowflag = 13;
    }
    param->col_width = colflag;
    param->row_width = rowflag;
    if (param->row_width != 13) {
        param->size = 16;
    } else if (param->col_width == 10) {
        param->size = 64;
    } else {
        param->size = 32;
    }
    __dram_set_autofresh_cycle(param->clk);
    param->access_mode = 0;
    __dram_param_setup(param);

    return 0;
}

int f1cx00s_dram_init(f1cx00s_dram_param_t *param)
{
    uint_fast32_t val = 0;
    uint_fast32_t i;

    PIO_BASE->PORT[1].CFG0 |= 7 << 12;
    __dram_delay(5);
    if (((param->cas) >> 3) & 0x1) {
        PIO_BASE->SDR_PAD_PUL |= (0x1 << 23) | (0x20 << 17);
    }
    if ((param->clk >= 144) && (param->clk <= 180)) {
        PIO_BASE->SDR_PAD_DRV = 0xAAA;
    }
    if (param->clk >= 180) {
        PIO_BASE->SDR_PAD_DRV = 0xFFF;
    }
    if ((param->clk) <= 96) {
        val = (0x1 << 0) | (0x0 << 4) | (((param->clk * 2) / 12 - 1) << 8) | (0x1u << 31);
    } else {
        val = (0x0 << 0) | (0x0 << 4) | (((param->clk * 2) / 24 - 1) << 8) | (0x1u << 31);
    }

    if (param->cas & (0x1 << 4)) {
        CCU_BASE->PLL_DDR0_PAT_CTRL = 0xd1303333;
    } else if (param->cas & (0x1 << 5)) {
        CCU_BASE->PLL_DDR0_PAT_CTRL = 0xcce06666;
    } else if (param->cas & (0x1 << 6)) {
        CCU_BASE->PLL_DDR0_PAT_CTRL = 0xc8909999;
    } else if (param->cas & (0x1 << 7)) {
        CCU_BASE->PLL_DDR0_PAT_CTRL = 0xc440cccc;
    }
    if (param->cas & (0xf << 4)) {
        val |= 0x1 << 24;
    }
    CCU_BASE->PLL_DDR_CTRL = val;
    CCU_BASE->PLL_DDR_CTRL |= PLL_DDR_CTRL_PLL_DDR_CFG_UPDATE;
    while ((CCU_BASE->PLL_DDR_CTRL & PLL_DDR_CTRL_LOCK) == 0);
    __dram_delay(5);
    CCU_BASE->BUS_CLK_GATING0 |= BUS_CLK_GATING0_SDRAM_GATING;
    CCU_BASE->BUS_SOFT_RST0 &= ~BUS_SOFT_RST0_SDRAM_RST;
    __delay_loops(10);
    CCU_BASE->BUS_SOFT_RST0 |= BUS_SOFT_RST0_SDRAM_RST;

    val = PIO_BASE->SDR_PAD_PUL;
    (param->sdr_ddr == DRAM_TYPE_DDR) ? (val |= (0x1 << 16)) : (val &= ~(0x1 << 16));
    PIO_BASE->SDR_PAD_PUL = val;

    DRAM_BASE->STMG0R = (SDR_T_CAS << 0) | (SDR_T_RAS << 3) | (SDR_T_RCD << 7) | (SDR_T_RP << 10) | (SDR_T_WR << 13) | (SDR_T_RFC << 15) | (SDR_T_XSR << 19) | (SDR_T_RC << 28);
    DRAM_BASE->STMG1R = (SDR_T_INIT << 0) | (SDR_T_INIT_REF << 16) | (SDR_T_WTR << 20) | (SDR_T_RRD << 22) | (SDR_T_XP << 25);
    __dram_param_setup(param);
    __dram_check_type(param);

    val = PIO_BASE->SDR_PAD_PUL;
    (param->sdr_ddr == DRAM_TYPE_DDR) ? (val |= (0x1 << 16)) : (val &= ~(0x1 << 16));
    PIO_BASE->SDR_PAD_PUL = val;

    __dram_set_autofresh_cycle(param->clk);
    __dram_scan_readpipe(param);
    __dram_get_dram_size(param);

    for (i = 0; i < 128; i++) {
        *((volatile uint32_t *)(param->base + 4 * i)) = param->base + 4 * i;
    }
    for (i = 0; i < 128; i++) {
        if (*((volatile uint32_t *)(param->base + 4 * i)) != (param->base + 4 * i)) {
            return 0;
        }
    }
    return param->size;
}
