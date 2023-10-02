#ifndef __VSF_LINUX_ASM_BYTEORDER_H__
#define __VSF_LINUX_ASM_BYTEORDER_H__

#include "hal/arch/vsf_arch.h"

#define __le16_to_cpu           le16_to_cpu
#define __le32_to_cpu           le32_to_cpu
#define __le64_to_cpu           le64_to_cpu
#define __cpu_to_le16           cpu_to_le16
#define __cpu_to_le32           cpu_to_le32
#define __cpu_to_le64           cpu_to_le64

#define __be16_to_cpu           be16_to_cpu
#define __be32_to_cpu           be32_to_cpu
#define __be64_to_cpu           be64_to_cpu
#define __cpu_to_be16           cpu_to_be16
#define __cpu_to_be32           cpu_to_be32
#define __cpu_to_be64           cpu_to_be64

#endif      // __VSF_LINUX_ASM_BYTEORDER_H__
