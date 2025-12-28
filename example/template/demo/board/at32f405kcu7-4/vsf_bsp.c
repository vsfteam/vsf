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

#define __VSF_HEAP_CLASS_INHERIT__
#include "vsf.h"

#ifdef __AT32F405KCU7_4__

#include "hal/driver/vendor_driver.h"
#include "hal/driver/Artery/AT32F402_405/common/device.h"
#include "hal/driver/Artery/AT32F402_405/common/usart/usart.h"
#include "hal/driver/Artery/AT32F402_405/common/gpio/gpio.h"
#include "service/simple_stream/vsf_simple_stream.h"

#if APP_USE_LINUX_DEMO == ENABLED
//  for VSF_LINUX_CFG_BIN_PATH
#   if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#       include "shell/sys/linux/include/unistd.h"
#   else
#       include <unistd.h>
#   endif
#endif

#if APP_USE_HAL_DEMO == ENABLED
#   include "../../hal_demo/hal_demo.h"
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE
#   define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE     32
#endif

#ifndef VSF_DEBUG_STREAM_CFG_BAUDRATE
#   define VSF_DEBUG_STREAM_CFG_BAUDRATE        921600
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/******************************************************************************/
/*                             Debug Streams                                  */
/******************************************************************************/

describe_mem_stream(VSF_DEBUG_STREAM_RX, VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE)

static void __debug_stream_usart_isr_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        uint8_t *buffer;
        uint_fast32_t buflen;

        while (vsf_usart_rxfifo_get_data_count(usart_ptr) > 0) {
            buflen = VSF_STREAM_GET_WBUF(&VSF_DEBUG_STREAM_RX, &buffer);
            if (buflen) {
                VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, NULL, vsf_usart_rxfifo_read(usart_ptr, buffer, buflen));
            } else {
                VSF_ASSERT(0);
            }
        }
    }
}

static void __VSF_DEBUG_STREAM_TX_INIT(void)
{
    vsf_err_t err;
    fsm_rt_t fsm_rt;
    vsf_usart_cfg_t usart_cfg = {
        .baudrate = VSF_DEBUG_STREAM_CFG_BAUDRATE,
        .mode = VSF_USART_8_BIT_LENGTH
              | VSF_USART_1_STOPBIT
              | VSF_USART_NO_PARITY
              | VSF_USART_TX_ENABLE
              | VSF_USART_RX_ENABLE,
        .rx_timeout = 0,
        .isr = {
            .handler_fn = __debug_stream_usart_isr_handler,
            .target_ptr = NULL,
            .prio = vsf_arch_prio_0,
        },
    };

    /* enable the uart and gpio clock */
    vsf_hw_peripheral_enable(VSF_HW_EN_USART2);
    vsf_hw_peripheral_enable(VSF_HW_EN_GPIOA);

    /* configure the uart tx and rx pins (PA2, PA3) */
    vsf_gpio_cfg_t gpio_cfg_tx = {
        .mode = VSF_GPIO_AF_OUTPUT_PUSH_PULL | VSF_GPIO_DRIVE_STRENGTH_HIGH | VSF_GPIO_NO_PULL_UP_DOWN,
        .alternate_function = VSF_HW_AF_USART2_TX_P0_2,
    };
    vsf_gpio_cfg_t gpio_cfg_rx = {
        .mode = VSF_GPIO_AF_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
        .alternate_function = VSF_HW_AF_USART2_RX_P0_3,
    };
    err = vsf_gpio_port_config_pins((vsf_gpio_t *)&vsf_hw_gpio0, (1 << 2), &gpio_cfg_tx);
    VSF_ASSERT(VSF_ERR_NONE == err);
    err = vsf_gpio_port_config_pins((vsf_gpio_t *)&vsf_hw_gpio0, (1 << 3), &gpio_cfg_rx);
    VSF_ASSERT(VSF_ERR_NONE == err);

    /* initialize and enable usart */
    err = vsf_usart_init((vsf_usart_t *)&vsf_hw_usart2, &usart_cfg);
    VSF_ASSERT(VSF_ERR_NONE == err);

    vsf_usart_irq_enable((vsf_usart_t *)&vsf_hw_usart2, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_ERR);

    fsm_rt = vsf_usart_enable((vsf_usart_t *)&vsf_hw_usart2);
    VSF_ASSERT(fsm_rt_cpl == fsm_rt);

    VSF_STREAM_CONNECT_TX(&VSF_DEBUG_STREAM_RX);
}

