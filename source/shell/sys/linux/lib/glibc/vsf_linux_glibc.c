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

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/errno.h"
#   include "../../include/simple_libc/mntent.h"
#   include "../../include/simple_libc/dlfcn.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <mntent.h>
#   include <dlfcn.h>
#endif
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// mntent

FILE * setmntent(const char *filename, const char *type)
{
    VSF_LINUX_ASSERT(false);
    return (FILE *)NULL;
}

struct mntent * getmntent(FILE *stream)
{
    VSF_LINUX_ASSERT(false);
    return (struct mntent *)NULL;
}

int addmntent(FILE *stream, const struct mntent *mnt)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int endmntent(FILE *stream)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

char * hasmntopt(const struct mntent *mnt, const char *opt)
{
    VSF_LINUX_ASSERT(false);
    return (char *)NULL;
}

struct mntent * getmntent_r(FILE *stream, struct mntent *mntbuf, char *buf, int buflen)
{
    VSF_LINUX_ASSERT(false);
    return (struct mntent *)NULL;
}

// dlfcn

void * dlsym(void *handle, const char *name)
{
    if (RTLD_DEFAULT == handle) {
#if VSF_USE_APPLET == ENABLED && VSF_LINUX_USE_APPLET == ENABLED && VSF_APPLET_CFG_LINKABLE == ENABLED
        return vsf_vplt_link((void *)&vsf_vplt, (char *)name);
#else
        return NULL;
#endif
    } else {
        // TODO: load from elf
        return NULL;
    }
}

void vsf_linux_glibc_init(void)
{

}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC
