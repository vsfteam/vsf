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
#include "hal/vsf_hal.h"

#include "./arch/vsf_arch.h"
#include "./driver/driver.h"

/*============================ MACROS ========================================*/


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_ARCH_INIT)                                             \
    &&  defined(WEAK_VSF_ARCH_INIT_EXTERN)
WEAK_VSF_ARCH_INIT_EXTERN
#endif

#if     defined(WEAK_VSF_DRIVER_INIT_EXTERN)                                    \
    &&  defined(WEAK_VSF_DRIVER_INIT)
WEAK_VSF_DRIVER_INIT_EXTERN
#endif

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_DRIVER_INIT
WEAK(vsf_driver_init)
bool vsf_driver_init(void) 
{
    return true;
}
#endif

/*! \note initialize level 0/1 hardware abstract layer
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */  
bool vsf_hal_init( void )
{
    
    if (    
        #ifndef WEAK_VSF_ARCH_INIT
            !vsf_arch_init() 
        #else
            !WEAK_VSF_ARCH_INIT() 
        #endif
        ||  
        #ifndef WEAK_VSF_DRIVER_INIT
            !vsf_driver_init()
        #else
            !WEAK_VSF_DRIVER_INIT()
        #endif
        ) {
        
        return false;
    }

    return true;
}

/* EOF */
