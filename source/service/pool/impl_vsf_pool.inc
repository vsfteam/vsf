
#include "utilities/vsf_utilities.h"

#if defined(__name) && defined(__type)


WEAK(VSF_MCONNECT(__name, _pool_init))
void VSF_MCONNECT(__name, _pool_init)(vsf_pool(__name) *this_ptr, vsf_pool_cfg_t *cfg_ptr)
{
    vsf_pool_init(  &(this_ptr->use_as__vsf_pool_t),
                    sizeof(__type),
                    __alignof__(__type),
                    cfg_ptr);
}

void VSF_MCONNECT(__name, _pool_init_ex)( vsf_pool(__name) *this_ptr,
                            uint_fast16_t align,
                            vsf_pool_cfg_t *cfg_ptr)
{
    vsf_pool_init(  &(this_ptr->use_as__vsf_pool_t),
                    sizeof(__type),
                    vsf_max(align,__alignof__(__type)),
                    cfg_ptr);
}

WEAK(VSF_MCONNECT(__name, _pool_add_buffer))
bool VSF_MCONNECT(__name, _pool_add_buffer)(
    vsf_pool(__name) *this_ptr, uintptr_t buffer_ptr, uint_fast32_t u32_size)
{
    return vsf_pool_add_buffer((vsf_pool_t *)this_ptr, buffer_ptr,
                            u32_size, sizeof(vsf_pool_item(__name)));
}

WEAK(VSF_MCONNECT(__name, _pool_add_buffer_ex))
SECTION(".text." VSF_STR(__name) "_pool_add_buffer_ex")
bool VSF_MCONNECT(__name, _pool_add_buffer_ex)(
        vsf_pool(__name) *this_ptr,
        void *buffer_ptr,
        uint_fast32_t u32_size,
        vsf_pool_item_init_evt_handler_t *handler_fn)
{
    return vsf_pool_add_buffer_ex(
                    (vsf_pool_t *)this_ptr, (uintptr_t)buffer_ptr,
                    u32_size,
                    sizeof(vsf_pool_item(__name)),
                    handler_fn);
}

WEAK(VSF_MCONNECT(__name, _pool_alloc))
__type *VSF_MCONNECT(__name, _pool_alloc)(vsf_pool(__name) *this_ptr)
{
    return (__type *)vsf_pool_alloc((vsf_pool_t *)this_ptr);
}

WEAK(VSF_MCONNECT(__name, _pool_free))
void VSF_MCONNECT(__name, _pool_free)(vsf_pool(__name) *this_ptr, __type *item_ptr)
{
    vsf_pool_free((vsf_pool_t *)this_ptr, (uintptr_t)item_ptr);
}

WEAK(VSF_MCONNECT(__name, _get_pool_item_count))
SECTION(".text." VSF_STR(__name) "_get_pool_item_count")
uint_fast32_t VSF_MCONNECT(__name, _get_pool_item_count)(vsf_pool(__name) *this_ptr)
{
    return vsf_pool_get_count((vsf_pool_t *)this_ptr);
}

WEAK(VSF_MCONNECT(__name, _pool_get_region))
SECTION(".text." VSF_STR(__name) "_pool_get_region")
vsf_protect_region_t *VSF_MCONNECT(__name, _pool_get_region)(vsf_pool(__name) *this_ptr)
{
    return vsf_pool_get_region((vsf_pool_t *)this_ptr);
}

__implement_vsf_pool_tag(__name)

#endif

#undef __name
#undef __type
