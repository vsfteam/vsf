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

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_HW_CLK_PRESCALER_NONE           = 0,
    VSF_HW_CLK_PRESCALER_CONST,
    VSF_HW_CLK_PRESCALER_DIV,
    VSF_HW_CLK_PRESCALER_RSHIFT,
    VSF_HW_CLK_PRESCALER_FUNC,
};
enum {
    VSF_HW_CLK_TYPE_CONST,
    VSF_HW_CLK_TYPE_CLK,
    VSF_HW_CLK_TYPE_SEL,
};
enum {
    VSF_HW_CLK_RDY_BIT                  = 0,
    VSF_HW_CLK_RDY_MATCH_CLKSEL,
};

struct vsf_hw_clk_t {
    union {
        uint32_t clksel_region;
        uint32_t clkbypass_region;
        uint32_t clkcaltrim_region;
    };
    union {
        uint32_t clkprescaler_region;
        uint32_t clkcakkey_region;
    };
    uint32_t clken_region;
    uint32_t clkrdy_region;

    union {
        const vsf_hw_clk_t * const *clksel_mapper;
        uint32_t clk_freq_hz;
        const vsf_hw_clk_t *clksrc;
    };
    union {
        const uint16_t *clkprescaler_mapper;
        uint32_t prescaler;
        uint32_t (*getclk)(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler);
    };

    uint8_t clksrc_type;
    uint8_t clkprescaler_type;
    uint8_t clkrdy_type;
    uint8_t clkprescaler_max;
};

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_hw_clk_t VSF_HW_CLK_HSE = {
    .clkbypass_region           = VSF_HW_CLKRST_REGION(0x00, 18,  1),   // CRM_CTRL.HEXTBYPS
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 16,  1),   // CRM_CTRL.HEXTEN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 17,  1),   // CRM_CTRL.HEXTSTBL
    .clk_freq_hz                = HEXT_VALUE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_HSE_TO_SYS = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x29, 19,  3),   // CRM_MISC2.HEXT_TO_SCLK_DIV
    .clksrc                     = &VSF_HW_CLK_HSE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_RSHIFT,
    .clkprescaler_max           = 5,
};

const vsf_hw_clk_t VSF_HW_CLK_HSI48 = {
    .clkcaltrim_region          = VSF_HW_CLKRST_REGION(0x00,  2, 14),   // CRM_CTRL.HICKCAL & HICKTRIM
    .clkcakkey_region           = VSF_HW_CLKRST_REGION(0x28,  0,  8),   // CRM_MISC1.HICKCAL_KEY
    .clken_region               = VSF_HW_CLKRST_REGION(0x00,  0,  1),   // CRM_CTRL.HICKEN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00,  1,  1),   // CRM_CTRL.HICKSTBL
    .clk_freq_hz                = 48 * 1000 * 1000,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_HSI8 = {
    .clksrc                     = &VSF_HW_CLK_HSI48,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
    .prescaler                  = 6,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_CONST,
};

static const vsf_hw_clk_t * const __VSF_HW_CLK_HSI_CLKSEL_MAPPER[2] = {
    &VSF_HW_CLK_HSI8, &VSF_HW_CLK_HSI48
};
const vsf_hw_clk_t VSF_HW_CLK_HSI = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x28, 12,  1),   // CRM_MISC1.HICKDIV
    .clksel_mapper              = __VSF_HW_CLK_HSI_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
};

const vsf_hw_clk_t VSF_HW_CLK_LSE = {
    .clkbypass_region           = VSF_HW_CLKRST_REGION(0x00, 18,  1),   // CRM_CTRL.HEXTBYPS
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 16,  1),   // CRM_CTRL.HEXTEN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 17,  1),   // CRM_CTRL.HEXTSTBL
    .clk_freq_hz                = HEXT_VALUE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
};

const vsf_hw_clk_t VSF_HW_CLK_LSI = {
    .clkbypass_region           = VSF_HW_CLKRST_REGION(0x00, 18,  1),   // CRM_CTRL.HEXTBYPS
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 16,  1),   // CRM_CTRL.HEXTEN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 17,  1),   // CRM_CTRL.HEXTSTBL
    .clk_freq_hz                = HEXT_VALUE,
    .clksrc_type                = VSF_HW_CLK_TYPE_CONST,
};

