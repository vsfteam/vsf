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

#include "hal/vsf_hal.h"
#include "../../driver.h"

/*============================ MACROS ========================================*/

#define IMAGE_MAGIC             0x474D495F

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct boot_head_t {
    uint32_t instruction;
    uint8_t magic[8];
    uint32_t checksum;
    uint32_t length;
    uint8_t spl_signature[4];
    uint32_t fel_script_address;
    uint32_t fel_uenv_length;
    uint32_t dt_name_offset;
    uint32_t reserved1;
    uint32_t boot_media;
    uint32_t string_pool[13];
} boot_head_t;

enum {
    IMAGE_FLAGS_COMPRESSED      = 1 << 0,
    IMAGE_FLAGS_COMPRESSED_LZ4  = 1 << 1,
};

typedef struct image_head_t {
    uint32_t magic;
    uint32_t flags;             // IMAGE_FLAGS_COMPRESSED, IMAGE_FLAGS_COMPRESSED_LZ4, IMAGE_FLAG_SHA, IMAGE_FLAG_ECDSA, etc
    uint32_t compressed_start;
    uint32_t compressed_size;
    uint32_t original_size;
    uint32_t link_address;
    uint32_t jmp_instruction;   // ARM mode B instruction jumping to entry
    // append public keys/hashes/signatures according to flags
} image_head_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_HAL_SPL_CFG_DEBUG_UART == ENABLED
// debug uart
static void __debug_uart_init(uart_reg_t *reg, uint_fast32_t baudrate, uint_fast8_t datalen, uint_fast8_t parity, uint_fast8_t stop)
{
    uint_fast32_t lcr = 0;
    uint_fast32_t udiv;

    switch (datalen) {
    case 5:
        lcr = LCR_DLS_5;
        break;
    case 6:
        lcr = LCR_DLS_6;
        break;
    case 7:
        lcr = LCR_DLS_7;
        break;
    case 8:
        lcr = LCR_DLS_8;
        break;
    default:
        VSF_HAL_ASSERT(false);
    }

    switch (parity) {
    case 0:
        break;
    case 1:
        lcr |= LCR_PEN | LCR_EPS_ODD;
        break;
    case 2:
        lcr |= LCR_PEN | LCR_EPS_EVEN;
        break;
    default:
        VSF_HAL_ASSERT(false);
    }

    switch(stop) {
    case 1:
        lcr |= LCR_STOP_1;
        break;
    case 2:
        lcr |= LCR_STOP_2;
        break;
    default:
        VSF_HAL_ASSERT(false);
    }

    reg->LCR |= 1 << 7;
        // APB is 100M according to __clock_init
        udiv = (100UL * 1000 * 1000) / (16 * baudrate);
        reg->DLL = (udiv >> 0) & 0xff;
        reg->DLH = (udiv >> 8) & 0xff;
    reg->LCR &= ~(1 << 7);
    reg->LCR = lcr;
}

void __debug_uart_putc(char c)
{
    while (!(UART0_BASE->USR & USR_TFNF));
    UART0_BASE->THR = c;
}

void __debug_uart_puts(char *str)
{
    char ch;
    while ((ch = *str++) != '\0') {
        __debug_uart_putc(ch);
    }
}

void __debug_uart_puti(uint_fast32_t i, uint_fast8_t base)
{
    uint_fast32_t cur;
    char str[9], *pos = &str[sizeof(str) - 1];
    const char map[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    };

    *pos-- = '\0';
    if (0 == i) {
        *pos-- = '0';
    } else {
        while (i != 0) {
            cur = i % base;
            i /= base;

            *pos-- = map[cur];
        }
    }
    __debug_uart_puts(pos + 1);
}
#endif

// spi_nor
static void __spinor_init(void)
{
    uint_fast32_t value;

    // PC0-PC3
    value = PIO_BASE->PORTC.CFG0;
    value &= ~((0xf << ((0 & 0x7) << 2)) | (0xf << ((1 & 0x7) << 2)) | (0xf << ((2 & 0x7) << 2)) | (0xf << ((3 & 0x7) << 2)));
    value |= ((0x2 & 0x7) << ((0 & 0x7) << 2)) | ((0x2 & 0x7) << ((1 & 0x7) << 2)) | ((0x2 & 0x7) << ((2 & 0x7) << 2)) | ((0x2 & 0x7) << ((3 & 0x7) << 2));
    PIO_BASE->PORTC.CFG0 = value;

    CCU_BASE->BUS_SOFT_RST0 |= BUS_SOFT_RST0_SPI0_RST;
    CCU_BASE->BUS_CLK_GATING0 |= BUS_CLK_GATING0_SPI0_GATING;

    SPI0_BASE->CCR = CCR_DRS_CDR2 | CCR_CDR2(1);
    SPI0_BASE->GCR = GCR_SRST | GCR_TP_EN | GCR_MODE_MASTE | GCR_EN;
    while (SPI0_BASE->GCR & GCR_SRST);

    SPI0_BASE->TCR = TCR_CPHA(0) | TCR_CPOL(1) | TCR_SPOL(0) | TCR_SS_OWNER_SOFTWARE | TCR_SS_LEVEL(1);
    SPI0_BASE->FCR |= FCR_TF_RST | FCR_RF_RST;
    SPI0_BASE->IER = 0;
    SPI0_BASE->ISR = 0xFFFFFFFF;
}

