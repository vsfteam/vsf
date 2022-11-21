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

#ifndef __HAL_DRIVER_ADC_INTERFACE_H__
#define __HAL_DRIVER_ADC_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ADC == ENABLED

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name class: adc_t
//! @{
def_interface(i_adc_t)
    implement(i_peripheral_t);

    vsf_err_t (*Init)(vsf_adc_cfg_t *pCfg);

    //! Irq
    struct {
        void (*Enable)(void);
        void (*Disable)(void);
    } Irq;

    //! Channel
    struct {
        vsf_err_t (*Config)(vsf_adc_channel_cfg_t *ptChannelCfgs, uint32_t nCount);
        vsf_err_t (*Request)(void *pBuffer, uint32_t nCount);
    } Channel;

end_def_interface(i_adc_t)
//! @}


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif

#endif
/* EOF */