static void __VSF_DEBUG_STREAM_TX_WRITE_BLOCKED(uint8_t *buf, uint_fast32_t size)
{
    vsf_usart_t *usart = (vsf_usart_t *)&vsf_hw_usart2;
    uint_fast32_t written;

    for (uint_fast32_t i = 0; i < size; i++) {
        while (vsf_usart_txfifo_get_free_count(usart) == 0);
        written = vsf_usart_txfifo_write(usart, &buf[i], 1);
        VSF_ASSERT(written == 1);
    }
    /* wait for transmission complete */
    while (vsf_usart_status(usart).is_tx_busy);
}

#define VSF_HAL_USE_DEBUG_STREAM                ENABLED
#include "hal/driver/common/debug_stream/debug_stream_tx_blocked.inc"

static void __clock_init(void)
{
    // Configure clock to maximum performance mode
    // Reference: AT32 SDK at32f402_405_clock.c
    // PLL configuration:
    //   HEXT = 12MHz
    //   PLL_MS = 1, PLL_NS = 72 → VCO = 12MHz × 72 / 1 = 864MHz (satisfies 500~1000MHz)
    //   PLLP = 864MHz / 4 = 216MHz → PLL_FP = 4
    //   PLLU = 864MHz / 18 = 48MHz → PLL_FU = 18
    // System clock: 216 MHz
    // AHB: 216 MHz (div=1)
    // APB2: 216 MHz (div=1)
    // APB1: 108 MHz (div=2, max 120 MHz)

    // Step 1: Update flash latency for 216 MHz (need 6 wait cycles)
    // Equivalent to: flash_psr_set(FLASH_WAIT_CYCLE_6)
    vsf_hw_update_flash_latency(216 * 1000 * 1000);

    // Step 2: Enable HSE (HEXT)
    // Equivalent to: crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE)
    vsf_hw_clk_enable(&VSF_HW_CLK_HSE);
    while (!vsf_hw_clk_is_ready(&VSF_HW_CLK_HSE));

    // Step 3: Configure PLL: PLL_MS = 1, PLL_NS = 72
    // Equivalent to: crm_pll_config(CRM_PLL_SOURCE_HEXT, 72, 1, CRM_PLL_FP_4)
    // VCO = HEXT × PLL_NS / PLL_MS = 12 × 72 / 1 = 864MHz
    // mul = PLL_NS = 72, div = PLL_MS = 1
    vsf_hw_pll_config(&VSF_HW_CLK_PLL, &VSF_HW_CLK_HSE, 72, 1, 0);

    // Step 4: Configure PLLU prescaler before enabling PLL
    // Equivalent to: crm_pllu_div_set(CRM_PLL_FU_18)
    // PLLU prescaler options: [11, 13, 12, 14, 16, 18, 20, 11]
    // PLL_FU = 18 → function will find index 5 (value 18) in mapper
    vsf_hw_clk_config(&VSF_HW_CLK_PLLU, NULL, 18, 0);  // prescaler value 18

    // Step 5: Configure PLLP output before enabling PLL (PLLP is used as system clock source)
    // PLLP divider: PLL_FP = 4 → function will find index 2 (value 4) in mapper
    // PLLP prescaler options: [1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30]
    vsf_hw_clk_config(&VSF_HW_CLK_PLLP, NULL, 4, 0);  // prescaler value 4

    // Step 6: Enable PLL
    // Equivalent to: crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE)
    vsf_hw_clk_enable(&VSF_HW_CLK_PLL);
    while (!vsf_hw_clk_is_ready(&VSF_HW_CLK_PLL));

    // Step 7: Configure AHB: div=1 (216 MHz)
    // Equivalent to: crm_ahb_div_set(CRM_AHB_DIV_1)
    vsf_hw_clk_config(&VSF_HW_CLK_AHB, NULL, 1, 0);

    // Step 8: Configure APB2: div=1 (216 MHz, max 216 MHz)
    // Equivalent to: crm_apb2_div_set(CRM_APB2_DIV_1)
    vsf_hw_clk_config(&VSF_HW_CLK_APB2, NULL, 1, 0);

    // Step 9: Configure APB1: div=2 (108 MHz, max 120 MHz)
    // Equivalent to: crm_apb1_div_set(CRM_APB1_DIV_2)
    vsf_hw_clk_config(&VSF_HW_CLK_APB1, NULL, 2, 0);

    // Step 10: Switch system clock to PLLP
    // Equivalent to: crm_sysclk_switch(CRM_SCLK_PLL)
    // Note: vsf_hw_clk_config will wait for clock ready internally if clkrdy_type is VSF_HW_CLK_RDY_MATCH_CLKSEL
    vsf_hw_clk_config(&VSF_HW_CLK_SYS, &VSF_HW_CLK_PLLP, 0, 0);

    // Step 11: Update system core clock variable
    // Equivalent to: system_core_clock_update()
    system_core_clock_update();

    // Initialize USB clocks
#if defined(VSF_HW_USB_OTG_MASK) && (VSF_HW_USB_OTG_MASK & (1 << 0))
    // Enable USB OTG0 peripheral clock (equivalent to crm_periph_clock_enable(OTGFS_CLOCK, TRUE))
    vsf_hw_peripheral_enable(VSF_HW_EN_USBOTG0);

    // Select USB 48MHz clock source from PLL (via PLLU)
    // Equivalent to usb_clock48m_select(USB_CLK_HEXT)
    // Enable PLLU output (equivalent to crm_pllu_output_set(TRUE))
    // Note: PLLU prescaler is already configured above (index 5 = 18)
    // PLLU frequency = PLL / prescaler, but actual calculation may be different
    // For 48MHz: need to verify PLLU output frequency with hardware
    vsf_hw_clk_enable(&VSF_HW_CLK_PLLU);
    while (!vsf_hw_clk_is_ready(&VSF_HW_CLK_PLLU));

    // Configure USBOTG0 to use PLLU as clock source
    // Equivalent to crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_PLLU)
    // clksel_mapper[0] = PLLU, clksel_mapper[1] = HSI48
    vsf_hw_clk_config(&VSF_HW_CLK_USBOTG0, &VSF_HW_CLK_PLLU, 0, 0);
    while (!vsf_hw_clk_is_ready(&VSF_HW_CLK_USBOTG0));
#endif

#if defined(VSF_HW_USB_OTG_MASK) && (VSF_HW_USB_OTG_MASK & (1 << 1))
    // Enable USB OTG1 (OTGHS) peripheral clock (equivalent to crm_periph_clock_enable(OTGHS_CLOCK, TRUE))
    vsf_hw_peripheral_enable(VSF_HW_EN_OTGHS);

    // USB OTG1 uses HEXT (12MHz) as PHY clock, which is already enabled above
    // No additional clock configuration needed for OTG1
#endif
}

