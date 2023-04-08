#include "shell/sys/linux/vsf_linux_cfg.h"

#if     VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED              \
    &&  VSF_USE_LINUX == ENABLED && APP_USE_LINUX_ELFLOADER_DEMO == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
// Elf related constants, types and structures
#define __VSF_ELFLOADER_CLASS_INHERIT__
#include <unistd.h>
#include <stdio.h>

#if defined(__WIN__) || defined(__LINUX__)
#   define ELFLOADER_CFG_STDIO
#endif

#if VSF_APPLET_CFG_ABI_PATCH == ENABLED
static int pls_applet_ctx = -1;
vsf_applet_ctx_t * vsf_applet_ctx(void)
{
    vsf_linux_localstorage_t *pls;
    if ((pls_applet_ctx < 0) || (NULL == (pls = vsf_linux_pls_get(pls_applet_ctx)))) {
        return NULL;
    }
    return (vsf_applet_ctx_t *)pls->data;
}
int vsf_vplt_init_array(void *target)
{
    return vsf_elfloader_call_init_array(target);
}
void vsf_vplt_fini_array(void *target)
{
    vsf_elfloader_call_fini_array(target);
}
#endif

int elfloader_main(int argc, char **argv)
{
    if (argc < 2) {
        printf("format: %s ADDRESS/FILE [ARGUS]\n", argv[0]);
        return -1;
    }

#ifdef ELFLOADER_CFG_STDIO
    FILE *f = fopen(argv[1], "r");
    if (NULL == f) {
        printf("fail to open %s\n", argv[1]);
        return -1;
    }
#endif

    vsf_elfloader_t elfloader = {
        .heap_op        = &vsf_loader_default_heap_op,
        .vplt           = (void *)&vsf_linux_vplt,
    };
    vsf_loader_target_t elftarget = {
#ifdef ELFLOADER_CFG_STDIO
        .object         = (uintptr_t)f,
        .support_xip    = false,
        .fn_read        = vsf_loader_stdio_read,
#else
        .object         = strtoul((const char *)argv[1], NULL, 0),
        .support_xip    = true,
        .fn_read        = vsf_loader_xip_read,
#endif
    };

    int result = -1;
    if (!vsf_elfloader_load(&elfloader, &elftarget) && (elfloader.entry != NULL)) {
        vsf_applet_ctx_t applet_ctx = {
            .target     = &elfloader,
            .fn_init    = (int (*)(void *))vsf_elfloader_call_init_array,
            .fn_fini    = (void (*)(void *))vsf_elfloader_call_fini_array,
            .argc       = argc - 1,
            .argv       = argv + 1,
            .vplt       = (void *)&vsf_linux_vplt,
        };

#if VSF_APPLET_CFG_ABI_PATCH == ENABLED
        if (pls_applet_ctx < 0) {
            pls_applet_ctx = vsf_linux_pls_alloc();
        }
        VSF_LINUX_ASSERT(pls_applet_ctx >= 0);
        vsf_linux_localstorage_t *pls = vsf_linux_pls_get(pls_applet_ctx);
        VSF_LINUX_ASSERT(pls != NULL);
        pls->data = &applet_ctx;

        vsf_linux_set_process_reg((uintptr_t)elfloader.static_base);
        result = ((int (*)(void))elfloader.entry)();
#else
        vsf_linux_set_process_reg((uintptr_t)elfloader.static_base);
        result = ((int (*)(vsf_applet_ctx_t*))elfloader.entry)(&applet_ctx);
#endif
        vsf_elfloader_cleanup(&elfloader);
    } else {
        printf("fail to parse %s or no entry found\n", argv[1]);
    }

#ifdef ELFLOADER_CFG_STDIO
    if (f != NULL) {
        fclose(f);
    }
#endif

    return result;
}

#endif