#include "vsf.h"
#include <assert.h>

void vsf_assert(int expression)
{
    assert(expression);
}

void app_main(void)
{
    vsf_freertos_start();
}
