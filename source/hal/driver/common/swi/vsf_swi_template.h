/*******************************************************************************
                       How to expand user priority                                                 
Follow the steps below in order:

Tutorials for end users:
    1.Create a "user_swi" folder under the project folder.

    2.Create two files "user_swi.h" and "user_swi.c" under the "user_swi" folder.

    3.Create a "group" under the project.

    4.Add the "user_swi.h" file and "user_swi.c" file to the "group" created in 
      the third step.

    5.Include the "vsf_swi_template.h" file in the "user_swi.h" file.
      The path is "hal/driver/common/vsf_swi_template.h".

    6.Include the register header file of the specific chip in the "user_swi.c" 
      file.

    7.Include the "vsf_swi_template.inc" file in the "user_swi.c" file.
      The path is "hal/driver/common/vsf_swi_template.inc".

    8.Modify the interrupt vector table of the specific chip.
      For specific methods, please refer to Chapter 7 of this document:
      https://github.com/vsfteam/vsf/tree/master/source/vsf/documents/porting

Tutorials for driver developers:
    1.Create the "interrupt_swi.h" file and "interrupt_swi.c" file in the 
      specific chip folder under the driver folder.

    2.Include the following file path in the "interrupt_swi.h" file:
          "hal/vsf_hal_cfg.h"
          "./device.h"
          "hal/driver/common/vsf_swi_template.h"

    3.Include the following file paths in the "interrupt_swi.c" file:
          "hal/vsf_hal_cfg.h"
          "./device.h"
          "hal/driver/common/vsf_swi_template.inc"

    4.Include the "interrupt_swi.h" file in the "driver.h" file in the same folder.

    5.Check whether the following macro definitions exist in the file "device.h" 
      included in the third operation. If not, you need to complete it according 
      to the specific chip resources and requirements:
         "VSF_ARCH_PRI_BIT"
         meaning:The number of interrupt nesting levels supported by the chip is 
                 converted into a binary number.

         "VSF_ARCH_PRI_NUM"
         meaning:The number of interrupt nesting levels supported by the chip.

         "VSF_DEV_SWI_NUM"
         meaning:Number of user priorities expected to expand.

         "VSF_DEV_SWI_LIST"
         meaning:The number of the interrupt used by the user extended priority 
         in the interrupt vector table.

    6.Modify the interrupt vector table of the specific chip.
      For specific methods, please refer to Chapter 7 of this document:
      https://github.com/vsfteam/vsf/tree/master/source/vsf/documents/porting
*******************************************************************************/

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