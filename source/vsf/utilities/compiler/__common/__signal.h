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
#define __SAFE_ATOM_CODE(__CODE)                                                \
        {                                                                       \
            vsf_gint_state_t tState = DISABLE_GLOBAL_INTERRUPT();               \
            __CODE;                                                             \
            SET_GLOBAL_INTERRUPT_STATE(tState);                                 \
        }
#define __safe_atom_code(__CODE)           __SAFE_ATOM_CODE(__CODE)
#else
#define __SAFE_ATOM_CODE(...)                                                   \
        {                                                                       \
            vsf_gint_state_t tState = DISABLE_GLOBAL_INTERRUPT();               \
            __VA_ARGS__;                                                        \
            SET_GLOBAL_INTERRUPT_STATE(tState);                                 \
        }
#define __safe_atom_code(...)           __SAFE_ATOM_CODE(__VA_ARGS__)
#endif

//! \brief Exit from the safe atom operations
#define EXIT_SAFE_ATOM_CODE()           SET_GLOBAL_INTERRUPT_STATE(tState)  

#define exit_safe_atom_code()           EXIT_SAFE_ATOM_CODE()
#define safe_atom_code()                SAFE_ATOM_CODE()


/*! \name ES_LOCKER value */
/*! @{ */
#define LOCKED          true            //!< locked
#define UNLOCKED        false           //!< unlocked
/*! @} */


#define LOCK_INIT(__LOCKER)     do {(__LOCKER) = UNLOCKED;}while(false)


#define EXIT_LOCK()     do {\
                            (*pLocker) = UNLOCKED;\
                            SET_GLOBAL_INTERRUPT_STATE(tState);\
                        } while(false)

#define ENTER_LOCK(__LOCKER)            enter_lock(__LOCKER)

#define LEAVE_LOCK(__LOCKER)            leave_lock(__LOCKER)
                        
#define GET_LOCK_STATUS(__LOCKER)       check_lock(__LOCKER)

#define INIT_LOCK(__LOCKER)             init_lock(__LOCKER)
                        
/*! \brief exit lock checker structure */
#define EXIT_LOCK_CHECKER()             EXIT_SAFE_ATOM_CODE()

/*! \note check specified locker and run code segment
 *! \param __LOCKER a ES_LOCKER variable
 *! \param __CODE target code segment
 */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define LOCK_CHECKER(__LOCKER, __CODE)  {                                    \
            {                                                                   \
                locker_t *pLocker = &(__LOCKER);                                \
                if (UNLOCKED == (*pLocker))                                     \
                {                                                               \
                    SAFE_ATOM_CODE(                                             \
                        if (UNLOCKED == (*pLocker)) {                           \
                            __CODE;                                             \
                        }                                                       \
                    )                                                           \
                }                                                               \
            }
#else
#   define LOCK_CHECKER(__LOCKER, ...)  {                                       \
            {                                                                   \
                locker_t *pLocker = &(__LOCKER);                                \
                if (UNLOCKED == (*pLocker))                                     \
                {                                                               \
                    SAFE_ATOM_CODE(                                             \
                        if (UNLOCKED == (*pLocker)) {                           \
                            __VA_ARGS__;                                        \
                        }                                                       \
                    )                                                           \
                }                                                               \
            }
#endif
            
/*! \note critical code section protection
 *! \note LOCKER could be only used among FSMs and there should be no ISR involved.
 *! \param __LOCKER ES_LOCKER variable
 *! \param __CODE   target code segment
 */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define LOCK(__LOCKER,__CODE)                                                \
            LOCK_CHECKER((__LOCKER),                                            \
                (*pLocker) = LOCKED;                                            \
                ENABLE_GLOBAL_INTERRUPT();                                      \
                __CODE;                                                         \
                (*pLocker) = UNLOCKED;                                          \
            )
#else
#   define LOCK(__LOCKER,...)                                                   \
            LOCK_CHECKER((__LOCKER),                                            \
                (*pLocker) = LOCKED;                                            \
                ENABLE_GLOBAL_INTERRUPT();                                      \
                __VA_ARGS__;                                                    \
                (*pLocker) = UNLOCKED;                                          \
            )
#endif

#define vsf_protect_t                   uint_fast32_t
#define vsf_protect_interrupt()         DISABLE_GLOBAL_INTERRUPT()
#define vsf_unprotect_interrupt(__state)SET_GLOBAL_INTERRUPT_STATE(__state)
#define vsf_protect_none()              (0)
#define vsf_unprotect_none(__state)     UNUSED_PARAM(__state)

#define vsf_protect_int()               vsf_protect_interrupt()
#define vsf_unprotect_int(__state)      vsf_unprotect_interrupt(__state)

#define __vsf_protect(__type)           vsf_protect_##__type
#define __vsf_unprotect(__type)         vsf_unprotect_##__type
#define vsf_protect(__type)             __vsf_protect(__type)
#define vsf_unprotect(__type)           __vsf_unprotect(__type)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define vsf_protect_region(__type, __CODE)                                   \
    do {                                                                        \
        vsf_protect_t __state = vsf_protect(__type)();                          \
        __CODE;                                                                 \
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
 *! \param plock locker object
 *! \retval lock section is entered
 *! \retval The section is locked
 */
extern bool enter_lock(locker_t *plock);

/*! \brief leave a section
 *! \param plock locker object
 *! \return none
 */
extern void leave_lock(locker_t *plock);
            
/*! \brief get locker status
 *! \param plock locker object
 *! \return locker status
 */
extern bool check_lock(locker_t *plock);            
           
/*! \brief initialize a locker
 *! \param plock locker object
 *! \return none
 */
extern void init_lock(locker_t *plock);

#ifdef __cplusplus
}
#endif

#endif
