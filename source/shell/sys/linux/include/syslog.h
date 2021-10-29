#ifndef __VSF_LINUX_SYSLOG_H__
#define __VSF_LINUX_SYSLOG_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif
// for vsf_trace
#include "service/vsf_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_ALERT           VSF_TRACE_WARNING
#define LOG_CRIT            VSF_TRACE_ERROR
#define LOG_ERR             VSF_TRACE_ERROR
#define LOG_WARNING         VSF_TRACE_WARNING
#define LOG_NOTICE          VSF_TRACE_INFO
#define LOG_INFO            VSF_TRACE_INFO
#define LOG_DEBUG           VSF_TRACE_DEBUG

#define openlog(...)
#define closelog()
#define setlogmask(...)
#define syslog(...)         vsf_trace(__VA_ARGS__)
#define vsyslog(...)        vsf_trace_arg(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
