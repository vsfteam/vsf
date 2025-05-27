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

#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__) || defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#   include "../__device.h"

#else

#ifndef __VSF_HAL_DRIVER_${VENDOR}_${DEVICE}_H__
#define __VSF_HAL_DRIVER_${VENDOR}_${DEVICE}_H__

/*============================ INCLUDES ======================================*/

#   include "hal/vsf_hal_cfg.h"
#   include "./device.h"

// for common hal structure/enum/etc, include before user driver headers below.
#   include "hal/driver/common/template/vsf_template_hal_driver.h"

// For SWI
#   include "hal/driver/common/swi/vsf_swi_template.h"

/*\note It's up to developer to decide the actual path of the driver headers. */

#   if VSF_HAL_USE_ADC == ENABLED
#       include "../common/adc/adc.h"
#   endif
#   if VSF_HAL_USE_FLASH == ENABLED
#       include "../common/flash/flash.h"
#   endif
#   if VSF_HAL_USE_GPIO == ENABLED
#       include "../common/gpio/gpio.h"
#   endif
#   if VSF_HAL_USE_I2C == ENABLED
#       include "../common/i2c/i2c.h"
#   endif
#   if VSF_HAL_USE_PWM == ENABLED
#       include "../common/pwm/pwm.h"
#   endif
#   if VSF_HAL_USE_RTC == ENABLED
#       include "../common/rtc/rtc.h"
#   endif
#   if VSF_HAL_USE_SPI == ENABLED
#       include "../common/spi/spi.h"
#   endif
#   if VSF_HAL_USE_TIMER == ENABLED
#       include "../common/timer/timer.h"
#   endif
#   if VSF_HAL_USE_TRNG == ENABLED
#       include "../common/trng/trng.h"
#   endif
#   if VSF_HAL_USE_USART == ENABLED
#       include "../common/uart/uart.h"
#   endif
#   if VSF_HAL_USE_USB == ENABLED
#       include "../common/usb/usb.h"
#   endif
#   if VSF_HAL_USE_SDIO == ENABLED
#       include "../common/sdio/sdio.h"
#   endif
#   if VSF_HAL_USE_I2S == ENABLED
#       include "../common/i2s/i2s.h"
#   endif
#   if VSF_HAL_USE_WDT == ENABLED
#       include "../common/wdt/wdt.h"
#   endif
#   if VSF_HAL_USE_ETH == ENABLED
#       include "../common/mac/mac.h"
#   endif

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
#endif

#endif      // __VSF_HAL_DRIVER_${VENDOR}_${DEVICE}_H__
#endif      // !__VSF_HEADER_ONLY_SHOW_ARCH_INFO__ && !__VSF_HAL_SHOW_VENDOR_INFO__
/* EOF */
