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

#   include "__device.h"

#else

#ifndef __HAL_DRIVER_AIC_AIC8800_H__
#define __HAL_DRIVER_AIC_AIC8800_H__

/*============================ INCLUDES ======================================*/

#       include "hal/vsf_hal_cfg.h"
#       include "hal/driver/common/swi/vsf_swi_template.h"
#       include "./device.h"

#       include "./io/io.h"
#       include "./adc/adc.h"
#       include "./debug_uart/debug_uart.h"
#       include "./flash/flash.h"
#       include "./gpio/gpio.h"
#       include "./i2c/i2c.h"
#       include "./pwm/pwm.h"
#       include "./rtc/rtc.h"
#       include "./spi/spi.h"
#       include "./timer/timer.h"
#       include "./trng/trng.h"
#       include "./uart/uart.h"
#       include "./usb/usb.h"
#       include "./mmc/mmc.h"
#       include "./i2s/i2s.h"
#       include "./wdt/wdt.h"

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_IO == ENABLED
#   define VSF_IO_CFG_REIMPLEMENT_TYPE_MODE                 ENABLED
#   include "hal/driver/common/template/vsf_template_io.h"

#   define VSF_IO_CFG_DEC_PREFIX                            vsf_hw
#   define VSF_IO_CFG_DEC_UPCASE_PREFIX                     VSF_HW
#   include "hal/driver/common/io/io_template.h"
#endif

#if VSF_HAL_USE_GPIO == ENABLED
#   define VSF_GPIO_USE_IO_MODE_TYPE                        ENABLED
#   include "hal/driver/common/template/vsf_template_gpio.h"

#   define VSF_GPIO_CFG_DEC_PREFIX                          vsf_hw
#   define VSF_GPIO_CFG_DEC_UPCASE_PREFIX                   VSF_HW
#   include "hal/driver/common/gpio/gpio_template.h"
#endif

#if VSF_HAL_USE_ADC == ENABLED
#   include "hal/driver/common/template/vsf_template_adc.h"

#   define VSF_ADC_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_ADC_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/adc/adc_template.h"
#endif

#if VSF_HAL_USE_FLASH == ENABLED
#   include "hal/driver/common/template/vsf_template_flash.h"

#   define VSF_FLASH_CFG_DEC_PREFIX                         vsf_hw
#   define VSF_FLASH_CFG_DEC_UPCASE_PREFIX                  VSF_HW
#   include "hal/driver/common/flash/flash_template.h"
#endif

#if VSF_HAL_USE_I2C == ENABLED
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD                 ENABLED
#   include "hal/driver/common/template/vsf_template_i2c.h"

#   define VSF_I2C_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_I2C_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/i2c/i2c_template.h"
#endif

#if VSF_HAL_USE_I2S == ENABLED
#   define VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK            ENABLED
#   define VSF_I2S_CFG_REIMPLEMENT_TYPE_MODE                ENABLED
#   include "hal/driver/common/template/vsf_template_i2s.h"

#   define VSF_I2S_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_I2S_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/i2s/i2s_template.h"
#endif

#if VSF_HAL_USE_MMC == ENABLED
#   define VSF_MMC_CFG_REIMPLEMENT_TYPE_TRANSOP             ENABLED
#   define VSF_MMC_CFG_REIMPLEMENT_TYPE_IRQ_MASK            ENABLED
#   define VSF_MMC_CFG_REIMPLEMENT_TYPE_TRANSACT_STATUS     ENABLED
#   include "hal/driver/common/template/vsf_template_mmc.h"

#   define VSF_MMC_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_MMC_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/mmc/mmc_template.h"
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
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE                ENABLED
#   include "hal/driver/common/template/vsf_template_spi.h"

#   define VSF_SPI_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_SPI_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/spi/spi_template.h"
#endif

#if VSF_HAL_USE_TIMER == ENABLED
#   include "hal/driver/common/template/vsf_template_timer.h"

#   define VSF_TIMER_CFG_DEC_PREFIX                         vsf_hw
#   define VSF_TIMER_CFG_DEC_UPCASE_PREFIX                  VSF_HW
#   include "hal/driver/common/timer/timer_template.h"

#   define VSF_TIMER_CFG_DEC_PREFIX                         vsf_hw_lp
#   define VSF_TIMER_CFG_DEC_UPCASE_PREFIX                  VSF_HW_LP
#   include "hal/driver/common/timer/timer_template.h"
#endif

#if VSF_HAL_USE_RNG == ENABLED
#   include "hal/driver/common/template/vsf_template_rng.h"

#   define VSF_RNG_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_RNG_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/rng/rng_template.h"
#endif

#if VSF_HAL_USE_USART == ENABLED
#   define VSF_USART_CFG_FIFO_TO_REQUEST                    ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE              ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK          ENABLED
#   include "hal/driver/common/template/vsf_template_usart.h"

#   define VSF_USART_CFG_DEC_INSTANCE_PREFIX                __vsf_hw
#   define VSF_USART_CFG_DEC_PREFIX                         vsf_hw
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX                  VSF_HW
#   include "hal/driver/common/usart/usart_template.h"

#   define VSF_USART_CFG_DEC_INSTANCE_PREFIX                vsf_hw
#   define VSF_FIFO2REQ_USART_COUNT                         VSF_HW_USART_COUNT
#   define VSF_FIFO2REQ_USART_MASK                          VSF_HW_USART_MASK
#   include "hal/driver/common/usart/fifo2req_usart.h"
#endif

#if VSF_HAL_USE_WDT == ENABLED
#   define VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE              ENABLED
#   include "hal/driver/common/template/vsf_template_wdt.h"

#   define VSF_WDT_CFG_DEC_PREFIX                           vsf_hw
#   define VSF_WDT_CFG_DEC_UPCASE_PREFIX                    VSF_HW
#   include "hal/driver/common/wdt/wdt_template.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#   endif   // __HAL_DRIVER_AIC_AIC8800_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
