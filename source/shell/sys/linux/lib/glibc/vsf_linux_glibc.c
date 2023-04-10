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

#if VSF_USE_LOADER == ENABLED
typedef struct vsf_linux_dynloader_t {
    union {
#if VSF_LOADER_USE_ELF == ENABLED
        vsf_elfloader_t elfloader;
#endif
#if VSF_LOADER_USE_PE == ENABLED
        vsf_peloader_t peloader;
#endif
        vsf_loader_t generic;
    } loader;
    vsf_loader_target_t target;
} vsf_linux_dynloader_t;
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
#if VSF_USE_LOADER == ENABLED
    FILE *f = fopen(pathname, "r");
    if (NULL == f) {
        return NULL;
    }

    vsf_linux_dynloader_t *linux_loader = calloc(1, sizeof(vsf_linux_dynloader_t));
    if (NULL == linux_loader) {
        goto close_and_fail;
    }

    linux_loader->loader.generic.heap_op    = &vsf_loader_default_heap_op;
    linux_loader->loader.generic.vplt       = (void *)&vsf_linux_vplt;
    linux_loader->loader.generic.alloc_vplt = __dlmalloc;
    linux_loader->loader.generic.free_vplt  = free;
    linux_loader->target.object             = (uintptr_t)f;
    linux_loader->target.support_xip        = false;
    linux_loader->target.fn_read            = vsf_loader_stdio_read;

    uint8_t header[16];
    uint32_t size = vsf_loader_read(&linux_loader->target, 0, header, sizeof(header));
#if VSF_LOADER_USE_PE == ENABLED
    if ((size >= 2) && (header[0] == 'M') && (header[1] == 'Z')) {
        linux_loader->loader.generic.op     = &vsf_peloader_op;
    } else
#endif
    if ((size >= 4) && (header[0] == 0x7F) && (header[1] == 'E') && (header[2] == 'L') && (header[3] == 'F')) {
        linux_loader->loader.generic.op     = &vsf_elfloader_op;
    } else {
        printf("unsupported file format\n");
        goto close_and_fail;
    }

    if (!vsf_loader_load(&linux_loader->loader.generic, &linux_loader->target)) {
        vsf_loader_call_init_array(&linux_loader->loader.generic);
        return linux_loader;
    }

    vsf_loader_cleanup(&linux_loader->loader.generic);
    free(linux_loader);

close_and_fail:
    fclose(f);
    return NULL;
#else
    return NULL;
#endif
}

int dlclose(void *handle)
{
#if VSF_USE_LOADER == ENABLED
    vsf_linux_dynloader_t *linux_loader = handle;
    vsf_loader_call_fini_array(&linux_loader->loader.generic);
    vsf_loader_cleanup(&linux_loader->loader.generic);
    free(linux_loader);
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
        vsf_linux_dynloader_t *linux_loader = handle;
        vplt = (void*)linux_loader->loader.generic.vplt_out;
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
