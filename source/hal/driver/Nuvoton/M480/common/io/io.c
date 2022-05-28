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

#define VSF_GPIO_CFG_PREFIX                         vsf_hw
#define VSF_GPIO_CFG_UPPERCASE_PREFIX               VSF_HW
#define VSF_GPIO_CFG_REIMPLEMENT_SET_INPUT          ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_SET_OUTPUT         ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_SWITCH_DIRECTION   ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_SET                ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_CLEAR              ENABLED

/*============================ INCLUDES ======================================*/
#include "./io.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_CFG_GPIO_PROTECT_LEVEL
#   ifndef VSF_HAL_CFG_PROTECT_LEVEL
#       define VSF_HAL_CFG_GPIO_PROTECT_LEVEL       interrupt
#   else
#       define VSF_HAL_CFG_GPIO_PROTECT_LEVEL       VSF_HAL_CFG_PROTECT_LEVEL
#   endif
#endif

#define vsf_hw_gpio_protect                         vsf_protect(VSF_HAL_CFG_GPIO_PROTECT_LEVEL)
#define vsf_hw_gpio_unprotect                       vsf_unprotect(VSF_HAL_CFG_GPIO_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __gpio_is_output(__mode)              ((__mode) > 0)

/*============================ TYPES =========================================*/

typedef struct vsf_hw_gpio_t {
#if VSF_GPIO_CFG_IMPLEMENT_OP == ENABLED
    vsf_gpio_t vsf_gpio;
#endif

    GPIO_T *reg;
    uint32_t output_mode;
} vsf_hw_gpio_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_hw_gpio_config_pin(vsf_hw_gpio_t *pthis, uint32_t pin_mask, uint32_t feature)
{
    GPIO_T *reg = pthis->reg;
    uint_fast8_t mode_setting = (feature >> 0) & 0x03;
    uint_fast8_t pull_setting = (feature >> 4) & 0x03;
    uint32_t pin_mask_tmp;

    uint32_t mode = 0, pull = 0, mask = 0;
    uint_fast8_t offset = 32, tmp8;

    VSF_HAL_ASSERT(!(pin_mask >> 16));

    pin_mask <<= 16;
    pin_mask_tmp = pin_mask;
    while (pin_mask_tmp) {
        tmp8 = __CLZ(pin_mask_tmp) + 1;
        pin_mask_tmp <<= tmp8;
        offset -= tmp8 << 1;

        mode |= mode_setting << offset;
        pull |= pull_setting << offset;
        mask |= 0x03 << offset;
    }

    if (__gpio_is_output(mode_setting)) {
        // save output setting
        pthis->output_mode = (pthis->output_mode & ~mask) | mode;
    }

    vsf_protect_t state = vsf_hw_gpio_protect();
        reg->MODE  = (reg->MODE & ~mask) | mode;
        reg->PUSEL = (reg->PUSEL & ~mask) | pull;
        if (feature & IO_DISABLE_INPUT) {
            reg->DINOFF |= pin_mask;
        } else {
            reg->DINOFF &= ~pin_mask;
        }
    vsf_hw_gpio_unprotect(state);
}

void vsf_hw_gpio_set_direction(vsf_hw_gpio_t *pthis, uint32_t direction_mask, uint32_t pin_mask)
{
    GPIO_T *reg = pthis->reg;

    uint32_t mode = 0, mask = 0;
    uint_fast8_t offset = 32, tmp8;

    VSF_HAL_ASSERT(!(pin_mask >> 16));

    pin_mask <<= 16;
    while (pin_mask) {
        tmp8 = __CLZ(pin_mask) + 1;
        pin_mask <<= tmp8;
        offset -= tmp8 << 1;

        if (direction_mask & (1 << offset)) {
            // set saved output mode
            uint32_t mode_orig = pthis->output_mode & (0x03 << offset);
            if (mode_orig) {
                mode |= mode_orig;
            } else {
                mode |= IO_OUTPUT_PP << offset;
            }
        }

        mask |= 0x03 << offset;
    }

    vsf_protect_t state = vsf_hw_gpio_protect();
        reg->MODE  = (reg->MODE & ~mask) | mode;
    vsf_hw_gpio_unprotect(state);
}

uint32_t vsf_hw_gpio_get_direction(vsf_hw_gpio_t *pthis, uint32_t pin_mask)
{
    GPIO_T *reg = pthis->reg;

    uint32_t mode = 0;
    uint_fast8_t offset = 32, tmp8;

    VSF_HAL_ASSERT(!(pin_mask >> 16));

    pin_mask <<= 16;
    while (pin_mask) {
        tmp8 = __CLZ(pin_mask) + 1;
        pin_mask <<= tmp8;
        offset -= tmp8 << 1;

        if (reg->MODE & (0x03 << offset)) {
            mode |= 1 << offset;
        }
    }
    return mode;
}

