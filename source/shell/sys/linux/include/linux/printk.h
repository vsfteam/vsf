#ifndef __VSF_LINUX_PRINTK_H__
#define __VSF_LINUX_PRINTK_H__

#include "service/trace/vsf_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KERN_SOH                        "\001"
#define KERN_SOH_ASCII                  '\001'

#define KERN_EMERG                      KERN_SOH "0"
#define KERN_ALERT                      KERN_SOH "1"
#define KERN_CRIT                       KERN_SOH "2"
#define KERN_ERR                        KERN_SOH "3"
#define KERN_WARNING                    KERN_SOH "4"
#define KERN_NOTICE                     KERN_SOH "5"
#define KERN_INFO                       KERN_SOH "6"
#define KERN_DEBUG                      KERN_SOH "7"

#define KERN_DEFAULT                    ""

enum {
    DUMP_PREFIX_NONE,
    DUMP_PREFIX_ADDRESS,
    DUMP_PREFIX_OFFSET
};

#ifndef pr_fmt
#   define pr_fmt(__fmt)                __fmt
#endif

#define printk(__fmt, ...)              vsf_trace_info(__fmt, ##__VA_ARGS__)
#define pr_emerg(__fmt, ...)            printk(KERN_EMERG pr_fmt(__fmt), ##__VA_ARGS__)
#define pr_alert(__fmt, ...)            printk(KERN_ALERT pr_fmt(__fmt), ##__VA_ARGS__)
#define pr_crit(__fmt, ...)             printk(KERN_CRIT pr_fmt(__fmt), ##__VA_ARGS__)
#define pr_err(__fmt, ...)              printk(KERN_ERR pr_fmt(__fmt), ##__VA_ARGS__)
#define pr_warn(__fmt, ...)             printk(KERN_WARNING pr_fmt(__fmt), ##__VA_ARGS__)
#define pr_notice(__fmt, ...)           printk(KERN_NOTICE pr_fmt(__fmt), ##__VA_ARGS__)
#define pr_info(__fmt, ...)             printk(KERN_INFO pr_fmt(__fmt), ##__VA_ARGS__)
#define pr_devel(__fmt, ...)            printk(KERN_DEBUG pr_fmt(__fmt), ##__VA_ARGS__)
#define pr_debug(__fmt, ...)            printk(KERN_DEBUG pr_fmt(__fmt), ##__VA_ARGS__)

#define pr_cont(__fmt, ...)             printk(KERN_CONT __fmt, ##__VA_ARGS__)

#define print_hex_dump_debug(prefix_str, prefix_type, rowsize, groupsize, buf, len, ascii)\
        print_hex_dump(KERN_DEBUG, prefix_str, prefix_type, rowsize, groupsize, buf, len, ascii)
#define print_hex_dump(level, prefix_str, prefix_type, rowsize, groupsize, buf, len, ascii)\
        vsf_trace_buffer(VSF_TRACE_INFO, buf, len,                              \
                VSF_TRACE_DF_DS(1) | VSF_TRACE_DF_DPL(rowsize) | VSF_TRACE_DF_NEWLINE\
            |   ((ascii) ? VSF_TRACE_DF_CHAR : 0)                               \
        )

#ifdef __cplusplus
}
#endif

#endif
