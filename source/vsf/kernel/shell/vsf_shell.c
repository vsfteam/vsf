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
#include "../vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".vsf.feature.kernel")
const struct {
    union {
        struct {
            uint8_t     Major;
            uint8_t     Minor;
        };
        uint16_t        CoreVersion;
    } ;
    union {
        struct {
            uint8_t    is_support_teda                 : 1;
            uint8_t    is_support_task                 : 1;
            uint8_t    is_support_thread               : 1;
            uint8_t    is_support_simple_shell         : 1;
            uint8_t    is_support_callback_timer       : 1;
            uint8_t                                    : 3;     //!< reserved
        };
        uint8_t Task;
    };
    union {
        struct {
            uint8_t    is_support_sync                 : 1;
            uint8_t    is_support_bmpevt               : 1;
            uint8_t    is_support_premption            : 1;
            uint8_t    is_support_msg_queue            : 1;
            uint8_t                                    : 4;     //!< reserved
        };
        uint8_t IPC;
    };
    
    union {
        struct {
            uint16_t    is_support_dynamic_priority     : 1;
            uint16_t    is_support_on_terminate         : 1;
            uint16_t    is_support_sub_call             : 1;
            uint16_t    is_support_fsm                  : 1;
            uint16_t                                    : 12;
        };
        uint16_t EDA;
    };
    union {
        struct {
            uint16_t    ClientID;
            union {
                struct {
                    uint8_t Major;
                    uint8_t Minor;
                };
                uint16_t Version;
            };
        };
    }Customisation;
} VSF_FEATURE_KERNEL = {
    .is_support_teda =              (VSF_KERNEL_CFG_EDA_SUPPORT_TIMER ? (true) : false),
    .is_support_thread =            (VSF_KERNEL_CFG_SUPPORT_THREAD ? (true) : false),
    .is_support_simple_shell =      (VSF_USE_KERNEL_SIMPLE_SHELL ? (true) : false),
    .is_support_callback_timer =    (VSF_KERNEL_CFG_CALLBACK_TIMER ? true : false),


    .is_support_sync =              (VSF_KERNEL_CFG_SUPPORT_SYNC ? (true) : false),
    .is_support_bmpevt =            (VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT ? (true) : false),
    .is_support_premption =         (VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED ? (true) : false),
    .is_support_msg_queue =         (VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE ? (true) : false),
    
    .is_support_dynamic_priority =  (VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY ? (true) : false),
    .is_support_on_terminate =      (VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE ? true : false),
    .is_support_sub_call =          (VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL ? (true) : false),
    .is_support_fsm =               (VSF_KERNEL_CFG_EDA_SUPPORT_FSM ? (true) : false),

    .Major =                        1,
    .Minor =                        0x20,
};

/*! Kernal Version Log:
 *!     v1.20       Introduce param_eda and the concept of local, arg and param
 *!     v1.1x       Introduce cross call among different types of tasks
 *!     v1.0x       First stable release
 *!     v0.9x       Alpha release 
 *!
 */
/*============================ IMPLEMENTATION ================================*/

#endif
/* EOF */
