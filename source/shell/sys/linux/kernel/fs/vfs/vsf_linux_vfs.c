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

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/unistd.h"
#else
#   include <unistd.h>
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

    err = mkdir("/var", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /var\r\n");
        return err;
    }

    err = mkdir("/var/run", 0);
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
    err = mkdir("/usr", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /usr\r\n");
        return err;
    }

    err = mkdir("/usr/share", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /usr/share\r\n");
        return err;
    }
#endif

#if VSF_LINUX_USE_TERMIOS == ENABLED
    putenv("TERM=xtem");
    // TODO: implement terminfo at /usr/share/terminfo
#endif

    return 0;
}

#endif
