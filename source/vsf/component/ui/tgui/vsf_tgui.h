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

#ifndef __VSF_TINY_GUI_H__
#define __VSF_TINY_GUI_H__

/*============================ INCLUDES ======================================*/
#include "./vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

#include "utilities/vsf_tgui_text.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */
#if     defined(__VSF_TGUI_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_TGUI_CLASS_IMPLEMENT
#elif   defined(__VSF_TGUI_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_TGUI_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_vsf_rng_buf(__vsf_tgui_evt_queue_t)

def_vsf_rng_buf(__vsf_tgui_evt_queue_t, vsf_tgui_evt_t)

declare_structure(__vk_tgui_focus_t)

def_structure(__vk_tgui_focus_t)
    const vsf_tgui_control_t* ptCurrent;
    const vsf_tgui_control_t* ptPrevious;
end_def_structure(__vk_tgui_focus_t)

declare_vsf_pt(__vsf_tgui_evt_shooter_t)

def_vsf_pt(__vsf_tgui_evt_shooter_t,
    def_params(
        __vsf_tgui_evt_queue_t* ptQueue;
        vsf_msgt_t* ptMSGTree;
        const vsf_tgui_top_container_t * ptRootNode;
        const vsf_msgt_node_t*          ptNode;
        vsf_tgui_evt_t                  tEvent;
        const vsf_tgui_region_t*        ptRegion;
        vsf_tgui_region_t               tTempRegion;

        __vk_tgui_focus_t tActivated;

#if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
        __vk_tgui_focus_t tPointerAbove;
#endif

        struct {
            //uint8_t bIsActivatedControlChanged  : 1;
            uint8_t bIsQueueDrain               : 1;
        } tAttribute;
        
    ))
end_def_vsf_pt(__vsf_tgui_evt_shooter_t)


declare_class(vsf_tgui_t)

def_class(vsf_tgui_t,
    private_member(
        implement(vsf_msgt_t)
        implement_ex(__vsf_tgui_evt_shooter_t, tConsumer)
        __vsf_tgui_evt_queue_t tMSGQueue;
    )
)
end_def_class(vsf_tgui_t)

typedef struct vsf_tgui_cfg_t {
    vsf_mem_t tEVTQueue;

#if VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL
    vsf_mem_t tBFSQueue;
#endif

    const vsf_tgui_top_container_t* ptRootNode;
    vsf_prio_t                      tPriority;


}vsf_tgui_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


extern 
vsf_err_t vk_tgui_init(vsf_tgui_t* ptGUI, const vsf_tgui_cfg_t *ptCFG);

extern 
vsf_err_t vk_tgui_set_top_container(vsf_tgui_t* ptGUI, 
                                    vsf_tgui_top_container_t *ptRootNode);

extern 
bool vk_tgui_send_message(vsf_tgui_t *ptGUI, vsf_tgui_evt_t tEvent);

extern 
bool vk_tgui_update(vsf_tgui_t *ptGUI, 
                    const vsf_tgui_control_t *ptTarget);

extern
bool vk_tgui_update_tree(vsf_tgui_t* ptGUI,
                        const vsf_tgui_control_t* ptTarget);

extern
bool vk_tgui_send_timer_event(  vsf_tgui_t* ptGUI,
                                const vsf_tgui_control_t* ptTarget);

extern
const vsf_tgui_control_t *vsf_tgui_actived_control_get(vsf_tgui_t *ptGUI);

#if VSF_TGUI_CFG_SUPPORT_MOUSE == ENABLED
extern
const vsf_tgui_control_t *vsf_tgui_pointed_control_get(vsf_tgui_t *ptGUI);
#endif

#if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
extern
bool vk_tgui_refresh(vsf_tgui_t *ptGUI);

extern 
bool vk_tgui_refresh_ex(   vsf_tgui_t *ptGUI, 
                            const vsf_tgui_control_t *ptTarget, 
                            const vsf_tgui_region_t *ptRegion);



/*! \brief begin a refresh loop
 *! \param ptGUI the tgui object address
 *! \param ptPlannedRefreshRegion the planned refresh region
 *! \retval NULL    No need to refresh (or rendering service is not ready)
 *! \retval !NULL   The actual refresh region
 *!
 *! \note When NULL is returned, current refresh iteration (i.e. a refresh activites
 *!       between vsf_tgui_v_refresh_loop_begin and vsf_tgui_v_refresh_loop_end )
 *!       will be ignored and vsf_tgui_v_refresh_loop_end is called immediately
 */
extern
vsf_tgui_region_t* vsf_tgui_v_refresh_loop_begin(
                            vsf_tgui_t* ptGUI,
                            const vsf_tgui_region_t* ptPlannedRefreshRegion);

/*! \brief end of a refresh loop
 *! \param ptGUI the tgui object address
 *! \retval true   we have to repeat the loop
 *! \retval false  the entire refresh loop ends
 */
extern
bool vsf_tgui_v_refresh_loop_end(vsf_tgui_t* ptGUI);

extern
void vsf_tgui_low_level_refresh_ready(vsf_tgui_t *ptGUI);


/*============================ INCLUDES ======================================*/
#include "controls/vsf_tgui_controls.h"
#include "view/vsf_tgui_v.h"

#endif
#endif

#endif
/* EOF */