void vsf_hw_gpio_set_input(vsf_hw_gpio_t *pthis, uint32_t pin_mask)
{
    vsf_hw_gpio_set_direction(pthis, 0, pin_mask);
}

void vsf_hw_gpio_set_output(vsf_hw_gpio_t *pthis, uint32_t pin_mask)
{
    vsf_hw_gpio_set_direction(pthis, pin_mask, pin_mask);
}

void vsf_hw_gpio_switch_direction(vsf_hw_gpio_t *pthis, uint32_t pin_mask)
{
    uint32_t direction = vsf_hw_gpio_get_direction(pthis, pin_mask);
    vsf_hw_gpio_set_direction(pthis, pin_mask ^ direction, pin_mask);
}

uint32_t vsf_hw_gpio_read(vsf_hw_gpio_t *pthis)
{
    GPIO_T *reg = pthis->reg;
    return reg->PIN;
}

void vsf_hw_gpio_write(vsf_hw_gpio_t *pthis, uint32_t value, uint32_t pin_mask)
{
    GPIO_T *reg = pthis->reg;
    uint32_t mask = reg->DATMSK;
    vsf_protect_t state = vsf_hw_gpio_protect();
        reg->DATMSK = ~pin_mask;
        reg->DOUT = value;
        reg->DATMSK = mask;
    vsf_hw_gpio_unprotect(state);
}

void vsf_hw_gpio_set(vsf_hw_gpio_t *pthis, uint32_t pin_mask)
{
    GPIO_T *reg = pthis->reg;
    uint32_t mask = reg->DATMSK;
    vsf_protect_t state = vsf_hw_gpio_protect();
        reg->DATMSK = ~pin_mask;
        reg->DOUT = pin_mask;
        reg->DATMSK = mask;
    vsf_hw_gpio_unprotect(state);
}

void vsf_hw_gpio_clear(vsf_hw_gpio_t *pthis, uint32_t pin_mask)
{
    GPIO_T *reg = pthis->reg;
    uint32_t mask = reg->DATMSK;
    vsf_protect_t state = vsf_hw_gpio_protect();
        reg->DATMSK = ~pin_mask;
        reg->DOUT = 0;
        reg->DATMSK = mask;
    vsf_hw_gpio_unprotect(state);
}

void vsf_hw_gpio_toggle(vsf_hw_gpio_t *pthis, uint32_t pin_mask)
{
    GPIO_T *reg = pthis->reg;
    uint32_t mask = reg->DATMSK;
    vsf_protect_t state = vsf_hw_gpio_protect();
        reg->DATMSK = ~pin_mask;
        reg->DOUT ^= pin_mask;
        reg->DATMSK = mask;
    vsf_hw_gpio_unprotect(state);
}

/*! \brief gpio batch configuration
           an implementation example:

static bool vsf_hw_gpio_config( io_cfg_t *cfg_ptr, uint_fast8_t count )
{
    bool result = true;
    if (NULL == cfg_ptr || 0 == count) {
        return false;
    }

    //! io configure
    do {
        uint_fast8_t pin_index = cfg_ptr->pin_index;  //!< get pin index number
        uint32_t function = cfg_ptr->function;  //!< get pin function selection

        //! get pin feature and make sure pin-input is enabled by default
        //! this is an example to enable some feature to be default.
        uint_fast8_t feature = cfg_ptr->feature ^ IOCTRL_PIN_IE_MSK;

        //! set pin feature: this is the most optimal solution
        GSP_IOCTRL.PIN[pin_index].Value = feature;

        //! but if we are not lucky enough, we can only use the following way
        if (feature & IO_PULL_UP) {
            IOCTRL_ENABLE_PULL_UP(pin_index);
        } else {
            IOCTRL_DISABLE_PULL_UP
        }
        if (feature & IO_HIGH_DRV) {
            IOCTRL_ENABLE_HIGH_DRIVER_STRENGH(pin_index);
        } else {
            IOCTRL_DISABLE_HIGH_DRIVER_STRENGH(pin_index);
        }
        ...

        //! I know this is ugly, but some times, the two methods aforementioned
        //! can be combined. So you should fully use the 32 bit of the
        //! cfg_ptr->feature

        //! set pin function selection
        IOCTRL_FUNCTION_SELECT(pin_index, function);


        cfg_ptr++;                                //!< next one...
    } while(--count);

    return result;
}


 *! \param cfg the pointer points to configuration array
 *! \param count the count of configurations in the array
 *! \return configuration result
 */
vsf_err_t vsf_hw_gpio_config(io_cfg_t *cfg, uint_fast8_t count)
{
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NONE;
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_IMP_LV0(__COUNT, __HAL_OP)                                 \
    vsf_hw_gpio_t vsf_hw_gpio##__COUNT = {                                      \
        .reg = (GPIO_T *)VSF_HW_GPIO ## __COUNT ##_ADDRESS,                     \
        __HAL_OP                                                                \
    };

#include "hal/driver/common/io/io_template.inc"
