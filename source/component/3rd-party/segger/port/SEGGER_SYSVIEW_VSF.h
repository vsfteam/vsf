/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2021 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: 3.30                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------
File    : SEGGER_SYSVIEW_VSF.h
Purpose : VSF Porting.
Revision: $Rev: 21292 $
*/

#ifndef SEGGER_SYSVIEW_VSF_H
#define SEGGER_SYSVIEW_VSF_H

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/

#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
#define VSF_APIID_START                             32
#define VSF_APIID_EDA_READY                         (VSF_APIID_START + 0)
#define VSF_APIID_EDA_BUSY                          (VSF_APIID_START + 1)

/*********************************************************************
*
*       VSF Kernel Trace APIs
*/
void vsf_kernel_trace_init                          (void);
void vsf_kernel_trace_eda_init                      (vsf_eda_t *eda);
void vsf_kernel_trace_eda_info                      (vsf_eda_t *eda, char *name, void *stack, uint_fast32_t stack_size);
void vsf_kernel_trace_eda_fini                      (vsf_eda_t *eda);
void vsf_kernel_trace_eda_ready                     (vsf_eda_t *eda);
void vsf_kernel_trace_eda_idle                      (vsf_eda_t *eda);
void vsf_kernel_trace_eda_evt_begin                 (vsf_eda_t *eda, vsf_evt_t evt);
void vsf_kernel_trace_eda_evt_end                   (vsf_eda_t *eda, vsf_evt_t evt);
void vsf_kernel_trace_idle                          (void);

void vsf_kernel_trace_isr_info                      (int id, char *name);
void vsf_kernel_trace_isr_enter                     (int id);
void vsf_kernel_trace_isr_leave                     (int id);

#ifdef __cplusplus
}
#endif

#endif

/*************************** End of file ****************************/
