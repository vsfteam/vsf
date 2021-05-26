#ifndef __VSF_LINUX_INET_H__
#define __VSF_LINUX_INET_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../netinet/in.h"
#else
#   include <netinet/in.h>
#endif
#include <inttypes.h>

// for endian APIs
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_FAKE_API == ENABLED
#endif

#define htonl               cpu_to_be32
#define htons               cpu_to_be16
#define ntohl               be32_to_cpu
#define ntohs               be16_to_cpu

#ifdef __cplusplus
}
#endif

#endif
