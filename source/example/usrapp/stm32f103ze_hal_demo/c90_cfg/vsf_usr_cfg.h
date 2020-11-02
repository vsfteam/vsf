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


//! \note Top Level Application Configuration 

#ifndef __TOP_APP_CFG_H__
#define __TOP_APP_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

//#define ASSERT(...)         if (!(__VA_ARGS__)) {vsf_interrupt_safe() {while(1);}};
#define ASSERT(...)

#define VSF_SYSTIMER_FREQ               25000000ul

#define VSF_HEAP_SIZE                   4096

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//http://www.keil.com/support/man/docs/uv4/uv4_ut_configwizard.htm

//! \name kernel resource configuration
//! @{

//  <h> Kernel Resource Configuration

//      <h> Kernal event-driven system configuration
//          <o>Maximum event pool size
//          <i>Simon, please add description here...
//#define VSF_OS_CFG_EVTQ_POOL_SIZE                     16

//          <o>The number of preemptive priorities <1-4>
//          <i>Simon, please add description here...
//#define VSF_OS_CFG_PRIORITY_NUM                         1
//      </h>

//#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED

//      <o>The default eda stack frame pool size <1-65535>
//      <i>The default eda stack frame pool is shared among all eda tasks. 
//#define VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE       16
//  </h>

//! @}

//! \name kernel feature configuration
//! @{
//  <h> Kernel Feature Configuration
//      <h> Main Function
//          <o>Main Stack Size              <128-65536:8>
//          <i>When main function is configured as a thread, this option controls the size of the stack.
#define VSF_OS_CFG_MAIN_STACK_SIZE              2048

//          <c1>Run main as a thread
//          <i>This feature will run main function as a thread. RTOS thread support must be enabled. 
#define VSF_OS_CFG_MAIN_MODE                    VSF_OS_CFG_MAIN_MODE_IDLE
//          </c>
//      </h>
//  </h>
//! @}

#define VSF_USE_MSG_TREE                                    ENABLED
#define VSF_USE_DYNARR                                      DISABLED

#   define VSF_USE_TINY_GUI                             ENABLED
#       define VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL      VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
#       define VSF_TGUI_CFG_COLOR_MODE                  VSF_TGUI_COLOR_ARGB_8888
#       define VSF_TGUI_CFG_SUPPORT_NAME_STRING         ENABLED         /* Enabled for debug */

#       define VSF_TGUI_CFG_SV_BUTTON_ADDITIONAL_TILES  ENABLED
#       define VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR  VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#       define VSF_TGUI_CFG_SV_PANEL_ADDITIONAL_TILES   ENABLED
#       define VSF_TGUI_CFG_SV_PANEL_BACKGROUND_COLOR   VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)
#       define VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES   ENABLED
#       define VSF_TGUI_CFG_SV_LABEL_BACKGROUND_COLOR   VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)

#       define VSF_TGUI_LOG                             

#   define VSF_USE_MSG_TREE                             ENABLED
#       define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING     ENABLED         /* Enabled for debug */
#       define VSF_MSGT_NODE_OFFSET_TYPE                int8_t

//#define VSF_POOL_LOCK()             
//#define VSF_POOL_UNLOCK()
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
