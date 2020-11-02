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

#define __VSF_EDA_CLASS_INHERIT__
#include "vsf.h"

#if     VSF_USE_USB_HOST == ENABLED                                             \
    &&  VSF_USE_SCSI == ENABLED                                                 \
    &&  VSF_SCSI_USE_MAL_SCSI == ENABLED                                        \
    &&  APP_USE_SCSI_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_scsi_mounter_t {
    implement(vk_scsi_mal_t)
    implement(vk_malfs_mounter_t)
    vsf_eda_t eda;
    enum {
        STATE_INIT,
        STATE_OPEN_DIR,
        STATE_MBR_MOUNT,
    } state;
    bool is_mounted;
} vsf_scsi_mounter_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vsf_scsi_mounter_t __usr_scsi_mounter;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __user_scsi_mounter(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_scsi_mounter_t *mounter = container_of(eda, vsf_scsi_mounter_t, eda);

    switch (evt) {
    case VSF_EVT_INIT:
        mounter->state = STATE_INIT;
        vk_mal_init(&mounter->use_as__vk_mal_t);
        break;
    case VSF_EVT_RETURN:
        switch (mounter->state) {
        case STATE_INIT:
            if (vsf_eda_get_return_value() != VSF_ERR_NONE) {
                break;
            }
            mounter->state = STATE_OPEN_DIR;
            vk_file_open(NULL, "/scsi", 0, &mounter->dir);
            break;
        case STATE_OPEN_DIR:
            mounter->mal = &mounter->use_as__vk_mal_t;
            mounter->state = STATE_MBR_MOUNT;
            vk_malfs_mount_mbr(&mounter->use_as__vk_malfs_mounter_t);
            break;
        case STATE_MBR_MOUNT:
            break;
        }
        break;
    }
}

void vsf_scsi_on_new(vk_scsi_t *scsi)
{
    if (!__usr_scsi_mounter.is_mounted) {
        __usr_scsi_mounter.scsi = scsi;
        __usr_scsi_mounter.drv = &vk_scsi_mal_drv;
        __usr_scsi_mounter.eda.fn.evthandler = __user_scsi_mounter;
        vsf_eda_init(&__usr_scsi_mounter.eda, vsf_prio_0, false);
    }
}

void vsf_scsi_on_delete(vk_scsi_t *scsi)
{
    if (scsi == __usr_scsi_mounter.scsi) {
        __usr_scsi_mounter.is_mounted = false;
    }
}

#endif
