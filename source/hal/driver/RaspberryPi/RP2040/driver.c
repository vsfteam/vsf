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
#include "./device.h"
#include "./driver.h"

#include "pico/platform.h"
#include "hardware/platform_defs.h"
#include "hardware/address_mapped.h"
#include "hardware/regs/resets.h"
#include "hardware/structs/xosc.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/resets.h"

/*============================ MACROS ========================================*/

/*******************************************************************************
*      code from src/rp2_common/hardware_clocks/include/hardware/clocks.h      *
*******************************************************************************/

#define KHZ 1000
#define MHZ 1000000

/// \tag::pll_settings[]
//
// There are two PLLs in RP2040:
// 1. The 'SYS PLL' generates the 125MHz system clock, the frequency is defined by `SYS_CLK_KHZ`.
// 2. The 'USB PLL' generates the 48MHz USB clock, the frequency is defined by `USB_CLK_KHZ`.
//
// The two PLLs use the crystal oscillator output directly as their reference frequency input; the PLLs reference
// frequency cannot be reduced by the dividers present in the clocks block. The crystal frequency is defined by `XOSC_KHZ` or
// `XOSC_MHZ`.
//
// The system's default definitions are correct for the above frequencies with a 12MHz
// crystal frequency.  If different frequencies are required, these must be defined in
// the board configuration file together with the revised PLL settings
// Use `vcocalc.py` to check and calculate new PLL settings if you change any of these frequencies.
//
// Default PLL configuration:
//                   REF     FBDIV VCO            POSTDIV
// PLL SYS: 12 / 1 = 12MHz * 125 = 1500MHz / 6 / 2 = 125MHz
// PLL USB: 12 / 1 = 12MHz * 100 = 1200MHz / 5 / 5 =  48MHz
/// \end::pll_settings[]

// PICO_CONFIG: PLL_COMMON_REFDIV, PLL reference divider setting - used for both PLLs, type=int, default=1, advanced=true, group=hardware_clocks
#ifndef PLL_COMMON_REFDIV
#define PLL_COMMON_REFDIV                   1
#endif

#if (SYS_CLK_KHZ == 125000) && (XOSC_KHZ == 12000) && (PLL_COMMON_REFDIV == 1)
// PLL settings for standard 125 MHz system clock.
// PICO_CONFIG: PLL_SYS_VCO_FREQ_KHZ, System clock PLL frequency, type=int, default=1500 * KHZ, advanced=true, group=hardware_clocks
#ifndef PLL_SYS_VCO_FREQ_KHZ
#define PLL_SYS_VCO_FREQ_KHZ                (1500 * KHZ)
#endif
// PICO_CONFIG: PLL_SYS_POSTDIV1, System clock PLL post divider 1 setting, type=int, default=6, advanced=true, group=hardware_clocks
#ifndef PLL_SYS_POSTDIV1
#define PLL_SYS_POSTDIV1                    6
#endif
// PICO_CONFIG: PLL_SYS_POSTDIV2, System clock PLL post divider 2 setting, type=int, default=2, advanced=true, group=hardware_clocks
#ifndef PLL_SYS_POSTDIV2
#define PLL_SYS_POSTDIV2                    2
#endif
#endif // SYS_CLK_KHZ == 125000 && XOSC_KHZ == 12000 && PLL_COMMON_REFDIV == 1
#if !defined(PLL_SYS_VCO_FREQ_KHZ) || !defined(PLL_SYS_POSTDIV1) || !defined(PLL_SYS_POSTDIV2)
#error PLL_SYS_VCO_FREQ_KHZ, PLL_SYS_POSTDIV1 and PLL_SYS_POSTDIV2 must all be specified when using custom clock setup
#endif

/*******************************************************************************
*                       code from board configuration                          *
*******************************************************************************/

// On some samples, the xosc can take longer to stabilize than is usual
#ifndef PICO_XOSC_STARTUP_DELAY_MULTIPLIER
#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER  64
#endif

/*******************************************************************************
*             code from src/rp2_common/hardware_xosc/xosc.c                    *
*******************************************************************************/

