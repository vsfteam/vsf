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
#include "kernel/vsf_kernel_cfg.h"


#if VSF_USE_SIMPLE_SHELL == ENABLED && VSF_CFG_BMPEVT_EN == ENABLED

#   include "../../vsf_kernel_common.h"
#   include "./vsf_simple.h"
#   include "../../task/vsf_thread.h"
/*============================ MACROS ========================================*/
#undef  this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
SECTION(".text.vsf.kernel.vsf_eda_polling_state_get")
extern bool vsf_eda_polling_state_get(vsf_eda_t *peda);

SECTION(".text.vsf.kernel.vsf_eda_polling_state_set")
extern void vsf_eda_polling_state_set(vsf_eda_t *peda, bool state);

/*============================ IMPLEMENTATION ================================*/


static vsf_sync_reason_t __vsf_bmpevt_pend(  vsf_bmpevt_t *pbmpevt,
                                    vsf_bmpevt_pender_t *ppender,
                                    int_fast32_t time_out)
{

    vsf_sync_reason_t reason = VSF_SYNC_PENDING;
    ASSERT(NULL != pbmpevt && NULL != ppender);

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    vsf_eda_t *peda = vsf_eda_get_cur();
    ASSERT(NULL != peda);

    if (vsf_eda_is_stack_owner(peda)) {
        //! a thread
        reason =  vsf_thread_bmpevt_pend(pbmpevt, ppender, time_out);
    } else 
#endif
    {
        switch(vsf_eda_bmpevt_pend(pbmpevt, ppender, time_out)) {
            case VSF_ERR_NONE:
                reason = VSF_SYNC_GET;
                break;
            case VSF_ERR_NOT_READY:
                break;
            default:
            case VSF_ERR_FAIL:
                reason = VSF_SYNC_CANCEL;
                break;
        }
    }
    
    return reason;
}

SECTION("text.vsf.kernel.vsf_bmpevt_wait_for")
vsf_sync_reason_t vsf_bmpevt_wait_for(  vsf_bmpevt_t *pbmpevt,
                                        const vsf_bmpevt_pender_t *ppender,
                                        int_fast32_t time_out)
{
    vsf_sync_reason_t result = VSF_SYNC_PENDING;
    enum {
        VSF_APP_STATE_PEND = 0,
        VSF_APP_STATE_POLL,
    };
    
    do {
        vsf_eda_t *peda = vsf_eda_get_cur();                                
        if (vsf_eda_polling_state_get(peda)) {
            /* VSF_APP_STATE_POLL */
            if (time_out < 0) {
                if (vsf_eda_get_cur_evt() != VSF_EVT_SYNC_POLL) {
                    break;
                }
            }
            result = vsf_bmpevt_poll(   pbmpevt, 
                                        (vsf_bmpevt_pender_t *)ppender, 
                                        VSF_EVT_SYNC_POLL);
            if (VSF_SYNC_GET == result) {
                vsf_eda_polling_state_set(peda, (bool)VSF_APP_STATE_PEND);
            }
        } else {
            /* VSF_APP_STATE_PEND */
            result = __vsf_bmpevt_pend( pbmpevt, 
                                        (vsf_bmpevt_pender_t *)ppender, 
                                        time_out); 
            if (    vsf_eda_is_stack_owner(peda) 
                ||  VSF_SYNC_GET == result
                ||  VSF_SYNC_CANCEL == result) {
                break;
            }
            vsf_eda_polling_state_set(peda, (bool)VSF_APP_STATE_POLL);
            
        }
    } while(false); 

    return result;
}

SECTION("text.vsf.kernel.grouped_evts_init")
void grouped_evts_init(vsf_bmpevt_t *ptThis, 
                    vsf_bmpevt_adapter_t **ppadapters, 
                    uint_fast8_t adapter_count,
                    uint_fast32_t auto_reset)
{
    ASSERT(NULL != ptThis && NULL != ppadapters);
    this.auto_reset = auto_reset;
    
    if (adapter_count) {
        this.adapters = ppadapters;
    }
    vsf_bmpevt_init(ptThis, adapter_count);
}

#endif
/*EOF*/
