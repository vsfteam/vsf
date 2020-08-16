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
#include "utilities/vsf_utilities_cfg.h"
#include "code_region.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
static void __default_code_region_none_on_enter(void *obj_ptr, void *local_ptr);
static void __default_code_region_none_on_leave(void *obj_ptr,void *local_ptr);

/*============================ LOCAL VARIABLES ===============================*/

static const i_code_region_t __vsf_i_default_code_region_none = {
    0,
    &__default_code_region_none_on_enter,
    &__default_code_region_none_on_leave
};

/*============================ GLOBAL VARIABLES ==============================*/
const code_region_t DEFAULT_CODE_REGION_NONE = {
    NULL,
    (i_code_region_t *)&__vsf_i_default_code_region_none,
};

/*============================ IMPLEMENTATION ================================*/

static void __default_code_region_none_on_enter(void *obj_ptr, void *local_ptr)
{
    UNUSED_PARAM(obj_ptr);
    UNUSED_PARAM(local_ptr);
}

static void __default_code_region_none_on_leave(void *obj_ptr,void *local_ptr)
{
    UNUSED_PARAM(obj_ptr);
    UNUSED_PARAM(local_ptr);
}

/* EOF */

