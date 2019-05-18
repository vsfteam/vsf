/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Simon Goldschmidt
 *
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define LWIP_DEBUG
//#define IP_DEBUG                        LWIP_DBG_ON
//#define TCP_DEBUG                       LWIP_DBG_ON
//#define UDP_DEBUG                       LWIP_DBG_ON
#define DHCP_DEBUG                      LWIP_DBG_ON

#define LWIP_PROVIDE_ERRNO              1
#define SYS_LIGHTWEIGHT_PROT            1
#define NO_SYS                          0

#define MEM_ALIGNMENT                   4
#define MEM_SIZE                        16000
#define MEM_USE_POOLS                   0
#define MEM_USE_POOLS_TRY_BIGGER_POOL   0
#define MEMP_USE_CUSTOM_POOLS           0
#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(1514)

#define LWIP_DHCP                       1
#define LWIP_ARP                        1
#define LWIP_DNS                        1

#define TCPIP_THREAD_STACKSIZE          (8 * 1024)
#define TCPIP_THREAD_PRIO               vsf_priority_0
#define TCPIP_MBOX_SIZE                 4
#define DEFAULT_THREAD_STACKSIZE        (8 * 1024)
#define DEFAULT_THREAD_PRIO             vsf_priority_0

#endif /* __LWIPOPTS_H__ */
