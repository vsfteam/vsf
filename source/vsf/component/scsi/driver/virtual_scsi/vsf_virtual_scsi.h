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

#ifndef __VSF_VIRTUAL_SCSI_H__
#define __VSF_VIRTUAL_SCSI_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_scsi_cfg.h"

#if VSF_USE_SCSI == ENABLED && VSF_USE_VIRTUAL_SCSI == ENABLED

#if     defined(VSF_VIRTUAL_SCSI_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_VIRTUAL_SCSI_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_virtual_scsi_t)
declare_simple_class(vk_virtual_scsi_drv_t)

enum vsf_virtual_scsi_drv_type_t {
    VSF_VIRTUAL_SCSI_DRV_NORMAL,
    VSF_VIRTUAL_SCSI_DRV_PARAM_SUBCALL,
};
typedef enum vsf_virtual_scsi_drv_type_t vsf_virtual_scsi_drv_type_t;

def_simple_class(vk_virtual_scsi_drv_t) {
    protected_member(
        uint8_t drv_type        : 4;
        uint8_t feature         : 4;
        union {
            struct {
                bool (*buffer)(vk_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem);
                void (*init)(void);
                void (*read)(void);
                void (*write)(void);
            };
            struct {
                bool (*buffer)(vk_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem);
                vsf_err_t (*init)(vk_scsi_t *scsi);
                int_fast32_t (*read)(vk_scsi_t *scsi, uint_fast64_t addr, uint_fast32_t size);
                int_fast32_t (*write)(vk_scsi_t *scsi, uint_fast64_t addr, uint_fast32_t size);
            } normal;
            struct {
                bool (*buffer)(vk_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem);
                vsf_peda_evthandler_t init;
                vsf_peda_evthandler_t read;
                vsf_peda_evthandler_t write;
            } param_subcall;
        };
    )
};

enum scsi_pdt_t {
    SCSI_PDT_DIRECT_ACCESS_BLOCK                = 0x00,
    SCSI_PDT_CD_DVD                             = 0x05,
};
typedef enum scsi_pdt_t scsi_pdt_t;

struct vk_virtual_scsi_param_t {
    uint32_t block_size;
    uint32_t block_num;
    char vendor[8];
    char product[16];
    char revision[4];
    bool removable;
    scsi_pdt_t type;
};
typedef struct vk_virtual_scsi_param_t vk_virtual_scsi_param_t;

def_simple_class(vk_virtual_scsi_t) {
    implement(vk_scsi_t)
    public_member(
        const vk_virtual_scsi_drv_t *virtual_scsi_drv;
    )

    protected_member(
        union {
            uint8_t reply[36];
        };

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        bool is_stream;
#endif
        scsi_sensekey_t sense_key;
        scsi_asc_t asc;
    )
};

#if defined(VSF_VIRTUAL_SCSI_IMPLEMENT) || defined(VSF_VIRTUAL_SCSI_INHERIT)
__vsf_component_peda_ifs(vk_virtual_scsi_init)
__vsf_component_peda_ifs(vk_virtual_scsi_read,
    uint64_t addr;
    uint32_t size;
    void *mem_stream;
)
__vsf_component_peda_ifs(vk_virtual_scsi_write,
    uint64_t addr;
    uint32_t size;
    void *mem_stream;
)
#endif

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_scsi_drv_t vk_virtual_scsi_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#undef VSF_VIRTUAL_SCSI_IMPLEMENT
#undef VSF_VIRTUAL_SCSI_INHERIT

#endif      // VSF_USE_SCSI && VSF_USE_VIRTUAL_SCSI
#endif      // __VSF_MAL_SCSI_H__
