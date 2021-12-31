#include "../btstack_oo_cfg.h"

#ifndef __BTSTACK_HOST_CFG_H__
#define __BTSTACK_HOST_CFG_H__

#ifndef BTSTACK_HOST_CFG_DEV_INHERIT
#   define BTSTACK_HOST_CFG_DEV_INHERIT         ENABLED
#endif

#if BTSTACK_HOST_CFG_DEV_INHERIT == ENABLED
#   ifndef WEAK_BTSTACK_HOST_DYNAMIC_DEV
#       define WEAK_BTSTACK_HOST_DYNAMIC_DEV
#   endif
#endif

#endif
