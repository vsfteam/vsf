#ifndef __VSF_LINUX_BITFIELD_H__
#define __VSF_LINUX_BITFIELD_H__

#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __bf_shf(__mask)                vsf_ffs32(__mask)

#define FIELD_GET(__MASK, __VAL)        (((__VAL) & (__MASK)) >> __bf_shf(__MASK))
#define FIELD_PREP(__MASK, __VAL)       (((__VAL) << __bf_shf(__MASK)) & (__MASK))
#define FIELD_FIT(__MASK, __VAL)        !(((__VAL) << __bf_shf(__MASK)) & ~(__MASK))

#ifdef __cplusplus
}
#endif

#endif
