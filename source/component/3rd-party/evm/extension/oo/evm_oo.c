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

#include "vsf_cfg.h"

#if VSF_USE_EVM == ENABLED

#include "evm_module.h"
#include "./evm_oo.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint32_t __evm_class_get_vt_len(const evm_class_t *c)
{
    uint_fast16_t vt_len = 0;
    for (; c != NULL; c = c->parent) {
        vt_len += c->vt_len;
    }
    return vt_len;
}

static void __evm_instance_create(evm_t * e, evm_val_t *obj, const evm_class_t *c)
{
    if (c != NULL) {
        const evm_class_vt_t *vt = c->vt;
        if (vt != NULL) {
            evm_val_t val;
            for (uint32_t i = 0; i < c->vt_len; i++, vt++) {
                if (NULL == evm_prop_get(e, obj, vt->name, 0)) {
                    switch (vt->type) {
                    case EVM_CLASS_VTT_METHOD:
                        val = evm_mk_native((intptr_t)vt->v.fn);
                        break;
                    case EVM_CLASS_VTT_INTEGER:
                        val = evm_mk_number(vt->v.num_int);
                        break;
                    case EVM_CLASS_VTT_INSTANCE:
                    case EVM_CLASS_VTT_STRUCT:
                        val = EVM_VAL_UNDEFINED;
                        break;
                    }
                    evm_prop_append(e, obj, vt->name, val);
                }
            }
        }
    }
    __evm_instance_create(e, obj, c->parent);
}

evm_val_t * evm_instance_create(evm_t * e, const evm_class_t *c)
{
    evm_val_t *obj = evm_object_create(e, GC_OBJECT, __evm_class_get_vt_len(c), 0);
    if (obj != NULL) {
        __evm_instance_create(e, obj, c);
    }
    return obj;
}

#endif      // VSF_USE_EVM
