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

#ifndef __HAL_DRIVER_H__
#define __HAL_DRIVER_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

/*! \note User can define VSF_DRIVER_HEADER to specify the targer device driver 
 *!       header file. If it is not specified, this driver abstraction header 
 *!       file will use predefined device-specific macros to decide which device
 *!       driver header file should be included.
 */
#ifndef VSF_DRIVER_HEADER

#   if      defined(__STMicro__)
#       define  VSF_DRIVER_HEADER       "./STMicro/driver.h"
#   elif    defined(__NXP__)  
#       define  VSF_DRIVER_HEADER       "./NXP/driver.h"
#   elif    defined(__MicroChip__)  
#       define  VSF_DRIVER_HEADER       "./MicroChip/driver.h"
#   elif    defined(__Nuvoton__)
#       define  VSF_DRIVER_HEADER       "./Nuvoton/driver.h"
#   elif    defined(__WinnerMicro__)
#       define  VSF_DRIVER_HEADER       "./WinnerMicro/driver.h"
#   elif    defined(__CPU_PC__)  
#       define  VSF_DRIVER_HEADER       "./pc/driver.h"
/* example
#   elif    defined(__COMPANY_NAME_A__)
#       define  VSF_DRIVER_HEADER       "./company_name_a/driver.h"
#   elif    defined(__COMPANY_NAME_B__)
#       define  VSF_DRIVER_HEADER       "./company_name_b/driver.h"
*/
#   elif    defined(__ARM__)
#       define  VSF_DRIVER_HEADER       "./arm/driver.h"
#   else
#       define  VSF_DRIVER_HEADER       "./template/driver.h"
#   endif
#endif

/* include specified device driver header file */
#include VSF_DRIVER_HEADER

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \note initialize drivers 
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
extern bool vsf_driver_init(void);

#endif
/* EOF */
