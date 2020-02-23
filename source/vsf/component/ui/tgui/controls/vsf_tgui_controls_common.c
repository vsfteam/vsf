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
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

declare_class(vsf_tgui_t)
#include "./vsf_tgui_controls.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/


const i_msg_tree_node_t
c_tControlInterfaces[__VSF_TGUI_COMPONENT_TYPE_NUM - VSF_MSGT_NODE_ID_USER] = {
    __VSF_TGUI_INTERFACE_CONTROLS_CONTROL,
    __VSF_TGUI_INTERFACE_CONTROLS_CONTAINER,
    __VSF_TGUI_INTERFACE_CONTROLS_LABEL,
    __VSF_TGUI_INTERFACE_CONTROLS_BUTTON,
    __VSF_TGUI_INTERFACE_CONTROLS_PANEL,
    __VSF_TGUI_INTERFACE_CONTROLS_LIST,
    __VSF_TGUI_INTERFACE_CONTROLS_TEXT_LIST,
};


/*============================ IMPLEMENTATION ================================*/


bool vsf_tgui_region_intersect( vsf_tgui_region_t* ptRegionOut,
                                const vsf_tgui_region_t* ptRegionIn0,
                                const vsf_tgui_region_t* ptRegionIn1)
{
    vsf_tgui_location_t tLocationIn0End;
    vsf_tgui_location_t tLocationIn1End;
    vsf_tgui_location_t tLocationOutStart;
    vsf_tgui_location_t tLocationOutEnd;

    //VSF_TGUI_ASSERT(ptRegionOut != NULL);
    VSF_TGUI_ASSERT(ptRegionIn0 != NULL);
    VSF_TGUI_ASSERT(ptRegionIn1 != NULL);

    do {
        tLocationIn0End.iX = ptRegionIn0->tLocation.iX + ptRegionIn0->tSize.iWidth - 1;
        tLocationIn0End.iY = ptRegionIn0->tLocation.iY + ptRegionIn0->tSize.iHeight - 1;

        tLocationIn1End.iX = ptRegionIn1->tLocation.iX + ptRegionIn1->tSize.iWidth - 1;
        tLocationIn1End.iY = ptRegionIn1->tLocation.iY + ptRegionIn1->tSize.iHeight - 1;

        tLocationOutStart.iX = max(ptRegionIn0->tLocation.iX, ptRegionIn1->tLocation.iX);
        tLocationOutStart.iY = max(ptRegionIn0->tLocation.iY, ptRegionIn1->tLocation.iY);

        tLocationOutEnd.iX = min(tLocationIn0End.iX, tLocationIn1End.iX);
        tLocationOutEnd.iY = min(tLocationIn0End.iY, tLocationIn1End.iY);

        if ((tLocationOutStart.iX > tLocationOutEnd.iX) || (tLocationOutStart.iY > tLocationOutEnd.iY)) {
            return false;
        }

        if (NULL != ptRegionOut) {
            ptRegionOut->tLocation = tLocationOutStart;
            ptRegionOut->tSize.iWidth = tLocationOutEnd.iX - tLocationOutStart.iX + 1;
            ptRegionOut->tSize.iHeight = tLocationOutEnd.iY - tLocationOutStart.iY + 1;
        }
    } while(0);

    return true;
}


vsf_tgui_location_t * vsf_tgui_region_get_relative_location(vsf_tgui_location_t *ptOutLocation,
                                                            vsf_tgui_location_t *ptReferenceLocation,
                                                            vsf_tgui_location_t *ptAbsoluteLocation)
{
    VSF_TGUI_ASSERT(ptOutLocation != NULL);
    VSF_TGUI_ASSERT(ptReferenceLocation != NULL);
    VSF_TGUI_ASSERT(ptAbsoluteLocation != NULL);

    ptOutLocation->iX = ptAbsoluteLocation->iX - ptReferenceLocation->iX;
    ptOutLocation->iY = ptAbsoluteLocation->iY - ptReferenceLocation->iY;

    return ptOutLocation;
}

vsf_tgui_region_t * vsf_tgui_region_get_relative_region(vsf_tgui_region_t *ptOutRegion,
                                                        vsf_tgui_region_t *ptReferenceRegion,
                                                        vsf_tgui_region_t *ptAbsoluteRegion)
{
    VSF_TGUI_ASSERT(ptOutRegion != NULL);
    VSF_TGUI_ASSERT(ptReferenceRegion != NULL);
    VSF_TGUI_ASSERT(ptAbsoluteRegion != NULL);

    vsf_tgui_region_get_relative_location(  &ptOutRegion->tLocation,
                                            &ptReferenceRegion->tLocation,
                                            &ptAbsoluteRegion->tLocation);
    return ptOutRegion;
}


#endif


/* EOF */
