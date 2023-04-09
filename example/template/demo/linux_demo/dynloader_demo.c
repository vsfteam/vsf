#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LOADER == ENABLED && VSF_USE_LINUX == ENABLED && APP_USE_LINUX_DYNLOADER_DEMO == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
// Elf related constants, types and structures
#define __VSF_ELFLOADER_CLASS_INHERIT__
#include <unistd.h>
#include <stdio.h>

#if defined(__WIN__) || defined(__LINUX__)
#   define LOADER_DEMO_CFG_STDIO
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
    return vsf_loader_call_init_array((vsf_loader_t *)target);
}
void vsf_vplt_fini_array(void *target)
{
    vsf_loader_call_fini_array((vsf_loader_t *)target);
}
#endif

int dynloader_main(int argc, char **argv)
{
    if (argc < 2) {
        printf("format: %s ADDRESS/FILE [ARGUS]\n", argv[0]);
        return -1;
    }

#ifdef LOADER_DEMO_CFG_STDIO
    FILE *f = fopen(argv[1], "r");
    if (NULL == f) {
        printf("fail to open %s\n", argv[1]);
        return -1;
    }
#endif

    union {
#if VSF_LOADER_USE_ELF == ENABLED
        vsf_elfloader_t elfloader;
#endif
#if VSF_LOADER_USE_PE == ENABLED
        vsf_peloader_t peloader;
#endif
        vsf_loader_t generic;
    } loader = { 0 };
    loader.generic.heap_op  = &vsf_loader_default_heap_op;
    loader.generic.vplt     = (void *)&vsf_linux_vplt;

    int result = -1;
    uint8_t header[16];
    size_t size = fread(header, 16, 1, f);
#if VSF_LOADER_USE_PE == ENABLED
    if ((size >= 2) && (header[0] == 'M') && (header[1] == 'Z')) {
        loader.generic.op = &vsf_peloader_op;
    } else
#endif
    if ((size >= 4) && (header[0] == 0x7F) && (header[1] == 'E') && (header[2] == 'L') && (header[3] == 'F')) {
        loader.generic.op = &vsf_elfloader_op;
    } else {
        printf("unsupported file format\n");
        goto close_and_exit;
    }
    rewind(f);

    vsf_loader_target_t target = {
#ifdef LOADER_DEMO_CFG_STDIO
        .object         = (uintptr_t)f,
        .support_xip    = false,
        .fn_read        = vsf_loader_stdio_read,
#else
        .object         = strtoul((const char *)argv[1], NULL, 0),
        .support_xip    = true,
        .fn_read        = vsf_loader_xip_read,
#endif
    };

    if (!vsf_loader_load(&loader.generic, &target) && (loader.generic.entry != NULL)) {
        vsf_applet_ctx_t applet_ctx = {
            .target     = &loader.generic,
            .fn_init    = (int (*)(void *))vsf_loader_call_init_array,
            .fn_fini    = (void (*)(void *))vsf_loader_call_fini_array,
            .argc       = argc - 1,
            .argv       = argv + 1,
            .vplt       = loader.generic.vplt,
        };

#if VSF_APPLET_CFG_ABI_PATCH == ENABLED
        if (pls_applet_ctx < 0) {
            pls_applet_ctx = vsf_linux_pls_alloc();
        }
        VSF_LINUX_ASSERT(pls_applet_ctx >= 0);
        vsf_linux_localstorage_t *pls = vsf_linux_pls_get(pls_applet_ctx);
        VSF_LINUX_ASSERT(pls != NULL);
        pls->data = &applet_ctx;

        vsf_linux_set_process_reg((uintptr_t)loader.generic.static_base);
        result = ((int (*)(void))loader.generic.entry)();
#else
        vsf_linux_set_process_reg((uintptr_t)loader.generic.static_base);
        result = ((int (*)(vsf_applet_ctx_t*))loader.generic.entry)(&applet_ctx);
#endif
        vsf_loader_cleanup(&loader.generic);
    } else {
        printf("fail to parse %s or no entry found\n", argv[1]);
    }

close_and_exit:
#ifdef LOADER_DEMO_CFG_STDIO
    if (f != NULL) {
        fclose(f);
    }
#endif

    return result;
}

#endif