#ifndef __VSF_LINUX_SYSLOG_H__
#define __VSF_LINUX_SYSLOG_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

// for vsf_trace
#include "service/vsf_service.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    LOG_NOTICE      = VSF_TRACE_INFO,
    LOG_INFO        = VSF_TRACE_INFO,
    LOG_DEBUG       = VSF_TRACE_DEBUG,
    LOG_ALERT       = VSF_TRACE_WARNING,
    LOG_WARNING     = VSF_TRACE_WARNING,
    LOG_CRIT        = VSF_TRACE_ERROR,
    LOG_ERR         = VSF_TRACE_ERROR,
};

#define openlog(ident, option, facility)
#define syslog(priority, format, ...)       vsf_trace(priority, format, __VA_ARGS__)
#define vsyslog(priotiry, format, ap)       vsf_trace_arg(priority, format, ap)
#define closelog()

#ifdef __cplusplus
}
#endif

#endif
