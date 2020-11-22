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

#ifndef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#   include "utilities/vsf_utilities.h"
#endif

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
#   elif    defined(__CPU_X86__) || defined(__CPU_X64__)
#       define  VSF_DRIVER_HEADER       "./x86/driver.h"
#   elif    defined(__SiliconLabs__)
#       define  VSF_DRIVER_HEADER       "./SiliconLabs/driver.h"
#   elif    defined(__GigaDevice__)
#       define  VSF_DRIVER_HEADER       "./GigaDevice/driver.h"
#   elif    defined(__TI__)
#       define  VSF_DRIVER_HEADER       "./TI/driver.h"
#   elif    defined(__ST__)
#       define  VSF_DRIVER_HEADER       "./ST/driver.h"
#   elif    defined(__WCH__)
#       define  VSF_DRIVER_HEADER       "./WCH/driver.h"
#   elif    defined(__Allwinner__)
#       define  VSF_DRIVER_HEADER       "./Allwinner/driver.h"
#   elif    defined(__Espressif__)
#       define  VSF_DRIVER_HEADER       "./Espressif/driver.h"
/* example
#   elif    defined(__COMPANY_NAME_A__)
#       define  VSF_DRIVER_HEADER       "./company_name_a/driver.h"
#   elif    defined(__COMPANY_NAME_B__)
#       define  VSF_DRIVER_HEADER       "./company_name_b/driver.h"
*/
#   elif    defined(__ARM__) || defined(__arm__)
#       define  VSF_DRIVER_HEADER       "./arm/driver.h"

/*! \note please try not ignore the __UNKNOWN_VENDOR__ if you want to select
 *!       the chip inside unknown folder
 */
#   elif    defined(__UNKOWN_VENDOR__)
#       define  VSF_DRIVER_HEADER       "./unknown/driver.h"
#   else
#       define  VSF_DRIVER_HEADER       "./unknown/driver.h"
#   endif
#endif

//! you don't have to implement a driver in your system.
#ifdef VSF_DRIVER_HEADER
/* include specified device driver header file */
#   include    VSF_DRIVER_HEADER
#endif

#if !defined(__HAL_DRIVER_H__) && !defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)
#define __HAL_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

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

/*! \note common entry for upper layer to poll driver servcie
 *  \param none
 *  \retval true it is safe to enter sleep mode
 *  \retval false polling work is on going, please keep calling the function
 */
extern bool vsf_driver_poll(void);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
