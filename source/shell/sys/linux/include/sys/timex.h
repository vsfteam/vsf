#ifndef __VSF_LINUX_SYS_TIMEX_H__
#define __VSF_LINUX_SYS_TIMEX_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ADJ_OFFSET              0x0001
#define ADJ_FREQUENCY           0x0002
#define ADJ_MAXERROR            0x0004
#define ADJ_ESTERROR            0x0008
#define ADJ_STATUS              0x0010
#define ADJ_TIMECONST           0x0020
#define ADJ_TICK                0x4000
#define ADJ_OFFSET_SINGLESHOT   0x8001

#define MOD_OFFSET              ADJ_OFFSET
#define MOD_FREQUENCY           ADJ_FREQUENCY
#define MOD_MAXERROR            ADJ_MAXERROR
#define MOD_ESTERROR            ADJ_ESTERROR
#define MOD_STATUS              ADJ_STATUS
#define MOD_TIMECONST           ADJ_TIMECONST

#define STA_PLL                 0x0001
#define STA_PPSFREQ             0x0002
#define STA_PPSTIME             0x0004
#define STA_FLL                 0x0008

#define STA_INS                 0x0010
#define STA_DEL                 0x0020
#define STA_UNSYNC              0x0040
#define STA_FREQHOLD            0x0080

#define STA_PPSSIGNAL           0x0100
#define STA_PPSJITTER           0x0200
#define STA_PPSWANDER           0x0400
#define STA_PPSERROR            0x0800

#define STA_CLOCKERR            0x1000

#define STA_RONLY               (STA_PPSSIGNAL | STA_PPSJITTER | STA_PPSWANDER | STA_PPSERROR | STA_CLOCKERR)

#define TIME_OK                 0
#define TIME_INS                1
#define TIME_DEL                2
#define TIME_OOP                3
#define TIME_WAIT               4
#define TIME_ERROR              5
#define TIME_BAD                TIME_ERROR

struct timex {
    unsigned int                modes;
    long                        offset;
    long                        freq;
    long                        maxerror;
    long                        esterror;
    int                         status;
    long                        constant;
    long                        precision;
    long                        tolerance;
};

#if VSF_LINUX_APPLET_USE_SYS_TIMEX == ENABLED
typedef struct vsf_linux_sys_timex_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(adjtimex);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(clock_adjtime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ntp_adjtime);
} vsf_linux_sys_timex_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_timex_vplt_t vsf_linux_sys_timex_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_TIMEX_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_TIMEX == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_TIMEX_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_TIMEX_VPLT                                   \
            ((vsf_linux_sys_timex_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_timex_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_TIMEX_VPLT                                   \
            ((vsf_linux_sys_timex_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_TIMEX_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_TIMEX_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_TIMEX_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_TIMEX_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_TIMEX_IMP(adjtimex, int, struct timex *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIMEX_ENTRY(adjtimex)(buf);
}
VSF_LINUX_APPLET_SYS_TIMEX_IMP(clock_adjtime, int, clockid_t clk_id, struct timex *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIMEX_ENTRY(clock_adjtime)(clk_id, buf);
}
VSF_LINUX_APPLET_SYS_TIMEX_IMP(ntp_adjtime, int, struct timex *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIMEX_ENTRY(ntp_adjtime)(buf);
}

#else

int adjtimex(struct timex *buf);
int clock_adjtime(clockid_t clk_id, struct timex *buf);
int ntp_adjtime(struct timex *buf);

#endif

#ifdef __cplusplus
}
#endif

#endif
