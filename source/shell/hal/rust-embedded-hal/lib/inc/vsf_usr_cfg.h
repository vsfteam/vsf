#define VSF_ASSERT(...)

// linker script of some targets in rust will not define text region with _stext and _etext
//  callstack trace will be unavailable
#define VSF_ARCH_CFG_CALLSTACK_TRACE                    DISABLED

#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED
#   define VSF_DEBUG_STREAM_CFG_HW_PRIORITY             vsf_arch_prio_0

#define VSF_APPLET_USE_ARCH_ABI                         DISABLED
#define VSF_APPLET_USE_COMPILER                         DISABLED

#ifndef VSF_USE_DISTBUS
#   define VSF_USE_DISTBUS                              DISABLED
#endif
#ifndef VSF_USE_HEAP
#   define VSF_USE_HEAP                                 DISABLED
#endif
#define VSF_USE_DYNARR                                  DISABLED
#define VSF_USE_DYNSTACK                                DISABLED
#define VSF_USE_JSON                                    DISABLED
#define VSF_USE_LOADER                                  DISABLED
#define VSF_USE_PBUF                                    DISABLED
#define VSF_USE_POOL                                    ENABLED
#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_TRACE                                   DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED
