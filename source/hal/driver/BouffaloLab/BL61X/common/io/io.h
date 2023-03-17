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

#ifndef __HAL_DRIVER_BL61X_HW_IO_H__
#define __HAL_DRIVER_BL61X_HW_IO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_IO == ENABLED

#include "../../__device.h"
#include "bflb_gpio.h"

/*============================ MACROS ========================================*/

#define VSF_IO_REIMPLEMENT_FEATURE      ENABLED

#define __BL61X_IO_IS_VAILID_PIN(__P)                                           \
    (((__P &  VSF_HW_IO_PIN_MASK) != 0) &&                                      \
     ((__P & ~VSF_HW_IO_PIN_MASK) == 0))

#define __BL61X_IO_IS_VAILID_FEATURE(__F)                                       \
    ((__F & ~(uint32_t)__VSF_HW_IO_FEATURE_ALL_BITS) == 0)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_io_feature_t {
    VSF_IO_OPEN_DRAIN           = GPIO_FLOAT,
    VSF_IO_PULL_UP              = GPIO_PULLUP,
    VSF_IO_PULL_DOWN            = GPIO_PULLDOWN,

    VSF_IO_INPUT                = GPIO_INPUT,
    VSF_IO_OUTPUT               = GPIO_OUTPUT,
    VSF_IO_ANALOG               = GPIO_ANALOG,
    VSF_IO_ALERNATE             = GPIO_ALTERNATE,

    VSF_IO_FEATURE_ALL_BITS     = GPIO_MODE_MASK | GPIO_PUPD_MASK
                                | GPIO_SMT_MASK | GPIO_DRV_MASK,

    // for hw_io
    __VSF_HW_IO_FEATURE_ALL_BITS= VSF_IO_FEATURE_ALL_BITS,
} vsf_io_feature_t;

/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#define VSF_IO_CFG_DEC_PREFIX              vsf_hw
#define VSF_IO_CFG_DEC_UPCASE_PREFIX       VSF_HW
#include "hal/driver/common/io/io_template.h"

#endif /* VSF_HAL_USE_IO */
#endif /* __HAL_DRIVER_BL61X_HW_IO_H__ */