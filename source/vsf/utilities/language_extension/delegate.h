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

#ifndef __DELEGATE_H__
#define __DELEGATE_H__

/*============================ INCLUDES ======================================*/
#include "..\compiler.h"

#define __PLOOC_CLASS_USE_BLACK_BOX_TEMPLATE__
#include "..\ooc_class.h"


#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/
#define EVENT_RT_UNREGISTER         4

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef fsm_rt_t delegate_handler_func_t(void *pArg, void *pParam);


declare_class( delegate_handler_t )
//! \name general event handler
//! @{
extern_class( delegate_handler_t ,,
    delegate_handler_func_t     *fnHandler;                                     //!< event handler
    void                        *pArg;                                          //!< Argument
    delegate_handler_t          *ptNext;                                        //!< next 
)
end_extern_class(delegate_handler_t)
//! @}

declare_class( delegate_t )
//! \name event
//! @{
extern_class(delegate_t,,
    delegate_handler_t     *ptEvent;
    delegate_handler_t     *ptBlockedList;
    delegate_handler_t     **pptHandler;
)
end_extern_class(delegate_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


/*! \brief initialize event 
 *! \param ptEvent target event
 *! \return the address of event item
 */
extern delegate_t *delegate_init(delegate_t *ptEvent);

/*! \brief initialize event handler item
 *! \param ptHandler the target event handler item
 *! \param fnRoutine event handler routine
 *! \param pArg handler extra arguments
 *! \return the address of event handler item
 */
extern delegate_handler_t *delegate_handler_init(
    delegate_handler_t *ptHandler, delegate_handler_func_t *fnRoutine, void *pArg);

/*! \brief register event handler to specified event
 *! \param ptEvent target event
 *! \param ptHandler target event handler
 *! \return access result
 */
extern vsf_err_t register_delegate_handler(delegate_t *ptEvent, delegate_handler_t *ptHandler);

/*! \brief unregister a specified event handler
 *! \param ptEvent target event
 *! \param ptHandler target event handler
 *! \return access result
 */
extern vsf_err_t unregister_delegate_handler( delegate_t *ptEvent, delegate_handler_t *ptHandler);

/*! \brief raise target event
 *! \param ptEvent the target event
 *! \param pArg event argument
 *! \return access result
 */
extern fsm_rt_t invoke_delegate( delegate_t *ptEvent, void *pParam);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