static const vsf_hw_clk_t * const __VSF_HW_CLK_HSI_TO_SYS_CLKSEL_MAPPER[2] = {
    &VSF_HW_CLK_HSI8, &VSF_HW_CLK_HSI
};
const vsf_hw_clk_t VSF_HW_CLK_HSI_TO_SYS = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x28, 14,  1),   // CRM_MISC1.HICK_TO_SCLK
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x29, 16,  3),   // CRM_MISC2.HICK_TO_SCLK_DIV
    .clksel_mapper              = __VSF_HW_CLK_HSI_TO_SYS_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_RSHIFT,
    .clkprescaler_max           = 4,
};

static const vsf_hw_clk_t * const __VSF_HW_CLK_PLL_CLKSEL_MAPPER[2] = {
    &VSF_HW_CLK_HSI8, &VSF_HW_CLK_HSE
};
static uint32_t __vsf_hw_pll_getclk(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler);
const vsf_hw_clk_t VSF_HW_CLK_PLL = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x00, 24,  1),   // CRM_CTRL.PLLEN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 25,  1),   // CRM_CTRL.PLLSTBL
    .clksel_region              = VSF_HW_CLKRST_REGION(0x01, 30,  1),   // CRM_PLLCFG.PLLRCS
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x01,  0, 16),   // CRM_PLLCFG.NS & MS
    .clksel_mapper              = __VSF_HW_CLK_PLL_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
    .getclk                     = __vsf_hw_pll_getclk,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_FUNC,
};
static const uint16_t __VSF_HW_CLK_PLLP_PRESCALER[16] = {
    1, 2, 4, 6, 8, 10 , 12, 14, 16, 18, 20, 22, 24, 26, 28, 30
};
const vsf_hw_clk_t VSF_HW_CLK_PLLP = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x01, 16,  4),   // CRM_PLLCFG.PLLRCS
    .clksrc                     = &VSF_HW_CLK_PLL,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
    .clkprescaler_mapper        = __VSF_HW_CLK_PLLP_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
};
static const uint16_t __VSF_HW_CLK_PLLU_PRESCALER[8] = {
    11, 13, 12, 14, 16, 18, 20, 11
};
const vsf_hw_clk_t VSF_HW_CLK_PLLU = {
    .clken_region               = VSF_HW_CLKRST_REGION(0x01, 29,  1),   // CRM_PLLCFG.PLLU_EN
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x00, 26,  1),   // CRM_CTRL.PLLUSTBL
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x01, 20,  3),   // CRM_PLLCFG.PLLRCS
    .clksrc                     = &VSF_HW_CLK_PLL,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,
    .clkprescaler_mapper        = __VSF_HW_CLK_PLLU_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
};

static const vsf_hw_clk_t * const __VSF_HW_CLK_SYS_CLKSEL_MAPPER[4] = {
    &VSF_HW_CLK_HSI_TO_SYS, &VSF_HW_CLK_HSE_TO_SYS, &VSF_HW_CLK_PLLP, NULL
};
const vsf_hw_clk_t VSF_HW_CLK_SYS = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x02,  0,  2),   // CRM_CFG.SCLKSEL
    .clkrdy_region              = VSF_HW_CLKRST_REGION(0x02,  2,  2),   // CRM_CFG.SCLKSTS
    .clksel_mapper              = __VSF_HW_CLK_SYS_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
    .clkrdy_type                = VSF_HW_CLK_RDY_MATCH_CLKSEL,
};

static const uint16_t __VSF_HW_CLK_AHB_PRESCALER[16] = {
    1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 64, 128, 256, 512
};
const vsf_hw_clk_t VSF_HW_CLK_AHB = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02,  4,  4),   // CRM_CFG.AHBDIV
    .clksrc                     = &VSF_HW_CLK_SYS,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_AHB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
};

const vsf_hw_clk_t VSF_HW_CLK_SYSTICK_EXT = {
    .clksrc                     = &VSF_HW_CLK_AHB,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .prescaler                  = 6,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_CONST,
};

static const uint16_t __VSF_HW_CLK_APB_PRESCALER[9] = {
    1, 1, 1, 1, 2, 4, 8, 16
};
const vsf_hw_clk_t VSF_HW_CLK_APB1 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02, 10,  3),   // CRM_CFG.APB1DIV
    .clksrc                     = &VSF_HW_CLK_AHB,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_APB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
};

