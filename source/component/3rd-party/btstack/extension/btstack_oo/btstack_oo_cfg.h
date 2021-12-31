#include "vsf_cfg.h"

#ifndef __BTSTACK_OO_CFG_H__
#define __BTSTACK_OO_CFG_H__

#ifndef BTSTACK_ASSERT
#   define BTSTACK_ASSERT(...)              VSF_ASSERT(__VA_ARGS__)
#endif

#ifndef BTSTACK_TRACE
#   include "service/trace/vsf_trace.h"
#   define BTSTACK_TRACE(...)               vsf_trace(VSF_TRACE_INFO, "btstack: " __VA_ARGS__)
#endif

#endif
