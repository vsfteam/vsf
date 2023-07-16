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

/*============================ INCLUDES ======================================*/

#include "./vsf.h"

#include "./shell/sys/linux/vsf_linux_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_APPLET == ENABLED
WEAK(vsf_vplt)
__VSF_VPLT_DECORATOR__ vsf_vplt_t vsf_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_vplt_t, 0, 0, false),

    .applet_vplt        = (void *)&vsf_applet_vplt,
#   if VSF_APPLET_USE_ARCH == ENABLED
    .arch_vplt          = (void *)&vsf_arch_vplt,
#   endif
#   if VSF_APPLET_USE_SERVICE == ENABLED
    .service_vplt       = (void *)&vsf_service_vplt,
#   endif

#   if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_APPLET == ENABLED
    .linux_vplt         = (void *)&vsf_linux_vplt,
#   endif
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_APPLET == ENABLED || (VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_APPLET == ENABLED)
#   if VSF_APPLET_CFG_LINKABLE == ENABLED

static void * __vsf_vplt_link(void *vplt, char *symname)
{
    vsf_vplt_info_t *vplt_info = vplt;
    unsigned short entry_num = vplt_info->entry_num;

    if (vplt_info->final) {
        vsf_vplt_entry_t *entry = (vsf_vplt_entry_t *)&vplt_info[1];
        for (unsigned short i = 0; i < entry_num; i++, entry++) {
            if ((entry->name != NULL) && (!strcmp(entry->name, symname))) {
                return entry->ptr;
            }
        }
    } else {
        void **subvplt = (void **)&vplt_info[1], *result;
        for (unsigned short i = 0; i < entry_num; i++, subvplt++) {
            if (*subvplt != NULL) {
                result = __vsf_vplt_link(*subvplt, symname);
                if (result != NULL) {
                    return result;
                }
            }
        }
    }

    return NULL;
}

WEAK(vsf_vplt_link)
void * vsf_vplt_link(void *vplt, char *symname)
{
    return __vsf_vplt_link(vplt, symname);
}
#   endif

#endif

#if (VSF_USE_APPLET == ENABLED || VSF_LINUX_USE_APPLET == ENABLED) && !defined(__VSF_APPLET__)

WEAK(vsf_vplt_init_array)
int vsf_vplt_init_array(void *target) { return -1; }
WEAK(vsf_vplt_fini_array)
void vsf_vplt_fini_array(void *target) { }

WEAK(vsf_applet_ctx)
vsf_applet_ctx_t * vsf_applet_ctx(void)
{
    return NULL;
}

WEAK(vsf_applet_remap)
void * vsf_applet_remap(vsf_applet_ctx_t *ctx, void *vaddr)
{
    return vaddr;
}

WEAK(vsf_vplt_trace_arg)
void vsf_vplt_trace_arg(const char* fmt, va_list arg)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_trace_arg(VSF_TRACE_DEBUG, fmt, arg);
#   endif
}

__VSF_VPLT_DECORATOR__ vsf_applet_vplt_t vsf_applet_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_applet_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_applet_ctx),
    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_vplt_trace_arg),
    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_vplt_init_array),
    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_vplt_fini_array),
    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_applet_remap),
};
#endif

/* EOF */
