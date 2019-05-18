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

#ifndef __SYS_TICK_INTERNAL_H__
#define __SYS_TICK_INTERNAL_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

/*! \note you can access systick register directly through ARM_SYSTICK
 *!       e.g.   ARM_SYSTICK.SYST_CSR.ENABLE = true;
 *!
 *!     NOTE: the register name defined by Armv7-M Architecture Reference Mannual
 */
#include "./i_io_systick.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
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


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

//! \name Level 0 interface for systick
//! @{

/*!\brief init SysTick Timer
 *! \param void
 *! \retval true : succeed
 *! \retval false: failed
 */
extern bool vsf_systick_init(systick_cfg_t *pCfg);

/*!\brief get count value
 *! \param void
 *! \retval return geh time count value
 */
extern uint32_t vsf_systick_get_count(void);

/*!\brief set reload value
 *! \param time count value
 *! \retval whether access successful or not
 */
extern bool vsf_systick_set_reload(uint32_t value);

/*!\brief clear count value
 *! \param none
 *! \retval none
 */
extern void vsf_systick_clear_count(void);
/*!\brief enable
 *! \param void
 *! \retval none
 */
extern void vsf_systick_enable(void);

/*!\brief enable
 *! \param void
 *! \retval bool
 */
extern bool vsf_systick_disable(void);

/*!\brief enable
 *! \param void
 *! \retval bool
 */
extern bool vsf_systick_is_match(void);

//! @}


#endif  /* end of __SYS_TICK_H__ */