static void __spinor_fini(void)
{
    SPI0_BASE->GCR &= ~GCR_EN;
}

static void __spinor_select(uint_fast8_t cs)
{
    uint_fast32_t value = SPI0_BASE->TCR;
    value &= ~(TCR_SS_SEL() | TCR_SS_LEVEL());
    value |= TCR_SS_SEL(cs) | TCR_SS_LEVEL(0);
    SPI0_BASE->TCR = value;
}

static void __spinor_deselect(uint_fast8_t cs)
{
    uint_fast32_t value = SPI0_BASE->TCR;
    value &= ~(TCR_SS_SEL() | TCR_SS_LEVEL());
    value |= TCR_SS_SEL(cs) | TCR_SS_LEVEL(1);
    SPI0_BASE->TCR = value;
}

static void __spinor_transfer(uint8_t *txbuffer, uint8_t *rxbuffer, uint_fast32_t len)
{
    uint_fast32_t cur_len;
    uint_fast8_t value;
    while (len > 0) {
        cur_len = min(len, 64);
        len -= cur_len;

        SPI0_BASE->MBC = cur_len;
        SPI0_BASE->MTC = cur_len;
        SPI0_BASE->BCC = cur_len;
        SPI0_BASE->TCR |= TCR_XCH;

        if (txbuffer != NULL) {
            for (uint_fast32_t i = 0; i < cur_len; i++) {
                SPI0_BASE->TXD_BYTE = *txbuffer++;
            }
        } else {
            for (uint_fast32_t i = 0; i < cur_len; i++) {
                SPI0_BASE->TXD_BYTE = 0xFF;
            }
        }

        while ((SPI0_BASE->FSR & 0xFF) < cur_len);
        for (uint_fast32_t i = 0; i < cur_len; i++) {
            value = SPI0_BASE->RXD_BYTE;
            if (rxbuffer != NULL) {
                *rxbuffer++ = value;
            }
        }
    }
}

#if VSF_HAL_SPL_CFG_LZ4 == ENABLED
// minimum lz4 decompress
static void __lz4_wildcopy(uint8_t *dst, const uint8_t *src, uint8_t *dst_end)
{
    do {
        memcpy(dst, src, 8);
        dst += 8;
        src += 8;
    } while (dst < dst_end);
}

