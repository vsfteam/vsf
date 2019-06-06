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


//! \note Top Level VSF Configuration 

#ifndef __TOP_VSF_USR_CFG_H__
#define __TOP_VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

//#define ASSERT(...)         if (!(__VA_ARGS__)) {while(1);};
#define ASSERT(...)

#define VSF_PROJ_CFG_USE_CUBE               DISABLED

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//http://www.keil.com/support/man/docs/uv4/uv4_ut_configwizard.htm

//! \name kernel resource configuration
//! @{

//  <o>System Frequency (Hz)                     <32768-0xFFFFFFFF>
//  <i>Please specify the system frequency here.
#define SYSTEM_FREQ                         25000000ul

//  <o>System Timer Frequency (Hz)/ Tick Resolution         <32768-0xFFFFFFFF>
//  <i>Please specify the system timer frequency here. The frequency determins the period of 1 systmer timer tick. For example, 1000 means tick resolution is 1ms. 1000000 means tick resolution is us.
#define VSF_SYSTIMER_RESOLUTION             1000000ul
//  <h> Kernel Resource Configuration

//      <h> Kernal event-driven system configuration
//          <o>Maximum event pool size
//          <i>Simon, please add description here...
//#define VSF_OS_EVTQ_POOL_SIZE               16

//          <o>The number of event queues   <1-4>
//          <i>Simon, please add description here...
#define VSF_OS_EVTQ_NUM                     1
//      </h>

//      <o>The number of Software Interrupts <1-2>
//      <i>Simon, please add description here...
#define VSF_OS_EVTQ_SWI_NUM                 1

//      <o>The default eda stack frame pool size <1-65535>
//      <i>The default eda stack frame pool is shared among all eda tasks. 
//#define VSF_TASK_DEFAULT_FRAME_POOL_SIZE    16

//      <o>The heap size                    <256-0xFFFFFFFF>
//      <i>Specify the vsf heap size
#define VSF_HEAP_SIZE                       8192
//  </h>


//! @}

//! \name kernel feature configuration
//! @{
//  <h> Kernel Feature Configuration

//      <h> Main Function
//          <o>Main Stack Size              <128-65536:8>
//          <i>When main function is configured as a thread, this option controls the size of the stack.
#define VSF_OS_MAIN_STACK_SIZE               1024

//          <c1>Run main as a thread
//          <i>This feature will run main function as a thread. RTOS thread support must be enabled. 
//#define VSF_OS_RUN_MAIN_AS_THREAD           ENABLED
//          </c>
//      </h>
//  </h>
//! @}




/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */