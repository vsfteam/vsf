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

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED
typedef struct vsf_linux_elfloader_t {
    vsf_elfloader_t loader;
    vsf_loader_target_t target;
} vsf_linux_elfloader_t;
#endif

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

static void * __dlmalloc(int size)
{
    return malloc((size_t)size);
}

void * dlopen(const char *pathname, int mode)
{
#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED
    FILE *f = fopen(pathname, "r");
    if (NULL == f) {
        return NULL;
    }

    vsf_linux_elfloader_t *linux_elfloader = calloc(1, sizeof(vsf_linux_elfloader_t));
    if (NULL == linux_elfloader) {
        goto close_and_fail;
    }

    linux_elfloader->loader.heap_op         = &vsf_loader_default_heap_op;
    linux_elfloader->loader.vplt            = (void *)&vsf_linux_vplt;
    linux_elfloader->loader.alloc_vplt      = __dlmalloc;
    linux_elfloader->loader.free_vplt       = free;
    linux_elfloader->target.object          = (uintptr_t)f;
    linux_elfloader->target.support_xip     = false;
    linux_elfloader->target.fn_read         = vsf_loader_stdio_read;

    if (!vsf_elfloader_load(&linux_elfloader->loader, &linux_elfloader->target)) {
        return linux_elfloader;
    }

    vsf_elfloader_cleanup(&linux_elfloader->loader);
    free(linux_elfloader);
close_and_fail:
    fclose(f);
    return NULL;
#else
    return NULL;
#endif
}

int dlclose(void *handle)
{
#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED
    vsf_linux_elfloader_t *linux_elfloader = handle;
    vsf_elfloader_cleanup(&linux_elfloader->loader);
    free(linux_elfloader);
    return 0;
#else
    return -1;
#endif
}

void * dlsym(void *handle, const char *name)
{
    void *vplt = NULL;

    if (RTLD_DEFAULT == handle) {
#if VSF_USE_APPLET == ENABLED
        vplt = (void *)&vsf_vplt;
#endif
    } else {
#if VSF_USE_APPLET == ENABLED && VSF_LINUX_USE_APPLET == ENABLED && VSF_APPLET_CFG_LINKABLE == ENABLED
        vsf_linux_elfloader_t *linux_elfloader = handle;
        vplt = (void*)linux_elfloader->loader.vplt_out;
#endif
    }
    if (NULL == vplt) {
        return NULL;
    }

#if VSF_USE_APPLET == ENABLED && VSF_LINUX_USE_APPLET == ENABLED && VSF_APPLET_CFG_LINKABLE == ENABLED
    return vsf_vplt_link(vplt, (char *)name);
#else
    return NULL;
#endif
}

char * dlerror(void)
{
    return "known";
}

void vsf_linux_glibc_init(void)
{

}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC
