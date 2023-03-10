#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "../__device.h"

#include "mem.h"

/*----------------------------------------------------------------------------*
 * Heap Implementation                                                        *
 *----------------------------------------------------------------------------*/

#if VSF_ARCH_PROVIDE_HEAP == ENABLED
void * vsf_arch_heap_malloc(uint_fast32_t size, uint_fast32_t alignment)
{
    return memalign(alignment, size);
}

void * vsf_arch_heap_realloc(void *buffer, uint_fast32_t size)
{
    return realloc(buffer, size);
}

void vsf_arch_heap_free(void *buffer)
{
    free(buffer);
}

unsigned int vsf_arch_heap_alignment(void)
{
    return 4;
}

uint_fast32_t vsf_arch_heap_size(void *buffer)
{
    return tlsf_block_size(buffer);
}
#endif
