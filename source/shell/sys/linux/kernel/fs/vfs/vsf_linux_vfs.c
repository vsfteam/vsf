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

#if VSF_USE_LINUX == ENABLED

#define __VSF_LINUX_FS_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/unistd.h"
#else
#   include <unistd.h>
#endif

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "../../../include/simple_libc/stdlib.h"
#   include "../../../include/simple_libc/stdio.h"
#else
#   include <stdlib.h>
#   include <stdio.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

int vsf_linux_vfs_init(void)
{
    int err;

    err = mkdirs("/var/run", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /var/run\r\n");
        return err;
    }

    err = mkdir("/var/lib", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /var/lib\r\n");
        return err;
    }

    err = mkdir("/run", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /run\r\n");
        return err;
    }

#if VSF_LINUX_USE_DEVFS == ENABLED
    err = vsf_linux_devfs_init();
    if (err != 0) {
        return err;
    }
#endif

#if VSF_LINUX_USE_TERMIOS == ENABLED
    char terminfo_path[MAX_PATH];
    int terminfo_pathlen;
    strcpy(terminfo_path, VSF_LINUX_CFG_TERMINFO_PATH "/");
    terminfo_pathlen = strlen(terminfo_path);
    terminfo_path[terminfo_pathlen + 0] = VSF_LINUX_CFG_TERMINFO_TYPE[0];
    terminfo_path[terminfo_pathlen + 1] = '\0';
    err = mkdirs(terminfo_path, 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir %s\r\n", terminfo_path);
        return err;
    }
    static const char __terminfo[] = {
#include VSF_LINUX_CFG_TERMINFO_TYPE
    };
    strcat(terminfo_path, "/" VSF_LINUX_CFG_TERMINFO_TYPE);
    vsf_linux_fs_bind_buffer(terminfo_path,
        (void *)__terminfo, VSF_FILE_ATTR_READ, sizeof(__terminfo));

#   if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
    putenv("TERM=" VSF_LINUX_CFG_TERMINFO_TYPE);
    putenv("TERMINFO=" VSF_LINUX_CFG_TERMINFO_PATH);
#   endif
#endif

    return 0;
}

#endif