#if XOSC_KHZ < (1 * KHZ) || XOSC_KHZ > (50 * KHZ)
// Note: Although an external clock can be supplied up to 50 MHz, the maximum frequency the
// XOSC cell is specified to work with a crystal is less, please see the RP2040 Datasheet.
#error XOSC_KHZ must be in the range 1,000-50,000KHz i.e. 1-50MHz XOSC frequency
#endif

#define STARTUP_DELAY (((XOSC_KHZ + 128) / 256) * PICO_XOSC_STARTUP_DELAY_MULTIPLIER)

// The DELAY field in xosc_hw->startup is 14 bits wide.
#if STARTUP_DELAY >= (1 << 13)
#error PICO_XOSC_STARTUP_DELAY_MULTIPLIER is too large: XOSC STARTUP.DELAY must be < 8192
#endif

/*******************************************************************************
*    code from src/rp2_common/hardware_resets/include/hardware/resets.h        *
*******************************************************************************/

/// \tag::reset_funcs[]

/*! \brief Reset the specified HW blocks
 *  \ingroup hardware_resets
 *
 * \param bits Bit pattern indicating blocks to reset. See \ref reset_bitmask
 */
static inline void reset_block(uint32_t bits) {
    hw_set_bits(&resets_hw->reset, bits);
}

/*! \brief bring specified HW blocks out of reset
 *  \ingroup hardware_resets
 *
 * \param bits Bit pattern indicating blocks to unreset. See \ref reset_bitmask
 */
static inline void unreset_block(uint32_t bits) {
    hw_clear_bits(&resets_hw->reset, bits);
}

/*! \brief Bring specified HW blocks out of reset and wait for completion
 *  \ingroup hardware_resets
 *
 * \param bits Bit pattern indicating blocks to unreset. See \ref reset_bitmask
 */
static inline void unreset_block_wait(uint32_t bits) {
    hw_clear_bits(&resets_hw->reset, bits);
    while (~resets_hw->reset_done & bits)
        tight_loop_contents();
}
/// \end::reset_funcs[]

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#include "hal/driver/common/swi/arm/vsf_swi_template.inc"

/*******************************************************************************
*             code from src/rp2_common/hardware_xosc/xosc.c                    *
*******************************************************************************/

void xosc_init(void) {
    // Assumes 1-15 MHz input, checked above.
    xosc_hw->ctrl = XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ;

    // Set xosc startup delay
    xosc_hw->startup = STARTUP_DELAY;

    // Set the enable bit now that we have set freq range and startup delay
    hw_set_bits(&xosc_hw->ctrl, XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB);

    // Wait for XOSC to be stable
    while(!(xosc_hw->status & XOSC_STATUS_STABLE_BITS));
}

/*******************************************************************************
*              code from src/rp2_common/hardware_pll/pll.c                     *
*******************************************************************************/

