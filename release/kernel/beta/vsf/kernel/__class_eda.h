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

//#ifndef __VSF_KERNEL_EDA_H__           /* deliberately comment this out! */
//#define __VSF_KERNEL_EDA_H__           /* deliberately comment this out! */


/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __VSF_CLASS_USE_STRICT_TEMPLATE__
#include "utilities/ooc_class.h"

declare_class(vsf_eda_t)
declare_class(vsf_teda_t)
declare_class(vsf_sync_t)
declare_class(vsf_sync_owner_t)
declare_class(vsf_bmpevt_t)
declare_class(vsf_bmpevt_pender_t)
declare_class(vsf_bmpevt_adapter_t)
declare_class(vsf_bmpevt_adapter_eda_t)
declare_class(vsf_queue_t)

typedef uint16_t vsf_evt_t;
typedef void (*vsf_eda_evthandler_t)(vsf_eda_t *peda, vsf_evt_t evt);
typedef void (*vsf_eda_on_terminate_t)(vsf_eda_t *eda);



//! \name eda
//! @{
def_class(vsf_eda_t)

    public_member(
        // you can add public member here
        vsf_eda_evthandler_t    evthandler;
    #ifdef VSF_CFG_EVTQ_EN
        vsf_eda_on_terminate_t  on_terminate;
    #endif
        //uint16_t                app_state;
    )

    private_member(
    #if VSF_CFG_SYNC_EN
        vsf_dlist_node_t    pending_list;
    #endif

    #if VSF_CFG_PREMPT_EN
    #   if VSF_CFG_DYNAMIC_PRIOTIRY_EN
        vsf_dlist_node_t    rdy_list;
        vsf_slist_queue_t   evt_list;
        uint8_t             cur_priority;
        uint8_t             new_priority;
        uint8_t             priority;

        union {
            struct {
                uint8_t     is_ready        : 1;
                uint8_t     is_new_prio     : 1;
    #   else
        uint8_t             evt_cnt;
        union {
            uint8_t         priority;
            uint8_t         cur_priority;
        };
        union {
            struct {
    #   endif
            uint8_t         is_to_exit      : 1;
    #else
        uint16_t            evt_pending;
        union {
            struct {
                uint8_t     is_processing   : 1;
    #endif

    #   if VSF_CFG_SYNC_EN
                /* if limitted is set, eda can only receive 1 event */
                uint8_t     is_limitted     : 1;
                uint8_t     is_sync_got     : 1;
    #   endif

    #if VSF_CFG_TIMER_EN
                /* has_timer and timed is used in teda */
                uint8_t     is_timed        : 1;
    #endif
                uint8_t     is_stack_owner  : 1;
                uint8_t     polling_state   : 1; 
            };
            uint8_t         flag;
        };
    )

end_def_class(vsf_eda_t)
//! @}

#if VSF_CFG_TIMER_EN
//! \name timed eda
//! @{
def_class(vsf_teda_t,
    which(
        implement(vsf_eda_t)
    ))
    private_member(
        vsf_dlist_node_t    timer_list;
        uint32_t            due;
    )
end_def_class(vsf_teda_t)
//! @}
#endif

#if VSF_CFG_SYNC_EN
//! \name sync
//! @{
def_class(vsf_sync_t)

    private_member(
        union {
            struct {
                uint16_t    cur         : 15;
                uint16_t    has_owner   : 1;
            };
            uint16_t        cur_value;
        };
        union {
            struct {
                uint16_t    max         : 15;
                uint16_t    manual_rst  : 1;
            };
            uint16_t        max_value;
        };

        vsf_dlist_t         pending_list;
    )
end_def_class(vsf_sync_t)
//! @}

//! \name sync_with_owner
//! @{
def_class(vsf_sync_owner_t,
    which(
        implement(vsf_sync_t)
    ))
    private_member(
        vsf_eda_t           *peda_owner;
    )
end_def_class(vsf_sync_owner_t)
//! @}

#ifndef __VSF_BITMAP_EVT_DEFINED__
#define __VSF_BITMAP_EVT_DEFINED__

#define VSF_BMPEVT_OR               0
#define VSF_BMPEVT_AND              1

struct vsf_bmpevt_adapter_op_t {
    vsf_err_t (*init)(vsf_bmpevt_adapter_t *pthis);
    vsf_err_t (*reset)(vsf_bmpevt_adapter_t *pthis);
};
typedef struct vsf_bmpevt_adapter_op_t vsf_bmpevt_adapter_op_t;
#endif
 
//! \name bmpevt_adapter
//! @{
def_class(vsf_bmpevt_adapter_t)

    public_member (
        const vsf_bmpevt_adapter_op_t   *pop;
        const uint32_t                  mask;
    )
    private_member(
        vsf_bmpevt_t                        *pbmpevt_host;
    )
end_def_class(vsf_bmpevt_adapter_t)
//! @}

//! \name bmpevt_adapter_eda
//! @{
def_class(vsf_bmpevt_adapter_eda_t,
    which(
        implement(vsf_bmpevt_adapter_t)
    ))
    private_member(
        vsf_eda_t           eda;
    )
end_def_class(vsf_bmpevt_adapter_t)
//! @}

//! \name bmpevt_pender
//! @{
def_class(vsf_bmpevt_pender_t)

    private_member(
        vsf_eda_t           *peda_pending;
    )
    public_member (
        uint32_t        mask;
        uint8_t         operator    : 1;
    )

end_def_class(vsf_bmpevt_pender_t)
//! @}

//! \name bmpevt
//! @{
def_class(vsf_bmpevt_t)

    private_member(
        vsf_dlist_t                 pending_list;
        uint32_t                    value;
        uint32_t                    cancelled_value;
    )
    public_member (
        uint32_t                auto_reset;
        vsf_bmpevt_adapter_t    **ppadapters;
    )
    private_member(
        union {
            struct {
                uint8_t             adapter_count   : 5;
                uint8_t             is_cancelling   : 1;
                uint8_t             is_polling      : 1;
                uint8_t             is_to_repoll    : 1;
            };
            uint8_t                 flag;
        };
    )
end_def_class(vsf_bmpevt_t)
//! @}

//! \name queue
//! @{
def_class(vsf_queue_t,
    which(
        implement(vsf_sync_t)
    ))
    private_member(
        vsf_slist_queue_t   msgq;
    )
end_def_class(vsf_queue_t)
//! @}

#endif      // VSF_CFG_SYNC_EN

//#endif                                /* deliberately comment this out! */
