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

File        : SEGGER_SYSVIEW_Config_VSF.c
Purpose     : Sample setup configuration of SystemView with VSF.
Revision: $Rev: 9599 $
*/
#include "SEGGER_SYSVIEW.h"
#include "kernel/vsf_kernel.h"

extern const SEGGER_SYSVIEW_OS_API SYSVIEW_VSF_TraceAPI;

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

/*********************************************************************
*
*       _cbSendSystemDesc()
*
*  Function description
*    Sends SystemView description strings.
*/
static void _cbSendSystemDesc(void) {
  SEGGER_SYSVIEW_SendSysDesc("N="SEGGER_SYSVIEW_APP_NAME",D="SEGGER_SYSVIEW_DEVICE_NAME",O=VSF");

#if VSF_KERNEL_CFG_TRACE == ENABLED
#   define SEGGER_SYSVIEW_VSF_EVT(__EVT)   SEGGER_SYSVIEW_NameMarker((__EVT), STR(__EVT))
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_NONE);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_YIELD);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_DUMMY);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_INIT);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_FINI);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_ENTER);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_EXIT);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_RETURN);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_TIMER);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_SYNC);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_SYNC_CANCEL);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_SYNC_POLL);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_MESSAGE);
  SEGGER_SYSVIEW_VSF_EVT(VSF_EVT_USER);
#endif
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
void SEGGER_SYSVIEW_Conf(void) {
  SEGGER_SYSVIEW_Init(SEGGER_SYSVIEW_TIMESTAMP_FREQ, 0,
                      &SYSVIEW_VSF_TraceAPI, _cbSendSystemDesc);
}

/*************************** End of file ****************************/
