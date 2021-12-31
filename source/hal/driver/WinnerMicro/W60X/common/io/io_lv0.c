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
#include "../common.h"
#include "./io.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_CFG_GPIO_PROTECT_LEVEL
#   ifndef VSF_HAL_CFG_PROTECT_LEVEL
#       define VSF_HAL_CFG_GPIO_PROTECT_LEVEL   interrupt
#   else
#       define VSF_HAL_CFG_GPIO_PROTECT_LEVEL   VSF_HAL_CFG_PROTECT_LEVEL
#   endif
#endif

#define vsf_gpio_protect                        vsf_protect(VSF_HAL_CFG_GPIO_PROTECT_LEVEL)
#define vsf_gpio_unprotect                      vsf_unprotect(VSF_HAL_CFG_GPIO_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct w600_gpio_reg_t {
    volatile uint32_t DATA;
    volatile uint32_t DATA_EN;
    volatile uint32_t DATA_DIR;         // 1 for output, 0 for input
    volatile uint32_t DATA_PULLEN;      // 1 for disable, 0 for enable
    volatile uint32_t AFSEL;
    volatile uint32_t AFS1;
    volatile uint32_t AFS0;
    volatile uint32_t dummy;
    volatile uint32_t IS;
    volatile uint32_t IBE;
    volatile uint32_t IEV;
    volatile uint32_t IE;
    volatile uint32_t RIS;
    volatile uint32_t MIS;
    volatile uint32_t IC;
};
typedef struct w600_gpio_reg_t w600_gpio_reg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_gpio_config_pin(       vsf_gpio_t *pthis,
                                uint32_t pin_mask,
                                uint_fast32_t feature)
{
    w600_gpio_reg_t *reg = (w600_gpio_reg_t *)pthis->reg;

    
}

void vsf_gpio_set_direction(    vsf_gpio_t *pthis,
                                uint_fast32_t direction_mask,
                                uint32_t pin_mask)
{
    gpio_reg_t *reg = (gpio_reg_t *)pthis->reg;
    vsf_protect_t state = vsf_gpio_protect();
        reg->DIR = (reg->DIR & ~pin_mask) | (direction_mask & pin_mask);
    vsf_gpio_unprotect(state);
}

uint_fast32_t vsf_gpio_get_direction(vsf_gpio_t *pthis, uint32_t pin_mask)
{
    return pthis->reg->DIR & pin_mask;
}

void vsf_gpio_set_input(vsf_gpio_t *pthis, uint32_t pin_mask)
{
    gpio_reg_t *reg = (gpio_reg_t *)pthis->reg;
    vsf_protect_t state = vsf_gpio_protect();
        reg->DIR &= ~pin_mask;
    vsf_gpio_unprotect(state);
}

void vsf_gpio_set_output(vsf_gpio_t *pthis, uint32_t pin_mask)
{
    gpio_reg_t *reg = (gpio_reg_t *)pthis->reg;
    vsf_protect_t state = vsf_gpio_protect();
        reg->DIR |= pin_mask;
    vsf_gpio_unprotect(state);
}

void vsf_gpio_switch_direction(vsf_gpio_t *pthis, uint32_t pin_mask)
{
    gpio_reg_t *reg = (gpio_reg_t *)pthis->reg;
    vsf_protect_t state = vsf_gpio_protect();
        reg->DIR ^= ~pin_mask;
    vsf_gpio_unprotect(state);
}

uint_fast32_t vsf_hal_gpio_read(vsf_gpio_t *pthis)
{
    return pthis->reg->IN;
}

void vsf_gpio_write(vsf_gpio_t *pthis, uint_fast32_t value, uint32_t pin_mask)
{
    gpio_reg_t *reg = (gpio_reg_t *)pthis->reg;
    vsf_protect_t state = vsf_gpio_protect();
        reg->OUT = (reg->OUT & ~pin_mask) | (value & pin_mask);
    vsf_gpio_unprotect(state);
}
     
void vsf_gpio_set(vsf_gpio_t *pthis, uint32_t pin_mask)
{
    gpio_reg_t *reg = (gpio_reg_t *)pthis->reg;
    vsf_protect_t state = vsf_gpio_protect();
        reg->OUT |= pin_mask;
    vsf_gpio_unprotect(state);
}

void vsf_gpio_clear(vsf_gpio_t *pthis, uint32_t pin_mask)
{
    gpio_reg_t *reg = (gpio_reg_t *)pthis->reg;
    vsf_protect_t state = vsf_gpio_protect();
        reg->OUT &= ~pin_mask;
    vsf_gpio_unprotect(state);
}

void vsf_gpio_toggle(vsf_gpio_t *pthis, uint32_t pin_mask)
{
    gpio_reg_t *reg = (gpio_reg_t *)pthis->reg;
    vsf_protect_t state = vsf_gpio_protect();
        reg->OUT ^= pin_mask;
    vsf_gpio_unprotect(state);
}

/*! \brief gpio batch configuration 
           an implementation example:

static bool vsf_gpio_config( io_cfg_t *cfg_ptr, uint_fast8_t count )
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
vsf_err_t vsf_gpio_config(io_cfg_t *cfg, uint_fast8_t count)
{
    for (; count > 0; count--, cfg++) {
        // TODO: config function
        VSF_IO.PORT[cfg->pin_index >> 4].ConfigPin(
                    1 << cfg->pin_index,
                    cfg->feature);
    }
    return VSF_ERR_NONE;
}
