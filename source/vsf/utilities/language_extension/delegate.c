/****************************************************************************
*  2020 Modified by VSF Team                                                *
*  Copyright 2017-2019 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)  *
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

#include ".\delegate.h"

/*============================ MACROS ========================================*/

/*============================ MACROS ========================================*/
#define EVENT_RT_UNREGISTER         4

#ifndef this
#   define this             (*ptThis)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef fsm_rt_t delegate_handler_func_t(void *pArg, void *pParam);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name general event handler
//! @{
def_class(delegate_handler_t,,
    delegate_handler_func_t     *fnHandler;         //!< event handler
    void                        *pArg;              //!< Argument
    class(delegate_handler_t)   *ptNext;            //!< next 
)
end_def_class(delegate_handler_t)
//! @}

//! \name event
//! @{
def_class(delegate_t,,
    delegate_handler_t          *ptEvent;
    delegate_handler_t          *ptBlockedList;
    class(delegate_handler_t)   **pptHandler;
)
end_def_class(delegate_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief initialize event 
 *! \param ptEvent target event
 *! \return the address of event item
 */
delegate_t *delegate_init(delegate_t *ptEvent)
{
    class_internal(ptEvent, ptThis, delegate_t);

    do {
        if (NULL == ptThis) {
            break;
        }

        this.ptEvent = NULL;
        this.ptBlockedList = NULL;
        this.pptHandler = (class(delegate_handler_t) **)&(this.ptEvent);
        
    } while (0);

    return ptEvent;
}

/*! \brief initialize event handler item
 *! \param ptHandler the target event handler item
 *! \param fnRoutine event handler routine
 *! \param pArg handler extra arguments
 *! \return the address of event handler item
 */
delegate_handler_t *delegate_handler_init(
    delegate_handler_t *ptHandler, delegate_handler_func_t *fnRoutine, void *pArg)
{
    
    class_internal(ptHandler, ptThis, delegate_handler_t);
    
    if (NULL == ptHandler || NULL == fnRoutine) {
        return NULL;
    }
    this.fnHandler = fnRoutine;
    this.pArg = pArg;
    this.ptNext = NULL;

    return ptHandler;
}


static class(delegate_handler_t) **search_list(
    class(delegate_handler_t) **pptHandler, class(delegate_handler_t) *ptHND)
{
    //! search event handler chain
    while (NULL != (*pptHandler)) {
        if ((*pptHandler) == ptHND) {
            return pptHandler;
        }
        pptHandler = &((*pptHandler)->ptNext);      //!< get next item
    }
    return NULL;
}

/*! \brief register event handler to specified event
 *! \param ptEvent target event
 *! \param ptHandler target event handler
 *! \return access result
 */
vsf_err_t register_delegate_handler(delegate_t *ptEvent, delegate_handler_t *ptHandler)
{
    class_internal(ptEvent, ptThis, delegate_t);
    class_internal(ptHandler, ptHND, delegate_handler_t);

    if ((NULL == ptEvent) || (NULL == ptHandler) || (NULL == ptHND->fnHandler)) {
        return VSF_ERR_INVALID_PTR;
    } else if (NULL != ptHND->ptNext) {     
        //! search ready list
        class(delegate_handler_t) **pptHandler = search_list(   
            (class(delegate_handler_t) **)&(this.ptBlockedList), ptHND );

        if (NULL != pptHandler) {
            //! safe to remove
            (*pptHandler) = ptHND->ptNext;
            ptHND->ptNext = NULL;
        } else {        
            return VSF_ERR_REQ_ALREADY_REGISTERED;
        }
    }

    //! add handler to the ready list
    ptHND->ptNext = (class(delegate_handler_t) *)(this.ptEvent);
    this.ptEvent = ptHandler;

    return VSF_ERR_NONE;
}



/*! \brief unregister a specified event handler
 *! \param ptEvent target event
 *! \param ptHandler target event handler
 *! \return access result
 */
vsf_err_t unregister_delegate_handler( delegate_t *ptEvent, delegate_handler_t *ptHandler)
{
    class_internal(ptEvent, ptThis, delegate_t);
    class_internal(ptHandler, ptHND, delegate_handler_t);
    
    class(delegate_handler_t) **pptHandler;
    if ((NULL == ptEvent) || (NULL == ptHandler)) {
        return VSF_ERR_INVALID_PTR;
    } 

    do {
        //! search ready list
        pptHandler = search_list(   (class(delegate_handler_t) **)&(this.ptEvent), 
                                    ptHND );
        if (NULL != pptHandler) {
            //! safe to remove
            (*pptHandler) = ptHND->ptNext;
            ptHND->ptNext = NULL;
            if (this.pptHandler == &(ptHND->ptNext)) {
                this.pptHandler = pptHandler;
            }
            break;
        }
        //! search ready list
        pptHandler = search_list(   (class(delegate_handler_t) **)&(this.ptBlockedList), 
                                    ptHND );
        if (NULL != pptHandler) {
            //! safe to remove
            (*pptHandler) = ptHND->ptNext;
            ptHND->ptNext = NULL;
            if (this.pptHandler == &(ptHND->ptNext)) {
                this.pptHandler = pptHandler;
            }
            break;
        }
    } while(false);
    
    return VSF_ERR_NONE;
}

static fsm_rt_t __move_to_block_list(class(delegate_t) *ptThis, class(delegate_handler_t) *ptHandler)
{
    class(delegate_handler_t) *ptHND = ptHandler;
    //! remove handler from ready list
    (*this.pptHandler) = ptHND->ptNext;
    //! add handler to block list
    ptHND->ptNext = (class(delegate_handler_t) *)this.ptBlockedList;
    this.ptBlockedList = (delegate_handler_t *)ptHND;

    if (NULL == this.ptEvent) {
        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}

#define RAISE_EVENT_START               0
#define RAISE_EVENT_CHECK_HANDLER       1
#define RAISE_EVENT_RUN_HANDLER         2
#define RAISE_EVENT_RESET_FSM()     do { this.chState = 0; } while (0)

/*! \brief raise target event
 *! \param ptEvent the target event
 *! \param pParam event parameter
 *! \return access result
 */
fsm_rt_t invoke_delegate( delegate_t *ptEvent, void *pParam)
{
    class_internal(ptEvent, ptThis, delegate_t);
    if (NULL == ptThis) {
        return (fsm_rt_t)VSF_ERR_INVALID_PTR;
    }

    if (NULL == this.ptEvent) {
        if (NULL == this.ptBlockedList) {
            //! nothing to do
            return fsm_rt_cpl;
        }
        
        //! initialize state
        this.ptEvent = this.ptBlockedList;
        this.ptBlockedList = NULL;
        this.pptHandler = (class(delegate_handler_t) **)&(this.ptEvent);
    } 

    if (NULL == (*this.pptHandler)) {
        //! finish visiting the ready list
        this.pptHandler = (class(delegate_handler_t) **)&(this.ptEvent);
        if (NULL == (*this.pptHandler)) {
            //! complete
            return fsm_rt_cpl;
        }
    } else {
        class(delegate_handler_t) *ptHandler = (*this.pptHandler);
        
        if (NULL != ptHandler->fnHandler) {
            //! run the event handler
            fsm_rt_t tFSM = ptHandler->fnHandler(ptHandler->pArg,pParam);

            if (fsm_rt_on_going == tFSM) { 
                this.pptHandler = &(ptHandler->ptNext);    //!< get next item
            } else if (EVENT_RT_UNREGISTER == tFSM) {
                //! return EVENT_RT_UNREGISTER means event handler could be removed
                class(delegate_handler_t) *ptHND = ptHandler;
                (*this.pptHandler) = ptHND->ptNext;
                ptHND->ptNext = NULL;
            } else {
                return __move_to_block_list(ptThis, ptHandler);
            }
        } else {
            return __move_to_block_list(ptThis, ptHandler);
        }
    }

    return fsm_rt_on_going;
}




/* EOF */