const vsf_hw_clk_t VSF_HW_CLK_APB2 = {
    .clkprescaler_region        = VSF_HW_CLKRST_REGION(0x02, 13,  3),   // CRM_CFG.APB2DIV
    .clksrc                     = &VSF_HW_CLK_AHB,
    .clksrc_type                = VSF_HW_CLK_TYPE_CLK,

    .clkprescaler_mapper        = __VSF_HW_CLK_APB_PRESCALER,
    .clkprescaler_type          = VSF_HW_CLK_PRESCALER_DIV,
};

#if defined(VSF_HW_USB_OTG_MASK) && (VSF_HW_USB_OTG_MASK & (1 << 0))
static const vsf_hw_clk_t * const __VSF_HW_CLK_USBOTG0_CLKSEL_MAPPER[2] = {
    &VSF_HW_CLK_PLLU, &VSF_HW_CLK_HSI48
};
const vsf_hw_clk_t VSF_HW_CLK_USBOTG0 = {
    .clksel_region              = VSF_HW_CLKRST_REGION(0x29, 10,  1),   // CRM_MISC2.PLLU_USB48_SEL
    .clksel_mapper              = __VSF_HW_CLK_USBOTG0_CLKSEL_MAPPER,
    .clksrc_type                = VSF_HW_CLK_TYPE_SEL,
};
#endif

VSF_CAL_WEAK(system_core_clock)
unsigned int system_core_clock = HICK_VALUE;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#include "hal/driver/common/swi/arm/vsf_swi_template.inc"

void vsf_hw_clkrst_region_set(uint32_t region, uint_fast8_t value)
{
    uint8_t bit_length = (region >> 8) & 0xFF, bit_offset = (region >> 0) & 0x1F;
    VSF_HAL_ASSERT(!(value & ~((1 << bit_length) - 1)));
    uint8_t reg_word_offset = region >> 16;
    uint32_t mask = ((1 << bit_length) - 1) << bit_offset;

    vsf_atom32_op(
        &(((uint32_t *)CRM_BASE)[reg_word_offset]),
        (_ & ~mask) | (value << bit_offset)
    );
}

uint_fast8_t vsf_hw_clkrst_region_get(uint32_t region)
{
    uint8_t bit_length = (region >> 8) & 0xFF, bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    return (((uint32_t *)CRM_BASE)[reg_word_offset] >> bit_offset) & ((1 << bit_length) - 1);
}

void vsf_hw_clkrst_region_set_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0xFF));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    vsf_atom_or(32,
        &(((uint32_t *)CRM_BASE)[reg_word_offset]),
        (1 << bit_offset)
    );
}

void vsf_hw_clkrst_region_clear_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0xFF));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    vsf_atom_and(32,
        &(((uint32_t *)CRM_BASE)[reg_word_offset]),
        ~(1 << bit_offset)
    );
}

uint_fast8_t vsf_hw_clkrst_region_get_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0xFF));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    return (((uint32_t *)CRM_BASE)[reg_word_offset] >> bit_offset) & 1;
}





void vsf_hw_update_flash_latency(uint32_t sys_clk_hz)
{
    uint32_t div = sys_clk_hz / (32 * 1000 * 1000);
    uint32_t remain = sys_clk_hz % (32 * 1000 * 1000);

    if ((div > 0) && !remain) {
        div--;
    }
    VSF_HAL_ASSERT(div <= 6);
    flash_psr_set(div);
}





const vsf_hw_clk_t * vsf_hw_clk_get_src(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);

    switch (clk->clksrc_type) {
    case VSF_HW_CLK_TYPE_CLK:
        return clk->clksrc;
    case VSF_HW_CLK_TYPE_SEL:
        return clk->clksel_mapper[vsf_hw_clkrst_region_get(clk->clksel_region)];
    default:
        return NULL;
    }
}

static uint32_t __vsf_hw_clk_get_src_freq_hz(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);
    switch (clk->clksrc_type) {
    case VSF_HW_CLK_TYPE_CONST:
        return clk->clk_freq_hz;
    case VSF_HW_CLK_TYPE_CLK:
        VSF_HAL_ASSERT(clk->clksrc != NULL);
        return vsf_hw_clk_get_freq_hz(clk->clksrc);
    case VSF_HW_CLK_TYPE_SEL: {
            const vsf_hw_clk_t *clksrc = clk->clksel_mapper[vsf_hw_clkrst_region_get(clk->clksel_region)];
            if (NULL == clksrc) {
                return 0;
            } else {
                return vsf_hw_clk_get_freq_hz(clksrc);
            }
        }
        break;
    default:
        VSF_HAL_ASSERT(false);
        return 0;
    }
}

