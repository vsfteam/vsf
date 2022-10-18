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

#ifndef __HAL_DRIVER_AIC8800_HW_IO_H__
#define __HAL_DRIVER_AIC8800_HW_IO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_IO == ENABLED

#include "../__device.h"
#include "../vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"

/*============================ MACROS ========================================*/

#define VSF_IO_REIMPLEMENT_FEATURE      ENABLED

#define __VSF_HW_IO_FUNCTION_MASK       IOMUX_GPIO_CONFIG_SEL_MASK

#define __VSF_HW_IO_PIN_MASK            ((1ul << VSF_HW_IO_PIN_MAX) - 1)

#define __VSF_HW_IO_IS_VAILID_PIN(__P)                                          \
    (((__P &  __VSF_HW_IO_PIN_MASK) != 0) &&                                    \
     ((__P & ~__VSF_HW_IO_PIN_MASK) == 0))

#define __VSF_HW_IO_IS_VAILID_FEATURE(__F)                                      \
    ((__F & ~(uint32_t)__HW_IO_FEATURE_ALL_BITS) == 0)


// TODO: add io function mode tables
//#defin VSF_PA0_AS_GPIO                  0
//#defin VSF_PA0_AS_SWCLK                 1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_io_feature_t {
    IO_OPEN_DRAIN           = (0 << IOMUX_GPIO_CONFIG_PULL_FRC_LSB),
    IO_PULL_UP              = ((1 << IOMUX_AGPIO_CONFIG_PULL_FRC_LSB) | (1 << IOMUX_AGPIO_CONFIG_PULL_UP_LSB)),
    IO_PULL_DOWN            = ((1 << IOMUX_AGPIO_CONFIG_PULL_FRC_LSB) | (1 << IOMUX_AGPIO_CONFIG_PULL_DN_LSB)),

    __IO_PULL_MASK          = IOMUX_AGPIO_CONFIG_PULL_FRC_MASK | IOMUX_AGPIO_CONFIG_PULL_DN_MASK | IOMUX_AGPIO_CONFIG_PULL_UP_MASK,

    // not support in aic8800
    IO_DISABLE_INPUT        = 1 << 17,          //!< disable input
    IO_INVERT_INPUT         = 1 << 18,          //!< invert the input pin level

    IO_FILTER_BYPASS        = 0 << 19,          //!< filter is bypassed
    IO_FILTER_2CLK          = 1 << 19,          //!< levels should keep 2 clks
    IO_FILTER_4CLK          = 2 << 19,          //!< levels should keep 4 clks
    IO_FILTER_8CLK          = 3 << 19,          //!< levels should keep 8 clks

    IO_FILTER_CLK_SRC0      = 0 << 21,          //!< select clock src 0 for filter
    IO_FILTER_CLK_SRC1      = 1 << 21,          //!< select clock src 1 for filter
    IO_FILTER_CLK_SRC2      = 2 << 21,          //!< select clock src 2 for filter
    IO_FILTER_CLK_SRC3      = 3 << 21,          //!< select clock src 3 for filter
    IO_FILTER_CLK_SRC4      = 4 << 21,          //!< select clock src 4 for filter
    IO_FILTER_CLK_SRC5      = 5 << 21,          //!< select clock src 5 for filter
    IO_FILTER_CLK_SRC6      = 6 << 21,          //!< select clock src 6 for filter
    IO_FILTER_CLK_SRC7      = 7 << 21,          //!< select clock src 7 for filter

    IO_HIGH_DRV             = 1 << 24,          //!< enable high drive strength
    IO_HIGH_DRIVE           = 1 << 24,          //!< enable high drive strength
    IO_HIGH_DRIVE_STRENGTH  = 1 << 24,          //!< enable high drive strength

    IO_FEATURE_ALL_BITS     = IO_OPEN_DRAIN | IO_PULL_UP | IO_PULL_DOWN |
                              IO_DISABLE_INPUT | IO_INVERT_INPUT |
                              IO_FILTER_BYPASS | IO_FILTER_2CLK | IO_FILTER_4CLK | IO_FILTER_8CLK |
                              IO_FILTER_CLK_SRC0 | IO_FILTER_CLK_SRC1 | IO_FILTER_CLK_SRC2 |
                              IO_FILTER_CLK_SRC3 | IO_FILTER_CLK_SRC4 | IO_FILTER_CLK_SRC5 |
                              IO_FILTER_CLK_SRC6 | IO_FILTER_CLK_SRC7 |
                              IO_HIGH_DRV,

    __HW_IO_FEATURE_ALL_BITS  = IO_PULL_UP | IO_OPEN_DRAIN | IO_OPEN_DRAIN

} vsf_io_feature_t;


static inline uint32_t __hw_io_reg_read(bool is_pmic, volatile uint32_t *reg)
{
    if (!is_pmic) {
        return *reg;
    } else {
        return PMIC_MEM_READ((unsigned int)reg);
    }
}

static inline void __hw_io_reg_mask_write(bool is_pmic, volatile uint32_t *reg,
                                          uint32_t wdata, uint32_t wmask)
{
    if (!is_pmic) {
        *reg = (*reg & ~wmask) | (wdata & wmask);
    } else {
        PMIC_MEM_MASK_WRITE((unsigned int)reg, wdata, wmask);
    }
}

/*============================ INCLUDES ======================================*/

#define VSF_IO_CFG_DEC_PREFIX              vsf_hw
#define VSF_IO_CFG_DEC_UPCASE_PREFIX       VSF_HW
#include "hal/driver/common/io/io_template.h"

#endif /* VSF_HAL_USE_IO */
#endif /* __HAL_DRIVER_AIC8800_HW_IO_H__ */