static void __lz4_decompress(uint8_t *src, uint_fast32_t src_len, uint8_t *dst, uint_fast32_t dst_len)
{
#define MINMATCH            4
#define WILDCOPYLENGTH      8
#define LASTLITERALS        5
#define MFLIMIT             12

#define ML_BITS             4
#define ML_MASK             ((1U<<ML_BITS)-1)
#define RUN_BITS            (8-ML_BITS)
#define RUN_MASK            ((1U<<RUN_BITS)-1)

    const uint8_t * ip = (const uint8_t *)src;
    const uint8_t * const iend = ip + src_len;
    uint8_t * op = dst;
    uint8_t * const oend = op + dst_len;
    uint8_t * cpy;

    uint8_t * lowprefix = dst;
    uint8_t * shortiend = (uint8_t *)iend - 14 - 2;
    uint8_t * shortoend = oend - 14 - 18;
    uint8_t * copylimit;
    uint8_t * match;
    size_t offset;
    size_t length;

    uint_fast8_t token, s;
    uint_fast32_t inc32table[8] = {
        0, 1, 2, 1, 0, 4, 4, 4,
    };
    int_fast32_t dec64table[8] = {
        0, 0, 0, -1, -4, 1, 2, 3,
    };

    while (1) {
        token = *ip++;
        length = token >> ML_BITS;

        if ((length != RUN_MASK) && ((ip < shortiend) & (op <= shortoend))) {
            memcpy(op, ip, 16);
            op += length;
            ip += length;

            length = token & ML_MASK;
            offset = get_unaligned_le16(ip);
            ip += 2;
            match = op - offset;

            if ((length != ML_MASK) && (offset >= 8) && (match >= lowprefix)) {
                memcpy(op + 0, match + 0, 8);
                memcpy(op + 8, match + 8, 8);
                memcpy(op + 16, match + 16, 2);
                op += length + MINMATCH;
                continue;
            }
            goto copy_match;
        }

        if (length == RUN_MASK) {
            if (ip >= iend - RUN_MASK) {
                return;
            }

            do {
                s = *ip++;
                length += s;
            } while ((ip < iend - RUN_MASK) && (s == 255));

            if ((op + length < op) || (ip + length < ip)) {
                return;
            }
        }

        cpy = op + length;
        if ((cpy > oend - MFLIMIT) || (ip + length > iend - (2 + 1 + LASTLITERALS))) {
            if ((ip + length != iend) || (cpy > oend)) {
                return;
            }
            memcpy(op, ip, length);
            ip += length;
            op += length;
            break;
        }
        __lz4_wildcopy(op, ip, cpy);
        ip += length;
        op = cpy;

        offset = get_unaligned_le16(ip);
        ip += 2;
        match = op - offset;
        length = token & ML_MASK;

copy_match:
        if (match < lowprefix) {
            return;
        }
        put_unaligned_cpu32((uint32_t)offset, op);

        if (length == ML_MASK) {
            do {
                s = *ip++;
                if (ip > iend - LASTLITERALS) {
                    return;
                }
                length += s;
            } while (s == 255);

            if (op + length < op) {
                return;
            }
        }
        length += MINMATCH;

        cpy = op + length;
        if (offset < 8) {
            op[0] = match[0];
            op[1] = match[1];
            op[2] = match[2];
            op[3] = match[3];
            match += inc32table[offset];
            memcpy(op + 4, match, 4);
            match -= dec64table[offset];
        } else {
            memcpy(op, match, 8);
            match += 8;
        }
        op += 8;

        if (cpy > oend - 12) {
            copylimit = oend - (WILDCOPYLENGTH - 1);
            if (cpy > oend - LASTLITERALS) {
                return;
            }
            if (op < copylimit) {
                __lz4_wildcopy(op, match, copylimit);
                match += copylimit - op;
                op = copylimit;
            }
            while (op < cpy) {
                *op++ = *match++;
            }
        } else {
            memcpy(op, match, 8);
            if (length > 16) {
                __lz4_wildcopy(op + 8, match + 8, cpy);
            }
        }
        op = cpy;
    }
}
#endif

void __spinor_read(uint_fast32_t addr, uint8_t *buffer, uint_fast32_t len)
{
    uint8_t cmd[4] = {
        0x03, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)(addr >> 0),
    };
    __spinor_select(0);
        __spinor_transfer(cmd, NULL, 4);
        __spinor_transfer(NULL, buffer, len);
    __spinor_deselect(0);
}

