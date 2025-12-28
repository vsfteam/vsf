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

#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__) || defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#   include "../__device.h"

#else

#   ifndef __HAL_DRIVER_ATTERY_AT32F402_405_H__
#       define __HAL_DRIVER_ATTERY_AT32F402_405_H__

/*============================ INCLUDES ======================================*/

#       include "hal/vsf_hal_cfg.h"
#       include "../__device.h"

#       include "./dma/dma.h"
#       include "./flash/flash.h"
#       include "./gpio/gpio.h"
#       include "./i2c/i2c.h"
#       include "./exti/exti.h"
#       include "./spi/spi.h"
#       include "./usart/usart.h"
#       include "./wdt/wdt.h"
#       include "./usb/usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_GPIO == ENABLED
#   include "hal/driver/common/template/vsf_template_gpio.h"

#   define VSF_GPIO_CFG_DEC_PREFIX                          vsf_hw
#   define VSF_GPIO_CFG_DEC_UPCASE_PREFIX                   VSF_HW
#   include "hal/driver/common/gpio/gpio_template.h"
#endif

#if VSF_HAL_USE_EXTI == ENABLED
#   include "hal/driver/common/template/vsf_template_exti.h"

#   define VSF_EXTI_CFG_DEC_PREFIX                          vsf_hw
#   define VSF_EXTI_CFG_DEC_UPCASE_PREFIX                   VSF_HW
#   include "hal/driver/common/exti/exti_template.h"
#endif

#if VSF_HAL_USE_ADC == ENABLED
#   include "hal/driver/common/template/vsf_template_adc.h"

#   define VSF_ADC_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_ADC_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/adc/adc_template.h"
#endif

#if VSF_HAL_USE_DAC == ENABLED
#   include "hal/driver/common/template/vsf_template_dac.h"

#   define VSF_DAC_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_DAC_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/dac/dac_template.h"
#endif

#if VSF_HAL_USE_DMA == ENABLED
#   include "hal/driver/common/template/vsf_template_dma.h"

#   define VSF_DMA_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_DMA_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/dma/dma_template.h"
#endif

#if VSF_HAL_USE_FLASH == ENABLED
#   include "hal/driver/common/template/vsf_template_flash.h"

#   define VSF_FLASH_CFG_DEC_PREFIX                         vsf_hw
#   define VSF_FLASH_CFG_DEC_UPCASE_PREFIX                  VSF_HW
#   include "hal/driver/common/flash/flash_template.h"
#endif

#if VSF_HAL_USE_I2C == ENABLED
#   include "hal/driver/common/template/vsf_template_i2c.h"

#   define VSF_I2C_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_I2C_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/i2c/i2c_template.h"
#endif

#if VSF_HAL_USE_PWM == ENABLED
#   include "hal/driver/common/template/vsf_template_pwm.h"

#   define VSF_PWM_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_PWM_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/pwm/pwm_template.h"
#endif

#if VSF_HAL_USE_RTC == ENABLED
#   include "hal/driver/common/template/vsf_template_rtc.h"

#   define VSF_RTC_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_RTC_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/rtc/rtc_template.h"
#endif

#if VSF_HAL_USE_SPI == ENABLED
#   include "hal/driver/common/template/vsf_template_spi.h"

#   define VSF_SPI_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_SPI_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/spi/spi_template.h"

// Optional: Add QSPI
//#   define VSF_SPI_CFG_DEC_DEVICE_PREFIX                  vsf_hw_qspi
//#   define VSF_SPI_CFG_DEC_DEVICE_UPCASE_PREFIX           VSF_HW_QSPI
//#   define VSF_SPI_CFG_DEC_RENAME_DEVICE_PREFIX           ENABLED
//#   include "hal/driver/common/spi/spi_template.h"
#endif

#if VSF_HAL_USE_TIMER == ENABLED
#   include "hal/driver/common/template/vsf_template_timer.h"

#   define VSF_TIMER_CFG_DEC_PREFIX                         vsf_hw
#   define VSF_TIMER_CFG_DEC_UPCASE_PREFIX                  VSF_HW
#   include "hal/driver/common/timer/timer_template.h"
#endif

#if VSF_HAL_USE_RNG == ENABLED
#   include "hal/driver/common/template/vsf_template_rng.h"

#   define VSF_RNG_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_RNG_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/rng/rng_template.h"
#endif

#if VSF_HAL_USE_USART == ENABLED
#   include "hal/driver/common/template/vsf_template_usart.h"

#   define VSF_USART_CFG_DEC_PREFIX                         vsf_hw
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX                  VSF_HW
#   include "hal/driver/common/usart/usart_template.h"
#endif

#if VSF_HAL_USE_WDT == ENABLED
#   include "hal/driver/common/template/vsf_template_wdt.h"

#   define VSF_WDT_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_WDT_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/wdt/wdt_template.h"

// Optional: Add WWDT with separate device prefix
#   define VSF_WDT_CFG_DEC_DEVICE_PREFIX                    vsf_hw_wwdt
#   define VSF_WDT_CFG_DEC_DEVICE_UPCASE_PREFIX             VSF_HW_WWDT
#   define VSF_WDT_CFG_DEC_RENAME_DEVICE_PREFIX             ENABLED
#   include "hal/driver/common/wdt/wdt_template.h"
#endif

#if VSF_HAL_USE_SDIO == ENABLED
#   include "hal/driver/common/template/vsf_template_sdio.h"

#   define VSF_SDIO_CFG_DEC_PREFIX                          vsf_hw
#   define VSF_SDIO_CFG_DEC_UPCASE_PREFIX                   VSF_HW
#   include "hal/driver/common/sdio/sdio_template.h"
#endif

#if VSF_HAL_USE_I2S == ENABLED
#   include "hal/driver/common/template/vsf_template_i2s.h"

#   define VSF_I2S_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_I2S_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/i2s/i2s_template.h"
#endif

#if VSF_HAL_USE_ETH == ENABLED
#   include "hal/driver/common/template/vsf_template_eth.h"

#   define VSF_ETH_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_ETH_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/eth/eth_template.h"
#endif

#   endif

#endif
/* EOF */
