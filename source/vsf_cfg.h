/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/


/*! \note Top Level Configuration */

#ifndef __VSF_CFG_H__
#define __VSF_CFG_H__

/*============================ MACROS ========================================*/

#ifndef ENABLED
#   define ENABLED                              1
#endif

#ifndef DISABLED
#   define DISABLED                             0
#endif

/*============================ INCLUDES ======================================*/

/* do not modify this */
#include "vsf_usr_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if     !defined(__VSF_RELEASE__) && !defined(__VSF_DEBUG__)
#   define __VSF_DEBUG__                        1
#elif   defined(__VSF_RELEASE__) && defined(__VSF_DEBUG__)
#   error Both __VSF_RELEASE__ and __VSF_DEBUG__ are defined!!!! They should be\
 mutually exclusive from each other, i.e. either define __VSF_RELEASE__ or\
 __VSF_DEBUG__. If neither of them are defined, __VSF_DEBUG__ will be assumed.
#endif


#ifndef VSF_USE_KERNEL
#   define VSF_USE_KERNEL                       ENABLED
#endif

#ifndef Hz
#   define Hz                                   ul
#endif

#ifndef VSF_USR_SWI_NUM
#   define VSF_USR_SWI_NUM                      0
#endif

#if VSF_USE_KERNEL == ENABLED && defined(VSF_OS_CFG_PRIORITY_NUM)
#   if (VSF_OS_CFG_PRIORITY_NUM < 1)
#       error VSF_OS_CFG_PRIORITY_NUM MUST be defined to calculate \
__VSF_HAL_SWI_NUM and its value must at least be 1.
#   endif

#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#       if VSF_OS_CFG_PRIORITY_NUM > 1
#           define __VSF_HAL_SWI_NUM            (VSF_OS_CFG_PRIORITY_NUM - 1)
#       else
#           define __VSF_HAL_SWI_NUM            0
#       endif
#   else
#       define __VSF_HAL_SWI_NUM                (VSF_OS_CFG_PRIORITY_NUM)
#   endif
// priority configurations
#   define __VSF_OS_SWI_NUM                     __VSF_HAL_SWI_NUM
#endif

#if     (defined(VSF_DEBUGGER_CFG_CONSOLE) && (defined(VSF_HAL_USE_DEBUG_STREAM) && VSF_HAL_USE_DEBUG_STREAM == ENABLED))\
    ||  (defined(VSF_CFG_DEBUG_STREAM_TX_T) && (defined(VSF_HAL_USE_DEBUG_STREAM) && VSF_HAL_USE_DEBUG_STREAM == ENABLED))\
    ||  (defined(VSF_DEBUGGER_CFG_CONSOLE) && defined(VSF_CFG_DEBUG_STREAM_TX_T))
#   error "please enable one of VSF_HAL_USE_DEBUG_STREAM/VSF_DEBUGGER_CFG_CONSOLE/VSF_CFG_DEBUG_STREAM_TX_T"
#endif

// for vplt
#ifndef __VSF_VPLT_DECORATE__
#   define __VSF_VPLT_DECORATOR__               const
#endif
typedef union vsf_vplt_info_t {
    struct {
        unsigned char major;
        unsigned char minor;
        unsigned short final : 1;
        unsigned short entry_num : 15;
    };
    void *__make_vplt_info_aligned;
} vsf_vplt_info_t;

typedef struct vsf_vplt_t {
    vsf_vplt_info_t info;

    void *linux_vplt;
} vsf_vplt_t;

#ifndef VSF_APPLET_CFG_LINKABLE
#   define VSF_APPLET_CFG_LINKABLE              ENABLED
#endif
#if VSF_APPLET_CFG_LINKABLE == ENABLED && !defined(__VSF_APPLET__)
extern void * vsf_vplt_link(void *vplt, char *symname);
extern __VSF_VPLT_DECORATOR__ vsf_vplt_t vsf_vplt;
#endif
typedef struct vsf_vplt_entry_t {
#if VSF_APPLET_CFG_LINKABLE == ENABLED
    // TODO: implement hash for link performance
    const char *name;
#endif
    void *ptr;
} vsf_vplt_entry_t;

#define VSF_APPLET_VPLT_INFO(__TYPE, __MAJOR, __MINOR, __FINAL)                 \
    .info = {                                                                   \
        .major = (__MAJOR),                                                     \
        .minor = (__MINOR),                                                     \
        .final = (__FINAL),                                                     \
        .entry_num =    (sizeof(__TYPE) - sizeof(vsf_vplt_info_t))              \
                    /   ((__FINAL) ? sizeof(vsf_vplt_entry_t) : sizeof(void *)),\
    }
