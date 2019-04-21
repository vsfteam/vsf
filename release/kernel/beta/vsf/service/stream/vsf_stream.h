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

#ifndef __VSF_SERVICE_STREAM_H__
#define __VSF_SERVICE_STREAM_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#include "./pbuf/vsf_pbuf.h"
#include "./pbuf/vsf_pbuf_pool.h"


#if VSF_USE_SERVICE_STREAM == ENABLED
#include "./vsf_stream_base.h"
#include "./vsf_stream_writer.h"
#include "./vsf_stream_reader.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_service_stream_init(void);

#endif
#endif
/* EOF */
