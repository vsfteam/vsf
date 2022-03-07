#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && APP_USE_LINUX_TELNETD_DEMO == ENABLED
#include "shell/sys/linux/app/telnetd/vsf_linux_telnetd.h"

int telnetd_main(int argc, char *argv[])
{
    vsf_linux_telnetd_t telnetd = {
        .port       = 23,
        .backlog    = 1,
    };
    vsf_linux_telnetd_thread(&telnetd);
    return 0;
}
#endif