#if APP_USE_LINUX_DEMO == ENABLED
static void __sysclock_dump_clk(const char *name, const vsf_hw_clk_t *clk)
{
    if (clk == NULL) {
        return;
    }

    uint32_t freq_hz = vsf_hw_clk_get_freq_hz(clk);
    bool enabled = vsf_hw_clk_is_enabled(clk);
    bool ready = vsf_hw_clk_is_ready(clk);

    printf("  %-16s: ", name);
    if (enabled && ready) {
        if (freq_hz >= 1000000) {
            printf("%lu MHz", freq_hz / 1000000);
        } else if (freq_hz >= 1000) {
            printf("%lu kHz", freq_hz / 1000);
        } else {
            printf("%lu Hz", freq_hz);
        }
    } else {
        printf("disabled");
    }
    printf("\r\n");
}

int sysclock_main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "dump") != 0) {
        printf("Usage: sysclock dump\r\n");
        return -1;
    }

    printf("Clock Tree Dump:\r\n");
    printf("================\r\n");

    __sysclock_dump_clk("HSE", &VSF_HW_CLK_HSE);
    __sysclock_dump_clk("HSI48", &VSF_HW_CLK_HSI48);
    __sysclock_dump_clk("HSI8", &VSF_HW_CLK_HSI8);
    __sysclock_dump_clk("HSI", &VSF_HW_CLK_HSI);
    __sysclock_dump_clk("PLL", &VSF_HW_CLK_PLL);
    __sysclock_dump_clk("PLLP", &VSF_HW_CLK_PLLP);
    __sysclock_dump_clk("PLLU", &VSF_HW_CLK_PLLU);
    __sysclock_dump_clk("SYS", &VSF_HW_CLK_SYS);
    __sysclock_dump_clk("AHB", &VSF_HW_CLK_AHB);
    __sysclock_dump_clk("APB1", &VSF_HW_CLK_APB1);
    __sysclock_dump_clk("APB2", &VSF_HW_CLK_APB2);
#if defined(VSF_HW_USART_MASK) && (VSF_HW_USART_MASK & (1 << 1))
    __sysclock_dump_clk("USART1", &VSF_HW_CLK_USART1);
#endif
#if defined(VSF_HW_USART_MASK) && (VSF_HW_USART_MASK & (1 << 2))
    __sysclock_dump_clk("USART2", &VSF_HW_CLK_USART2);
#endif
#if defined(VSF_HW_USART_MASK) && (VSF_HW_USART_MASK & (1 << 3))
    __sysclock_dump_clk("USART3", &VSF_HW_CLK_USART3);
#endif
#if defined(VSF_HW_USART_MASK) && (VSF_HW_USART_MASK & (1 << 4))
    __sysclock_dump_clk("USART4", &VSF_HW_CLK_USART4);
#endif
#if defined(VSF_HW_USART_MASK) && (VSF_HW_USART_MASK & (1 << 5))
    __sysclock_dump_clk("USART5", &VSF_HW_CLK_USART5);
