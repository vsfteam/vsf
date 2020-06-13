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
#   define this             (*this_ptr)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef fsm_rt_t delegate_handler_func_t(void *arg_ptr, void *param_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name general event handler
//! @{
def_class(delegate_handler_t,,
    delegate_handler_func_t     *handler_fn;         //!< event handler
    void                        *arg_ptr;              //!< Argument
    class(delegate_handler_t)   *next_ptr;            //!< next 
)
end_def_class(delegate_handler_t)
//! @}

//! \name event
//! @{
def_class(delegate_t,,
    delegate_handler_t          *event_ptr;
    delegate_handler_t          *blocked_list_ptr;
    class(delegate_handler_t)   **handler_pptr;
)
end_def_class(delegate_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief initialize event 
 *! \param event_ptr target event
 *! \return the address of event item
 */
delegate_t *delegate_init(delegate_t *event_ptr)
{
    class_internal(event_ptr, this_ptr, delegate_t);

    do {
        if (NULL == this_ptr) {
            break;
        }

        this.event_ptr = NULL;
        this.blocked_list_ptr = NULL;
        this.handler_pptr = (class(delegate_handler_t) **)&(this.event_ptr);
        
    } while (0);

    return event_ptr;
}

/*! \brief initialize event handler item
 *! \param handler_ptr the target event handler item
 *! \param routine_fn event handler routine
 *! \param arg_ptr handler extra arguments
 *! \return the address of event handler item
 */
delegate_handler_t *delegate_handler_init(
    delegate_handler_t *handler_ptr, delegate_handler_func_t *routine_fn, void *arg_ptr)
{
    
    class_internal(handler_ptr, this_ptr, delegate_handler_t);
    
    if (NULL == handler_ptr || NULL == routine_fn) {
        return NULL;
    }
    this.handler_fn = routine_fn;
    this.arg_ptr = arg_ptr;
    this.next_ptr = NULL;

    return handler_ptr;
}


static class(delegate_handler_t) **search_list(
    class(delegate_handler_t) **handler_pptr, class(delegate_handler_t) *hnd_ptr)
{
    //! search event handler chain
    while (NULL != (*handler_pptr)) {
        if ((*handler_pptr) == hnd_ptr) {
            return handler_pptr;
        }
        handler_pptr = &((*handler_pptr)->next_ptr);      //!< get next item
    }
    return NULL;
}

/*! \brief register event handler to specified event
 *! \param event_ptr target event
 *! \param handler_ptr target event handler
 *! \return access result
 */
vsf_err_t register_delegate_handler(delegate_t *event_ptr, delegate_handler_t *handler_ptr)
{
    class_internal(event_ptr, this_ptr, delegate_t);
    class_internal(handler_ptr, hnd_ptr, delegate_handler_t);

    if ((NULL == event_ptr) || (NULL == handler_ptr) || (NULL == hnd_ptr->handler_fn)) {
        return VSF_ERR_INVALID_PTR;
    } else if (NULL != hnd_ptr->next_ptr) {     
        //! search ready list
        class(delegate_handler_t) **handler_pptr = search_list(   
            (class(delegate_handler_t) **)&(this.blocked_list_ptr), hnd_ptr );

        if (NULL != handler_pptr) {
            //! safe to remove
            (*handler_pptr) = hnd_ptr->next_ptr;
            hnd_ptr->next_ptr = NULL;
        } else {        
            return VSF_ERR_REQ_ALREADY_REGISTERED;
        }
    }

    //! add handler to the ready list
    hnd_ptr->next_ptr = (class(delegate_handler_t) *)(this.event_ptr);
    this.event_ptr = handler_ptr;

    return VSF_ERR_NONE;
}



/*! \brief unregister a specified event handler
 *! \param event_ptr target event
 *! \param handler_ptr target event handler
 *! \return access result
 */
vsf_err_t unregister_delegate_handler( delegate_t *event_ptr, delegate_handler_t *handler_ptr)
{
    class_internal(event_ptr, this_ptr, delegate_t);
    class_internal(handler_ptr, hnd_ptr, delegate_handler_t);
    
    class(delegate_handler_t) **handler_pptr;
    if ((NULL == event_ptr) || (NULL == handler_ptr)) {
        return VSF_ERR_INVALID_PTR;
    } 

    do {
        //! search ready list
        handler_pptr = search_list(   (class(delegate_handler_t) **)&(this.event_ptr), 
                                    hnd_ptr );
        if (NULL != handler_pptr) {
            //! safe to remove
            (*handler_pptr) = hnd_ptr->next_ptr;
            hnd_ptr->next_ptr = NULL;
            if (this.handler_pptr == &(hnd_ptr->next_ptr)) {
                this.handler_pptr = handler_pptr;
            }
            break;
        }
        //! search ready list
        handler_pptr = search_list(   (class(delegate_handler_t) **)&(this.blocked_list_ptr), 
                                    hnd_ptr );
        if (NULL != handler_pptr) {
            //! safe to remove
            (*handler_pptr) = hnd_ptr->next_ptr;
            hnd_ptr->next_ptr = NULL;
            if (this.handler_pptr == &(hnd_ptr->next_ptr)) {
                this.handler_pptr = handler_pptr;
            }
            break;
        }
    } while(false);
    
    return VSF_ERR_NONE;
}

static fsm_rt_t __move_to_block_list(class(delegate_t) *this_ptr, class(delegate_handler_t) *handler_ptr)
{
    class(delegate_handler_t) *hnd_ptr = handler_ptr;
    //! remove handler from ready list
    (*this.handler_pptr) = hnd_ptr->next_ptr;
    //! add handler to block list
    hnd_ptr->next_ptr = (class(delegate_handler_t) *)this.blocked_list_ptr;
    this.blocked_list_ptr = (delegate_handler_t *)hnd_ptr;

    if (NULL == this.event_ptr) {
        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}

#define RAISE_EVENT_START               0
#define RAISE_EVENT_CHECK_HANDLER       1
#define RAISE_EVENT_RUN_HANDLER         2
#define RAISE_EVENT_RESET_FSM()     do { this.chState = 0; } while (0)

/*! \brief raise target event
 *! \param event_ptr the target event
 *! \param param_ptr event parameter
 *! \return access result
 */
fsm_rt_t invoke_delegate( delegate_t *event_ptr, void *param_ptr)
{
    class_internal(event_ptr, this_ptr, delegate_t);
    if (NULL == this_ptr) {
        return (fsm_rt_t)VSF_ERR_INVALID_PTR;
    }

    if (NULL == this.event_ptr) {
        if (NULL == this.blocked_list_ptr) {
            //! nothing to do
            return fsm_rt_cpl;
        }
        
        //! initialize state
        this.event_ptr = this.blocked_list_ptr;
        this.blocked_list_ptr = NULL;
        this.handler_pptr = (class(delegate_handler_t) **)&(this.event_ptr);
    } 

    if (NULL == (*this.handler_pptr)) {
        //! finish visiting the ready list
        this.handler_pptr = (class(delegate_handler_t) **)&(this.event_ptr);
        if (NULL == (*this.handler_pptr)) {
            //! complete
            return fsm_rt_cpl;
        }
    } else {
        class(delegate_handler_t) *handler_ptr = (*this.handler_pptr);
        
        if (NULL != handler_ptr->handler_fn) {
            //! run the event handler
            fsm_rt_t tFSM = handler_ptr->handler_fn(handler_ptr->arg_ptr,param_ptr);

            if (fsm_rt_on_going == tFSM) { 
                this.handler_pptr = &(handler_ptr->next_ptr);    //!< get next item
            } else if (EVENT_RT_UNREGISTER == tFSM) {
                //! return EVENT_RT_UNREGISTER means event handler could be removed
                class(delegate_handler_t) *hnd_ptr = handler_ptr;
                (*this.handler_pptr) = hnd_ptr->next_ptr;
                hnd_ptr->next_ptr = NULL;
            } else {
                return __move_to_block_list(this_ptr, handler_ptr);
            }
        } else {
            return __move_to_block_list(this_ptr, handler_ptr);
        }
    }

    return fsm_rt_on_going;
}




/* EOF */

