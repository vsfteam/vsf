#ifndef __VSF_LINUX_ENDIAN_H__
#define __VSF_LINUX_ENDIAN_H__

#include "hal/arch/vsf_arch.h"

#if __BYTE_ORDER == __BIG_ENDIAN
#   define BIG_ENDIAN       __BIG_ENDIAN
#else
#   define LITTLE_ENDIAN    __LITTLE_ENDIAN
#endif
#   define BYTE_ORDER       __BYTE_ORDER

#endif      // __VSF_LINUX_ENDIAN_H__