#endif
#if defined(VSF_HW_USART_MASK) && (VSF_HW_USART_MASK & (1 << 6))
    __sysclock_dump_clk("USART6", &VSF_HW_CLK_USART6);
#endif
#if defined(VSF_HW_USART_MASK) && (VSF_HW_USART_MASK & (1 << 7))
    __sysclock_dump_clk("USART7", &VSF_HW_CLK_USART7);
#endif
#if defined(VSF_HW_USART_MASK) && (VSF_HW_USART_MASK & (1 << 8))
    __sysclock_dump_clk("USART8", &VSF_HW_CLK_USART8);
#endif
#if defined(VSF_HW_USB_OTG_MASK) && (VSF_HW_USB_OTG_MASK & (1 << 0))
    __sysclock_dump_clk("USBOTG0", &VSF_HW_CLK_USBOTG0);
#endif
#if defined(VSF_HW_USB_OTG_MASK) && (VSF_HW_USB_OTG_MASK & (1 << 1))
    // USBOTG1 uses HEXT as PHY clock
    __sysclock_dump_clk("USBOTG1", &VSF_HW_CLK_HEXT);
#endif
#if defined(VSF_HW_SPI_MASK) && (VSF_HW_SPI_MASK & (1 << 1))
    __sysclock_dump_clk("SPI1", &VSF_HW_CLK_SPI1);
#endif
#if defined(VSF_HW_SPI_MASK) && (VSF_HW_SPI_MASK & (1 << 2))
    __sysclock_dump_clk("SPI2", &VSF_HW_CLK_SPI2);
#endif
#if defined(VSF_HW_SPI_MASK) && (VSF_HW_SPI_MASK & (1 << 3))
    __sysclock_dump_clk("SPI3", &VSF_HW_CLK_SPI3);
#endif
#if defined(VSF_HW_I2C_MASK) && (VSF_HW_I2C_MASK & (1 << 1))
    __sysclock_dump_clk("I2C1", &VSF_HW_CLK_I2C1);
#endif
#if defined(VSF_HW_I2C_MASK) && (VSF_HW_I2C_MASK & (1 << 2))
    __sysclock_dump_clk("I2C2", &VSF_HW_CLK_I2C2);
#endif
#if defined(VSF_HW_I2C_MASK) && (VSF_HW_I2C_MASK & (1 << 3))
    __sysclock_dump_clk("I2C3", &VSF_HW_CLK_I2C3);
#endif
#if defined(VSF_HW_WWDT_MASK) && (VSF_HW_WWDT_MASK & (1 << 0))
    __sysclock_dump_clk("WWDT0", &VSF_HW_WWDT0_CLK);
#endif

    return 0;
}
#endif

void vsf_board_init(void)
{
#if APP_USE_LINUX_DEMO == ENABLED
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sysclock", sysclock_main);
#endif
}

void vsf_app_driver_init(void)
{
    // Initialize clock first
    __clock_init();

    // Initialize debug stream after clock is configured
    __VSF_DEBUG_STREAM_TX_INIT();

    static const vsf_gpio_port_cfg_pins_t __cfgs[] = {
        // usart
        VSF_PORTA, 1 << 2, 0, VSF_HW_AF_USART2_TX_P0_2,
        VSF_PORTA, 1 << 3, 0, VSF_HW_AF_USART2_RX_P0_3,
        // i2c
        VSF_PORTA, 1 << 0, 0, VSF_HW_AF_I2C2_SCL_P0_0,
        VSF_PORTA, 1 << 1, 0, VSF_HW_AF_I2C2_SDA_P0_1,
        // spi
        VSF_PORTA, 1 << 4, 0, VSF_HW_AF_SPI1_CS_P0_4,
        VSF_PORTA, 1 << 5, 0, VSF_HW_AF_SPI1_SCK_P0_5,
        VSF_PORTA, 1 << 6, 0, VSF_HW_AF_SPI1_MISO_P0_6,
        VSF_PORTA, 1 << 7, 0, VSF_HW_AF_SPI1_MOSI_P0_7,
    };
    vsf_hw_gpio_ports_config_pins((vsf_gpio_port_cfg_pins_t *)__cfgs, dimof(__cfgs));

    vsf_dma_cfg_t cfg = {
        .prio = vsf_arch_prio_1,
    };

    vsf_hw_dma_init(&vsf_hw_dma1, &cfg);
    vsf_hw_dma_init(&vsf_hw_dma2, &cfg);

    //const char *message = "Hello, World!\n";
    //__VSF_DEBUG_STREAM_TX_WRITE_BLOCKED((uint8_t *)message, strlen(message));
}

#endif      // __AT32F405KCU7_4__

/* EOF */

