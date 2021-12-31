/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#ifndef __VSF_USR_CFG_ARCH_H__
#define __VSF_USR_CFG_ARCH_H__

/*============================ INCLUDES ======================================*/

#if     defined(__M484__)
#   include "./arch/vsf_usr_cfg_m484.h"
#elif   defined(__F1C100S__)
#   include "./arch/vsf_usr_cfg_f1c100s.h"
#elif   defined(__WIN__)
#   include "./arch/vsf_usr_cfg_win.h"
#elif   defined(__LINUX__)
#   include "./arch/vsf_usr_cfg_linux.h"
#elif   defined(__GD32E103__)
#   include "./arch/vsf_usr_cfg_gd32e103.h"
#elif   defined(__MPS2__)
#   include "./arch/vsf_usr_cfg_mps2.h"
#elif   defined(__AIC8800__)
#   include "./arch/vsf_usr_cfg_aic8800.h"
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
