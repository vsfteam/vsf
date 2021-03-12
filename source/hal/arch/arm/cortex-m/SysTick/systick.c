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

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "utilities/vsf_utilities.h"
#include "./systick_internal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name systick struct
//! @{
declare_interface(systick_t)
def_interface(systick_t)
    bool            (*Init)(systick_cfg_t *);       //!< initialize the systick
    void            (*Enable)(void);                //!< enable
    bool            (*Disable)(void);               //!< disable
    bool            (*IsMatch)(void);
    u32_property_t  Counter;
    void            (*Reset)(void);                 //!< clear count register
    systick_reg_t   *const RegPage;                 //!< register page
end_def_interface(systick_t)
//! @}
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
//! \brief define the SysTick
const systick_t SYSTICK = {
    
    &vsf_systick_init,                              //!< initialize the SysTick
    &vsf_systick_enable,
    &vsf_systick_disable,
    &vsf_systick_is_match,
    {
        &vsf_systick_set_reload,                    //!< set reload value
        &vsf_systick_get_count,                     //!< get count value
    },
    &vsf_systick_clear_count,                       //!< clear count value
    ((systick_reg_t *)(SYSTICK_BASE_ADDRESS)),      //!< register page
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
