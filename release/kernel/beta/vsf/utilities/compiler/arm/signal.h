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

#ifndef _USE_SIGNAL_H_
#define _USE_SIGNAL_H_

/*============================ INCLUDES ======================================*/
#include <stdbool.h>
#include <stdint.h>

/*! \brief How To Define and Use your own CODE_REGION
 *!        Example:

    static void __code_region_example_on_enter(void *pobj, void *plocal)
    {
        printf("-------enter-------\r\n");
    }

    static void __code_region_example_on_leave(void *pobj,void *plocal)
    {
        printf("-------leave-------\r\n");
    }

    const static i_code_region_t __example_code_region = {
        .OnEnter = __code_region_example_on_enter,
        .OnLeave = __code_region_example_on_leave,
    };


    void main(void)
    {
        ...
        code_region(&__example_code_region, NULL){
            printf("\tbody\r\n");
        }
        ...
    }

Output:

-------enter-------
        body
-------leave-------


 *! \note How to use code_region()
 *!       Syntax:
 *!             code_region(<Address of i_code_region_t obj>, <Object Address>) {
 *!                 //! put your code here
 *!             }
 *!
 *! \note <Address of i_code_region_t obj>: this can be NULL, if so, 
 *!         DEFAULT_CODE_REGION_NONE will be used.
 *! 
 *! \note <Object Address>: it is the address of the object you want to pass to 
 *!         your OnEnter and OnLeave functions. It can be NULL
 *!
 *! \note A local object will be generated from users' stack, the size is specified
 *!         by i_code_region_t.chLocalSize. The address of this local object will
 *!         be passed to your OnEnter and OnLeave functions. You can use it to
 *!         store some local status.
 *! 
 *! \name   List of Default Code Regions
 *! @{
 *!         DEFAULT_CODE_REGION_ATOM_CODE           //!< interrupt-safe region
 *!         DEFAULT_CODE_REGION_NONE                //!< do nothing
 *! @}
 */


/*============================ MACROS ========================================*/
#ifndef IAR_PATCH_CODE_REGION_LOCAL_SIZE
#   define IAR_PATCH_CODE_REGION_LOCAL_SIZE     4
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
//! \brief The safe ATOM code section macro
#define SAFE_ATOM_CODE()        code_region(&DEFAULT_CODE_REGION_ATOM_CODE)

         
#define __SAFE_ATOM_CODE(...)                                                   \
        {                                                                       \
            istate_t tState = DISABLE_GLOBAL_INTERRUPT();                       \
            __VA_ARGS__;                                                        \
            SET_GLOBAL_INTERRUPT_STATE(tState);                                 \
        }
    


//! \brief Exit from the safe atom operations
#define EXIT_SAFE_ATOM_CODE()           SET_GLOBAL_INTERRUPT_STATE(tState)  

 
#define exit_safe_atom_code()           EXIT_SAFE_ATOM_CODE()
#define safe_atom_code()                SAFE_ATOM_CODE()
#define __safe_atom_code(...)           __SAFE_ATOM_CODE(__VA_ARGS__)


//! \name ES_LOCKER value
//! @{
#define LOCKED          true            //!< locked
#define UNLOCKED        false           //!< unlocked
//! @}


#define LOCK_INIT(__LOCKER)     do {(__LOCKER) = UNLOCKED;}while(false)


#define EXIT_LOCK()     do {\
                            (*pLocker) = UNLOCKED;\
                            SET_GLOBAL_INTERRUPT_STATE(tState);\
                        } while(false)

#define ENTER_LOCK(__LOCKER)            enter_lock(__LOCKER)

#define LEAVE_LOCK(__LOCKER)            leave_lock(__LOCKER)
                        
#define GET_LOCK_STATUS(__LOCKER)       check_lock(__LOCKER)

#define INIT_LOCK(__LOCKER)             init_lock(__LOCKER)
                        
//! \brief exit lock checker structure
#define EXIT_LOCK_CHECKER()             EXIT_SAFE_ATOM_CODE()

/*! \note check specified locker and run code segment
 *! \param __LOCKER a ES_LOCKER variable
 *! \param __CODE target code segment
 */
#define LOCK_CHECKER(__LOCKER, ...)  {                                          \
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
            
//! \note critical code section protection
//! \note LOCKER could be only used among FSMs and there should be no ISR involved.
//! \param __LOCKER ES_LOCKER variable
//! \param __CODE   target code segment
#define LOCK(__LOCKER,...)                                                      \
            LOCK_CHECKER((__LOCKER),                                            \
                (*pLocker) = LOCKED;                                            \
                ENABLE_GLOBAL_INTERRUPT();                                      \
                __VA_ARGS__;                                                    \
                (*pLocker) = UNLOCKED;                                          \
            )

