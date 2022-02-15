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

#ifndef __VSF_LITTLEFS_H__
#define __VSF_LITTLEFS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_FS_USE_LITTLEFS == ENABLED

#include "kernel/vsf_kernel.h"
#include "lfs.h"

#if     defined(__VSF_LITTLEFS_CLASS_IMPLEMENT)
#   undef __VSF_LITTLEFS_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
#   error littlefs runs in thread mode, please enable VSF_KERNEL_CFG_SUPPORT_THREAD
#endif

#ifndef VSF_LITTLEFS_CFG_STACK_SIZE
// any comment value for the default stack size? 4K should be enough
#   define VSF_LITTLEFS_CFG_STACK_SIZE              4096
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vk_lfs_info_t)

vsf_class(vk_lfs_file_t) {
    public_member(
        implement(vk_file_t)
    )
    private_member(
        union {
            lfs_file_t lfs_file;
            lfs_dir_t lfs_dir;
        };
        vk_lfs_info_t *info;
    )
};

vsf_class(vk_lfs_info_t) {
    public_member(
        struct lfs_config config;
    )

    private_member(
        lfs_t lfs;

        vsf_crit_t crit;
        vsf_sem_t sem;
        vsf_thread_t thread;
        vsf_thread_cb_t thread_cb;
        uint64_t thread_stack[(__VSF_THREAD_STACK_SAFE_SIZE(VSF_LITTLEFS_CFG_STACK_SIZE) + 7 ) / 8]\
                ALIGN(1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT);

        int op;
        union {
            struct {
                vk_lfs_file_t *dir;
                const char *name;
            } open;
            struct {
                lfs_file_t *file;
            } close_file;
            struct {
                lfs_dir_t *dir;
            } close_dir;
            struct {
                lfs_file_t *file;
                void *buffer;
                lfs_size_t size;
            } read_file;
            struct {
                lfs_file_t *file;
                void *buffer;
                lfs_size_t size;
            } write_file;
            struct {
                uint64_t offset;
            } setpos;
        } param;
        struct {
            int ret;
            union {
                struct {
                    vk_lfs_file_t *file;
                } open;
                struct {
                    lfs_ssize_t size;
                } read_file;
                struct {
                    lfs_ssize_t size;
                } write_file;
            };
        } result;
        vsf_eda_t *caller;

        vk_lfs_file_t root;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_fs_op_t vk_lfs_op;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_FS && VSF_FS_USE_LITTLEFS
#endif      // __VSF_LITTLEFS_H__
