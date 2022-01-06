#ifndef __VSF_LINUX_CDEFS_H__
#define __VSF_LINUX_CDEFS_H__

#ifdef __cplusplus
#   define __BEGIN_DECLS    extern "C" {
#   define __END_DECLS      }
#else
#   define __BEGIN_DECLS
#   define __END_DECLS
#endif

#define __THROW
#define __THROWNL
#define __NTH(Fct)          fct
#define __NTHNL(fct)        fct

#endif      // __VSF_LINUX_CDEFS_H__