static void __spl_ddr_entry(boot_head_t *boothead)
{
    bool is_from_fel = !strncmp((const char *)boothead->magic, "eGON.FEL", sizeof(boothead->magic));

    if (!is_from_fel) {
        // enable mmu to enable cache
        uint32_t * ttb = (uint32_t *)(0x80000000 + 0x100000 * 31);
        // map 0x00000000 to 0x00000000 with 2G size
        vsf_arch_mmu_map(ttb, 0x00000000, 0x00000000, 0x80000000, MMU_MAP_TYPE_NCNB);
        // map 0x80000000 to 0x80000000 with 2G size
        vsf_arch_mmu_map(ttb, 0x80000000, 0x80000000, 0x80000000, MMU_MAP_TYPE_NCNB);
        // map 0x80000000 to 0x80000000 with 32M size
        vsf_arch_mmu_map(ttb, 0x80000000, 0x80000000, 0x02000000, MMU_MAP_TYPE_CB);

        vsf_arch_mmu_enable(ttb);

        // TODO: add support to other media type: SD/NAND etc.
        image_head_t image_head = { 0 };
#ifdef VSF_HAL_SPL_CFG_IMAGE_ADDR
        boothead->length = VSF_HAL_SPL_CFG_IMAGE_ADDR;
#endif

#if VSF_HAL_SPL_CFG_DEBUG_UART == ENABLED
        __debug_uart_puts("load image at: 0x");
        __debug_uart_puti(boothead->length, 16);
        __debug_uart_puts("\r\n");
#endif

        __spinor_init();
            __spinor_read(boothead->length, (uint8_t *)&image_head, sizeof(image_head));
            if (IMAGE_MAGIC == image_head.magic) {
                if (image_head.flags & IMAGE_FLAGS_COMPRESSED) {
                    __spinor_read(boothead->length + image_head.compressed_start, (uint8_t *)0x81000000, image_head.compressed_size);
#if VSF_HAL_SPL_CFG_LZ4 == ENABLED
                    if (image_head.flags & IMAGE_FLAGS_COMPRESSED_LZ4) {
                        __lz4_decompress((uint8_t *)0x81000000, image_head.compressed_size, (uint8_t *)image_head.link_address, image_head.original_size);
                    }
#endif
                    memcpy(&image_head, (void *)image_head.link_address, sizeof(image_head));
                } else {
                    __spinor_read(boothead->length, (uint8_t *)image_head.link_address, image_head.original_size);
                }
            }
        __spinor_fini();

        if (IMAGE_MAGIC == image_head.magic) {
#if VSF_HAL_SPL_CFG_DEBUG_UART == ENABLED
            __debug_uart_puts("valid image:\r\n");
            __debug_uart_puts("\tflags: 0x");
            __debug_uart_puti(image_head.flags, 16);
            __debug_uart_puts("\r\n");
            __debug_uart_puts("\tlink_address: 0x");
            __debug_uart_puti(image_head.link_address, 16);
            __debug_uart_puts("\r\n");
            __debug_uart_puts("\toriginal_size: 0x");
            __debug_uart_puti(image_head.original_size, 16);
            __debug_uart_puts("\r\n");
#endif

            uint32_t target_addr = image_head.link_address + offset_of(image_head_t, jmp_instruction);
            __asm__ __volatile__ ("BX %0" : : "r" (target_addr) : "memory");
        } else {
#if VSF_HAL_SPL_CFG_DEBUG_UART == ENABLED
            __debug_uart_puts("invalid magic: 0x");
            __debug_uart_puti(image_head.magic, 16);
            __debug_uart_puts("\r\n");
#endif
        }
        while (1);
    }
}

void spl_c_entry(void)
{
    boot_head_t *boothead = (boot_head_t *)0;
    void (*ddr_entry)(boot_head_t *);
    uint_fast32_t val;

    f1cx00s_clock_init(F1CX00S_PLL_CPU_CLK_HZ);

#if VSF_HAL_SPL_CFG_DEBUG_UART == ENABLED
    // PE0 PE1 -> TXD0 RXD0
    val = PIO_BASE->PORTE.CFG0;
    val &= ~((0xf << ((1 & 0x7) << 2)) | (0xf << ((0 & 0x7) << 2)));
    val |= ((0x5 & 0x7) << ((1 & 0x7) << 2)) | ((0x5 & 0x7) << ((0 & 0x7) << 2));
    PIO_BASE->PORTE.CFG0 = val;
    CCU_BASE->BUS_CLK_GATING2 |= BUS_CLK_GATING2_UART0_GATING;
    CCU_BASE->BUS_SOFT_RST2 |= BUS_SOFT_RST2_UART0_RST;
    UART0_BASE->IER = 0;
    UART0_BASE->FCR = FCR_XFIFOR | FCR_RFIFOR | FCR_FIFOE;
    UART0_BASE->MCR = 0;
    __debug_uart_init(UART0_BASE, 115200, 8, 0, 1);
    __debug_uart_puts("vsf_spl for f1c100s\r\n");
#endif

    f1cx00s_dram_param_t param = {
        .base           = 0x80000000,
        .size           = 32,
        .clk            = F1CX00S_PLL_DDR_CLK_HZ / 1000000,
        .access_mode    = 1,
        .cs_num         = 1,
        .ddr8_remap     = 0,
        .sdr_ddr        = DRAM_TYPE_DDR,
        .bwidth         = 16,
        .col_width      = 10,
        .row_width      = 13,
        .bank_size      = 4,
        .cas            = 0x3,
    };
    if (!f1cx00s_dram_init(&param)) {
#if VSF_HAL_SPL_CFG_DEBUG_UART == ENABLED
        __debug_uart_puts("fail to initialize dram\r\n");
#endif
        while (1);
    }
    boothead->reserved1 = param.size;
#if VSF_HAL_SPL_CFG_DEBUG_UART == ENABLED
    __debug_uart_puts("dram initialized, size = ");
    __debug_uart_puti(param.size, 10);
    __debug_uart_puts("\r\n");
#endif

    memcpy((void *)0x81F80000, (const void *)0, boothead->length);
    ddr_entry = (void (*)(boot_head_t *))((uint32_t)__spl_ddr_entry + 0x81F80000);
    ddr_entry(boothead);
}
