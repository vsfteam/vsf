
#include "vsf.h"

#if VSF_USE_EVM == ENABLED && VSF_EVM_USE_LUA == ENABLED

#include "luat_base.h"
#include "luat_log.h"
#include "luat_uart.h"

static uint8_t luat_log_level_cur = LUAT_LOG_DEBUG;

void luat_log_set_uart_port(int port) {
}

void luat_print(const char *str) {
    luat_nprint((char *)str, strlen(str));
}

void luat_nprint(char *str, size_t l) {
    __vsf_trace_output(str, l);
}

void luat_log_set_level(int level) {
    luat_log_level_cur = level;
}

int luat_log_get_level() {
    return luat_log_level_cur;
}

void luat_log_log(int level, const char* tag, const char* _fmt, ...) {
    if (luat_log_level_cur > level) return;

    vsf_trace_level_t trace_level;
    char prefix;
    switch (level) {
    case LUAT_LOG_DEBUG:
        trace_level = VSF_TRACE_DEBUG;
        prefix = 'D';
        break;
    case LUAT_LOG_INFO:
        trace_level = VSF_TRACE_INFO;
        prefix = 'I';
        break;
    case LUAT_LOG_WARN:
        trace_level = VSF_TRACE_WARNING;
        prefix = 'W';
        break;
    case LUAT_LOG_ERROR:
        trace_level = VSF_TRACE_ERROR;
        prefix = 'E';
        break;
    default:
        trace_level = VSF_TRACE_NONE;
        prefix = '?';
        break;
    }

    vsf_trace(trace_level, "%d/%s ", prefix, tag);

    va_list args;
    va_start(args, _fmt);
        vsf_trace_arg(trace_level, _fmt, args);
    va_end(args);
}

#endif      // VSF_USE_EVM && VSF_EVM_USE_LUA
