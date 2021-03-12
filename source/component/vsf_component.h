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

#ifndef __VSF_COMPONENT_H__
#define __VSF_COMPONENT_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_component_cfg.h"

#include "./crypto/vsf_crypto.h"
#include "./mal/vsf_mal.h"
#include "./scsi/vsf_scsi.h"
#include "./fs/vsf_fs.h"
#include "./av/vsf_av.h"
#include "./input/vsf_input.h"
#include "./usb/vsf_usb.h"
#include "./tcpip/vsf_tcpip.h"
#include "./ui/vsf_ui.h"
#include "./debugger/vsf_debugger.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_component_init(void);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