#if __IS_COMPILER_IAR__
#   define __CODE_REGION(__REGION_ADDR)                                         \
    for(code_region_t *pcode_region = (code_region_t *)(__REGION_ADDR);         \
        NULL != pcode_region;                                                   \
        pcode_region = NULL)                                                    \
        for(uint8_t local[IAR_PATCH_CODE_REGION_LOCAL_SIZE],                    \
                TPASTE2(__code_region_, __LINE__) = 1;                          \
            TPASTE2(__code_region_, __LINE__)-- ?                               \
                (pcode_region->pmethods->OnEnter(  pcode_region->ptarget, local)\
                    ,1)                                                         \
                : 0;                                                            \
            pcode_region->pmethods->OnLeave(pcode_region->ptarget, local))

#   define __CODE_REGION_SIMPLE(__REGION_ADDR, ...)                             \
    do {                                                                        \
        code_region_t *pcode_region = (code_region_t *)(__REGION_ADDR);         \
        uint8_t local[IAR_PATCH_CODE_REGION_LOCAL_SIZE];                        \
        pcode_region->pmethods->OnEnter(pcode_region->ptarget, local);          \
        __VA_ARGS__;                                                            \
        pcode_region->pmethods->OnLeave(pcode_region->ptarget, local);          \
    } while (0);
#else
#   define __CODE_REGION(__REGION_ADDR)                                         \
    for(code_region_t *pcode_region = (code_region_t *)(__REGION_ADDR);         \
        NULL != pcode_region;                                                   \
        pcode_region = NULL)                                                    \
        for(uint8_t local[pcode_region->pmethods->local_obj_size],              \
                TPASTE2(__code_region_, __LINE__) = 1;                          \
            TPASTE2(__code_region_, __LINE__)-- ?                               \
                (pcode_region->pmethods->OnEnter(  pcode_region->ptarget, local)\
                    ,1)                                                         \
                : 0;                                                            \
            pcode_region->pmethods->OnLeave(pcode_region->ptarget, local))

#   define __CODE_REGION_SIMPLE(__REGION_ADDR, ...)                             \
    do {                                                                        \
        code_region_t *pcode_region = (code_region_t *)(__REGION_ADDR);         \
        uint8_t local[pcode_region->pmethods->local_obj_size];                  \
        pcode_region->pmethods->OnEnter(pcode_region->ptarget, local);          \
        __VA_ARGS__;                                                            \
        pcode_region->pmethods->OnLeave(pcode_region->ptarget, local);          \
    } while (0);
#endif
        
#define EXIT_CODE_REGION()                                                      \
            pcode_region->ptMethods->OnLeave(pcode_region->ptarget, local)
#define exit_code_region()  EXIT_CODE_REGION()

#define CODE_REGION(__REGION_ADDR)          __CODE_REGION((__REGION_ADDR))
#define code_region(__REGION_ADDR)          __CODE_REGION((__REGION_ADDR))
#define CODE_REGION_SIMPLE(__REGION_ADDR, ...)                                  \
            __CODE_REGION_SIMPLE((__REGION_ADDR), __VA_ARGS__)
#define code_region_simple(__REGION_ADDR, ...)                                  \
            __CODE_REGION_SIMPLE((__REGION_ADDR), __VA_ARGS__)



#define vsf_protect_t                   uint_fast32_t
#define vsf_protect_interrupt()         GET_GLOBAL_INTERRUPT_STATE()
#define vsf_unprotect_interrupt(__state)SET_GLOBAL_INTERRUPT_STATE(__state)
#define vsf_protect_none()              0
#define vsf_unprotect_none(__state)     UNUSED_PARAM(__state)

#define vsf_protect_int()               vsf_protect_interrupt()
#define vsf_unprotect_int(...)          vsf_unprotect_interrupt(__VA_ARGS__)

#define __vsf_protect(__type)           vsf_protect_##__type
#define __vsf_unprotect(__type)         vsf_unprotect_##__type
#define vsf_protect(__type)             __vsf_protect(__type)
#define vsf_unprotect(__type)           __vsf_unprotect(__type)
#define vsf_protect_region(__type, ...)                                         \
    do {                                                                        \
        vsf_protect_t __state = vsf_protect(__type)();                          \
        __VA_ARGS__;                                                            \
        vsf_unprotect(__type)(__state);                                         \
    } while (0);

/*============================ TYPES =========================================*/
typedef volatile bool locker_t;

typedef struct {
    uint_fast8_t    local_obj_size;
    void (*OnEnter)(void *pobj, void *plocal);
    void (*OnLeave)(void *pobj, void *plocal);
}i_code_region_t;

typedef struct {
    void *ptarget;
    i_code_region_t *pmethods;
} code_region_t;

/*============================ GLOBAL VARIABLES ==============================*/
extern const code_region_t DEFAULT_CODE_REGION_ATOM_CODE;
extern const code_region_t DEFAULT_CODE_REGION_NONE;
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
#endif
