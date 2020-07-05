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
#include "../compiler.h"
#include "signal.h"

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief initialize a locker
 *! \param lock_ptr locker object
 *! \return none
 */
void init_lock(locker_t *lock_ptr)
{
    if (NULL == lock_ptr) {
        return ;
    }
    
    (*lock_ptr) = UNLOCKED;
}

/*! \brief try to enter a section
 *! \param lock_ptr locker object
 *! \retval lock section is entered
 *! \retval The section is locked
 */
bool enter_lock(locker_t *lock_ptr)
{
    bool result = false;
    if (NULL == lock_ptr) {
        return true;
    }
    if (UNLOCKED == (*lock_ptr)) {
		__SAFE_ATOM_CODE(
            if (UNLOCKED == (*lock_ptr)) {
                (*lock_ptr) = LOCKED;
                result = true;
            }
		)
    }
        
    return result;
}


/*! \brief leave a section
 *! \param lock_ptr locker object
 *! \return none
 */
void leave_lock(locker_t *lock_ptr)
{
    if (NULL == lock_ptr) {
        return ;
    }
    
    (*lock_ptr) = UNLOCKED;
}

/*! \brief get locker status
 *! \param lock_ptr locker object
 *! \return locker status
 */
bool check_lock(locker_t *lock_ptr)
{
    if (NULL == lock_ptr) {
        return false;
    }

    return (*lock_ptr);
}

/* EOF */

