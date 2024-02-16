#ifndef __VSF_LINUX_LIMITS_H__
#define __VSF_LINUX_LIMITS_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PATH_MAX
#   define PATH_MAX             260
#endif

#ifndef HOST_NAME_MAX
#   define HOST_NAME_MAX        64
#endif

#define NAME_MAX                255
#define PHYS_ADDR_MAX           (~(phys_addr_t)0)
#define PIPE_BUF                4096

#define U8_MAX                  ((u8)~0U)
#define S8_MAX                  ((s8)(U8_MAX >> 1))
#define S8_MIN                  ((s8)(-S8_MAX - 1))
#define U16_MAX                 ((u16)~0U)
#define S16_MAX                 ((s16)(U16_MAX >> 1))
#define S16_MIN                 ((s16)(-S16_MAX - 1))
#define U32_MAX                 ((u32)~0U)
#define U32_MIN                 ((u32)0)
#define S32_MAX                 ((s32)(U32_MAX >> 1))
#define S32_MIN                 ((s32)(-S32_MAX - 1))
#define U64_MAX                 ((u64)~0ULL)
#define S64_MAX                 ((s64)(U64_MAX >> 1))
#define S64_MIN                 ((s64)(-S64_MAX - 1))

#ifdef __cplusplus
}
#endif

#endif