/// \tag::pll_init_calculations[]
void pll_init(pll_hw_t *pll, uint refdiv, uint vco_freq, uint post_div1, uint post_div2) {
    uint32_t ref_freq = XOSC_KHZ * KHZ / refdiv;

    // Check vco freq is in an acceptable range
    assert(vco_freq >= (PICO_PLL_VCO_MIN_FREQ_KHZ * KHZ) && vco_freq <= (PICO_PLL_VCO_MAX_FREQ_KHZ * KHZ));

    // What are we multiplying the reference clock by to get the vco freq
    // (The regs are called div, because you divide the vco output and compare it to the refclk)
    uint32_t fbdiv = vco_freq / ref_freq;
/// \end::pll_init_calculations[]

    // fbdiv
    assert(fbdiv >= 16 && fbdiv <= 320);

    // Check divider ranges
    assert((post_div1 >= 1 && post_div1 <= 7) && (post_div2 >= 1 && post_div2 <= 7));

    // post_div1 should be >= post_div2
    // from appnote page 11
    // postdiv1 is designed to operate with a higher input frequency than postdiv2

    // Check that reference frequency is no greater than vco / 16
    assert(ref_freq <= (vco_freq / 16));

    // div1 feeds into div2 so if div1 is 5 and div2 is 2 then you get a divide by 10
    uint32_t pdiv = (post_div1 << PLL_PRIM_POSTDIV1_LSB) |
                    (post_div2 << PLL_PRIM_POSTDIV2_LSB);

/// \tag::pll_init_finish[]
    if ((pll->cs & PLL_CS_LOCK_BITS) &&
        (refdiv == (pll->cs & PLL_CS_REFDIV_BITS)) &&
        (fbdiv  == (pll->fbdiv_int & PLL_FBDIV_INT_BITS)) &&
        (pdiv   == (pll->prim & (PLL_PRIM_POSTDIV1_BITS | PLL_PRIM_POSTDIV2_BITS)))) {
        // do not disrupt PLL that is already correctly configured and operating
        return;
    }

    uint32_t pll_reset = (pll_usb_hw == pll) ? RESETS_RESET_PLL_USB_BITS : RESETS_RESET_PLL_SYS_BITS;
    reset_block(pll_reset);
    unreset_block_wait(pll_reset);

    // Load VCO-related dividers before starting VCO
    pll->cs = refdiv;
    pll->fbdiv_int = fbdiv;

    // Turn on PLL
    uint32_t power = PLL_PWR_PD_BITS | // Main power
                     PLL_PWR_VCOPD_BITS; // VCO Power

    hw_clear_bits(&pll->pwr, power);

    // Wait for PLL to lock
    while (!(pll->cs & PLL_CS_LOCK_BITS)) tight_loop_contents();

    // Set up post dividers
    pll->prim = pdiv;

    // Turn on post divider
    hw_clear_bits(&pll->pwr, PLL_PWR_POSTDIVPD_BITS);
/// \end::pll_init_finish[]
}

/*******************************************************************************
*           code from src/rp2_common/hardware_clocks/clocks.c                  *
*******************************************************************************/

static uint32_t configured_freq[CLK_COUNT];

// Clock muxing consists of two components:
// - A glitchless mux, which can be switched freely, but whose inputs must be
//   free-running
// - An auxiliary (glitchy) mux, whose output glitches when switched, but has
//   no constraints on its inputs
// Not all clocks have both types of mux.
static inline bool has_glitchless_mux(enum clock_index clk_index) {
    return clk_index == clk_sys || clk_index == clk_ref;
}

/// \tag::clock_configure[]
bool clock_configure(enum clock_index clk_index, uint32_t src, uint32_t auxsrc, uint32_t src_freq, uint32_t freq) {
    uint32_t div;

    assert(src_freq >= freq);

    if (freq > src_freq)
        return false;

    // Div register is 24.8 int.frac divider so multiply by 2^8 (left shift by 8)
    div = (uint32_t) (((uint64_t) src_freq << CLOCKS_CLK_GPOUT0_DIV_INT_LSB) / freq);

    clock_hw_t *clock = &clocks_hw->clk[clk_index];

    // If increasing divisor, set divisor before source. Otherwise set source
    // before divisor. This avoids a momentary overspeed when e.g. switching
    // to a faster source and increasing divisor to compensate.
    if (div > clock->div)
        clock->div = div;

    // If switching a glitchless slice (ref or sys) to an aux source, switch
    // away from aux *first* to avoid passing glitches when changing aux mux.
    // Assume (!!!) glitchless source 0 is no faster than the aux source.
    if (has_glitchless_mux(clk_index) && src == CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX) {
        hw_clear_bits(&clock->ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
        while (!(clock->selected & 1u))
            tight_loop_contents();
    }
    // If no glitchless mux, cleanly stop the clock to avoid glitches
    // propagating when changing aux mux. Note it would be a really bad idea
    // to do this on one of the glitchless clocks (clk_sys, clk_ref).
    else {
        // Disable clock. On clk_ref and clk_sys this does nothing,
        // all other clocks have the ENABLE bit in the same position.
        hw_clear_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);
        if (configured_freq[clk_index] > 0) {
            // Delay for 3 cycles of the target clock, for ENABLE propagation.
            // Note XOSC_COUNT is not helpful here because XOSC is not
            // necessarily running, nor is timer...:
            uint delay_cyc = configured_freq[clk_sys] / configured_freq[clk_index] + 1;
            busy_wait_at_least_cycles(delay_cyc * 3);
        }
    }

    // Set aux mux first, and then glitchless mux if this clock has one
    hw_write_masked(&clock->ctrl,
        (auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB),
        CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS
    );

    if (has_glitchless_mux(clk_index)) {
        hw_write_masked(&clock->ctrl,
            src << CLOCKS_CLK_REF_CTRL_SRC_LSB,
            CLOCKS_CLK_REF_CTRL_SRC_BITS
        );
        while (!(clock->selected & (1u << src)))
            tight_loop_contents();
    }

    // Enable clock. On clk_ref and clk_sys this does nothing,
    // all other clocks have the ENABLE bit in the same position.
    hw_set_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);

    // Now that the source is configured, we can trust that the user-supplied
    // divisor is a safe value.
    clock->div = div;

    // Store the configured frequency
    configured_freq[clk_index] = (uint32_t)(((uint64_t) src_freq << 8) / div);

    return true;
}
/// \end::clock_configure[]

