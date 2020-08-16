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

#ifndef __USE_COMMON_SIGNAL_H__
#define __USE_COMMON_SIGNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*! \brief The safe ATOM code section macro */
#define SAFE_ATOM_CODE()        code_region(&DEFAULT_CODE_REGION_ATOM_CODE)

         
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define __SAFE_ATOM_CODE(__code)                                                \
        {                                                                       \
            vsf_gint_state_t gint_state = DISABLE_GLOBAL_INTERRUPT();           \
            __code;                                                             \
            SET_GLOBAL_INTERRUPT_STATE(gint_state);                             \
        }
#define __safe_atom_code(__code)           __SAFE_ATOM_CODE(__code)
#else
#define __SAFE_ATOM_CODE(...)                                                   \
        {                                                                       \
            vsf_gint_state_t gint_state = DISABLE_GLOBAL_INTERRUPT();           \
            __VA_ARGS__;                                                        \
            SET_GLOBAL_INTERRUPT_STATE(gint_state);                             \
        }
#define __safe_atom_code(...)           __SAFE_ATOM_CODE(__VA_ARGS__)
#endif

//! \brief Exit from the safe atom operations
#define EXIT_SAFE_ATOM_CODE()           SET_GLOBAL_INTERRUPT_STATE(gint_state)  

#define exit_safe_atom_code()           EXIT_SAFE_ATOM_CODE()
#define safe_atom_code()                SAFE_ATOM_CODE()


/*! \name ES_LOCKER value */
/*! @{ */
#define LOCKED          true            //!< locked
#define UNLOCKED        false           //!< unlocked
/*! @} */


#define LOCK_INIT(__locker)     do {(__locker) = UNLOCKED;}while(false)


#define EXIT_LOCK()     do {                                                    \
                            (*lock_ptr) = UNLOCKED;                             \
                            SET_GLOBAL_INTERRUPT_STATE(gint_state);             \
                        } while(false)

#define ENTER_LOCK(__locker)            enter_lock(__locker)

#define LEAVE_LOCK(__locker)            leave_lock(__locker)
                        
#define GET_LOCK_STATUS(__locker)       check_lock(__locker)

#define INIT_LOCK(__locker)             init_lock(__locker)
                        
/*! \brief exit lock checker structure */
#define EXIT_LOCK_CHECKER()             EXIT_SAFE_ATOM_CODE()

/*! \note check specified locker and run code segment
 *! \param __locker a ES_LOCKER variable
 *! \param __code target code segment
 */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define LOCK_CHECKER(__locker, __code)  {                                    \
            {                                                                   \
                locker_t *lock_ptr = &(__locker);                               \
                if (UNLOCKED == (*lock_ptr))                                    \
                {                                                               \
                    SAFE_ATOM_CODE(                                             \
                        if (UNLOCKED == (*lock_ptr)) {                          \
                            __code;                                             \
                        }                                                       \
                    )                                                           \
                }                                                               \
            }
#else
#   define LOCK_CHECKER(__locker, ...)  {                                       \
            {                                                                   \
                locker_t *lock_ptr = &(__locker);                               \
                if (UNLOCKED == (*lock_ptr))                                    \
                {                                                               \
                    SAFE_ATOM_CODE(                                             \
                        if (UNLOCKED == (*lock_ptr)) {                          \
                            __VA_ARGS__;                                        \
                        }                                                       \
                    )                                                           \
                }                                                               \
            }
#endif
            
/*! \note critical code section protection
 *! \note LOCKER could be only used among FSMs and there should be no ISR involved.
 *! \param __locker ES_LOCKER variable
 *! \param __code   target code segment
 */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define LOCK(__locker,__code)                                                \
            LOCK_CHECKER((__locker),                                            \
                (*lock_ptr) = LOCKED;                                           \
                ENABLE_GLOBAL_INTERRUPT();                                      \
                __code;                                                         \
                (*lock_ptr) = UNLOCKED;                                         \
            )
#else
#   define LOCK(__locker,...)                                                   \
            LOCK_CHECKER((__locker),                                            \
                (*lock_ptr) = LOCKED;                                           \
                ENABLE_GLOBAL_INTERRUPT();                                      \
                __VA_ARGS__;                                                    \
                (*lock_ptr) = UNLOCKED;                                         \
            )
#endif

#define vsf_protect_t                       uint_fast32_t
#define vsf_protect_interrupt()              DISABLE_GLOBAL_INTERRUPT()
#define vsf_unprotect_interrupt(__state)    SET_GLOBAL_INTERRUPT_STATE(__state)
#define vsf_protect_none()                  (0)
#define vsf_unprotect_none(__state)         UNUSED_PARAM(__state)

#define vsf_protect_int()                   vsf_protect_interrupt()
#define vsf_unprotect_int(__state)          vsf_unprotect_interrupt(__state)

#define __vsf_protect(__type)               vsf_protect_##__type
#define __vsf_unprotect(__type)             vsf_unprotect_##__type
#define vsf_protect(__type)                 __vsf_protect(__type)
#define vsf_unprotect(__type)               __vsf_unprotect(__type)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_protect_region(__type, __code)                                   \
    do {                                                                        \
        vsf_protect_t __state = vsf_protect(__type)();                          \
        __code;                                                                 \
        vsf_unprotect(__type)(__state);                                         \
    } while (0);
#else
#   define vsf_protect_region(__type, ...)                                      \
    do {                                                                        \
        vsf_protect_t __state = vsf_protect(__type)();                          \
        __VA_ARGS__;                                                            \
        vsf_unprotect(__type)(__state);                                         \
    } while (0);
#endif

/*============================ TYPES =========================================*/
typedef volatile bool locker_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief try to enter a section
 *! \param lock_ptr locker object
 *! \retval lock section is entered
 *! \retval The section is locked
 */
extern bool enter_lock(locker_t *lock_ptr);

/*! \brief leave a section
 *! \param lock_ptr locker object
 *! \return none
 */
extern void leave_lock(locker_t *lock_ptr);
            
/*! \brief get locker status
 *! \param lock_ptr locker object
 *! \return locker status
 */
extern bool check_lock(locker_t *lock_ptr);            
           
/*! \brief initialize a locker
 *! \param lock_ptr locker object
 *! \return none
 */
extern void init_lock(locker_t *lock_ptr);

#ifdef __cplusplus
}
#endif

#endif
