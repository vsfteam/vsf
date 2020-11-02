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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_RTP_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_RTP_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

#if     defined(__VSF_RTP_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_RTP_CLASS_IMPLEMENT
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vsf_rtp_t)
def_simple_class(vsf_rtp_t) {
    public_member(
        // todo: inherit vsf_rtp_common_t, which include bitlen
        uint8_t bitlen;
    )
    private_member(
        struct tsfilter_t * filter;
        int x, y;
        bool is_down;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_rtp_t VSF_RTP0;

/*============================ PROTOTYPES ====================================*/

// todo: remove cal, calibration is not done in driver layer
extern void vsf_rtp_init(vsf_rtp_t *rtp, void *cal);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */