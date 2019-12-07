//See LICENSE for license details.
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "hal/vsf_hal_cfg.h"
//#include "../drivers/riscv_encoding.h"
//#include "../drivers/n200_func.h"

__attribute__((weak)) uintptr_t handle_nmi()
{
    _exit(1);
    return 0;
}


__attribute__((weak)) uintptr_t handle_trap(uintptr_t mcause, uintptr_t sp)
{
    if(mcause == 0xFFF) {
        handle_nmi();
    }
    _exit(mcause);
    return 0;
}