uint32_t vsf_hw_clk_get_freq_hz(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);

    if (    (clk->clken_region != 0)
        &&  !vsf_hw_clkrst_region_get_bit(clk->clken_region)) {
        return 0;
    }

    uint32_t clk_freq_hz = __vsf_hw_clk_get_src_freq_hz(clk);

    if (VSF_HW_CLK_PRESCALER_CONST == clk->clkprescaler_type) {
        VSF_HAL_ASSERT(clk->prescaler > 0);
        clk_freq_hz /= clk->prescaler;
    } else if (clk->clkprescaler_type != VSF_HW_CLK_PRESCALER_NONE) {
        bool isfunc = clk->clkprescaler_type == VSF_HW_CLK_PRESCALER_FUNC;
        uint32_t prescaler = 0;

        if (clk->clkprescaler_region != 0) {
            prescaler = vsf_hw_clkrst_region_get(clk->clkprescaler_region);
            if ((clk->clkprescaler_max != 0) && (prescaler > clk->clkprescaler_max)) {
                VSF_HAL_ASSERT(false);
                return 0;
            }
            if ((clk->clkprescaler_mapper != NULL) && !isfunc) {
                prescaler = clk->clkprescaler_mapper[prescaler];
            }
        }

        switch (clk->clkprescaler_type) {
        case VSF_HW_CLK_PRESCALER_DIV:
            VSF_HAL_ASSERT(prescaler > 0);
            clk_freq_hz /= prescaler;
            break;
        case VSF_HW_CLK_PRESCALER_RSHIFT:
            clk_freq_hz >>= prescaler;
            break;
        case VSF_HW_CLK_PRESCALER_FUNC:
            clk_freq_hz = clk->getclk(clk, clk_freq_hz, prescaler);
            break;
        }
    }
    return clk_freq_hz;
}

void vsf_hw_clk_enable(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);
    if (clk->clken_region != 0) {
        vsf_hw_clkrst_region_set_bit(clk->clken_region);
    }
    while (!vsf_hw_clk_is_ready(clk));
}

void vsf_hw_clk_disable(const vsf_hw_clk_t *clk)
{
    VSF_HAL_ASSERT(clk != NULL);
    if (clk->clken_region != 0) {
        vsf_hw_clkrst_region_clear_bit(clk->clken_region);
    }
}

bool vsf_hw_clk_is_enabled(const vsf_hw_clk_t *clk)
{
    if (clk->clken_region != 0) {
        return !!vsf_hw_clkrst_region_get_bit(clk->clken_region);
    }
    return true;
}

bool vsf_hw_clk_is_ready(const vsf_hw_clk_t *clk)
{
    if (clk->clkrdy_region != 0) {
        switch (clk->clkrdy_type) {
        case VSF_HW_CLK_RDY_BIT:
            return !!vsf_hw_clkrst_region_get_bit(clk->clkrdy_region);
        case VSF_HW_CLK_RDY_MATCH_CLKSEL:
            return vsf_hw_clkrst_region_get(clk->clksel_region) == vsf_hw_clkrst_region_get(clk->clkrdy_region);
        }
    }
    return true;
}

