#include "./mbedtls_vplt.h"

#ifndef __VSF_APPLET__
#	warning this file should be used in applets.
#else

static vsf_mbedtls_vplt_t * __mbedtls_get_vplt(void)
{
    static vsf_mbedtls_vplt_t *__mbedtls_vplt = NULL;
    if (NULL == __mbedtls_vplt) {
        extern void * vsf_vplt_link(void *vplt, char *symname);
        __mbedtls_vplt = vsf_vplt_link(NULL, "mbedtls");
    }
    return __mbedtls_vplt;
}

#define VSF_APPLET_MBEDTLS_VPLT                                                 \
            ((vsf_mbedtls_vplt_t *)__mbedtls_get_vplt())
#define VSF_APPLET_MBEDTLS_ENTRY(__NAME)                                        \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_APPLET_MBEDTLS_VPLT, __NAME)
#define VSF_APPLET_MBEDTLS_IMP(...)                                             \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_APPLET_MBEDTLS_VPLT, __VA_ARGS__)

VSF_APPLET_MBEDTLS_IMP(mbedtls_version_get_number, unsigned int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_version_get_number)();
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_version_get_string, void, char *string) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_version_get_string)(string);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_version_get_string_full, void, char *string) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_version_get_string_full)(string);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_version_check_feature, int, const char *feature) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_version_check_feature)(feature);
}

#endif
