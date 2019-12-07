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
#include "vsf_cfg.h"
#include "./mcs51_compiler.h"
#include "./signal.h"

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#if __IS_COMPILER_51_KEIL__
vsf_gint_state_t __disable_interrupt(void)
{
    vsf_gint_state_t orig = EA;
    EA = 0;
    return orig;
}

void __enable_interrupt(void)
{
    EA = 1;
}

vsf_gint_state_t __get_interrupt_state(void)
{
    return EA;
}

void __set_interrupt_state(vsf_gint_state_t state)
{
    EA = state;
}
    
#endif


/*! \brief initialize a locker
 *! \param plock locker object
 *! \return none
 */
void init_lock(locker_t *plock)
{
    if (NULL == plock) {
        return ;
    }
    
    (*plock) = UNLOCKED;
}

/*! \brief try to enter a section
 *! \param plock locker object
 *! \retval lock section is entered
 *! \retval The section is locked
 */
bool enter_lock(locker_t *plock)
{
    bool result = false;
    if (NULL == plock) {
        return true;
    }
    if (UNLOCKED == (*plock)) {
        __SAFE_ATOM_CODE(
            if (UNLOCKED == (*plock)) {
                (*plock) = LOCKED;
                result = true;
            }
        )
    }
        
    return result;
}


/*! \brief leave a section
 *! \param plock locker object
 *! \return none
 */
void leave_lock(locker_t *plock)
{
    if (NULL == plock) {
        return ;
    }
    
    (*plock) = UNLOCKED;
}

/*! \brief get locker status
 *! \param plock locker object
 *! \return locker status
 */
bool check_lock(locker_t *plock)
{
    if (NULL == plock) {
        return false;
    }

    return (*plock);
}

/* EOF */

