#include "shell/sys/linux/vsf_linux_cfg.h"
#if VSF_USE_LINUX == ENABLED && APP_USE_LINUX_HTTPD_DEMO == ENABLED

#include <unistd.h>
#include <stdio.h>

#include "shell/sys/linux/app/httpd/vsf_linux_httpd.h"

static const vsf_linux_httpd_urihandler_t __user_httpd_urihandler[] = {
    {
        .match              = VSF_LINUX_HTTPD_URI_MATCH_URI,
        .uri                = "/",
        .type               = VSF_LINUX_HTTPD_URI_REMAP,
        .target_uri         = "/index.html",
    },
};

int httpd_main(int argc, char *argv[])
{
    static vsf_linux_httpd_t __user_httpd = {
        .root_path          = APP_LINUX_HTTPD_DEMO_CFG_ROOT,
        .port               = 80,
        .backlog            = 1,

        .num_of_urihandler  = dimof(__user_httpd_urihandler),
        .urihandler         = __user_httpd_urihandler,
    };
    vsf_linux_httpd_start(&__user_httpd);
    return 0;
}
#endif