vsf_err_t vsf_hw_clk_config(const vsf_hw_clk_t *clk, const vsf_hw_clk_t *clksrc, uint16_t prescaler, uint32_t freq_hz)
{
    VSF_HAL_ASSERT(clk != NULL);
    if (clksrc != NULL) {
        VSF_HAL_ASSERT(clk->clksel_region != 0);
        if (clk->clksrc_type != VSF_HW_CLK_TYPE_SEL) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }

        uint8_t clksel_num = 1 << ((clk->clksel_region >> 8) & 0xFF);
        uint8_t clksel_idx = 0;
        for (; clksel_idx < clksel_num; clksel_idx++) {
            if (clk->clksel_mapper[clksel_idx] == clksrc) {
                break;
            }
        }
        if (clksel_idx >= clksel_num) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }

        vsf_hw_clkrst_region_set(clk->clksel_region, clksel_idx);
        if (clk->clkrdy_region != 0 && clk->clkrdy_type == VSF_HW_CLK_RDY_MATCH_CLKSEL) {
            while (!vsf_hw_clk_is_ready(clk));
        }
    }

    if (prescaler != 0) {
        VSF_HAL_ASSERT(clk->clkprescaler_region != 0);

        switch (clk->clkprescaler_type) {
        case VSF_HW_CLK_PRESCALER_DIV:
            if (clk->clkprescaler_mapper != NULL) {
                uint8_t prescaler_num = 1 << ((clk->clkprescaler_region >> 8) & 0xFF);
                for (uint8_t i = 0; i < prescaler_num; i++) {
                    if (clk->clkprescaler_mapper[i] == prescaler) {
                        prescaler = i;
                        break;
                    }
                }
            }
            break;
        case VSF_HW_CLK_PRESCALER_RSHIFT: {
                uint16_t i = 0;
                while (prescaler > 1) {
                    prescaler >>= 1;
                    i++;
                }
                if (((1 << ((clk->clkprescaler_region >> 8) & 0xFF)) - 1) < i) {
                    VSF_HAL_ASSERT(false);
                    return VSF_ERR_INVALID_PARAMETER;
                }
                prescaler = i;
            }
            break;
        case VSF_HW_CLK_PRESCALER_CONST:
        case VSF_HW_CLK_PRESCALER_FUNC:
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }

        if ((clk->clkprescaler_max != 0) && (prescaler > clk->clkprescaler_max)) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }
        vsf_hw_clkrst_region_set(clk->clkprescaler_region, prescaler);
    } else if (freq_hz != 0) {
        uint32_t clk_freq_hz = __vsf_hw_clk_get_src_freq_hz(clk);
        VSF_HAL_ASSERT(clk_freq_hz >= freq_hz);
        return vsf_hw_clk_config(clk, NULL, clk_freq_hz / freq_hz, 0);
    }
    return VSF_ERR_NONE;
}

static uint32_t __vsf_hw_pll_getclk(const vsf_hw_clk_t *clk, uint32_t clksrc_freq_hz, uint32_t prescaler)
{
    uint16_t pll_ns_ms = vsf_hw_clkrst_region_get(clk->clkprescaler_region);
    return clksrc_freq_hz * (pll_ns_ms >> 6) / (pll_ns_ms & 15);
}

vsf_err_t vsf_hw_pll_config(const vsf_hw_clk_t *clk, const vsf_hw_clk_t *clksrc, uint16_t mul, uint16_t div, uint32_t out_freq_hz)
{
    VSF_HAL_ASSERT(clk->getclk == __vsf_hw_pll_getclk);

    vsf_err_t ret = vsf_hw_clk_config(clk, clksrc, 0, 0);
    if (ret != VSF_ERR_NONE) {
        return ret;
    }

    if (mul != 0 && div != 0) {
        VSF_HAL_ASSERT(mul >= 31 && mul <= 500);
        VSF_HAL_ASSERT(div >= 1 && div <= 15);
        uint16_t pll_ns_ms = div | (mul << 6);
        vsf_hw_clkrst_region_set(clk->clkprescaler_region, pll_ns_ms);
    } else if (out_freq_hz != 0) {
        // TODO
        VSF_HAL_ASSERT(false);
    }

    return VSF_ERR_NONE;
}

VSF_CAL_WEAK(SystemInit)
void SystemInit(void)
{
    /* reset the crm clock configuration to the default reset state(for debug purpose) */
    /* enable auto step mode */
    CRM->misc2_bit.auto_step_en = CRM_AUTO_STEP_MODE_ENABLE;

    vsf_hw_clk_enable(&VSF_HW_CLK_HCLK);
    vsf_hw_clk_config(&VSF_HW_CLK_SCLK, &VSF_HW_CLK_HSI_TO_SYS, 0, 0);

    /* reset cfg register, include sclk switch, ahbdiv, apb1div, apb2div, adcdiv, clkout bits */
    CRM->cfg = (0x40000000U);

    /* reset hexten, hextbyps, cfden and pllen bits */
    CRM->ctrl &= ~(0x010D0000U);

    /* reset pllms pllns pllfr pllrcs bits */
    CRM->pllcfg = 0x000007C1U;

    /* reset clkout_sel, clkoutdiv, pllclk_to_adc, hick_to_usb */
    CRM->misc1 &= 0x00005000U;
    CRM->misc1 |= 0x000F0000U;

    /* disable all interrupts enable and clear pending bits  */
    CRM->clkint = 0x009F0000U;
}

VSF_CAL_WEAK(system_core_clock_update)
void system_core_clock_update(void)
{
    system_core_clock = vsf_hw_clk_get_freq_hz(&VSF_HW_CLK_AHB);
}

// TODO: if these initialization is implemented in startup file, remove here
int __low_level_init(void)
{
    return 1;
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    return true;
}


/* EOF */
