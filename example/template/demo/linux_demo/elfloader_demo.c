#include "shell/sys/linux/vsf_linux_cfg.h"

#if     VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED              \
    &&  VSF_USE_LINUX == ENABLED && APP_USE_LINUX_ELFLOADER_DEMO == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#include <unistd.h>
#include <stdio.h>

int elfloader_main(int argc, char **argv)
{
    if (argc != 2) {
        printf("format: %s ADDRESS\n", argv[0]);
        return -1;
    }

    vsf_elfloader_t elfloader = {
        .heap_op    = &vsf_loader_default_heap_op,
    };
    vsf_loader_target_t elftarget = {
        .object     = strtoul((const char *)argv[1], NULL, 0),
        .is_xip     = true,
        .fn_read    = vsf_loader_xip_read,
    };
    elftarget.is_epi = 0 == vsf_elfloader_get_section(&elfloader, &elftarget, ".got", NULL);

    void *entry = vsf_elfloader_load(&elfloader, &elftarget);
    if (entry != NULL) {
        vsf_linux_set_process_reg((uintptr_t)elfloader.static_base);
        int result = ((int (*)(int, char **, vsf_applet_ctx_t*))entry)(argc, argv, &(vsf_applet_ctx_t) {
            .target     = &elfloader,
            .fn_init    = (int (*)(void *))vsf_elfloader_call_init_array,
            .fn_fini    = (void (*)(void *))vsf_elfloader_call_fini_array,
            .vplt       = &vsf_linux_vplt,
        });
        vsf_elfloader_cleanup(&elfloader);
        return result;
    }
    return -1;
}

#endif
