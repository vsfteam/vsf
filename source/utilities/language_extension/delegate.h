/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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

#ifndef __DELEGATE_H__
#define __DELEGATE_H__

/*============================ INCLUDES ======================================*/
#include "../compiler/compiler.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__

#if     defined(__VSF_DELEGATE_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_DELEGATE_CLASS_IMPLEMENT
#elif   defined(__VSF_DELEGATE_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_DELEGATE_CLASS_INHERIT__
#endif   

#include "../ooc_class.h"


#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/
#define EVENT_RT_UNREGISTER         4

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef fsm_rt_t delegate_handler_func_t(void *arg_ptr, void *param_ptr);


dcl_class( delegate_handler_t )
//! \name general event handler
//! @{
def_class( delegate_handler_t,
    private_member(
        delegate_handler_func_t     *handler_fn;                                    //!< event handler
        void                        *arg_ptr;                                       //!< Argument
        delegate_handler_t          *next_ptr;                                      //!< next 
    )
)
end_def_class(delegate_handler_t)
//! @}

dcl_class( delegate_t )
//! \name event
//! @{
def_class(delegate_t,
    private_member(
        delegate_handler_t     *event_ptr;
        delegate_handler_t     *blocked_list_ptr;
        delegate_handler_t     **handler_pptr;
    )
)
end_def_class(delegate_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


/*! \brief initialize event 
 *! \param event_ptr target event
 *! \return the address of event item
 */
extern delegate_t *delegate_init(delegate_t *event_ptr);

/*! \brief initialize event handler item
 *! \param handler_ptr the target event handler item
 *! \param routine_fn event handler routine
 *! \param arg_ptr handler extra arguments
 *! \return the address of event handler item
 */
extern delegate_handler_t *delegate_handler_init(
                                            delegate_handler_t *handler_ptr, 
                                            delegate_handler_func_t *routine_fn, 
                                            void *arg_ptr);

/*! \brief register event handler to specified event
 *! \param event_ptr target event
 *! \param handler_ptr target event handler
 *! \return access result
 */
extern vsf_err_t register_delegate_handler( delegate_t *event_ptr, 
                                            delegate_handler_t *handler_ptr);

/*! \brief unregister a specified event handler
 *! \param event_ptr target event
 *! \param handler_ptr target event handler
 *! \return access result
 */
extern vsf_err_t unregister_delegate_handler(   delegate_t *event_ptr, 
                                                delegate_handler_t *handler_ptr);

/*! \brief raise target event
 *! \param event_ptr the target event
 *! \param arg_ptr event argument
 *! \return access result
 */
extern fsm_rt_t invoke_delegate( delegate_t *event_ptr, void *param_ptr);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
