/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __SYS_TICK_H__
#define __SYS_TICK_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "./systick_internal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*! \note Example of systick configuration using systick_cfg()

     systick_cfg (
        ENABLE_SYSTICK              |       //!< enable systick immediatly 
        SYSTICK_SOURCE_SYSCLK       |       //!< using system clock as clock source
        ENABLE_SYSTICK_INTERRUPT,           //!< enable exception generation
        (VSF_GET_MAIN_CLK() / frequency)    //!< generate desired frequency
    );

 */

#define vsf_systick_cfg(...)                    \
        do {                                    \
            systick_cfg_t tCFG = {__VA_ARGS__}; \
            vsf_systick_init(&tCFG);            \
        } while(false)

/*============================ TYPES =========================================*/

/* defined in systick_internal.h
//! \name SysTick initialization arguments defination
//! @{
typedef enum {
    //! Enable the SysTick
    DISABLE_SYSTICK                         = 0x00,
    //! Enable the SysTick
    ENABLE_SYSTICK                          = 0x01, 

    //! Enable the SysTick interrupt
    DISABLE_SYSTICK_INTERRUPT               = 0x00,
    //! Enable the SysTick interrupt
    ENABLE_SYSTICK_INTERRUPT                = 0x02, 

    //! Select the reference clock as SysTick source
    SYSTICK_SOURCE_REFERCLK                 = 0x00,
    //! Select the system clock as SysTick source
    SYSTICK_SOURCE_SYSCLK                   = 0x04,
} em_systick_cfg_mode_t;
//! @}

//! \name systick config struct
//! @{
typedef struct {
    uint8_t         mode;                           //!< config ward
    uint32_t        reload_value;                   //!< Match value
}systick_cfg_t;
//! @}
*/

//! \name systick struct
//! @{
def_interface(systick_t)
    bool            (*Init)(systick_cfg_t *);       //!< initialize the systick
    void            (*Enable)(void);                //!< enable
    bool            (*Disable)(void);               //!< disable
    bool            (*IsMatch)(void);               //!< check whether a overflow happened 
    u32_property_t  Counter;                        //!< get or set the counter
    void            (*Reset)(void);                 //!< reset the counter
    systick_reg_t   *const RegPage;                 //!< register page
end_def_interface(systick_t)
//! @}
/*============================ GLOBAL VARIABLES ==============================*/
//! \brief declare the PMU
extern const systick_t SYSTICK;
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif  /* end of __SYS_TICK_H__ */