#define VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__NAME)                                  \
    vsf_vplt_entry_t fn_##__NAME
#if VSF_APPLET_CFG_DEBUG_VPLT == ENABLED
#   define VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(__VPLT, __NAME)                     \
    vsf_linux_trace(VSF_TRACE_DEBUG, "call vplt API: %s\n", #__NAME);           \
    ((__##__NAME##_prototype_t)((__VPLT)->fn_##__NAME.ptr))
#else
#   define VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(__VPLT, __NAME)                     \
    ((__##__NAME##_prototype_t)((__VPLT)->fn_##__NAME.ptr))
#endif
#define VSF_APPLET_VPLT_ENTRY_FUNC_IMP(__VPLT, __NAME, __RET, ...)              \
    typedef __RET (*__##__NAME##_prototype_t)(__VA_ARGS__);                     \
    static inline __RET __NAME(__VA_ARGS__)
#if VSF_APPLET_CFG_LINKABLE == ENABLED
#   define VSF_APPLET_VPLT_ENTRY_FUNC_EX(__ENTRY, __NAME, __PTR)                \
    .__ENTRY = {                                                                \
        .name = (__NAME),                                                       \
        .ptr = (void *)(__PTR),                                                 \
    }
#   define VSF_APPLET_VPLT_ENTRY_FUNC(__NAME)                                   \
    .fn_##__NAME = {                                                            \
        .name = VSF_STR(__NAME),                                                \
        .ptr = (void *)(__NAME),                                                \
    }
#else
#   define VSF_APPLET_VPLT_ENTRY_FUNC_EX(__ENTRY, __NAME, __PTR)                \
    .__ENTRY = {                                                                \
        .ptr = (void *)(__PTR),                                                 \
    }
#   define VSF_APPLET_VPLT_ENTRY_FUNC(__NAME)                                   \
    .fn_##__NAME = {                                                            \
        .ptr = (void *)(__NAME),                                                \
    }
#endif

#ifndef __VSF_APPLET_CTX_DEFINED__
#define __VSF_APPLET_CTX_DEFINED__
typedef struct vsf_applet_ctx_t {
    void *target;
    int (*fn_init)(void *);
    void (*fn_fini)(void *);

    int argc;
    char **argv;
    char **envp;
    void *vplt;
} vsf_applet_ctx_t;
#endif

#ifdef __VSF_APPLET__
#   if VSF_USE_APPLET == ENABLED && !defined(VSF_APPLET_VPLT)
#       define VSF_APPLET_VPLT              ((vsf_vplt_t *)vsf_vplt((void *)0))
#   endif

#   ifndef applet_entry
#       ifdef __WIN__
#           define applet_entry             _dllstart
#       else
#           define applet_entry             _start
#       endif
#   endif

#   ifndef applet_entry_with_ctx
#       if VSF_APPLET_CFG_ABI_PATCH == ENABLED
extern vsf_applet_ctx_t * vsf_applet_ctx(void);
extern int vsf_vplt_init_array(void *target);
extern void vsf_vplt_fini_array(void *target);
#           define applet_entry_with_ctx                                        \
                applet_entry(void) { vsf_applet_ctx_t *ctx = vsf_applet_ctx();
#           define applet_init_array        vsf_vplt_init_array
#           define applet_fini_array        vsf_vplt_fini_array
#       else
#           define applet_entry_with_ctx                                        \
                applet_entry(vsf_applet_ctx_t *ctx) {
#           define applet_init_array        ctx->fn_init
#           define applet_fini_array        ctx->fn_fini
#       endif
#   endif

#if VSF_APPLET_MAIN_HAS_ENVP == ENABLED
extern int main(int, char **, char **);
#else
extern int main(int, char **);
#endif

extern void * vsf_vplt(void *vplt);
#   define main(...)                                                            \
    applet_entry_with_ctx                                                       \
        int result;                                                             \
        vsf_vplt(ctx->vplt);                                                    \
        if (applet_init_array != NULL) {                                        \
            result = applet_init_array(ctx->target);                            \
            if (result) {                                                       \
                return result;                                                  \
            }                                                                   \
        }                                                                       \
        result = ((int (*)(int, char **, char **))main)(ctx->argc, ctx->argv, ctx->envp);\
        if (applet_fini_array != NULL) {                                        \
            applet_fini_array(ctx->target);                                     \
        }                                                                       \
        return result;                                                          \
    }                                                                           \
    void * vsf_vplt(void *vplt)                                                 \
    {                                                                           \
        static void *__vplt;                                                    \
        if (vplt != (void *)0) {                                                \
            __vplt = vplt;                                                      \
        }                                                                       \
        return __vplt;                                                          \
    }                                                                           \
    int main(__VA_ARGS__)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */