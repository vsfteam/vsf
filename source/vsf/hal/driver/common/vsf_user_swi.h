#ifndef __VSF_USER_SWI_H__
#define __VSF_USER_SWI_H__

#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

typedef void vsf_swi_handler_t(void *p);

extern vsf_err_t vsf_drv_usr_swi_init(uint_fast8_t idx, 
                                      vsf_arch_prio_t priority,
                                      vsf_swi_handler_t *handler, 
                                      void *param);
extern void vsf_drv_usr_swi_trigger(uint_fast8_t idx);
#endif