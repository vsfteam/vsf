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

/*============================ INCLUDES ======================================*/

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_DEVFS == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/sys/stat.h"
#else
#   include <unistd.h>
#   include <sys/stat.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "shell/sys/linux/include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int __vsf_linux_create_open_path(char *path);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_DEVFS_USE_RAND == ENABLED
__vsf_component_peda_ifs_entry(__vk_devfs_rand_read, vk_file_read)
{
    vsf_peda_begin();

    uint_fast32_t size = vsf_local.size;
    uint8_t *buff = vsf_local.buff;

    // TODO: use hal trng instead of rand
    for (uint_fast32_t i = 0; i < size; i++) {
        buff[i] = (uint8_t)rand();
    }
    vsf_eda_return(size);
    vsf_peda_end();
}

int vsf_linux_fs_bind_rand(char *path)
{
    int err = vsf_linux_fs_bind_target_ex(path, NULL,
            (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_rand_read), NULL,
            VSF_FILE_ATTR_READ, (uint64_t)-1);
    if (!err) {
        printf("%s bound.\r\n", path);
    }
    return err;
}
#endif

#if VSF_USE_MAL == ENABLED
#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif
__vsf_component_peda_ifs_entry(__vk_devfs_mal_read, vk_file_read)
{
    vsf_peda_begin();

    // GCC: -Wcast-align
    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)&vsf_this;
    vk_mal_t *mal = vfs_file->f.param;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_mal_read(mal, vfs_file->pos, vsf_local.size, vsf_local.buff);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_devfs_mal_write, vk_file_write)
{
    vsf_peda_begin();

    // GCC: -Wcast-align
    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)&vsf_this;
    vk_mal_t *mal = vfs_file->f.param;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_mal_write(mal, vfs_file->pos, vsf_local.size, vsf_local.buff);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }

    vsf_peda_end();
}
#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

int vsf_linux_fd_bind_mal(char *path, vk_mal_t *mal)
{
    return vsf_linux_fs_bind_target_ex(path, mal,
                (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_mal_read),
                (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_mal_write),
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_FILE_ATTR_BLK, mal->size);
}
#endif

__vsf_component_peda_ifs_entry(__vk_devfs_null_write, vk_file_write)
{
    vsf_peda_begin();
    vsf_eda_return(vsf_local.size);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_devfs_zero_read, vk_file_read)
{
    vsf_peda_begin();
    memset(vsf_local.buff, 0, vsf_local.size);
    vsf_eda_return(vsf_local.size);
    vsf_peda_end();
}

int vsf_linux_devfs_init(void)
{
    int err = mkdir("/dev", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /dev\r\n");
        return err;
    }

    err = vsf_linux_fs_bind_target_ex("dev/null", NULL,
                NULL, (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_null_write),
                VSF_FILE_ATTR_WRITE, 0);
    if (err != 0) {
        fprintf(stderr, "fail to bind /dev/null\r\n");
        return err;
    }

    err = vsf_linux_fs_bind_target_ex("dev/zero", NULL,
                (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_zero_read), NULL,
                VSF_FILE_ATTR_READ, 0);
    if (err != 0) {
        fprintf(stderr, "fail to bind /dev/zero\r\n");
        return err;
    }

#if VSF_LINUX_DEVFS_USE_RAND == ENABLED
    // TODO: use hal trng instead of rand
    srand(vsf_systimer_get_tick());
    vsf_linux_fs_bind_rand("/dev/random");
    vsf_linux_fs_bind_rand("/dev/urandom");
#endif
    return 0;
}

#endif
