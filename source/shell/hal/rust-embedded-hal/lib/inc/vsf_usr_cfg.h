#define VSF_ASSERT(...)

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
