#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && APP_USE_LINUX_NTPDATE_DEMO == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/netdb.h"
#else
#   include <unistd.h>
#   include <netdb.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "shell/sys/linux/include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif

#include "shell/sys/linux/app/ntp/vsf_linux_ntp.h"

int ntpdate_main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("format: %s NTP_SERVER\n", argv[0]);
        return -1;
    }

    struct hostent *host = gethostbyname(argv[1]);
    if (NULL == host) {
        printf("fail to resolve %s\n", argv[1]);
        return -1;
    }

    vsf_linux_ntp_rtc_t ntp_rtc = {
        .timeout_ms             = 5000,
        .host_addr              = {
            .sin_family         = AF_INET,
            .sin_port           = htons(123),
            .sin_addr.s_addr    = *(in_addr_t *)host->h_addr,
        },
    };
    vsf_rtc_tm_t rtc_tm;

    vsf_linux_ntp_rtc_init((vsf_rtc_t *)&ntp_rtc, NULL);
    vsf_linux_ntp_rtc_enable((vsf_rtc_t *)&ntp_rtc);
    if (VSF_ERR_NONE == vsf_linux_ntp_rtc_get((vsf_rtc_t *)&ntp_rtc, &rtc_tm)) {
#ifdef VSF_LINUX_CFG_RTC
        vsf_rtc_set(&VSF_LINUX_CFG_RTC, &rtc_tm);
#endif

        struct tm t = {
            .tm_sec     = rtc_tm.tm_sec,
            .tm_min     = rtc_tm.tm_min,
            .tm_hour    = rtc_tm.tm_hour,
            .tm_mday    = rtc_tm.tm_mday,
            .tm_mon     = rtc_tm.tm_mon,
            .tm_year    = rtc_tm.tm_year - 1900,
        };
        puts(asctime(&t));
        return 0;
    }

    printf("fail to get reply from %s\n", argv[1]);
    return -1;
}
#endif
