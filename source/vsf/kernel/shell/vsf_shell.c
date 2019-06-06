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
            uint16_t    is_support_frame_pool           : 1;
            uint16_t                                    : 6;
            uint16_t    evt_queue_bit_size              : 5;     //!< 1~32
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
    .is_support_teda =              (VSF_CFG_TIMER_EN ? (true) : false),
    .is_support_thread =            (VSF_USE_KERNEL_THREAD_MODE ? (true) : false),
    .is_support_simple_shell =      (VSF_USE_SIMPLE_SHELL ? (true) : false),
    .is_support_callback_timer =    (VSF_KERNEL_CFG_CALLBACK_TIMER ? true : false),


    .is_support_sync =              (VSF_CFG_SYNC_EN ? (true) : false),
    .is_support_bmpevt =            (VSF_CFG_BMPEVT_EN ? (true) : false),
    .is_support_premption =         (VSF_CFG_PREMPT_EN ? (true) : false),
    .is_support_msg_queue =         (VSF_CFG_QUEUE_EN ? (true) : false),
    
    .is_support_dynamic_priority =  (VSF_CFG_DYNAMIC_PRIOTIRY_EN ? (true) : false),
    .is_support_on_terminate =      (VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE ? true : false),
    .is_support_sub_call =          (VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL ? (true) : false),
    .is_support_fsm =               (VSF_KERNEL_CFG_EDA_SUPPORT_FSM ? (true) : false),
    .is_support_frame_pool =        (VSF_KERNEL_CFG_EDA_FRAME_POOL ? (true) : false),
    .evt_queue_bit_size =           VSF_OS_EVTQ_BITSIZE - 1,

    .Major =                        0,
    .Minor =                        0x91,
};

/*============================ IMPLEMENTATION ================================*/


/* EOF */
