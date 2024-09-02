#include "utilities/compiler/compiler.h"

#if VSF_APPLET_USE_COMPILER == ENABLED
VSF_CAL_WEAK(__paritysi2)
int __paritysi2(unsigned int a)
{
    int x = (int)a;
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    return (0x6996 >> (x & 0xF)) & 1;
}

__VSF_VPLT_DECORATOR__ vsf_compiler_vplt_t vsf_compiler_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_compiler_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(__paritysi2),
};
#endif

#ifdef __VSF_CPP__
VSF_CAL_WEAK(vsf_compiler_cpp_startup)
int vsf_compiler_cpp_startup(void)
{
    return -1;
}
#endif