/// \tag::clock_get_hz[]
uint32_t clock_get_hz(enum clock_index clk_index) {
    return configured_freq[clk_index];
}
/// \end::clock_get_hz[]

/*******************************************************************************
*            code from src/rp2_common/pico_runtime/runtime.c                   *
*******************************************************************************/

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    // Reset all peripherals to put system into a known state,
    // - except for QSPI pads and the XIP IO bank, as this is fatal if running from flash
    // - and the PLLs, as this is fatal if clock muxing has not been reset on this boot
    // - and USB, syscfg, as this disturbs USB-to-SWD on core 1
    reset_block(~(
            RESETS_RESET_IO_QSPI_BITS |
            RESETS_RESET_PADS_QSPI_BITS |
            RESETS_RESET_PLL_USB_BITS |
            RESETS_RESET_USBCTRL_BITS |
            RESETS_RESET_SYSCFG_BITS |
            RESETS_RESET_PLL_SYS_BITS
    ));
    // Remove reset from peripherals which are clocked only by clk_sys and
    // clk_ref. Other peripherals stay in reset until we've configured clocks.
    unreset_block_wait(RESETS_RESET_BITS & ~(
            RESETS_RESET_ADC_BITS |
            RESETS_RESET_RTC_BITS |
            RESETS_RESET_SPI0_BITS |
            RESETS_RESET_SPI1_BITS |
            RESETS_RESET_UART0_BITS |
            RESETS_RESET_UART1_BITS |
            RESETS_RESET_USBCTRL_BITS
    ));

    // Disable resus that may be enabled from previous software
    clocks_hw->resus.ctrl = 0;

    // Enable the xosc
    xosc_init();

    // Before we touch PLLs, switch sys and ref cleanly away from their aux sources.
    hw_clear_bits(&clocks_hw->clk[clk_sys].ctrl, CLOCKS_CLK_SYS_CTRL_SRC_BITS);
    while (clocks_hw->clk[clk_sys].selected != 0x1)
        tight_loop_contents();
    hw_clear_bits(&clocks_hw->clk[clk_ref].ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
    while (clocks_hw->clk[clk_ref].selected != 0x1)
        tight_loop_contents();

    /// \tag::pll_init[]
    pll_init(pll_sys_hw, PLL_COMMON_REFDIV, PLL_SYS_VCO_FREQ_KHZ * KHZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
// initialize usb pll in usb module when required
//    pll_init(pll_usb_hw, PLL_COMMON_REFDIV, PLL_USB_VCO_FREQ_KHZ * KHZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
    /// \end::pll_init[]

    /// \tag::configure_clk_sys[]
    // CLK SYS = PLL SYS (usually) 125MHz / 1 = 125MHz
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                    SYS_CLK_KHZ * KHZ,
                    SYS_CLK_KHZ * KHZ);
    /// \end::configure_clk_sys[]

    // CLK PERI = clk_sys. Used as reference clock for Peripherals. No dividers so just select and enable
    // Normally choose clk_sys or clk_usb
    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    SYS_CLK_KHZ * KHZ,
                    SYS_CLK_KHZ * KHZ);

    unreset_block_wait(RESETS_RESET_BITS);
    return true;
}


/* EOF */
