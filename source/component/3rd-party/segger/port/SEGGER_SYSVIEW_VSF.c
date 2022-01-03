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

File    : SEGGER_SYSVIEW_VSF.c
Purpose : Interface between VSF and SystemView.
*/

#define __VSF_EDA_CLASS_INHERIT__
#include "SEGGER_SYSVIEW.h"
#include "./SEGGER_SYSVIEW_VSF.h"
#include "kernel/vsf_kernel.h"

/*********************************************************************
*
*       _cbSendTaskList()
*
*  Function description
*    This function is part of the link between VSF and SYSVIEW.
*    Called from SystemView when asked by the host, it uses SYSVIEW
*    functions to send the entire task list to the host.
*/
//static void _cbSendTaskList(void) {
//}

/*********************************************************************
*
*       _cbGetTime()
*
*  Function description
*    This function is part of the link between VSF and SYSVIEW.
*    Called from SystemView when asked by the host, returns the
*    current system time in micro seconds.
*/
static U64 _cbGetTime(void) {
  return SEGGER_SYSVIEW_GET_TIMESTAMP();
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

void vsf_kernel_trace_init(void) {
  SEGGER_SYSVIEW_Conf();

  // poll for started
  while (!SEGGER_SYSVIEW_IsStarted());
}

void vsf_kernel_trace_eda_info(vsf_eda_t *eda, char *name, void *stack, uint_fast32_t stack_size) {
  SEGGER_SYSVIEW_TASKINFO TaskInfo = {
    .TaskID         = (U32)eda,
    .sName          = name,
#if     (VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED)                \
    &&  (VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED)
    .Prio           = (U32)eda->cur_priority,
#endif
    .StackBase      = (U32)stack,
    .StackSize      = (U32)stack_size,
  };

  SEGGER_SYSVIEW_SendTaskInfo(&TaskInfo);
}

void vsf_kernel_trace_eda_init(vsf_eda_t *eda) {
  SEGGER_SYSVIEW_OnTaskCreate((U32)eda);
}

void vsf_kernel_trace_eda_fini(vsf_eda_t *eda) {
  SEGGER_SYSVIEW_OnTaskTerminate((U32)eda);
}

void vsf_kernel_trace_eda_ready(vsf_eda_t *eda) {
  SEGGER_SYSVIEW_RecordU32(VSF_APIID_EDA_READY, (U32)eda);
}

void vsf_kernel_trace_eda_idle(vsf_eda_t *eda) {
  SEGGER_SYSVIEW_RecordEndCall(VSF_APIID_EDA_READY);
}

void vsf_kernel_trace_eda_evt_begin(vsf_eda_t *eda, vsf_evt_t evt) {
  SEGGER_SYSVIEW_OnTaskStartReady((U32)eda);
  SEGGER_SYSVIEW_OnTaskStartExec((U32)eda);
  SEGGER_SYSVIEW_Mark((U32)evt);
}

void vsf_kernel_trace_eda_evt_end(vsf_eda_t *eda, vsf_evt_t evt) {
  SEGGER_SYSVIEW_OnTaskStopReady((U32)eda, 0);
}

void vsf_kernel_trace_idle(void) {
  SEGGER_SYSVIEW_OnIdle();
}

void vsf_kernel_trace_isr_info(int id, char *name) {
  size_t len;
  char desc[256] = "I#";
  itoa(id, &desc[2], 10);
  len = strlen(desc);
  desc[len] = '=';
  VSF_ASSERT((sizeof(desc) - len - 2) > strlen(name));
  strcpy(&desc[len + 1], name);

  SEGGER_SYSVIEW_SendSysDesc(desc);
}

void vsf_kernel_trace_isr_enter(int id) {
  SEGGER_SYSVIEW_RecordEnterISR();
}

void vsf_kernel_trace_isr_leave(int id) {
  SEGGER_SYSVIEW_RecordExitISR();
}

/*********************************************************************
*
*       Public API structures
*
**********************************************************************
*/
const SEGGER_SYSVIEW_OS_API SYSVIEW_VSF_TraceAPI = {
  _cbGetTime,
  NULL,
};

/*************************** End of file ****************************/
