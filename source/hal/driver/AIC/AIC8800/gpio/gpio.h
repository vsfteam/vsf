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

#ifndef __HAL_DRIVER_AIC8800_HW_GPIO_H__
#define __HAL_DRIVER_AIC8800_HW_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

#   include "../io/io.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief To ensure that the GPIO is working properly, it needs to be called inside vsf_driver_init()

 \~chinese
 @brief 为了确保 AIC8800 的 GPIO 工作正常，需要在 vsf_driver_init() 里调用这个 API
 */
extern void __vsf_hw_aic8800_gpio_init(void);

#define VSF_GPIO_CFG_DEC_PREFIX         vsf_hw
#define VSF_GPIO_CFG_DEC_UPCASE_PREFIX  VSF_HW
#include "hal/driver/common/gpio/gpio_template.h"

#endif /* VSF_HAL_USE_GPIO */
#endif /* __HAL_DRIVER_AIC8800_HW_GPIO_H__ */
