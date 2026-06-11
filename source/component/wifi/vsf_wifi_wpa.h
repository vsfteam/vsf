/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_WIFI_WPA_H__
#define __VSF_WIFI_WPA_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_WPA == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ PROTOTYPES =====================================*/

/*
 * WPA2-PSK 4-way handshake EAPOL-Key entry point.
 *
 * Called by vsf_wifi_data_rx() with the EAPOL payload that follows the
 * 802.11 data header and the LLC/SNAP + 0x888E ethertype, i.e. `eapol`
 * points at the 802.1X header (version / type / length) and `len` is the
 * number of EAPOL bytes.  The 4-way handshake state machine (M1..M4,
 * PTK/GTK derivation and installation) is driven from here.
 *
 * Task 3 provides only the routing plumbing; the handshake logic is added
 * in Task 4.
 */
void vsf_wifi_eapol_rx(vsf_wifi_t *wifi, const uint8_t *eapol, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // VSF_USE_WIFI && VSF_WIFI_USE_WPA
#endif // __VSF_WIFI_WPA_H__
